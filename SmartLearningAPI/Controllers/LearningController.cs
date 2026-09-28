using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using SmartLearningAPI.Models;
using SmartLearningAPI.Services;
using SmartLearningAPI.DTOs;

[ApiController]
[Route("api/[controller]")]
public class LearningController : ControllerBase
{
    private readonly AppDbContext _db;

    public LearningController(AppDbContext db)
    {
        _db = db;
    }

    [HttpGet("status")]
    public IActionResult GetCurrentStatus()
    {
        var settings = _db.AppSettings.FirstOrDefault();

        if (settings == null)
        {
            return Ok(new
            {
                mode = "Learning",
                category = "All",
                examTargetCardId = (int?)null,
                quizTrack = -1,
                targetCardName = "",
                targetImageName = ""
            });
        }

        int quizTrack = -1;
        string targetName = "";
        string targetImage = "";

        if (string.Equals(settings.CurrentMode, "Exam", StringComparison.OrdinalIgnoreCase)
            && settings.CurrentExamTargetCardId.HasValue)
        {
            var targetCard = _db.Cards.FirstOrDefault(c => c.Id == settings.CurrentExamTargetCardId.Value);
            if (targetCard != null)
            {
                quizTrack = targetCard.QuizTrackNumber;
                targetName = targetCard.Name;
                targetImage = targetCard.ImageName;
            }
        }

        return Ok(new
        {
            mode = settings.CurrentMode ?? "Learning",
            category = settings.CurrentCategory ?? "All",
            examTargetCardId = settings.CurrentExamTargetCardId,
            quizTrack = quizTrack,
            targetCardName = targetName,
            targetImageName = targetImage
        });
    }

    [HttpPost("update-settings")]
    public IActionResult UpdateSettings([FromBody] UpdateSettingsDto dto)
    {
        var settings = _db.AppSettings.FirstOrDefault();
        if (settings == null)
        {
            settings = new AppSettings();
            _db.AppSettings.Add(settings);
        }

        settings.CurrentMode = dto.Mode;
        settings.CurrentCategory = dto.Category;

        if (string.Equals(dto.Mode, "Exam", StringComparison.OrdinalIgnoreCase))
        {
            var query = _db.Cards.Include(c => c.Category).AsQueryable();
            if (!string.Equals(dto.Category, "All", StringComparison.OrdinalIgnoreCase))
            {
                query = query.Where(c => c.Category != null &&
                    c.Category.Name.ToLower() == dto.Category.ToLower());
            }

            var availableCards = query.ToList();

            if (availableCards.Any())
            {
                var cardUids = availableCards.Select(c => c.UID).ToList();

                var learnedUids = _db.Progress
                    .Where(p => cardUids.Contains(p.UID) && p.Count > 0)
                    .Select(p => p.UID)
                    .ToList();

                var candidateCards = availableCards.Where(c => learnedUids.Contains(c.UID)).ToList();

                if (!candidateCards.Any())
                {
                    candidateCards = availableCards;
                }

                var random = new Random();
                var selectedTargetCard = candidateCards[random.Next(candidateCards.Count)];

                settings.CurrentExamTargetCardId = selectedTargetCard.Id;
            }
        }
        else
        {
            settings.CurrentExamTargetCardId = null;
        }

        _db.SaveChanges();
        return Ok(new { success = true, message = "تم تحديث الإعدادات بنجاح" });
    }
}