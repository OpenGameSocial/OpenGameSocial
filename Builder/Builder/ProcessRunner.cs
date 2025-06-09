using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Text;

namespace Builder;

public static class ProcessRunner
{
    public static string Run(string command, out int exitCode, params IEnumerable<string> args)
    {
        return RunWithEnv(command, out exitCode, new(), args);
    }

    public static string RunWithEnv(string command, out int exitCode, Dictionary<string, string> env,
        params IEnumerable<string> args)
    {
        using var process = new Process();
        process.StartInfo = new ProcessStartInfo(command, args)
        {
            UseShellExecute = false,
            RedirectStandardError = true,
            RedirectStandardOutput = true,
            CreateNoWindow = true,
        };

        foreach (var kvp in env)
        {
            process.StartInfo.Environment.Add(kvp.Key, kvp.Value);
        }

        var result = new List<string>();

        DataReceivedEventHandler saveOutput = (_, a) =>
        {
            if (string.IsNullOrEmpty(a.Data))
            {
                return;
            }

            result.Add(a.Data);
        };

        process.OutputDataReceived += saveOutput;
        process.ErrorDataReceived += saveOutput;

        process.EnableRaisingEvents = true;

        if (!process.Start())
        {
            exitCode = process.ExitCode;
            return string.Empty;
        }

        process.BeginOutputReadLine();
        process.BeginErrorReadLine();

        process.WaitForExit();

        exitCode = process.ExitCode;

        return string.Join('\n', result);
    }
}