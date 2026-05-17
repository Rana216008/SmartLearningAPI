using SmartLearningAPI.Models;
using Microsoft.AspNetCore.Mvc;
using System.Linq;
using Microsoft.EntityFrameworkCore;
using Microsoft.Extensions.DependencyInjection;

public class LearningService : Controller
{
    private readonly AppDbContext _db;
    private readonly IServiceScopeFactory _scopeFactory;

    public LearningService(AppDbContext db, IServiceScopeFactory scopeFactory)
    {
        _db = db;
        _scopeFactory = scopeFactory;
    }

    public ScanResponse HandleScan(string uid)
    {
        // 1. Find the card with its category
        var card = _db.Cards.Include(c => c.Category).SingleOrDefault(c => c.UID == uid);
        if (card == null)
            return new ScanResponse
            {
                Action = "error",
                Message = "الكرت غير مسجل في النظام",
                Track = 0
            };

        // 2. Read settings from a fresh context (no caching)
        string currentMode = "Learn";
        using (var scope = _scopeFactory.CreateScope())
        {
            var freshDb = scope.ServiceProvider.GetRequiredService<AppDbContext>();
            var settings = freshDb.AppSettings.FirstOrDefault();
            if (settings != null)
                currentMode = settings.CurrentMode ?? "Learn";
        }

        // 3. Progress update and mastery check
        var progress = _db.Progress.SingleOrDefault(p => p.UID == uid);
        if (progress == null)
        {
            progress = new UserProgress { UID = uid, Count = 1, IsLearned = false };
            _db.Progress.Add(progress);
        }
        else
        {
            progress.Count++;
            if (progress.Count >= 3)
                progress.IsLearned = true;
        }
        _db.SaveChanges();

        // 4. Action based on mode (Learn / Exam)
        if (currentMode == "Exam")
            return new ScanResponse
            {
                Action = "quiz",
                Track = card.TrackNumber,
                Message = "ما هذا؟"
            };
        else
            return new ScanResponse
            {
                Action = "play",
                Track = card.TrackNumber,
                Message = card.Name
            };
    }
}