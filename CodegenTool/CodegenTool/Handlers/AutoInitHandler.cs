using System.Text.RegularExpressions;

namespace CodegenTool.Handlers;

public class AutoInitHandler : IMacroHandler
{
    public string Macro => MacroNames.AutoInitMacro;

    private static readonly Regex InitializerRegex = new(@"CAutoInit\s+([A-z0-9]+)\s*[(|;?]", RegexOptions.Compiled);

    private readonly List<string> _initializers = new();

    public bool CanHandle(string line)
    {
        return line.Trim().StartsWith(MacroNames.AutoInitMacro, StringComparison.InvariantCulture);
    }

    public void Handle(string[] lines, int index)
    {
        var match = InitializerRegex.Match(lines[index]);

        if (!match.Success)
        {
            Console.WriteLine($"[WARNING] Cannot match AUTOINIT expression: {lines[index]}");
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

        file.AppendLine("void CAutoInitRegistry::RegisterInitializers()");
        file.AppendLine("{");

        foreach (var initializer in _initializers)
        {
            file.AppendLine($"\tRegister(&{initializer});");
        }

        file.AppendLine("}");
    }
}