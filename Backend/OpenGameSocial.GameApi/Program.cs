using Microsoft.AspNetCore.Authentication;
using MongoDB.Driver;
using OpenGameSocial.Core.Config;
using Scalar.AspNetCore;

namespace OpenGameSocial.GameApi;

internal static class Program
{
    public static void Main(string[] args)
    {
        var builder = WebApplication.CreateBuilder(args);

        builder.Services.AddControllers();
        builder.Services.AddOpenApi();
        
        builder.Services.AddAuthentication("TestScheme")
            .AddScheme<AuthenticationSchemeOptions, CustomTokenAuthenticationHandler>("TestScheme", null);

        builder.Services
            .AddOpenGameSocial(typeof(Program).Assembly)
            .AddLogging()
            .AddOptions();

        var configuration = builder.Configuration
            .AddJsonFile("appsettings.json")
            .AddJsonFile("appsettings.Development.json", optional: true)
            .Build();

        var mongoSection = configuration.GetSection("MongoConfig");

        builder.Services.AddSingleton<IMongoClient>(
            new MongoClient(mongoSection[nameof(MongoConfig.ConnectionString)]));

        builder.Services.Configure<MongoConfig>(mongoSection);
        builder.Services.Configure<IdentityProvidersConfig>(configuration.GetSection("IdentityProviders"));

        var app = builder.Build();

        if (app.Environment.IsDevelopment())
        {
            app.MapOpenApi();
            app.MapScalarApiReference(options =>
            {
                options.WithDefaultHttpClient(ScalarTarget.C, ScalarClient.Libcurl);
            });
        }

        // app.UseHttpsRedirection();

        app.UseAuthorization();

        app.MapControllers();

        app.Run();
    }
}