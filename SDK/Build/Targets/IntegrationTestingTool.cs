using Builder.API.Target;

namespace Build.Targets;

public class IntegrationTestingTool : TargetBase<IntegrationTestingTool>
{
    public override TargetType Type => TargetType.Executable;
    
    public override void Setup()
    {
        AddDependency<IntegrationTestingToolModule>();
    }
}