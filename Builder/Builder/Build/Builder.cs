using System.Collections.Immutable;
using System.Diagnostics;
using Builder.API;
using Builder.Modules;
using Builder.Platforms;

namespace Builder.Build;

public class Builder
{
    public static void Build()
    {
        var project = BuildProject.BuildProject.Project;

        if (!PlatformsRegistry.Instance.TryGetPlatform(Options.CommandLine.Instance.Platform, out var platform))
        {
            Console.WriteLine("ERROR: Platform not found");
            return;
        }

        var sw = Stopwatch.StartNew();

        foreach (var target in project.GetTargets())
        {
            sw.Restart();

            Console.WriteLine($"Building target {target.Name}");

            target.Setup();

            foreach (var targetDependency in target.Dependencies)
            {
                var objects = ImmutableHashSet<string>.Empty;

                if (ModulesRegistry.Instance.TryGetModule(targetDependency, out var info))
                {
                    var compiledObjects = BuildModule(info, platform);

                    if (compiledObjects is null)
                    {
                        Console.WriteLine($"Failed to build target {target.Name}");
                        return;
                    }

                    objects = objects.Union(compiledObjects);
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

    private static IImmutableSet<string>? BuildModule(ModuleInfo info, IPlatform platform)
    {
        var objects = info.Objects;

        var module = info.Module;
        var baseDir = info.BaseDir;

        if (!info.IsBuilt)
        {
            module.Setup();
        }

        foreach (var moduleDependency in module.Dependencies)
        {
            if (ModulesRegistry.Instance.TryGetModule(moduleDependency, out var dependencyInfo))
            {
                var buildResult = BuildModule(dependencyInfo, platform);

                if (buildResult is null)
                {
                    Console.WriteLine($"Failed to build module {module.Name}");
                    return null;
                }

                objects = objects.Union(buildResult);
            }
        }

        if (info.IsBuilt)
        {
            return objects;
        }

        var privatePath = Path.Combine(baseDir, "Private");
        var publicPath = Path.Combine(baseDir, "Public");
        var compilationUnits = module.GetCompilationUnits(privatePath);
        compilationUnits = compilationUnits.Union(module.GetCompilationUnits(publicPath));

        if (compilationUnits.IsEmpty)
        {
            Console.WriteLine("Nothing to build.");
            return ImmutableHashSet<string>.Empty;
        }

        var builtObjects = platform.GetCompiler().Compile(baseDir, module, compilationUnits);
        objects = objects.Union(builtObjects);

        info.MarkBuilt(objects);

        if (builtObjects.Count != compilationUnits.Count)
        {
            Console.WriteLine($"Failed to build module {module.Name}");
            return null;
        }

        return objects;
    }
}