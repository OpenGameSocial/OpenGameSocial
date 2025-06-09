using Builder.Options;

namespace Builder.ProjectGenerators;

public class ProjectGeneratorsRegistry
{
    public static readonly ProjectGeneratorsRegistry Instance = new();

    private readonly Dictionary<string, IProjectGenerator> _generators;

    private ProjectGeneratorsRegistry()
    {
        _generators = GetType().Assembly.GetTypes()
            .Where(x => x.GetInterfaces().Any(i => i == typeof(IProjectGenerator)))
            .Select(x => Activator.CreateInstance(x) as IProjectGenerator)
            .Where(x => x is not null)
            .Select(x => x!)
            .ToDictionary(x => x.Name);
    }

    public void GenerateProjects(GenerateProjectsVerb options)
    {
        if (!_generators.TryGetValue(options.TargetIde, out var generator))
        {
            Console.WriteLine("ERROR: Generator not found");
            return;
        }

        generator.Generate();
    }
}