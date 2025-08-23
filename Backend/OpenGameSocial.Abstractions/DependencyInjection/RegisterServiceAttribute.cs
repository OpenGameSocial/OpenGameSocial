using Microsoft.Extensions.DependencyInjection;

namespace OpenGameSocial.Abstractions.DependencyInjection;

[AttributeUsage(AttributeTargets.Class)]
public class RegisterServiceAttribute : Attribute
{
    public readonly ServiceLifetime Lifetime;
    public readonly Type[] Interfaces;
    public readonly bool OverrideInterfaces;

    public RegisterServiceAttribute(ServiceLifetime lifetime, bool overrideInterfaces = false, params Type[] interfaces)
    {
        Lifetime = lifetime;
        OverrideInterfaces = overrideInterfaces;
        Interfaces = interfaces;
    }

    public RegisterServiceAttribute(bool overrideInterfaces = false, params Type[] interfaces)
    {
        Lifetime = ServiceLifetime.Singleton;
        OverrideInterfaces = overrideInterfaces;
        Interfaces = interfaces;
    }
}