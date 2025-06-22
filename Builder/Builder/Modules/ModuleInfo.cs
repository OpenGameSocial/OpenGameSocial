using System.Collections.Immutable;
using Builder.API;

namespace Builder.Modules;

public class ModuleInfo
{
    public readonly IModule Module;
    public readonly string BaseDir;
    
    public bool IsBuilt { get; private set; }
    public IImmutableSet<string> Objects { get; private set; } = ImmutableHashSet<string>.Empty;

    public ModuleInfo(IModule module, string baseDir)
    {
        Module = module;
        BaseDir = baseDir;
    }

    public void MarkBuilt(IImmutableSet<string> objects)
    {
        IsBuilt = true;
        Objects = objects;
    }
}