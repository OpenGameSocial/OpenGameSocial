using System.IO.Pipes;
using System.Reflection;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using OpenGameSocial.Abstractions.DependencyInjection;

namespace OpenGameSocial.Core.DependencyInjection;

public static class ServiceRegistrationExtensions
{
    private static readonly MethodInfo _configureMethod;

    static ServiceRegistrationExtensions()
    {
        _configureMethod = typeof(OptionsConfigurationServiceCollectionExtensions)
            .GetMethods()
            .First(m => m is
                        {
                            Name: nameof(OptionsConfigurationServiceCollectionExtensions.Configure),
                            IsGenericMethod: true
                        }
                        && m.GetParameters().Any(p => p.ParameterType == typeof(IConfiguration)));
    }
    
    public static IHostApplicationBuilder RegisterServices(this IHostApplicationBuilder builder, Assembly assembly)
    {
        var types = assembly.GetTypes()
            .Where(x =>
                x.IsClass &&
                x.GetCustomAttribute<RegisterServiceAttribute>(true) != null);
        var services = builder.Services;

        foreach (var type in types)
        {
            var attribute = type.GetCustomAttribute<RegisterServiceAttribute>(true);
            var interfaces = type.GetInterfaces();

            interfaces = attribute!.OverrideInterfaces
                ? attribute.Interfaces
                : interfaces.Union(attribute.Interfaces).ToArray();

            services.Add(ServiceDescriptor.Describe(type, type, attribute.Lifetime));

            foreach (var iface in interfaces)
                services.Add(ServiceDescriptor.Describe(iface, x => x.GetRequiredService(type),
                    attribute.Lifetime));
        }

        return builder;
    }

    public static IHostApplicationBuilder RegisterConfigurations(this IHostApplicationBuilder builder,
        Assembly assembly)
    {
        var types = assembly.GetTypes()
            .Where(x =>
                x.IsClass &&
                x.GetCustomAttribute<RegisterConfigurationAttribute>(true) != null);

        foreach (var type in types)
        {
            var attribute = type.GetCustomAttribute<RegisterConfigurationAttribute>(true);

            var section = builder.Configuration.GetSection(attribute!.Section);
            _configureMethod.MakeGenericMethod(type).Invoke(null, [builder.Services, section]);
        }

        return builder;
    }

    public static IHostApplicationBuilder RegisterServices<TType>(this IHostApplicationBuilder builder)
    {
        return RegisterServices(builder, typeof(TType).Assembly);
    }

    public static IHostApplicationBuilder RegisterServices(this IHostApplicationBuilder builder)
    {
        return RegisterServices(builder, Assembly.GetCallingAssembly());
    }
}