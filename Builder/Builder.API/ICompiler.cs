using System.Collections.Immutable;

namespace Builder.API;

public interface ICompiler
{
    IImmutableSet<string> Compile(string baseDir, IModule module, IImmutableSet<string> compilationUnits);
}