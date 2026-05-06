using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Http;

public class AuthController : Controller
{
    // كلمات المرور المحددة
    private const string ADMIN_PASSWORD = "admin_password"; // كلمة سر الأدمن
    private const string USER_PASSWORD = "user_password";   // كلمة سر المستخدم

    public IActionResult Login()
    {
        return View();
    }

    [HttpPost]
    public IActionResult Login(string password)
    {
        // 1. التحقق إذا كان الداخل هو الأدمن
        if (password == ADMIN_PASSWORD)
        {
            HttpContext.Session.SetString("Role", "Admin");
            HttpContext.Session.SetString("Auth", "OK");
            return RedirectToAction("Index", "Admin"); // التوجه لصفحة الإدارة
        }

        // 2. التحقق إذا كان الداخل هو المستخدم (الأم)
        if (password == USER_PASSWORD)
        {
            HttpContext.Session.SetString("Role", "User");
            HttpContext.Session.SetString("Auth", "OK");
            return RedirectToAction("Index", "Dashboard"); // التوجه للداش بورد
        }

        ViewBag.Error = "كلمة المرور غير صحيحة، حاول مرة أخرى";
        return View();
    }

    public IActionResult Logout()
    {
        HttpContext.Session.Clear();
        return RedirectToAction("Login");
    }
}