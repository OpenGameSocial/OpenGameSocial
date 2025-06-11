using System.Diagnostics;
using Builder.API;
using Builder.Platforms.Windows.X64;

namespace Builder.ProjectGenerators;

public class VisualStudioGenerator : IProjectGenerator
{
    public string Name => "vs";

    public void Generate()
    {
        var project = BuildProject.BuildProject.Project;

        var platform = new PlatformWin64();
        var sw = Stopwatch.StartNew();

        foreach (var target in project.GetTargets())
        {
            sw.Restart();

            Console.WriteLine($"Generating project {target.Name}");

            target.Setup();

            var sourceRoot = Path.Combine(target.BaseDir, target.SourcesDir);
            var compilationUnits = target.GetCompilationUnits(sourceRoot);

            if (compilationUnits.IsEmpty)
            {
                Console.WriteLine("Nothing to build.");
                continue;
            }

            var objects = platform.GetCompiler().Compile(target, compilationUnits);

            if (objects.Count != compilationUnits.Count)
            {
                Console.WriteLine($"Failed to build target {target.Name}");
                continue;
            }

            if (!platform.GetLinker().Link(target, objects))
            {
                Console.WriteLine($"Failed to link target {target.Name}");
                continue;
            }
            
            sw.Stop();

            Console.WriteLine($"{target.Name} build took {sw.ElapsedMilliseconds}ms");
        }
    }
}