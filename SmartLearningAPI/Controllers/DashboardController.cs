using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using SmartLearningAPI.Models;
using System;
using System.Linq;

namespace SmartLearningAPI.Controllers
{
    public class DashboardController : Controller
    {
        private readonly AppDbContext _db;

        public DashboardController(AppDbContext db)
        {
            _db = db;
        }

        public IActionResult Index()
        {
            System.Diagnostics.Debug.WriteLine("=== ACTUAL DB: " + _db.Database.GetConnectionString());

            var settings = _db.AppSettings.FirstOrDefault();

            var model = new DashboardViewModel
            {
                LearnedCount = _db.Progress.Count(p => p.IsLearned),
                Attempts = _db.Progress.Any() ? _db.Progress.Sum(p => p.Count) : 0,
                Errors = 0,
                ActiveMode = settings?.CurrentMode,
                ActiveCategory = settings?.CurrentCategory
            };

            return View(model);
        }

        [HttpPost]
        public IActionResult UpdateSettings(string type, string value)
        {
            var settings = _db.AppSettings.FirstOrDefault();

            // إنشاء سجل افتراضي إذا كان الجدول فارغاً
            if (settings == null)
            {
                settings = new AppSettings { CurrentMode = "Learning", CurrentCategory = "All" };
                _db.AppSettings.Add(settings);
                _db.SaveChanges();
            }

            if (string.Equals(type, "Category", StringComparison.OrdinalIgnoreCase))
            {
                settings.CurrentCategory = value;
            }
            else if (string.Equals(type, "Status", StringComparison.OrdinalIgnoreCase) || string.Equals(type, "Mode", StringComparison.OrdinalIgnoreCase))
            {
                settings.CurrentMode = value;

                // تحويل الوضع إلى "وضع الاختبار"
                if (value == "Exam")
                {
                    var query = _db.Cards.Include(c => c.Category).AsQueryable();

                    if (!string.IsNullOrEmpty(settings.CurrentCategory) && settings.CurrentCategory != "All")
                    {
                        query = query.Where(c => c.Category != null &&
                            (c.Category.Name.ToLower() == settings.CurrentCategory.ToLower()));
                    }

                    var availableCards = query.ToList();

                    if (availableCards.Any())
                    {
                        var random = new Random();
                        var targetCard = availableCards[random.Next(availableCards.Count)];

                        settings.CurrentExamTargetCardId = targetCard.Id;
                        int questionTrack = 9 + targetCard.TrackNumber;

                        _db.SaveChanges();

                        return Json(new
                        {
                            Status = "Success",
                            Message = $"تم التحويل للاختبار. المطلوب: {targetCard.Name}",
                            AskTrack = questionTrack
                        });
                    }
                    else
                    {
                        settings.CurrentExamTargetCardId = null;
                    }
                }
                else
                {
                    settings.CurrentExamTargetCardId = null;
                }
            }

            _db.SaveChanges();
            return Ok(new { Status = "Success", Message = "تم تحديث الإعدادات بنجاح" });
        }

        [HttpGet]
        public IActionResult GetStats()
        {
            var learned = _db.Progress.Count(p => p.IsLearned);
            var total = _db.Progress.Count();

            return Json(new
            {
                learned,
                remaining = total - learned,
                days = new[] { "الأحد", "الاثنين", "الثلاثاء", "الأربعاء", "الخميس" },
                minutes = new[] { 20, 35, 40, 25, 50 }
            });
        }

        public IActionResult ResetProgress()
        {
            _db.Progress.RemoveRange(_db.Progress);
            _db.SaveChanges();
            return RedirectToAction("Analytics");
        }

        public IActionResult Analytics()
        {
            var items = (from p in _db.Progress
                         join c in _db.Cards on p.UID equals c.UID into cardGroup
                         from c in cardGroup.DefaultIfEmpty()
                         join cat in _db.Categories on c.CategoryId equals cat.Id into catGroup
                         from cat in catGroup.DefaultIfEmpty()
                         select new LearnedItem
                         {
                             CardName = c != null ? c.Name : "غير معروف",
                             Category = cat != null ? cat.Name : "بدون فئة",
                             UID = p.UID,
                             TrackNumber = c != null ? c.TrackNumber : 0,
                             ScanCount = p.Count,
                             IsLearned = p.IsLearned
                         }).ToList();

            var model = new AnalyticsViewModel
            {
                LearnedItems = items,
                TotalScans = items.Sum(i => i.ScanCount),
                TotalLearned = items.Count(i => i.IsLearned),
                Remaining = items.Count(i => !i.IsLearned),
                CategoryCounts = items
                    .GroupBy(i => i.Category)
                    .ToDictionary(g => g.Key, g => g.Count())
            };

            return View(model);
        }
    }
}