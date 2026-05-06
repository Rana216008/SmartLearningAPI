using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;

public class AdminController : Controller
{
    private readonly AppDbContext _db;
    public AdminController(AppDbContext db) => _db = db;

    public IActionResult Index()
    {
        // حماية الصفحة
        if (HttpContext.Session.GetString("Role") != "Admin")
            return RedirectToAction("Login", "Auth");

        // تعبئة الـ ViewModel بالبيانات من قاعدة البيانات
        var model = new AdminDashboardViewModel
        {
            Cards = _db.Cards.Include(c => c.Category).ToList(),
            Categories = _db.Categories.ToList()
        };

        return View(model);
    }

    //  إدارة الكروت 
    [HttpPost]
    public IActionResult SaveCard(Card card)
    {
        if (card.Id == 0) _db.Cards.Add(card);
        else _db.Cards.Update(card);

        _db.SaveChanges();
        return RedirectToAction("Index");
    }

    public IActionResult DeleteCard(int id)
    {
        var card = _db.Cards.Find(id);
        if (card != null) { _db.Cards.Remove(card); _db.SaveChanges(); }
        return RedirectToAction("Index");
    }

    //  إدارة الفئات 
    [HttpPost]
    public IActionResult AddCategory(Category cat)
    {
        _db.Categories.Add(cat);
        _db.SaveChanges();
        return RedirectToAction("Index");
    }

    public IActionResult DeleteCategory(int id)
    {
        var cat = _db.Categories.Find(id);
        if (cat != null) { _db.Categories.Remove(cat); _db.SaveChanges(); }
        return RedirectToAction("Index");
    }
}