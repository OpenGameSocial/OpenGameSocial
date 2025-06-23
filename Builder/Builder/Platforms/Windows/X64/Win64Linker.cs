using System.Collections.Immutable;
using Builder.API;
using Builder.Options;

namespace Builder.Platforms.Windows.X64;

public class Win64Linker : ILinker
{
    public Win64Linker()
    {
        VisualStudioEnvironment.EnsureInitialized("x64");
    }

    public bool Link(ITarget target, IImmutableSet<string> objects)
    {
        var buildOptions = (BuildVerb)Options.CommandLine.Instance;
        
        Directory.CreateDirectory(PathConstants.BinDir);

        var exePath = Path.Combine(PathConstants.BinDir, $"{target.Name}_{buildOptions.Configuration}{buildOptions.Platform}.exe");

        var args = new List<string>();
        args.AddRange(objects);
        args.AddRange(["/nologo", $"/OUT:{exePath}", "/SUBSYSTEM:CONSOLE", "/MACHINE:X64"]);

        var linkResult = ProcessRunner.RunWithEnv(
            VisualStudioEnvironment.GetLinker(),
            out var exitCode,
            VisualStudioEnvironment.Environment,
            args.ToArray());

        Console.WriteLine(linkResult);
        
        return exitCode == 0;
    }
}