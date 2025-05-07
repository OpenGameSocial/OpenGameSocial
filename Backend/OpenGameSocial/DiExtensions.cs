using System.Reflection;
using Microsoft.Extensions.DependencyInjection;
using OpenGameSocial.Core.Database;
using OpenGameSocial.Core.Dispatching;
using OpenGameSocial.Services.Account;

namespace OpenGameSocial;

public static class DiExtensions
{
    public static IServiceCollection AddOpenGameSocial(this IServiceCollection services, params Assembly[] assemblies)
    {
        foreach (var assembly in assemblies)
        {
            services.RegisterDispatchingFromAssembly(assembly);
        }

        return services.AddDispatching()
            .RegisterDispatchingFromAssembly(typeof(DiExtensions).Assembly)
            .AddDefaultDatabaseProviders()
            .AddSingleton<IAccountRepository, AccountRepository>()
            .AddSingleton<ITitleAccountIdFactory, DefaultTitleAccountIdFactory>();
    }
}