using Builder.API;

namespace Builder;

public class PathConstants
{
    public static readonly string FullRoot = Path.GetFullPath(Options.CommandLine.Instance.Root);
    public static readonly string IntermediateDir = Path.Combine(Options.CommandLine.Instance.Root, "Intermediate"); 
    public static readonly string ProjectsDir = Path.Combine(IntermediateDir, "Projects"); 
    public static readonly string BuildProj = Path.Combine(ProjectsDir, "Build.csproj"); 
    
    public static readonly string BuilderApiPath = typeof(IProject).Assembly.Location;
}