using Microsoft.AspNetCore.Mvc;

[ApiController]
[Route("api/[controller]")]
public class ScanController : ControllerBase
{
    private readonly LearningService _service;

    public ScanController(LearningService service)
    {
        _service = service;
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
}