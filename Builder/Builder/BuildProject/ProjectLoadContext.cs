using System.Reflection;
using System.Runtime.Loader;

namespace Builder.BuildProject;

public class ProjectLoadContext : AssemblyLoadContext
{
    protected override Assembly Load(AssemblyName assemblyName)
    {
        return null!; // fallback to default
    }

    public Assembly LoadWithSymbols(string dllPath)
    {
        var pdbPath = Path.ChangeExtension(dllPath, ".pdb");

        var assemblyBytes = File.ReadAllBytes(dllPath);
        var pdbBytes = File.Exists(pdbPath) ? File.ReadAllBytes(pdbPath) : null;

        return LoadFromStream(new MemoryStream(assemblyBytes),
            pdbBytes != null ? new MemoryStream(pdbBytes) : null);
    }
}