using OpenGameSocial.Abstractions.DependencyInjection;

namespace OpenGameSocial.Abstractions.API;

[AttributeUsage(AttributeTargets.Class)]
public class ApiEndpointAttribute : RegisterServiceAttribute
{
}