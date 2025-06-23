using Builder.API.Module;

namespace Build;

public class IntegrationTestingToolModule : ModuleBase<IntegrationTestingToolModule>
{
    protected override void SetupModule()
    {
        AddDependency<TestModule>();
    }
}