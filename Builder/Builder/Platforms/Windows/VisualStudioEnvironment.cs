using System.Diagnostics;

namespace Builder.Platforms.Windows;

public static class VisualStudioEnvironment
{
    public static readonly Dictionary<string, string> Environment = new(StringComparer.OrdinalIgnoreCase);

    private static string _arch = null!;

    private static string _compiler = null!;
    private static string _linker = null!;

    public static void EnsureInitialized(string arch)
    {
        if (Environment.Count > 0)
        {
            if (arch.Equals(_arch, StringComparison.InvariantCultureIgnoreCase))
            {
                return;
            }

            throw new Exception("Visual Studio environment was initialized with a different architecture.");
        }

        _arch = arch;

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

        using var process = Process.Start(psi);

        while (process!.StandardOutput.ReadLine() is { } line)
        {
            var idx = line.IndexOf('=');

            if (idx <= 0)
            {
                continue;
            }

            var key = line.Substring(0, idx);
            var val = line.Substring(idx + 1);
            Environment[key] = val;
        }

        process.WaitForExit();

        if (process.ExitCode != 0)
            throw new Exception($"vcvarsall.bat failed with exit code {process.ExitCode}");

        _compiler = FindExecutableInEnv("cl.exe");
        _linker = FindExecutableInEnv("link.exe");
    }

    public static string GetCompiler()
    {
        return _compiler;
    }

    public static string GetLinker()
    {
        return _linker;
    }

    private static string FindExecutableInEnv(string executable)
    {
        if (!Environment.TryGetValue("PATH", out var pathVar))
        {
            throw new Exception("Environment variable PATH not found.");
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

        throw new Exception($"{executable} not found in PATH.");
    }

    private static string? GetPreferredVcVarsPath()
    {
        var vswherePath = Path.Combine(
            System.Environment.GetFolderPath(System.Environment.SpecialFolder.ProgramFilesX86),
            "Microsoft Visual Studio", "Installer", "vswhere.exe");

        if (!File.Exists(vswherePath))
            throw new FileNotFoundException("vswhere.exe not found");

        var installPath = GetVsProductInstallPath("Microsoft.VisualStudio.Product.BuildTools", vswherePath)
                          ?? GetVsProductInstallPath("*", vswherePath);

        if (string.IsNullOrWhiteSpace(installPath))
            throw new Exception("No MSVC installation found");

        var vcvarsPath = Path.Combine(installPath, "VC", "Auxiliary", "Build", "vcvarsall.bat");
        if (!File.Exists(vcvarsPath))
            throw new FileNotFoundException("vcvarsall.bat not found at: " + vcvarsPath);

        return vcvarsPath;
    }

    private static string? GetVsProductInstallPath(string product, string? vswherePath)
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
        var path = proc?.StandardOutput.ReadLine()?.Trim();
        proc?.WaitForExit();

        return string.IsNullOrWhiteSpace(path) ? null : path;
    }
}