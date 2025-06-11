using System.Collections.Immutable;

namespace Builder.API;

public interface ILinker
{
    bool Link(ITarget target, IImmutableSet<string> objects);
}