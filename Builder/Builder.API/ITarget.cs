using System.Collections.Immutable;
using System.Runtime.CompilerServices;
using Builder.API.Target;

namespace Builder.API;

public interface ITarget
{
    string Name { get; }

    TargetType Type { get; }

    ImmutableList<Type> Dependencies { get; }

    void Setup();
}