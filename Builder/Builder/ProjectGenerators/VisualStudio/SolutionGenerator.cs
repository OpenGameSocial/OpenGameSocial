using System.Text;

namespace Builder.ProjectGenerators.VisualStudio;

static class SolutionGenerator
{
    private static readonly Dictionary<string, string> ProjectTypeGuids = new()
    {
        [".csproj"] = "{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}",
        [".vcxproj"] = "{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}"
    };

    public static void GenerateSln(string slnPath,
        Dictionary<Guid, ProjectCfg> projects,
        string[] configurations,
        string[] platforms)
    {
        var sb = new StringBuilder();

        var slnVersion = 12; // Visual Studio 2013+
        sb.AppendLine("Microsoft Visual Studio Solution File, Format Version " + slnVersion + ".00");
        sb.AppendLine("# Visual Studio Version 17");
        sb.AppendLine("VisualStudioVersion = 17.0.31903.59");
        sb.AppendLine("MinimumVisualStudioVersion = 10.0.40219.1");

        var projectGuids = new List<(Guid Guid, string Config, string Platform)>();

        foreach (var (_, project) in projects)
        {
            var ext = Path.GetExtension(project.Path).ToLowerInvariant();

            if (!ProjectTypeGuids.TryGetValue(ext, out var projectTypeGuid))
            {
                throw new InvalidOperationException($"Unknown project type: {ext}");
            }

            var guid = project.Guid;
            sb.AppendLine(
                $"Project(\"{projectTypeGuid}\") = \"{project.Name}\", \"{project.Path}\", \"{{{guid.ToString().ToUpper()}}}\"");
            sb.AppendLine("EndProject");

            projectGuids.AddRange(from configuration in configurations
                from platform in platforms
                select (guid, configuration, platform));
        }

        sb.AppendLine("Global");
        sb.AppendLine("\tGlobalSection(SolutionConfigurationPlatforms) = preSolution");

        foreach (var configuration in configurations)
        {
            foreach (var platform in platforms)
            {
                sb.AppendLine($"\t\t{configuration}|{platform} = {configuration}|{platform}");
            }
        }

        sb.AppendLine("\tEndGlobalSection");

        sb.AppendLine("\tGlobalSection(ProjectConfigurationPlatforms) = postSolution");
        foreach (var (guid, config, platform) in projectGuids)
        {
            var guidStr = guid.ToString().ToUpper();
            var realConfig = config;
            var realPlatform = platform;

            if (projects[guid].IsBuildProject)
            {
                realConfig = "Debug";
                realPlatform = "Any CPU";
            }

            sb.AppendLine($"\t\t{{{guidStr}}}.{config}|{platform}.ActiveCfg = {realConfig}|{realPlatform}");

            if (!projects[guid].Buildable)
            {
                continue;
            }

            sb.AppendLine($"\t\t{{{guidStr}}}.{config}|{platform}.Build.0 = {config}|{platform}");
        }

        sb.AppendLine("\tEndGlobalSection");
        sb.AppendLine("EndGlobal");

        File.WriteAllText(slnPath, sb.ToString(), Encoding.UTF8);
        Console.WriteLine($"Generated: {slnPath}");
    }
}