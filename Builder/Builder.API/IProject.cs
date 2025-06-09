using System.Collections.Immutable;

namespace Builder.API;

public interface IProject
{
    IImmutableList<ITarget> GetTargets();
}