using Microsoft.Extensions.Hosting;
using OpenGameSocial.Core.DependencyInjection;

namespace OpenGameSocial;

public static class DependencyInjectionExtensions
{
    public static IHostApplicationBuilder AddOpenGameSocial(this IHostApplicationBuilder builder)
    {
        return builder.RegisterServices();
    }
}