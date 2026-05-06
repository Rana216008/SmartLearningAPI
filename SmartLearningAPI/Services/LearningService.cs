using SmartLearningAPI.Models;
using Microsoft.AspNetCore.Mvc;
using System.Linq;

public class LearningService : Controller
{
    private readonly AppDbContext _db;

    public LearningService(AppDbContext db)
    {
        _db = db;
    }

    public ScanResponse HandleScan(string uid)
    {
        // 1. البحث عن الكرت
        var card = _db.Cards.SingleOrDefault(c => c.UID == uid);

        if (card == null)
        {
            return new ScanResponse
            {
                Action = "error",
                Message = "الكرت غير مسجل في النظام",
                Track = 0
            };
        }

        // استخدمت FirstOrDefault بدلاً من First لتجنب الخطأ إذا كانت القاعدة فارغة
        var settings = _db.AppSettings.FirstOrDefault() ?? new AppSettings { CurrentMode = "Learn" };
        var session = _db.Sessions.FirstOrDefault();

        // 3. تحديث أو إضافة سجل التقدم للمستخدم
        var progress = _db.Progress.SingleOrDefault(p => p.UID == uid);

        if (progress == null)
        {
            progress = new UserProgress
            {
                UID = uid,
                Count = 1,
                IsLearned = false
            };
            _db.Progress.Add(progress);
        }
        else
        {
            progress.Count++;
        }

        // حفظ التغييرات في سجل التقدم
        _db.SaveChanges();

        if (settings.CurrentMode == "Exam")
        {
            return new ScanResponse
            {
                Action = "quiz",
                Track = card.TrackNumber,
                Message = "ما هذا؟"
            };
        }

        return new ScanResponse
        {
            Action = "play",
            Track = card.TrackNumber,
            Message = $"هذا هو كرت {card.Name}"
        };
    }
}