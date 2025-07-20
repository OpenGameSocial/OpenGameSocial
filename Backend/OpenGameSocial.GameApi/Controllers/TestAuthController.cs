using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;

namespace OpenGameSocial.GameApi.Controllers;

[ApiController]
[Route("[controller]")]
public class TestAuthController : ControllerBase
{
    public class Result
    {
        public string Message { get; init; }
    }

    [HttpGet]
    [Authorize]
    public Result GetSecret()
    {
        var username = User.Identity?.Name;
        return new Result
        {
            Message = $"Hello {username}!"
        };
    }
}