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
    [HttpPost]
    public IActionResult SaveCard(Card card)
    {
        // فحص أمان: للتأكد من أن الفئة المرسلة موجودة بالفعل في قاعدة البيانات
        var categoryExists = _db.Categories.Any(c => c.Id == card.CategoryId);
        if (!categoryExists)
        {
            // إذا كانت الفئة غير موجودة، يمكنك إرجاع رسالة خطأ بدلاً من عمل Crash للسيرفر
            ModelState.AddModelError("CategoryId", "الفئة المحددة غير صالحة أو غير موجودة.");
            return RedirectToAction("Index");
        }

        if (card.Id == 0)
        {
            _db.Cards.Add(card); //
        }
        else
        {
            _db.Cards.Update(card); //
        }

        _db.SaveChanges(); // هنا لن يحدث الخطأ مجدداً
        return RedirectToAction("Index"); //
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