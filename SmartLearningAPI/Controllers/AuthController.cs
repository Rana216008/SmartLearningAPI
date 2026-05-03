using Microsoft.AspNetCore.Mvc;

public class AuthController : Controller
{
    private const string PASSWORD = "12345678";
   

    public IActionResult Login()
    {
        return View();
    }

    [HttpPost]
    public IActionResult Login(string password)
    {
        if (password == PASSWORD)
        {
            HttpContext.Session.SetString("Auth", "OK");
            return RedirectToAction("Index", "Dashboard");
        }

        ViewBag.Error = "كلمة المرور غلط";
        return View();
    }

    public IActionResult Logout()
    {
        HttpContext.Session.Clear();
        return RedirectToAction("Login");
    }
}