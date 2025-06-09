using System.Reflection;
using System.Text;
using System.Xml;
using System.Xml.Serialization;
using Builder.API;

namespace Builder.BuildProject;

public class BuildProject
{
    public static IProject Project = null!;

    public void LoadProject()
    {
        Generate();
        Build();

        var dllPath = Directory.EnumerateFiles(Path.Combine(PathConstants.ProjectsDir, "bin"), "Build.dll",
            SearchOption.AllDirectories).Single();
        var assembly = Assembly.LoadFile(Path.GetFullPath(dllPath));

        Project = (IProject)Activator.CreateInstance(assembly.GetTypes()
            .Single(x => x.GetInterfaces().Any(i => i == typeof(IProject))))! ?? throw new InvalidOperationException();
    }

    private void Generate()
    {
        var path = PathConstants.BuildProj;

        CreateProjectDirs(path);

        var serializer = new XmlSerializer(typeof(CsharpProjectModel));
        var settings = new XmlWriterSettings
        {
            OmitXmlDeclaration = true,
            Indent = true
        };

        var emptyNamespaces = new XmlSerializerNamespaces();
        emptyNamespaces.Add("", "");

        var sb = new StringBuilder();
        using var writer = new StringWriter(sb);
        using var xmlWriter = XmlWriter.Create(writer, settings);

        var sourcePath = Path.GetRelativePath(PathConstants.ProjectsDir,
            Path.Combine(Options.CommandLine.Instance.Root, "**/*.cs"));
        var apiPath = Path.GetRelativePath(PathConstants.ProjectsDir, PathConstants.BuilderApiPath);

        var project = new CsharpProjectModel
        {
            ItemGroup =
            {
                Compile = new()
                {
                    new()
                    {
                        Include = sourcePath
                    }
                },
                Reference = new()
                {
                    new()
                    {
                        Include = "Builder.API",
                        HintPath = apiPath
                    }
                }
            }
        };

        serializer.Serialize(xmlWriter, project, emptyNamespaces);

        File.WriteAllText(path, sb.ToString());
    }

    private void Build()
    {
        var ret = ProcessRunner.Run("dotnet", out var exitCode, "build", PathConstants.BuildProj);

        if (exitCode != 0)
        {
            Console.WriteLine("An error occured during build:");
            Console.WriteLine(ret);

            throw new Exception();
        }
    }

    private void CreateProjectDirs(string path)
    {
        Directory.CreateDirectory(Path.GetDirectoryName(path)!);
    }
}