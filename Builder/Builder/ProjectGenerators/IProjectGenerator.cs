using Builder.API;

namespace Builder.ProjectGenerators;

public interface IProjectGenerator
{
    string Name { get; }

    void Generate();
}