namespace CodegenTool;

internal static class Program
{
    public static void Main(string[] args)
    {
        Console.WriteLine("Codegen started!");
        
        var root = args[0];
        var platform = args[1];
        var outputDir = args[2];

        var files = Directory.EnumerateFiles(root, "*.cpp", SearchOption.AllDirectories)
            .Union(Directory.EnumerateFiles(Path.Combine(root, "Platform", platform), "*.cpp",
                SearchOption.AllDirectories));

        foreach (var file in files)
        {
            var fileName = Path.GetFileName(file);

            if (OutputFiles.Files.Contains(fileName))
            {
                continue;
            }

            MacroHandlingManager.Instance.ProcessFile(Path.GetFullPath(file));
        }

        MacroHandlingManager.Instance.Flush(outputDir);

        Console.WriteLine("Codegen finished!");
    }
}