using System;
using System.Linq;

public class LearningService
{
    private readonly AppDbContext _db;

    public LearningService(AppDbContext db)
    {
        _db = db;
    }

    public ScanResponse HandleScan(string uid)
    {
        var card = _db.Cards.FirstOrDefault(c => c.UID == uid);

        if (card == null)
        {
            return new ScanResponse
            {
                Action = "error",
                Message = "بطاقة غير معروفة"
            };
        }

        var session = _db.Sessions.FirstOrDefault() ?? new Session { Mode = "learning" };

        if (session.Mode == "learning")
        {
            return HandleLearning(card);
        }
        else if (session.Mode == "quiz")
        {
            return HandleQuiz(card, session);
        }

        return new ScanResponse();
    }

    //private ScanResponse HandleLearning(Card card)
    //{
    //    var progress = _db.Progress.FirstOrDefault(p => p.UID == card.UID);

    //    if (progress == null)
    //    {
    //        progress = new UserProgress
    //        {
    //            UID = card.UID,
    //            Count = 0,
    //            IsLearned = false
    //        };
    //        _db.Progress.Add(progress);
    //    }

    //    progress.Count++;

    //    if (progress.Count >= 3)
    //        progress.IsLearned = true;

    //    _db.SaveChanges();

    //    // تحقق هل انتهى التعلم
    //    if (_db.Progress.All(p => p.IsLearned))
    //    {
    //        var session = _db.Sessions.FirstOrDefault();
    //        session.Mode = "quiz";
    //        _db.SaveChanges();
    //    }

    //    return new ScanResponse
    //    {
    //        Action = "play",
    //        Track = card.TrackNumber,
    //        Message = card.Name
    //    };
    //}
    private ScanResponse HandleLearning(Card card)
    {
        var progress = _db.Progress.FirstOrDefault(p => p.UID == card.UID);

        if (progress == null)
        {
            progress = new UserProgress
            {
                UID = card.UID,
                Count = 0,
                IsLearned = false
            };
            _db.Progress.Add(progress);
        }

        progress.Count++;

        if (progress.Count >= 3)
        {
            progress.IsLearned = true;
            _db.SaveChanges();

            return new ScanResponse
            {
                Action = "quiz",
                Track = 10, // سؤال
                Message = "ابدأ الاختبار"
            };
        }
        var session = _db.Sessions.FirstOrDefault();
        session.Mode = "quiz";
        _db.SaveChanges();

        return new ScanResponse
        {
            Action = "play",
            Track = card.TrackNumber,
            Message = card.Name
        };
    }

    private ScanResponse HandleQuiz(Card card, Session session)
    {
        if (session.CurrentExpectedUID == null)
        {
            var random = _db.Cards.OrderBy(x => Guid.NewGuid()).First();
            session.CurrentExpectedUID = random.UID;
            _db.SaveChanges();

            return new ScanResponse
            {
                Action = "quiz",
                Track = random.TrackNumber,
                Message = "أعطني: " + random.Name
            };
        }

        if (card.UID == session.CurrentExpectedUID)
        {
            session.CurrentExpectedUID = null;
            _db.SaveChanges();

            return new ScanResponse
            {
                Action = "correct",
                Track = 11,
                Message = "إجابة صحيحة"
            };
        }
        else
        {
            return new ScanResponse
            {
                Action = "wrong",
                Track = 12,
                Message = "إجابة خاطئة"
            };
        }
        //else
        //{
        //    return new ScanResponse
        //    {
        //        Action = "wrong",
        //        Track = 11, //  صوت خطأ
        //        Message = "حاول مرة أخرى"
        //    };
        //}
    }
}