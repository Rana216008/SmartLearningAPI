using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using System.Linq;

public class LearningController : Controller
{
    private readonly AppDbContext _context;

    public LearningController(AppDbContext context)
    {
        _context = context;
    }

    public IActionResult GetNextCard()
    {
        var settings = _context.AppSettings.FirstOrDefault();

        if (settings == null)
        {
            return Content("لم يتم ضبط الإعدادات بعد في قاعدة البيانات");
        }

        // 2. فلترة الكروت بناءً على القسم الذي اختارته الأم (عربي، إنجليزي، إلخ)
        var cards = _context.Cards
            .Where(c => c.Category == settings.CurrentCategory)
            .OrderBy(c => c.TrackNumber)
            .ToList();

        if (!cards.Any())
        {
            return Content($"لا توجد كروت مضافة في قسم {settings.CurrentCategory}");
        }

        // 3. تحديث جدول الجلسة (Session) ليعرف النظام الوضع الحالي (Learning أو Exam)
        var currentSession = _context.Sessions.FirstOrDefault(s => s.Id == 1);
        if (currentSession != null)
        {
            currentSession.Mode = settings.CurrentMode;
            _context.SaveChanges();
        }

        // 4. منطق اختيار الكرت التالي بناءً على التقدم (UserProgress)
        // نجلب أول كرت في القسم المختار لم يتعلمه الطفل بعد
        var nextCard = cards.FirstOrDefault(c =>
            !_context.Progress.Any(p => p.UID == c.UID && p.IsLearned));

        // إذا أتم الطفل كل الكروت، نعيد عرضها من جديد أو نظهر رسالة نجاح
        if (nextCard == null) nextCard = cards.First();

        // 5. توجيه المستخدم للواجهة المطلوبة بناءً على أمر الأم
        if (settings.CurrentMode == "Exam")
        {
            // في وضع الامتحان، نرسل كل الكروت المختارة ليتم اختبار الطفل فيها
            return View("ExamMode", cards);
        }
        else
        {
            // في وضع التعليم، نرسل الكرت التالي فقط للتركيز عليه
            return View("LearningMode", nextCard);
        }
    }

    // أكشن إضافي لتحديث التقدم عند مسح الكرت (يتم استدعاؤه عبر الـ API الخاص بالروبوت)
    [HttpPost]
    public IActionResult UpdateProgress(string uid)
    {
        var progress = _context.Progress.FirstOrDefault(p => p.UID == uid);

        if (progress == null)
        {
            _context.Progress.Add(new UserProgress { UID = uid, Count = 1, IsLearned = false });
        }
        else
        {
            progress.Count++;
            // إذا تكرر الكرت 3 مرات، نعتبره "تم تعلمه" تلقائياً
            if (progress.Count >= 3)
            {
                progress.IsLearned = true;
            }
        }

        _context.SaveChanges();
        return Ok();
    }
}