using System.Collections.Immutable;

namespace Builder.API.Target;

public abstract class TargetBase<T> : ITarget
{
    private static readonly string[] CompilationUnitsPatterns = ["*.c", "*.cpp"];
    private static readonly string[] IncludeFilesPatterns = ["*.h", "*.hpp"];

    public string Name => typeof(T).Name;

    public abstract TargetType Type { get; }

    public abstract string SourcesDir { get; }

    public virtual ImmutableHashSet<string> GetCompilationUnits(string currentDirectory)
    {
        return GetFilesWithPattern(currentDirectory, CompilationUnitsPatterns);
    }

    public virtual ImmutableHashSet<string> GetIncludeFiles(string currentDirectory)
    {
        return GetFilesWithPattern(currentDirectory, IncludeFilesPatterns);
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
}