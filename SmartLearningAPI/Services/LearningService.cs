using Microsoft.EntityFrameworkCore;
using Microsoft.Extensions.DependencyInjection;
using SmartLearningAPI.Models;
using System;
using System.Linq;

namespace SmartLearningAPI.Services
{
    public class LearningService
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
            //  البحث عن الكرت
            var card = _db.Cards.Include(c => c.Category).SingleOrDefault(c => c.UID == uid);
            if (card == null)
            {
                return new ScanResponse
                {
                    Action = "error",
                    Message = "الكرت غير مسجل في النظام",
                    Track = 19, // 0019.mp3: اختر كرتاً آخر
                    ImageName = "Error"
                };
            }

            // 2. قراءة الإعدادات
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

            // 3. التحقق من المجموعة
            //إذا كانت الأم محددة قسم معين(مثلاً: "الأرقام") والطفل سحب كرت من قسم "الحروف"، يرفض السيرفر الطلب ويُرجع 
            //    أكشن wrong_category مع الصوت المناسب للتنبيه.
            if (currentCategory != "All")
            {
                if (card.Category == null || card.Category.Name != currentCategory)
                {
                    return new ScanResponse
                    {
                        Action = "wrong_category",
                        Message = $"هذا الكرت خارج المجموعة المطلوبة: {currentCategory}",
                        Track = 19,
                        ImageName = "WrongCategory"
                    };
                }
            }

            // 4. تحديث التقدم
            //تم حفظ محاولة المسح الحالية وتأكيد تعلم الكرت بعد تكراره 3 مرات
            //مثل المنطق الموجود بالكنترولر لضمان استمرارية تحديث البيانات عبر الـ API.
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

            // 5. وضع الاختبار (Exam Mode)
            if (currentMode == "Exam")
            {
                var query = _db.Cards.Include(c => c.Category).AsQueryable();
                if (currentCategory != "All")
                {
                    query = query.Where(c => c.Category != null && c.Category.Name == currentCategory);
                }
                var availableCards = query.ToList();

                if (!availableCards.Any())
                {
                    return new ScanResponse { Action = "error", Message = "لا توجد كروت متاحة للاختبار", Track = 19, ImageName = "Error" };
                }

                //يفحص ما إذا كان هناك سؤال قائم
                //ومخزن مسبقاً في النظام(أي أن examTargetCardId ليس null)
                if (examTargetCardId.HasValue)
                {

                    //يقرأ المعرّف(Id) للكرت الذي مسحه الطفل الآن على الجهاز، ويقارنه بمعرّف
                    //الكرت المطلوب في السؤال الحاضر(examTargetCardId.Value).إذا تساويا، فهذا يعني أن الطفل أجاب إجابة صحيحة!


                    if (card.Id == examTargetCardId.Value)
                    {
                        // إجابة صحيحة -> اختيار سؤال جديد
                        var random = new Random();
                        var nextTargetCard = availableCards[random.Next(availableCards.Count)];

                        var settingsToUpdate = _db.AppSettings.FirstOrDefault();
                        if (settingsToUpdate != null)
                        {
                            settingsToUpdate.CurrentExamTargetCardId = nextTargetCard.Id;
                            _db.SaveChanges();
                        }

                        return new ScanResponse
                        {
                            Action = "correct_and_next",
                            Message = $"إجابة صحيحة! السؤال التالي: أين هو كرت {nextTargetCard.Name}؟",
                            Track = 9 + nextTargetCard.TrackNumber,
                            ImageName = card.ImageName
                        };
                    }
                    else
                    {
                        // إجابة خاطئة
                        return new ScanResponse
                        {
                            Action = "wrong",
                            Message = "حاول مرة أخرى",
                            Track = 18, // 0018.mp3
                            ImageName = "Wrong"
                        };
                    }
                }
                else
                {
                    // بداية الاختبار
                    //يستبدل الكرت المستهدف القديم بالكرت العشوائي الجديد الذي تم اختياره توًا.
                    var random = new Random();
                    var firstTargetCard = availableCards[random.Next(availableCards.Count)];

                    var settingsToUpdate = _db.AppSettings.FirstOrDefault();
                    if (settingsToUpdate != null)
                    {
                        settingsToUpdate.CurrentExamTargetCardId = firstTargetCard.Id;
                        _db.SaveChanges();
                    }

                    return new ScanResponse
                    {
                        Action = "ask_question",
                        Message = $"بدء الاختبار! أين هو كرت: {firstTargetCard.Name}؟",
                        Track = 9 + firstTargetCard.TrackNumber,
                        ImageName = "QuestionMark"
                    };
                }
            }

            // 6. وضع التعلم (Learning Mode)
            return new ScanResponse
            {
                Action = "play",
                Track = card.TrackNumber,
                Message = card.Name,
                ImageName = card.ImageName
            };
        }
    }
}