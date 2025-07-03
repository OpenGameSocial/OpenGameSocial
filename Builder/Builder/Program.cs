using Builder.Modules;
using Builder.Options;
using Builder.ProjectGenerators;
using CommandLine;

namespace Builder;

internal static class Program
{
    public static void Main(string[] args)
    {
        var parser = new Parser(settings =>
        {
            settings.IgnoreUnknownArguments = true;
            settings.HelpWriter = Console.Error;
        });

        parser.ParseArguments<GenerateProjectsVerb, BuildVerb>(args)
            .WithParsed(PerformVerb);
    }

    private static void PerformVerb(object options)
    {
        var parsedArgs = (Options.CommandLine)options;
        Options.CommandLine.Instance = parsedArgs;

        if (!Directory.Exists(PathConstants.IntermediateDir))
        {
            Directory.CreateDirectory(PathConstants.IntermediateDir);
        }

        var buildProject = new BuildProject.BuildProject();
        buildProject.LoadProject();

        var project = BuildProject.BuildProject.Project;
        ModulesRegistry.Instance.RegisterProject(project);

        switch (options)
        {
            case GenerateProjectsVerb generateProjects:
                ProjectGeneratorsRegistry.Instance.GenerateProjects(generateProjects);
                break;

            case BuildVerb build:
                BuildVerb.Instance = build;
                Build.Builder.Build();
                break;
        }
    }
}