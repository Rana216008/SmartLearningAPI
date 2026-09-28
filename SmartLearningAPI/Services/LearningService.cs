using Microsoft.EntityFrameworkCore;
using SmartLearningAPI.Models;
using System;
using System.Linq;

namespace SmartLearningAPI.Services
{
    public class LearningService
    {
        private readonly AppDbContext _db;

        public LearningService(AppDbContext db)
        {
            _db = db;
        }

        public ScanResponse HandleScan(string uid)
        {
            // قراءة الإعدادات الحالية من نفس سياق البيانات
            var settings = _db.AppSettings.FirstOrDefault();

            string currentMode = settings?.CurrentMode ?? "Learning";
            string currentCategory = settings?.CurrentCategory ?? "All";
            int? examTargetCardId = settings?.CurrentExamTargetCardId;

            string responseMode = currentMode == "Exam" ? "Exam" : "Learning";

            // 1. البحث عن الكرت
            var card = _db.Cards.Include(c => c.Category).SingleOrDefault(c => c.UID == uid);
            if (card == null)
            {
                return new ScanResponse
                {
                    Action = "error",
                    Message = "الكرت غير مسجل في النظام",
                    Track = 25, // "حاول مرة أخرى"
                    ImageName = "Error",
                    Mode = responseMode,
                    Category = currentCategory
                };
            }

            // 2. التحقق من مطابقة الفئة
            if (!string.Equals(currentCategory, "All", StringComparison.OrdinalIgnoreCase))
            {
                string cardCatName = card.Category?.Name?.Trim() ?? "";
                string targetCatName = currentCategory.Trim();

                if (!string.Equals(cardCatName, targetCatName, StringComparison.OrdinalIgnoreCase))
                {
                    int categoryErrorTrack = targetCatName.ToLower() switch
                    {
                        "arabic" => 19,
                        "english" => 20,
                        "colors" => 21,
                        _ => 19
                    };

                    return new ScanResponse
                    {
                        Action = "wrong_category",
                        Message = $"هذا الكرت خارج المجموعة المطلوبة: {currentCategory}",
                        Track = categoryErrorTrack,
                        ImageName = "WrongCategory",
                        Mode = responseMode,
                        Category = currentCategory
                    };
                }
            }

            // 3. تحديث التقدم
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

            // 4. وضع الاختبار (Exam Mode)
            if (currentMode == "Exam")
            {
                var query = _db.Cards.Include(c => c.Category).AsQueryable();
                if (!string.Equals(currentCategory, "All", StringComparison.OrdinalIgnoreCase))
                {
                    query = query.Where(c => c.Category != null &&
                        c.Category.Name.ToLower() == currentCategory.ToLower());
                }

                var allCategoryCards = query.ToList();

                if (!allCategoryCards.Any())
                {
                    return new ScanResponse
                    {
                        Action = "error",
                        Message = "لا توجد كروت مضافة في هذه المجموعة للاختبار",
                        Track = 25,
                        ImageName = "Error",
                        Mode = responseMode,
                        Category = currentCategory
                    };
                }

                var cardUids = allCategoryCards.Select(c => c.UID).ToList();
                var learnedProgress = _db.Progress
                    .Where(p => cardUids.Contains(p.UID) && p.Count > 0)
                    .ToList();

                var candidateCards = allCategoryCards
                    .Where(c => learnedProgress.Any(p => p.UID == c.UID))
                    .ToList();

                if (!candidateCards.Any())
                {
                    candidateCards = allCategoryCards;
                }

                Card GetNextSmartQuestionCard(int? excludeCardId = null)
                {
                    var filteredCandidates = candidateCards
                        .Where(c => !excludeCardId.HasValue || c.Id != excludeCardId.Value)
                        .ToList();

                    if (!filteredCandidates.Any()) filteredCandidates = candidateCards;

                    var random = new Random();
                    return filteredCandidates
                        .Select(c => new
                        {
                            Card = c,
                            Count = learnedProgress.FirstOrDefault(p => p.UID == c.UID)?.Count ?? 0
                        })
                        .OrderBy(x => x.Count)
                        .ThenBy(_ => random.Next())
                        .First().Card;
                }

                if (examTargetCardId.HasValue)
                {
                    if (card.Id == examTargetCardId.Value)
                    {
                        var nextTargetCard = GetNextSmartQuestionCard(excludeCardId: card.Id);

                        if (settings != null)
                        {
                            settings.CurrentExamTargetCardId = nextTargetCard.Id;
                            _db.SaveChanges();
                        }

                        return new ScanResponse
                        {
                            Action = "correct_and_next",
                            Message = $"إجابة صحيحة! السؤال التالي: أين هو كرت {nextTargetCard.Name}؟",
                            Track = nextTargetCard.QuizTrackNumber, 
                            FeedbackTrack = 23,                     
                            ImageName = card.ImageName,             
                            Category = currentCategory
                        };
                    }
                    else
                    {
                        return new ScanResponse
                        {
                            Action = "wrong",
                            Message = "إجابة خاطئة، حاول مرة أخرى",
                            Track = 25,                             // رقم تراك (حاول مرة أخرى)
                            FeedbackTrack = -1,
                            ImageName = "Wrong",                    // صورة الخطأ أو DefaultFace
                            Mode = responseMode,
                            Category = currentCategory
                        };
                    }
                }
                else
                {
                    var firstTargetCard = GetNextSmartQuestionCard();

                    if (settings != null)
                    {
                        settings.CurrentExamTargetCardId = firstTargetCard.Id;
                        _db.SaveChanges();
                    }

                    return new ScanResponse
                    {
                        Action = "ask_question",
                        Message = $"بدء الاختبار! أين هو كرت: {firstTargetCard.Name}؟",
                        Track = firstTargetCard.QuizTrackNumber,
                        ImageName = "QuestionMark",
                        Mode = responseMode,
                        Category = currentCategory
                    };
                }
            }

            // 5. وضع التعلم العادي (Learning Mode)
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