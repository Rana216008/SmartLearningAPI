namespace SmartLearningAPI.Models
{
    public class ScanRequest
    {
        public string UID { get; set; } = string.Empty;
    }

    public class ScanResponse
    {
        public string Action { get; set; } = string.Empty; // play / ask_question / correct_and_next / wrong / error / wrong_category
        public int Track { get; set; }
        public string Message { get; set; } = string.Empty;
        public string ImageName { get; set; } = string.Empty; 

    public class AppSettings
    {
        public int Id { get; set; }
        public string CurrentMode { get; set; } = "Learning"; // "Learning" أو "Exam"
        public string CurrentCategory { get; set; } = "All";  // "Arabic", "English", "Colors", "All"
        public int? CurrentExamTargetCardId { get; set; }
    }

    public class Card
    {
        public int Id { get; set; }
        public string Name { get; set; } = string.Empty;
        public string UID { get; set; } = string.Empty;
        public int CategoryId { get; set; }
        public Category? Category { get; set; }
        public int TrackNumber { get; set; }
        public int QuizTrackNumber { get; set; }
        public string ImageName { get; set; } = string.Empty; // اسم ملف الصورة للشاشة
    }

    public class Category
    {
        public int Id { get; set; }
        public string Name { get; set; } = string.Empty;
        public List<Card> Cards { get; set; } = new List<Card>();
    }

    public class UserProgress
    {
        public int Id { get; set; }
        public string UID { get; set; } = string.Empty;
        public int Count { get; set; }
        public bool IsLearned { get; set; }
    }

    public class Session
    {
        public int Id { get; set; }
        public string Mode { get; set; } = "Learning";
        public string? CurrentExpectedUID { get; set; }
    }

    // View Models للـ Dashboard والـ Analytics
    public class DashboardViewModel
    {
        public int LearnedCount { get; set; }
        public int Attempts { get; set; }
        public int Errors { get; set; }
        public string? ActiveMode { get; set; }
        public string? ActiveCategory { get; set; }
    }

    public class AnalyticsViewModel
    {
        public List<LearnedItem> LearnedItems { get; set; } = new List<LearnedItem>();
        public int TotalScans { get; set; }
        public int TotalLearned { get; set; }
        public int Remaining { get; set; }
        public Dictionary<string, int> CategoryCounts { get; set; } = new Dictionary<string, int>();
    }

    public class LearnedItem
    {
        public string CardName { get; set; } = string.Empty;
        public string Category { get; set; } = string.Empty;
        public string UID { get; set; } = string.Empty;
        public int TrackNumber { get; set; }
        public int ScanCount { get; set; }
        public bool IsLearned { get; set; }
    }

    }
}


