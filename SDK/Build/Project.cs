using System.Collections.Immutable;
using Build.Targets;
using Builder.API;

namespace Build;

public class Project : IProject
{
    public IImmutableList<ITarget> GetTargets()
    {
        return [new IntegrationTestingTool(), new OpenGameSocial()];
    }
}