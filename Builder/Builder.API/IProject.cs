using System.Collections.Immutable;

namespace Builder.API;

public interface IProject
{
    string Name { get; }
    
    IImmutableList<ITarget> GetTargets();
}