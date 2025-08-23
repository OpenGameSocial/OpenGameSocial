using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Mvc.ApplicationParts;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Scalar.AspNetCore;

// ReSharper disable once CheckNamespace
namespace OpenGameSocial.Vendor;

public static class VendorStartup<TProgram>
{
    public static WebApplicationBuilder Create(string[] args)
    {
        var builder = WebApplication.CreateBuilder(args);

        Configure(builder.Configuration);
        ConfigureServices(builder.Services);

        return builder;
    }

    public static WebApplication Build(WebApplicationBuilder builder)
    {
        var app = builder.Build();

        if (app.Environment.IsDevelopment())
        {
            app.MapOpenApi();
            app.MapScalarApiReference(options =>
            {
                options.WithDefaultHttpClient(ScalarTarget.C, ScalarClient.Libcurl);
            });
        }

        app.UseHttpsRedirection();

        app.UseAuthorization();

        app.MapControllers();

        return app;
    }

    public static async Task Run(WebApplication app)
    {
        await app.RunAsync();
    }

    private static IConfiguration Configure(IConfiguration configuration)
    {
        return configuration;
    }

    private static IServiceCollection ConfigureServices(IServiceCollection services)
    {
        services.AddControllers()
            .PartManager.ApplicationParts.Add(new AssemblyPart(typeof(TProgram).Assembly));

        services.AddOpenApi();

        return services;
    }
}