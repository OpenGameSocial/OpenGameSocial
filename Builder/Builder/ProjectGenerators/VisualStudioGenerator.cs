using System.Diagnostics;
using Builder.API;

namespace Builder.ProjectGenerators;

public class VisualStudioGenerator : IProjectGenerator
{
    public string Name => "vs";

    public void Generate()
    {
        var project = BuildProject.BuildProject.Project;

        var env = new Dictionary<string, string>
        {
            {
                "INCLUDE",
                "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\include;C:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.22621.0\\ucrt"
            },
            {
                "LIB",
                "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\lib;C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.22621.0\\ucrt\\x64;C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.22621.0\\um\\x64"
            }
        };

        env = GetVcVarsEnvironment();

        var objectsDir = Path.Combine(PathConstants.IntermediateDir, "Objects");
        var binDir = Path.Combine(PathConstants.FullRoot, "Bin");
        Directory.CreateDirectory(objectsDir);
        Directory.CreateDirectory(binDir);

        foreach (var target in project.GetTargets())
        {
            Console.WriteLine($"Generating project {target.Name}");

            var sourceRoot = Path.Combine(PathConstants.FullRoot, target.SourcesDir);
            var includes = target.GetIncludeFiles(sourceRoot);
            var compilationUnits = target.GetCompilationUnits(sourceRoot);

            var objPath = Path.Combine(objectsDir,
                Path.GetFileNameWithoutExtension(compilationUnits.First()) + ".obj");
            var exePath = Path.Combine(binDir, $"{target.Name}.exe");

            var compilationResult = ProcessRunner.RunWithEnv(
                FindExecutableInEnv("cl.exe", env)!,
                out var exitCode,
                env,
                "/c", "/EHsc", "/nologo", $"/Fo{objPath}", "/MT",
                "/I", "C:\\dev\\OpenGameSocial\\SDK\\Include",
                compilationUnits.First());

            Console.WriteLine(compilationResult);

            var linkResult =
                ProcessRunner.RunWithEnv(
                    FindExecutableInEnv("link.exe", env)!,
                    out exitCode,
                    env,
                    objPath, "/nologo", $"/OUT:{exePath}", "/SUBSYSTEM:CONSOLE", "/MACHINE:X64");

            Console.WriteLine(linkResult);
        }
    }

    private static string? FindExecutableInEnv(string executable, Dictionary<string, string> env)
    {
        if (!env.TryGetValue("PATH", out var pathVar))
        {
            return null;
        }

        var pathList = pathVar.Split(';');

        foreach (var path in pathList)
        {
            var fullPath = Path.Combine(path, executable);

            if (File.Exists(fullPath))
            {
                return fullPath;
            }
        }

        return null;
    }

    Dictionary<string, string> GetVcVarsEnvironment(string arch = "x64")
    {
        var batFile = GetPreferredVcVarsPath();
        var archArg = arch;

        var psi = new ProcessStartInfo
        {
            FileName = "cmd.exe",
            Arguments = $"/s /c \"\"{batFile}\" {archArg} >nul && set\"",
            RedirectStandardOutput = true,
            UseShellExecute = false,
            CreateNoWindow = true
        };

        var env = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);

        using (var process = Process.Start(psi))
        {
            string? line;
            while ((line = process.StandardOutput.ReadLine()) != null)
            {
                int idx = line.IndexOf('=');
                if (idx > 0)
                {
                    var key = line.Substring(0, idx);
                    var val = line.Substring(idx + 1);
                    env[key] = val;
                }
            }

            process.WaitForExit();
            if (process.ExitCode != 0)
                throw new Exception($"vcvarsall.bat failed with exit code {process.ExitCode}");
        }

        return env;
    }

    string? GetPreferredVcVarsPath()
    {
        string vswherePath = Path.Combine(
            Environment.GetFolderPath(Environment.SpecialFolder.ProgramFilesX86),
            "Microsoft Visual Studio", "Installer", "vswhere.exe");

        if (!File.Exists(vswherePath))
            throw new FileNotFoundException("vswhere.exe not found");

        string? GetInstallPath(string product)
        {
            var psi = new ProcessStartInfo
            {
                FileName = vswherePath,
                Arguments =
                    $"-latest -products {product} -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath",
                RedirectStandardOutput = true,
                UseShellExecute = false,
                CreateNoWindow = true
            };

            using var proc = Process.Start(psi);
            string? path = proc?.StandardOutput.ReadLine()?.Trim();
            proc?.WaitForExit();
            return string.IsNullOrWhiteSpace(path) ? null : path;
        }

        // 1. Try Build Tools first
        var installPath = GetInstallPath("Microsoft.VisualStudio.Product.BuildTools")
                          ?? GetInstallPath("*"); // 2. Fallback to any VS

        if (string.IsNullOrWhiteSpace(installPath))
            throw new Exception("No MSVC installation found");

        var vcvarsPath = Path.Combine(installPath, "VC", "Auxiliary", "Build", "vcvarsall.bat");
        if (!File.Exists(vcvarsPath))
            throw new FileNotFoundException("vcvarsall.bat not found at: " + vcvarsPath);

        return vcvarsPath;
    }
}