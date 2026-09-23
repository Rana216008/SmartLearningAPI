using Microsoft.EntityFrameworkCore;
using SmartLearningAPI;
using SmartLearningAPI.Services;

namespace SmartLearningAPI
{
    public class Program
    {
        public static void Main(string[] args)
        {
            var builder = WebApplication.CreateBuilder(args);

            // Configure Kestrel to listen on all IPv4 interfaces
            builder.WebHost.ConfigureKestrel(serverOptions =>
            {
                serverOptions.ListenAnyIP(5000);
            });

            // Database
            builder.Services.AddDbContext<AppDbContext>(options =>
                options.UseSqlServer(
                    builder.Configuration.GetConnectionString("DefaultConnection")
                ));

            // MVC
            builder.Services.AddControllersWithViews();

            // Session
            builder.Services.AddDistributedMemoryCache();
            builder.Services.AddSession();

            // Custom Services Registration
            builder.Services.AddScoped<LearningService>();

            var app = builder.Build();

            app.UseSession();

            if (!app.Environment.IsDevelopment())
            {
                app.UseExceptionHandler("/Home/Error");
                app.UseHsts();
            }

            // app.UseHttpsRedirection();
            app.UseStaticFiles();
            app.UseRouting();
            app.UseAuthorization();

            app.MapControllerRoute(
                name: "default",
                pattern: "{controller=Auth}/{action=Login}/{id?}");

            app.MapControllers();

            app.Run();
        }
    }
}