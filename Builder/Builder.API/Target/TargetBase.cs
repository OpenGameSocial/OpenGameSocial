using System.Collections.Immutable;
using System.Runtime.CompilerServices;

namespace Builder.API.Target;

public abstract class TargetBase<T> : ITarget
{
    private static readonly string[] CompilationUnitsPatterns = ["*.c", "*.cpp"];

    public string Name => typeof(T).Name;

    public abstract TargetType Type { get; }

    public abstract string SourcesDir { get; }
    public abstract string BaseDir { get; }

    public List<string> IncludeDirs { get; } = new();

    public virtual void Setup()
    {
    }

    public virtual ImmutableHashSet<string> GetCompilationUnits(string currentDirectory)
    {
        return GetFilesWithPattern(currentDirectory, CompilationUnitsPatterns);
    }
    
    public virtual ImmutableHashSet<string> GetAdditionalIncludeDirs(string currentDirectory)
    {
        return [];
    }

    public virtual ImmutableHashSet<Type> GetDependencies()
    {
        return [];
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

    protected static string GetCurrentBaseDirectory([CallerFilePath] string path = null!)
    {
        var raw = path.Split(Path.DirectorySeparatorChar);
        var rootIndex = -1;

        for (var i = raw.Length - 1; i >= 0; i--)
        {
            if (raw[i] != "Build")
            {
                continue;
            }

            rootIndex = i;
            break;
        }

        return string.Join(Path.DirectorySeparatorChar, raw.Take(rootIndex));
    }
}