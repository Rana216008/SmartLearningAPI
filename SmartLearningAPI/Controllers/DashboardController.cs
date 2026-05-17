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
        var model = new DashboardViewModel
        {
            LearnedCount = _db.Progress.Count(p => p.IsLearned),
            Attempts = _db.Progress.Sum(p => p.Count),
            Errors = 0
        };

        return View(model);
    }

    [HttpPost]
    public IActionResult UpdateSettings(string type, string value)
    {
        var settings = _db.AppSettings.FirstOrDefault();

        if (settings == null)
            return BadRequest("Settings not found");

        if (type == "Category")
            settings.CurrentCategory = value;

        else if (type == "Status")
            settings.CurrentMode = value;

        _db.SaveChanges();
        return Ok();
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