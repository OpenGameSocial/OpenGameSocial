namespace Builder.ProjectGenerators.VisualStudio;

public class ProjectCfg
{
    public readonly Guid Guid = Guid.NewGuid();
    public required string Name { get; init; }
    public required string Path { get; init; }
    
    public bool IsBuildProject = false;

    public bool Buildable = false;
}