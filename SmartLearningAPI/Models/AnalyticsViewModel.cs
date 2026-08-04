public class AnalyticsViewModel
{
    public List<LearnedItem> LearnedItems { get; set; } = new();
    public int TotalScans { get; set; }
    public int TotalLearned { get; set; }
    public int Remaining { get; set; }
    public Dictionary<string, int> CategoryCounts { get; set; } = new();
}

public class LearnedItem
{
    public string CardName { get; set; }
    public string Category { get; set; }
    public string UID { get; set; }
    public int TrackNumber { get; set; }
    public int ScanCount { get; set; }
    public bool IsLearned { get; set; }
}