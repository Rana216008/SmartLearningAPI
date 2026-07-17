using SmartLearningAPI.Models;
using Microsoft.AspNetCore.Mvc;
using System.Linq;
using Microsoft.EntityFrameworkCore;
using Microsoft.Extensions.DependencyInjection;
using System;

public class LearningService : Controller
{
    private readonly AppDbContext _db;
    private readonly IServiceScopeFactory _scopeFactory;

    public LearningService(AppDbContext db, IServiceScopeFactory scopeFactory)
    {
        _db = db;
        _scopeFactory = scopeFactory;
    }

    public ScanResponse HandleScan(string uid)
    {
        // 1. البحث عن الكرت وتضمين الفئة التابع لها في البداية
        var card = _db.Cards.Include(c => c.Category).SingleOrDefault(c => c.UID == uid);
        if (card == null)
        {
            return new ScanResponse
            {
                Action = "error",
                Message = "الكرت غير مسجل في النظام",
                Track = 19 // 0019.mp3: اختر كرتاً آخر
            };
        }

        // 2. قراءة الإعدادات الحالية للأم والكرت المستهدف للاختبار من Fresh Context لمنع الكاش
        string currentMode = "Learning";
        string currentCategory = "All";
        int? examTargetCardId = null;

        using (var scope = _scopeFactory.CreateScope())
        {
            var freshDb = scope.ServiceProvider.GetRequiredService<AppDbContext>();
            var settings = freshDb.AppSettings.FirstOrDefault();

            if (settings != null)
            {
                currentMode = settings.CurrentMode ?? "Learning";
                currentCategory = settings.CurrentCategory ?? "All";
                examTargetCardId = settings.CurrentExamTargetCardId;
            }
        }

        // 3. التحقق المنطقي: هل الكرت الممسوح يطابق الفئة التي حددتها الأم؟
        if (currentCategory != "All")
        {
            if (card.Category == null || card.Category.Name != currentCategory)
            {
                return new ScanResponse
                {
                    Action = "wrong_category",
                    Message = $"هذا الكرت خارج المجموعة المطلوبة. المجموعة الحالية هي: {currentCategory}",
                    Track = 19 // 0019.mp3: اختر كرتاً آخر (لأن المجموعة خاطئة)
                };
            }
        }

        // 4. معالجة وضع الاختبار (Exam Mode)
        if (currentMode == "Exam")
        {
            // جلب كروت الفئة الحالية للاعتماد عليها في توليد الأسئلة (التلقائي أو التالي)
            var query = _db.Cards.Include(c => c.Category).AsQueryable();
            if (currentCategory != "All")
            {
                query = query.Where(c => c.Category != null && c.Category.Name == currentCategory);
            }
            var availableCards = query.ToList();

            if (!availableCards.Any())
            {
                return new ScanResponse { Action = "error", Message = "لا توجد كروت متاحة للاختبار في هذه الفئة", Track = 19 };
            }

            // -------------------------------------------------------------
            // الحالة أ: يوجد سؤال نشط بالفعل، والطفل قام بمسح كرت للإجابة عليه
            // -------------------------------------------------------------
            if (examTargetCardId.HasValue)
            {
                // إذا كانت إجابة الطفل صحيحة (طابق الكرت المستهدف)
                if (card.Id == examTargetCardId.Value)
                {
                    // 1. اختيار الكرت التالي فوراً وبشكل عشوائي بدون تصفير
                    var random = new Random();
                    var nextTargetCard = availableCards[random.Next(availableCards.Count)];

                    // 2. حفظ الكرت المستهدف الجديد في قاعدة البيانات للسؤال القادم
                    var settingsToUpdate = _db.AppSettings.FirstOrDefault();
                    if (settingsToUpdate != null)
                    {
                        settingsToUpdate.CurrentExamTargetCardId = nextTargetCard.Id;
                        _db.SaveChanges();
                    }

                    // 3. حساب تراك السؤال الجديد (9 + TrackNumber للكرت الجديد)
                    int nextQuestionTrack = 9 + nextTargetCard.TrackNumber;

                    // 4. إرجاع النتيجة: نرسل تراك السؤال التالي مباشرة ليعمل تلقائياً!
                    return new ScanResponse
                    {
                        Action = "correct_and_next",
                        Message = $"إجابة صحيحة! أحسنت. السؤال التالي: أين هو كرت {nextTargetCard.Name}؟",
                        Track = nextQuestionTrack // الروبوت ينتقل فوراً لنطق السؤال الجديد (مثلاً تراك 12)
                    };
                }
                else
                {
                    // إذا كانت الإجابة خاطئة، يبقى نفس السؤال وطلب إعادة المحاولة
                    return new ScanResponse
                    {
                        Action = "wrong",
                        Message = "حاول مرة أخرى، ليس هذا الكرت المطلوب",
                        Track = 18 // 0018.mp3: حاول مرة أخرى
                    };
                }
            }
            // -------------------------------------------------------------
            // الحالة ب: بداية وضع الاختبار (لا يوجد سؤال نشط بعد) - أول مسحة لتوليد أول سؤال
            // -------------------------------------------------------------
            else
            {
                var random = new Random();
                var firstTargetCard = availableCards[random.Next(availableCards.Count)];

                var settingsToUpdate = _db.AppSettings.FirstOrDefault();
                if (settingsToUpdate != null)
                {
                    settingsToUpdate.CurrentExamTargetCardId = firstTargetCard.Id;
                    _db.SaveChanges();
                }

                int firstQuestionTrack = 9 + firstTargetCard.TrackNumber;

                return new ScanResponse
                {
                    Action = "ask_question",
                    Message = $"بدء الاختبار! أين هو كرت: {firstTargetCard.Name}؟",
                    Track = firstQuestionTrack // ينطق أول سؤال (من 10 إلى 15)
                };
            }
        }
        // 5. معالجة وضع التعليم (Learning Mode)
        var progress = _db.Progress.SingleOrDefault(p => p.UID == uid);
        if (progress == null)
        {
            progress = new UserProgress { UID = uid, Count = 1, IsLearned = false };
            _db.Progress.Add(progress);
        }
        else
        {
            progress.Count++;
            if (progress.Count >= 3)
                progress.IsLearned = true;
        }
        _db.SaveChanges();

        return new ScanResponse
        {
            Action = "play",
            Track = card.TrackNumber, // ينطق صوت الحرف أو اللون نفسه مباشرة (1 إلى 9)
            Message = card.Name
        };
    }
}