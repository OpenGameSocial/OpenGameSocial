namespace OpenGameSocial.GameApi.Controllers.Authenticate.Models;

public class AuthenticateRequest
{
    public required string Provider { get; init; }
    public required string Data { get; init; }
}

public class AuthenticateResponse
{
    public required string Token { get; init; }
    public required DateTime ValidUntil { get; init; }
}