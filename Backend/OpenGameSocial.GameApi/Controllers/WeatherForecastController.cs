using Microsoft.AspNetCore.Mvc;

namespace OpenGameSocial.GameApi.Controllers;

[ApiController]
[Route("[controller]")]
public class WeatherForecastController : ControllerBase
{
    private static readonly string?[] Summaries = new[]
    {
        "Freezing", "Bracing", "Chilly", "Cool", "Mild", "Warm", "Balmy", "Hot", "Sweltering", "Scorching", null
    };

    private readonly ILogger<WeatherForecastController> _logger;

    public class WeatherForecastResult
    {
        public WeatherForecast[] Data { get; init; }
    }

    public WeatherForecastController(ILogger<WeatherForecastController> logger)
    {
        _logger = logger;
    }

    [HttpPost(Name = "GetWeatherForecast")]
    public WeatherForecastResult Post([FromBody] WeatherRequestModel model)
    {
        return new()
        {
            Data = Enumerable.Range(1, model.Count).Select(index => new WeatherForecast
                {
                    Date = DateOnly.FromDateTime(DateTime.Now.AddDays(index)),
                    TemperatureC = Random.Shared.Next(-20, 55),
                    Summary = Summaries[Random.Shared.Next(Summaries.Length)]
                })
                .ToArray()
        };
    }
}