using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.DependencyInjection;
using System.Linq;

[ApiController]
[Route("api/[controller]")]
public class ScanController : ControllerBase
{
    private readonly LearningService _service;
    private readonly IServiceScopeFactory _scopeFactory;

    public ScanController(LearningService service, IServiceScopeFactory scopeFactory)
    {
        _service = service;
        _scopeFactory = scopeFactory;
    }

    [HttpPost]
    public IActionResult Scan([FromBody] ScanRequest request)
    {
        System.Diagnostics.Debug.WriteLine("=== Connection Attempt from ESP32 ===");
        Console.WriteLine("UID Received: " + request.UID);
        Console.WriteLine("UID Received: " + request.UID);

        var result = _service.HandleScan(request.UID);

        return Ok(result);
    }

    [HttpGet("api/settings")]
    public IActionResult GetSettings()
    {
        using (var scope = _scopeFactory.CreateScope())
        {
            var db = scope.ServiceProvider.GetRequiredService<AppDbContext>();
            var settings = db.AppSettings.FirstOrDefault();
            return Ok(new
            {
                mode = settings?.CurrentMode ?? "Learn",
                category = settings?.CurrentCategory ?? "Arabic"
            });
        }
    }
}