using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using System.Linq;

public class DashboardController : Controller
{
    private readonly AppDbContext _db;

    public DashboardController(AppDbContext db)
    {
        _db = db;
    }

    public IActionResult Index()
    {
<<<<<<< HEAD
        var settings = _db.AppSettings.FirstOrDefault();
=======
        // Read current settings (live from DB, no cache)
        var settings = _db.AppSettings.AsNoTracking().FirstOrDefault();
>>>>>>> saleh

        var model = new DashboardViewModel
        {
            LearnedCount = _db.Progress.Count(p => p.IsLearned),
<<<<<<< HEAD
            Attempts = _db.Progress.Any() ? _db.Progress.Sum(p => p.Count) : 0,
            Errors = 0,

            ActiveMode = settings?.CurrentMode,
            ActiveCategory = settings?.CurrentCategory
=======
            Attempts = _db.Progress.Sum(p => p.Count),
            Errors = 0,
            ActiveMode = settings?.CurrentMode ?? "Learn",
            ActiveCategory = settings?.CurrentCategory ?? "Arabic"
>>>>>>> saleh
        };

        return View(model);
    

    }

    [HttpPost]
    public IActionResult UpdateSettings(string type, string value)
    {
        // جلب سطر الإعدادات الوحيد من قاعدة البيانات
        var settings = _db.AppSettings.FirstOrDefault();

        if (settings == null)
            return BadRequest("Settings not found");

        if (type == "Category")
        {
            settings.CurrentCategory = value;
        }
        else if (type == "Status")
        {
            settings.CurrentMode = value;

            // إذا قامت الأم بتحويل الوضع إلى "وضع الاختبار"
            if (value == "Exam")
            {
                var availableCards = _db.Cards
                    .Where(c => c.Category.Name == settings.CurrentCategory)
                    .ToList();

                if (availableCards.Any())
                {
                    // 2. اختيار كارت عشوائي ليكون هو السؤال
                    var random = new Random();
                    var targetCard = availableCards[random.Next(availableCards.Count)];
                    // 3. تخزين الـ ID في الخاصية الموجودة عندك مسبقاً
                    settings.CurrentExamTargetCardId = targetCard.Id;

                    // 4. تحديد رقم التراك الصوتي للسؤال بناءً على ترتيب الـ SD Card
                    int questionTrack = 10; // افتراضي

                    if (settings.CurrentCategory == "Arabic")
                    {
                        // كارت 1 (أ) -> تراك 10 | كارت 2 (ب) -> تراك 11 | كارت 3 (ت) -> تراك 12
                        questionTrack = 9 + targetCard.TrackNumber;
                    }
                    else if (settings.CurrentCategory == "English")
                    {
                        // كارت 4 -> تراك 13 | كارت 5 -> تراك 14 | كارت 6 -> تراك 15
                        questionTrack = 9 + targetCard.TrackNumber;
                    }

                    _db.SaveChanges();

                    // نرجع النتيجة لصفحة المتصفح مع رقم تراك السؤال (AskTrack) ليرسل للروبوت فوراً
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
                // إذا رجعت لوضع التعلم، يتم تصفير السؤال
                settings.CurrentExamTargetCardId = null;
            }
        }

        _db.SaveChanges();
        return Ok(new { Status = "Success", Message = "تم تحديث الإعدادات بنجاح" });
    }

    /* API للإحصائيات */

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
        // Remove all progress rows, keep cards and settings
        _db.Progress.RemoveRange(_db.Progress);
        _db.SaveChanges();
        return RedirectToAction("Analytics");
    }
    public IActionResult Analytics()
    {
        // Manual join: Progress → Cards on UID, then Cards → Categories
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