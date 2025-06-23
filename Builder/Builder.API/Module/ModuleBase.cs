using System.Collections.Immutable;
using System.Diagnostics;

namespace Builder.API.Module;

public abstract class ModuleBase<T> : IModule
    where T : ModuleBase<T>
{
    private static readonly string[] CompilationUnitsPatterns = ["*.c", "*.cpp"];

    public string Name { get; }

    public List<string> IncludeDirs { get; } = new();

    public ImmutableList<Type> Dependencies { get; private set; } = ImmutableList<Type>.Empty;

    public bool HasSetup { get; private set; }

    public ModuleBase()
    {
        Name = typeof(T).Name;
    }

    public void Setup()
    {
        if (HasSetup)
        {
            return;
        }

        SetupModule();
        HasSetup = true;
    }

    protected abstract void SetupModule();

    public virtual ImmutableHashSet<string> GetCompilationUnits(string currentDirectory)
    {
        if (!Directory.Exists(currentDirectory))
        {
            return [];
        }

        return GetFilesWithPattern(currentDirectory, CompilationUnitsPatterns);
    }

    public virtual ImmutableHashSet<string> GetAdditionalIncludeDirs(string currentDirectory)
    {
        return [];
    }

    protected void AddDependency<TDependency>() where TDependency : ModuleBase<TDependency>
    {
        Dependencies = Dependencies.Add(typeof(TDependency));
    }

    private static ImmutableHashSet<string> GetFilesWithPattern(string currentDirectory, string[] patterns)
    {
        var result = new HashSet<string>();

        foreach (var pattern in patterns)
        {
            result.UnionWith(Directory.EnumerateFiles(currentDirectory, pattern, SearchOption.AllDirectories)
                .Select(Path.GetFullPath));
        }

        return result.ToImmutableHashSet();
    }
}