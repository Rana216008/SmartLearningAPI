using Microsoft.EntityFrameworkCore;

public class AppDbContext : DbContext
{
    public AppDbContext(DbContextOptions<AppDbContext> options) : base(options) { }

    public DbSet<Card> Cards { get; set; }
    public DbSet<UserProgress> Progress { get; set; }
    public DbSet<Session> Sessions { get; set; }
    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        base.OnModelCreating(modelBuilder);

        modelBuilder.Entity<Card>().HasData(
            new Card { Id = 1, UID = "47 27 DB A2", Name = "أ", Category = "Arabic", TrackNumber = 1 },
            new Card { Id = 2, UID = "A8 5F 7C A2", Name = "ب", Category = "Arabic", TrackNumber = 2 },
            new Card { Id = 3, UID = "B8 30 24 A2", Name = "ت", Category = "Arabic", TrackNumber = 3 },
            new Card { Id = 4, UID = "58 5 A5 A2", Name = "ث", Category = "Arabic", TrackNumber = 4 }
        );

        modelBuilder.Entity<Session>().HasData(
            new Session { Id = 1, Mode = "learning", CurrentExpectedUID = null }
        );
    }
}