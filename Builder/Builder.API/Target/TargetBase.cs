using System.Collections.Immutable;
using Builder.API.Module;

namespace Builder.API.Target;

public abstract class TargetBase<T> : ITarget
    where T : TargetBase<T>
{
    public string Name => typeof(T).Name;

    public abstract TargetType Type { get; }

    public ImmutableList<Type> Dependencies { get; private set; } = ImmutableList<Type>.Empty;

    public virtual void Setup()
    {
    }
    
    protected void AddDependency<TDependency>() where TDependency : ModuleBase<TDependency>
    {
        Dependencies = Dependencies.Add(typeof(TDependency));
    }
}