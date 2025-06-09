using System.Diagnostics;
using Builder.Options;
using Builder.Platforms;
using Builder.ProjectGenerators;
using CommandLine;

namespace Builder;

internal static class Program
{
    public static void Main(string[] args)
    {
        Parser.Default.ParseArguments<GenerateProjectsVerb, BuildVerb>(args)
            .WithParsed(PerformVerb);
    }

    private static void PerformVerb(object options)
    {
        var parsedArgs = (Options.CommandLine)options;
        Options.CommandLine.Instance = parsedArgs;

        if (!PlatformsRegistry.Instance.TryGetPlatform(Options.CommandLine.Instance.Platform, out _))
        {
            Console.WriteLine("ERROR: Platform not found");
            return;
        }

        if (!Directory.Exists(PathConstants.IntermediateDir))
        {
            Directory.CreateDirectory(PathConstants.IntermediateDir);
        }

        var buildProject = new BuildProject.BuildProject();
        buildProject.LoadProject();

        switch (options)
        {
            case GenerateProjectsVerb generateProjects:
                ProjectGeneratorsRegistry.Instance.GenerateProjects(generateProjects);
                break;
        }
    }
}