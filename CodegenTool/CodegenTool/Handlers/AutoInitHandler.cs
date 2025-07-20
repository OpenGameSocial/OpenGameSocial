using System.Text.RegularExpressions;

namespace CodegenTool.Handlers;

public class AutoInitHandler : IMacroHandler
{
    public string Macro => MacroNames.AutoInitMacro;

    private static readonly Regex InitializerRegex = new(@"CAutoInit\s+([A-z0-9]+)\s*[(|;?]", RegexOptions.Compiled);

    private readonly List<string> _initializers = new();

    public bool CanHandle(ref ParsingContext context)
    {
        var hasMacro = context.CurrentLine.StartsWith(Macro, StringComparison.InvariantCulture);
        var isCpp = context.File.EndsWith(".cpp");

        if (hasMacro && !isCpp)
        {
            Console.WriteLine(
                $"[ERROR] Macro {Macro.Trim()} is found in non-compilation-unit file: {context.File}:{context.LineNumber}");
            Console.WriteLine("[ERROR] This is fatal error and compilation will be aborted.");
            Environment.Exit(-1);
        }

        return hasMacro && isCpp;
    }

    public void Handle(ref ParsingContext context)
    {
        var match = InitializerRegex.Match(context.CurrentLine);

        if (!match.Success)
        {
            Console.WriteLine($"[WARNING] Cannot match AUTOINIT expression: {context.CurrentLine}");
            return;
        }

        var name = match.Groups[1].Value;

        _initializers.Add(name);
    }

    public void Flush()
    {
        var file = OutputFilesManager.Instance.GetFile(OutputFiles.AutoInit);
        file.AppendLine("#include \"Core/Common/Codegen.h\"");
        file.AppendLine();
        file.AppendLine();

        foreach (var initializer in _initializers)
        {
            file.AppendLine($"extern CAutoInit {initializer};");
        }

        file.AppendLine();

        file.AppendLine("void RunAutoInit()");
        file.AppendLine("{");

        foreach (var initializer in _initializers)
        {
            file.AppendLine($"\t{initializer}.Run();");
        }

        file.AppendLine("}");

        Console.WriteLine($"[INFO] Generated {_initializers.Count} initializer(s)");
    }
}