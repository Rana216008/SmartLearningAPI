using Microsoft.AspNetCore.Mvc;

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
        var settings = _db.AppSettings.First();

        if (type == "Category")
            settings.CurrentCategory = value;
        else if (type == "Status")
            settings.CurrentMode = value;

        _db.SaveChanges();
        return Ok();
    }
}
