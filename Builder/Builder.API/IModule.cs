using System.Collections.Immutable;
using Builder.API.Target;

namespace Builder.API;

public interface IModule
{
    string Name { get; }

    List<string> IncludeDirs { get; }

    ImmutableList<Type> Dependencies { get; }
    
    bool HasSetup { get; }

    void Setup();

    ImmutableHashSet<string> GetCompilationUnits(string currentDirectory);

    ImmutableHashSet<string> GetAdditionalIncludeDirs(string currentDirectory);
}