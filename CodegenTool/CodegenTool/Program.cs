namespace CodegenTool;

internal static class Program
{
    public static void Main(string[] args)
    {
        Console.WriteLine("Codegen started!");

        var root = args[0];
        var sourceDir = args[1];
        var platform = args[2];
        var outputDir = args[3];

        var mainSourcesDir = Path.Combine(root, sourceDir);
        var platformSourcesDir = Path.Combine(root, "Platform", platform);

        var files = CollectSources(mainSourcesDir, "*.cpp", []);
        files = CollectSources(mainSourcesDir, "*.h", files);
        var platformFiles = CollectSources(platformSourcesDir, "*.cpp", []);
        platformFiles = CollectSources(platformSourcesDir, "*.h", platformFiles);

        foreach (var file in files)
        {
            MacroHandlingManager.Instance.ProcessFile(mainSourcesDir, Path.GetFullPath(file));
        }

        foreach (var file in platformFiles)
        {
            MacroHandlingManager.Instance.ProcessFile(Path.Join(platformSourcesDir, "Source"),
                Path.GetFullPath(file));
        }

        MacroHandlingManager.Instance.Flush(outputDir);

        Console.WriteLine("Codegen finished!");
    }

    private static IEnumerable<string> CollectSources(string path, string pattern, IEnumerable<string> unionWith)
        => Directory.EnumerateFiles(path, pattern, SearchOption.AllDirectories).Union(unionWith);
}