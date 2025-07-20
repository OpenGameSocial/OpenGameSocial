using Microsoft.AspNetCore.Mvc;
using OpenGameSocial.GameApi.Controllers.Authenticate.Models;

namespace OpenGameSocial.GameApi.Controllers.Authenticate;

[ApiController]
[Route("Authenticate")]
public class Controller : ControllerBase
{
    [HttpPost]
    public AuthenticateResponse Post([FromBody] AuthenticateRequest request)
    {
        return new AuthenticateResponse
        {
            Token = "test-token",
            ValidUntil = DateTime.Now.AddMinutes(5)
        };
    }
}