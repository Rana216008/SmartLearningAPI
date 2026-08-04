using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.DependencyInjection;
using System;
using System.Linq;
using SmartLearningAPI.Models;
using SmartLearningAPI.Services;

namespace SmartLearningAPI.Controllers
{
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

        // POST: api/scan
        [HttpPost]
        public IActionResult Scan([FromBody] ScanRequest request)
        {
            if (request == null || string.IsNullOrEmpty(request.UID))
            {
                return BadRequest(new { Message = "Invalid UID" });
            }

            Console.WriteLine($"=== Connection Attempt from ESP32 | UID: {request.UID} ===");

            var result = _service.HandleScan(request.UID);
            return Ok(result);
        }

        // GET: api/scan/settings
        [HttpGet("settings")]
        public IActionResult GetSettings()
        {
            using (var scope = _scopeFactory.CreateScope())
            {
                var db = scope.ServiceProvider.GetRequiredService<AppDbContext>();
                var settings = db.AppSettings.FirstOrDefault();
                return Ok(new
                {
                    mode = settings?.CurrentMode ?? "Learning",
                    category = settings?.CurrentCategory ?? "All"
                });
            }
        }
    }
}