public class Session
{
    public int Id { get; set; }
    public string Mode { get; set; } // learning / quiz / final
    public string? CurrentExpectedUID { get; set; }
}