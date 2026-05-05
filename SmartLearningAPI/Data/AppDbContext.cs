using Microsoft.EntityFrameworkCore;

public class AppDbContext : DbContext
{
    public AppDbContext(DbContextOptions<AppDbContext> options) : base(options) { }

    public DbSet<Card> Cards { get; set; }
    public DbSet<UserProgress> Progress { get; set; }
    public DbSet<Session> Sessions { get; set; }
    public DbSet<AppSettings> AppSettings { get; set; }
    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        base.OnModelCreating(modelBuilder);

        modelBuilder.Entity<Session>().HasData(
            new Session { Id = 1, Mode = "learning", CurrentExpectedUID = null }
        );
    }
}