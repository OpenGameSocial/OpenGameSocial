using Builder.API.Target;

namespace Build.Targets;

public class IntegrationTestingTool : TargetBase<IntegrationTestingTool>
{
    public override TargetType Type => TargetType.Executable;

    public override string SourcesDir => "Testing";
    public override string BaseDir { get; } = GetCurrentBaseDirectory();
    
    public override void Setup()
    {
        IncludeDirs.Add("Include");
    }
}