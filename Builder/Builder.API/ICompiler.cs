using System.Collections.Immutable;

namespace Builder.API;

public interface ICompiler
{
    IImmutableSet<string> Compile(ITarget target, IImmutableSet<string> compilationUnits);
}