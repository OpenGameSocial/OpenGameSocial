using System.Diagnostics.CodeAnalysis;
using System.Reflection;
using Builder.API;

namespace Builder.Platforms;

public class PlatformsRegistry
{
    private readonly Dictionary<string, IPlatform> _platforms = new();

    public static PlatformsRegistry Instance = new();

    private PlatformsRegistry()
    {
        LoadFrom(GetType().Assembly);
    }

    public void LoadFrom(Assembly assembly)
    {
        var platforms = assembly.GetTypes().Where(x => x.GetInterfaces().Any(i => i == typeof(IPlatform)))
            .Select(x => Activator.CreateInstance(x) as IPlatform)
            .Where(x => x is not null)
            .Select(x => x!);

        foreach (var platform in platforms)
        {
            _platforms.Add(platform.Name, platform);
        }
    }
    
    public IReadOnlyCollection<string> GetPlatforms() => _platforms.Keys;

    public bool TryGetPlatform(string instancePlatform, [MaybeNullWhen(false)] out IPlatform platform)
        => _platforms.TryGetValue(instancePlatform, out platform);
}