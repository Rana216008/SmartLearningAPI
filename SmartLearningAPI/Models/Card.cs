public class Card
{
    public int Id { get; set; }

    public string Name { get; set; } = "";

    public string UID { get; set; } = "";

    public int CategoryId { get; set; }

    public Category? Category { get; set; }

    public int TrackNumber { get; set; }
}