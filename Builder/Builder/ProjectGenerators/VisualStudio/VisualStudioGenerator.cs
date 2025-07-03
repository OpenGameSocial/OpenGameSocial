using Builder.API;
using Builder.Modules;
using Builder.Platforms;
using Builder.Platforms.Windows;
using Microsoft.Build.Construction;

namespace Builder.ProjectGenerators.VisualStudio;

public class VisualStudioGenerator : IProjectGenerator
{
    public string Name => "vs";

    private readonly Dictionary<Guid, ProjectCfg> _projects = new();

    public VisualStudioGenerator()
    {
        VisualStudioEnvironment.EnsureInitialized("x64");
    }

    public void Generate()
    {
        AddProject(new ProjectCfg
        {
            Name = "Build",
            Path = Path.GetRelativePath(PathConstants.FullRoot, PathConstants.BuildProj),
            IsBuildProject = true,
            Buildable = true
        });

        var buildProject = BuildProject.BuildProject.Project;
        var configurations = ConfigurationsConstants.Configurations;
        var platforms = PlatformsRegistry.Instance.GetPlatforms().ToArray();

        foreach (var target in buildProject.GetTargets())
        {
            AddProject(GenerateProject(target, configurations, platforms));
        }

        var slnPath = Path.Combine(PathConstants.FullRoot, $"{buildProject.Name}.sln");
        SolutionGenerator.GenerateSln(slnPath, _projects, ConfigurationsConstants.Configurations, platforms);
    }

    private ProjectCfg GenerateProject(ITarget target, string[] configurations, string[] platforms)
    {
        target.Setup();

        var path = Path.Combine(PathConstants.ProjectsDir, $"{target.Name}.vcxproj");
        var projectDir = Path.GetDirectoryName(path)!;

        var resultConfig = new ProjectCfg
        {
            Name = target.Name,
            Path = Path.GetRelativePath(PathConstants.FullRoot, path),
            Buildable = true
        };

        var project = ProjectRootElement.Create();

        project.DefaultTargets = "Build";

        var projectConfigurations = project.AddItemGroup();
        projectConfigurations.Label = "ProjectConfigurations";

        foreach (var configuration in configurations)
        {
            foreach (var platform in platforms)
            {
                var projectConfiguration =
                    projectConfigurations.AddItem("ProjectConfiguration", $"{configuration}|{platform}");
                projectConfiguration.AddMetadata("Configuration", configuration);
                projectConfiguration.AddMetadata("Platform", platform);
            }
        }

        var globals = project.AddPropertyGroup();
        globals.Label = "Globals";
        globals.AddProperty("ProjectGuid", $"{{{resultConfig.Guid.ToString().ToUpper()}}}");
        globals.AddProperty("Keyword", "MakeFileProj");
        globals.AddProperty("MinimumVisualStudioVersion", VisualStudioProjectConstants.MinimumVisualStudioVersion);
        globals.AddProperty("VCProjectVersion", VisualStudioProjectConstants.ProjectVersion);
        globals.AddProperty("TargetRuntime", "Native");
        globals.AddProperty("PlatformToolset", VisualStudioProjectConstants.PlatformToolset);

        var builderPath = Path.GetFullPath(Path.Combine(PathConstants.FullRoot,
            "../", "Builder", "Builder", "Builder.csproj"));

        globals.AddProperty("NMakeBuildCommandLine",
            $"dotnet run --project {builderPath} -- build -p $(Platform) -c $(Configuration) -r {PathConstants.FullRoot}");
        globals.AddProperty("NMakeReBuildCommandLine",
            $"dotnet run --project {builderPath} -- rebuild -p $(Platform) -c $(Configuration) -r {PathConstants.FullRoot}");
        globals.AddProperty("NMakeCleanCommandLine",
            $"dotnet run --project {builderPath} -- clean -p $(Platform) -c $(Configuration) -r {PathConstants.FullRoot}");

        globals.AddProperty("LocalDebuggerCommand",
            $"{PathConstants.BinDir}/{target.Name}_$(Configuration)$(Platform).exe");
        globals.AddProperty("LocalDebuggerWorkingDirectory", PathConstants.BinDir);

        var configurationGroup = project.AddPropertyGroup();
        configurationGroup.Label = "Configuration";
        configurationGroup.AddProperty("ConfigurationType", "Makefile");
        configurationGroup.AddProperty("PlatformToolset", "v143");

        var nugetCrutch = project.AddPropertyGroup();
        nugetCrutch.AddProperty("BaseOutputPath", "$(MSBuildProjectDirectory)/out/$(MSBuildProjectName)/bin");
        nugetCrutch.AddProperty("BaseIntermediateOutputPath",
            "$(MSBuildProjectDirectory)/out/$(MSBuildProjectName)/obj");

        foreach (var dependency in target.Dependencies)
        {
            AddModule(project, projectDir, dependency);
        }

        project.AddItem("None", Path.GetRelativePath(projectDir,
            Path.Combine(PathConstants.FullRoot, "README.MD")));

        project.AddImport("$(VCTargetsPath)\\Microsoft.Cpp.Default.props");
        project.AddImport("$(VCTargetsPath)\\Microsoft.Cpp.props");
        project.AddImport("$(VCTargetsPath)\\Microsoft.Cpp.targets");

        project.Save(path);

        return resultConfig;
    }

    private void AddModule(ProjectRootElement project, string projectDir, Type moduleType)
    {
        if (!ModulesRegistry.Instance.TryGetModule(moduleType, out var info))
        {
            Console.WriteLine($"Module {moduleType} is not found!");
            return;
        }

        if (!info.Module.HasSetup)
        {
            info.Module.Setup();
        }

        var baseDir = info.BaseDir;
        var itemGroup = project.AddItemGroup();

        foreach (var file in Directory.EnumerateFiles(baseDir, "*", SearchOption.AllDirectories))
        {
            var relPath = Path.GetRelativePath(projectDir, file);

            if (file.EndsWith(".cpp") || file.EndsWith(".c"))
            {
                itemGroup.AddItem("ClCompile", relPath);
                continue;
            }

            if (file.EndsWith(".h") || file.EndsWith(".hpp"))
            {
                itemGroup.AddItem("ClInclude", relPath);
                continue;
            }

            itemGroup.AddItem("None", relPath);
        }

        foreach (var dependency in info.Module.Dependencies)
        {
            AddModule(project, projectDir, dependency);
        }
    }

    private void AddProject(ProjectCfg project)
        => _projects.Add(project.Guid, project);
}