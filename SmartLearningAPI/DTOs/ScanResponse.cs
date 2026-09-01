public class ScanResponse
{
    public string Action { get; set; } = string.Empty;
    public int Track { get; set; }
    public string Message { get; set; } = string.Empty;
    public string ImageName { get; set; } = string.Empty;
    public string Mode { get; set; } = string.Empty;
    public string Category { get; set; } = string.Empty;   // "All" or specific category
}