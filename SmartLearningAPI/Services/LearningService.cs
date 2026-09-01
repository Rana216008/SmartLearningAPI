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
            // البحث عن الكرت
            var card = _db.Cards.Include(c => c.Category).SingleOrDefault(c => c.UID == uid);
            if (card == null)
            {
                return new ScanResponse
                {
                    Action = "error",
                    Message = "الكرت غير مسجل في النظام",
                    Track = 19,
                    ImageName = "Error",
                    Mode = "Learning",
                    Category = "All"   // no category restriction when card not found
                };
            }

            // قراءة الإعدادات
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

            // وضع الاستجابة (نفس الوضع الحالي)
            string responseMode = currentMode == "Exam" ? "Exam" : "Learning";

            // التحقق من المجموعة
            if (currentCategory != "All")
            {
                if (card.Category == null || card.Category.Name != currentCategory)
                {
                    return new ScanResponse
                    {
                        Action = "wrong_category",
                        Message = $"هذا الكرت خارج المجموعة المطلوبة: {currentCategory}",
                        Track = 19,
                        ImageName = "WrongCategory",
                        Mode = responseMode,
                        Category = currentCategory
                    };
                }
            }

            // تحديث التقدم
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

            // وضع الاختبار (Exam Mode)
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
                    return new ScanResponse
                    {
                        Action = "error",
                        Message = "لا توجد كروت متاحة للاختبار",
                        Track = 19,
                        ImageName = "Error",
                        Mode = responseMode,
                        Category = currentCategory
                    };
                }

                if (examTargetCardId.HasValue)
                {
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
                            ImageName = card.ImageName,
                            Mode = responseMode,
                            Category = currentCategory
                        };
                    }
                    else
                    {
                        // إجابة خاطئة
                        return new ScanResponse
                        {
                            Action = "wrong",
                            Message = "حاول مرة أخرى",
                            Track = 18,
                            ImageName = "Wrong",
                            Mode = responseMode,
                            Category = currentCategory
                        };
                    }
                }
                else
                {
                    // بداية الاختبار
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
                        ImageName = "QuestionMark",
                        Mode = responseMode,
                        Category = currentCategory
                    };
                }
            }

            // وضع التعلم (Learning Mode)
            return new ScanResponse
            {
                Action = "play",
                Track = card.TrackNumber,
                Message = card.Name,
                ImageName = card.ImageName,
                Mode = responseMode,
                Category = currentCategory
            };
        }
    }
}