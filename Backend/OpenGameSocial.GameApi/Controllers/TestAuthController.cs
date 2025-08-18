using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;

namespace OpenGameSocial.GameApi.Controllers;

[ApiController]
[Route("[controller]")]
public class TestAuthController : ControllerBase
{
    public class Payload
    {
        public string Message { get; init; }
    }

    [HttpGet]
    [Authorize]
    public async Task<IActionResult> GetSecret([FromBody] Payload payload)
    {
        var username = User.Identity?.Name;

        return Ok();
    }
}