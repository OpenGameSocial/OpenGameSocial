using System.Reflection;
using Microsoft.Extensions.DependencyInjection;

namespace OpenGameSocial.Core.Dispatching;

public static class DiExtensions
{
    public static IServiceCollection AddDispatching(this IServiceCollection services)
    {
        return services.AddSingleton<IDispatcher, Dispatcher>();
    }

    public static IServiceCollection RegisterDispatchingFromAssembly(this IServiceCollection services,
        Assembly assembly)
    {
        return services.RegisterOps<ICommand<int>>(assembly)
            .RegisterOps<IEvent<int>>(assembly)
            .RegisterOps<IQuery<int, int>>(assembly);
    }

    private static IServiceCollection RegisterOps<TOp>(this IServiceCollection services, Assembly assembly)
    {
        var types = assembly.GetTypes()
            .Where(FilterOp<TOp>);

        foreach (var type in types)
        {
            var iface = GetOpInterface<TOp>(type);
            services.AddTransient(iface, type);
        }

        return services;
    }

    private static Type GetOpInterface<TOp>(this Type type)
    {
        var op = typeof(TOp).GetGenericTypeDefinition();

        return type.GetInterfaces().First(x => x.GetGenericTypeDefinition() == op);
    }

    private static bool FilterOp<TOp>(Type type)
    {
        var op = typeof(TOp).GetGenericTypeDefinition();

        return type is { IsClass: true, IsAbstract: false } &&
               type.GetInterfaces().Any(x => x.IsGenericType &&
                                             x.GetGenericTypeDefinition() == op);
    }
}