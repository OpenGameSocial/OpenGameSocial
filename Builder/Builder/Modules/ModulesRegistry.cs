using System.Diagnostics.CodeAnalysis;
using System.Reflection;
using System.Reflection.Metadata;
using System.Reflection.PortableExecutable;
using Builder.API;

namespace Builder.Modules;

public class ModulesRegistry
{
    public static readonly ModulesRegistry Instance = new();

    private readonly Dictionary<Type, ModuleInfo> _modules = new();

    public void RegisterProject(IProject project)
    {
        var assembly = project.GetType().Assembly;
        var debugInfo = LoadDebugInfo(assembly);

        var modules = project.GetType().Assembly.GetTypes()
            .Where(x => x.GetInterfaces().Any(i => i == typeof(IModule)))
            .Select(x => Activator.CreateInstance(x) as IModule)
            .Where(x => x is not null)
            .Select(x => x!);

        foreach (var module in modules)
        {
            var baseDir = Path.GetDirectoryName(debugInfo[module.GetType().FullName!])!;
            _modules.Add(module.GetType(), new(module, baseDir));
        }
    }

    public bool TryGetModule(Type type, [MaybeNullWhen(false)] out ModuleInfo info)
    {
        return _modules.TryGetValue(type, out info);
    }

    public static Dictionary<string, string> LoadDebugInfo(Assembly assembly)
    {
        var codeBase = assembly.Location;
        var uri = new UriBuilder(codeBase);
        var dllPath = Uri.UnescapeDataString(uri.Path);

        var result = new Dictionary<string, string>();

        var pdbPath = Path.ChangeExtension(dllPath, ".pdb");

        using var dllStream = File.OpenRead(dllPath);
        using var pdbStream = File.OpenRead(pdbPath);
        using var peReader = new PEReader(dllStream);

        if (!peReader.HasMetadata)
            throw new InvalidOperationException("Assembly has no metadata");

        var mdReader = peReader.GetMetadataReader();
        var pdbReader = MetadataReaderProvider.FromPortablePdbStream(pdbStream).GetMetadataReader();

        foreach (var typeDefHandle in mdReader.TypeDefinitions)
        {
            var typeDef = mdReader.GetTypeDefinition(typeDefHandle);
            var ns = mdReader.GetString(typeDef.Namespace);
            var name = mdReader.GetString(typeDef.Name);
            var fullName = string.IsNullOrEmpty(ns) ? name : $"{ns}.{name}";

            foreach (var methodHandle in typeDef.GetMethods())
            {
                var methodDebugInfo = pdbReader.GetMethodDebugInformation(methodHandle);

                foreach (var sp in methodDebugInfo.GetSequencePoints())
                {
                    if (sp.Document.IsNil) continue;

                    var doc = pdbReader.GetDocument(sp.Document);
                    var path = pdbReader.GetString(doc.Name);

                    result.TryAdd(fullName, path);
                }
            }
        }

        return result;
    }
}