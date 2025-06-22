using System.Collections.Immutable;
using Builder.API;
using Builder.Extensions;

namespace Builder.Platforms.Windows.X64;

public class Win64Compiler : ICompiler
{
    public Win64Compiler()
    {
        VisualStudioEnvironment.EnsureInitialized("x64");
    }

    public IImmutableSet<string> Compile(string baseDir, IModule module, IImmutableSet<string> compilationUnits)
    {
        var objectsDir = Path.Combine(PathConstants.ObjectsDir, module.Name);
        Directory.CreateDirectory(objectsDir);

        var result = new HashSet<string>();

        foreach (var unit in compilationUnits)
        {
            var objName = $"{Path.GetFileNameWithoutExtension(unit)}.obj";
            var objPath = Path.Combine(objectsDir, objName);

            Directory.CreateDirectory(Path.GetDirectoryName(objPath)!);

            var args = new List<string>
            {
                "/c", "/EHsc", "/nologo", $"/Fo{objPath}", "/MT", "/std:c++17"
            };

            args.Add($"/I{Path.Combine(baseDir, "Private")}");
            args.Add($"/I{Path.Combine(baseDir, "Public")}");
            args.AddRange(module.IncludeDirs.Select(x => $"/I{Path.GetFullPath(Path.Combine(baseDir, x))}"));

            args.Add(unit);

            var compilationResult = ProcessRunner.RunWithEnv(
                VisualStudioEnvironment.GetCompiler(),
                out var exitCode,
                VisualStudioEnvironment.Environment,
                args);

            Console.WriteLine(compilationResult);

            if (exitCode != 0)
            {
                continue;
            }

            result.Add(objPath);
        }

        return result.ToImmutableHashSet();
    }
}