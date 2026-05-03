using System;
using System.Linq;
using Microsoft.EntityFrameworkCore;

public class LearningService
{
    private readonly AppDbContext _db;
    private const int TOTAL_CARDS_COUNT = 9; // مجموع الكروت (3 عربية، 3 إنجليزية، 3 ألوان)

    public LearningService(AppDbContext db)
    {
        _db = db;
    }

    public ScanResponse HandleScan(string uid)
    {
        // 1. البحث عن الكرت في قاعدة البيانات
        var card = _db.Cards.FirstOrDefault(c => c.UID == uid);
        if (card == null)
        {
            return new ScanResponse { Action = "error", Message = "بطاقة غير معروفة" };
        }

        // 2. الحصول على الجلسة الحالية وسجل تقدم الكرت
        var session = _db.Sessions.First();
        var progress = _db.Progress.FirstOrDefault(p => p.UID == uid);

        // 3. إذا كان الكرت يمسح لأول مرة، ننشئ له سجل تقدم
        if (progress == null)
        {
            progress = new UserProgress { UID = uid, Count = 0, IsLearned = false };
            _db.Progress.Add(progress);
            _db.SaveChanges();
        }

        // 4. المرحلة النهائية: الامتحان الشامل العشوائي
        if (session.Mode == "final")
        {
            return HandleFinalExam(card, session);
        }

        // 5. المرحلة الأولى: وضع التعلم والاختبار الفردي
        // إذا مسح الطفل الكرت أقل من 3 مرات، يبقى في وضع التعلم
        if (progress.Count < 3)
        {
            progress.Count++;
            _db.SaveChanges();

            // فحص هل اكتملت كل الكروت للانتقال للمرحلة النهائية
            CheckAndEnableFinalMode(session);

            return new ScanResponse
            {
                Action = "play",
                Track = card.TrackNumber,
                Message = $"تعلم: {card.Name}"
            };
        }
        else
        {
            // إذا وصل لـ 3 مرات، نعتبره "تعلمها" ونحول الاستجابة لاختبار فردي لهذا الكرت
            if (!progress.IsLearned)
            {
                progress.IsLearned = true;
                _db.SaveChanges();
                CheckAndEnableFinalMode(session);
            }

            return new ScanResponse
            {
                Action = "quiz",
                Track = card.TrackNumber,
                Message = $"اختبار فردي لـ {card.Name}"
            };
        }
    }
    private ScanResponse HandleFinalExam(Card scannedCard, Session session)
    {
        if (string.IsNullOrEmpty(session.CurrentExpectedUID))
        {
            var randomCard = _db.Cards.OrderBy(x => Guid.NewGuid()).First();
            session.CurrentExpectedUID = randomCard.UID;
            _db.SaveChanges();

            return new ScanResponse
            {
                Action = "quiz",
                Track = randomCard.TrackNumber,
                Message = "أعطني: " + randomCard.Name
            };
        }

        // التحقق مما إذا كان الكرت الممسوح هو المطلوب
        if (scannedCard.UID == session.CurrentExpectedUID)
        {
            session.CurrentExpectedUID = null; 
            _db.SaveChanges();

            return new ScanResponse
            {
                Action = "correct",
                Track = 11, // تراك صوتي: أحسنت
                Message = "إجابة صحيحة"
            };
        }
        else
        {
            return new ScanResponse
            {
                Action = "wrong",
                Track = 12, //  حاول مرة أخرى
                Message = "إجابة خاطئة"
            };
        }
    }

    // دالة تفحص هل تم تعلم جميع الكروت الـ 9 للانتقال للوضع النهائي
    private void CheckAndEnableFinalMode(Session session)
    {
        var learnedCount = _db.Progress.Count(p => p.IsLearned);

        if (learnedCount >= TOTAL_CARDS_COUNT)
        {
            session.Mode = "final";
            _db.SaveChanges();
        }
    }
}