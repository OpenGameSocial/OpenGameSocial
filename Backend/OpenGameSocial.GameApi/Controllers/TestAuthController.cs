using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using OpenGameSocial.GameApi.Signal;

namespace OpenGameSocial.GameApi.Controllers;

[ApiController]
[Route("[controller]")]
public class TestAuthController : ControllerBase
{
    private readonly TestHub _hub;

    public TestAuthController(TestHub hub)
    {
        _hub = hub;
    }

    public class Payload
    {
        public string Message { get; init; }
    }

    [HttpGet]
    [Authorize]
    public async Task<IActionResult> GetSecret([FromBody] Payload payload)
    {
        var username = User.Identity?.Name;

        await _hub.SendMessage(username!, payload.Message);

        return Ok();
    }
}