using Builder.API.Module;

namespace Build;

public class IntegrationTestingToolModule : ModuleBase<IntegrationTestingToolModule>
{
    public override void Setup()
    {
        AddDependency<TestModule>();
    }
}