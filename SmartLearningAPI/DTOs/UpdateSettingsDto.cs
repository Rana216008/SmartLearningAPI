namespace SmartLearningAPI.DTOs
{
    public class UpdateSettingsDto
    {
        public string Mode { get; set; }     // "Learning" or "Exam"
        public string Category { get; set; } // "Arabic", "English", "Colors", "All"
    }
}