using Microsoft.AspNetCore.Mvc;
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
}