using Microsoft.AspNetCore.Mvc;

public class AuthController : Controller
{
    private const string PASSWORD = "12345678";
    //باسوورد الشبكة تبع الESP
    //{
    //  "AppSettings": {
    //    "AdminPassword": "12345678"
    //  }
    //} اقدر اعملها كمان في appsettings.json

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