using System.Collections.Immutable;
using Builder.API.Target;

namespace Builder.API;

public interface ITarget
{
    string Name { get; }

    TargetType Type { get; }

    string SourcesDir { get; }

    string BaseDir { get; }

    List<string> IncludeDirs { get; }

    void Setup();

    ImmutableHashSet<string> GetCompilationUnits(string currentDirectory);

    ImmutableHashSet<string> GetAdditionalIncludeDirs(string currentDirectory);

    ImmutableHashSet<Type> GetDependencies();
}