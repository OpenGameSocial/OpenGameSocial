using System.Text;
using System.Text.RegularExpressions;

namespace CodegenTool.Handlers;

public class SerializableHandler : IMacroHandler
{
    private static readonly Regex NamespaceRegex = new(@"namespace\s+([A-z0-9:]*)", RegexOptions.Compiled);
    private static readonly Regex MemberNameRegex = new(@"([A-z0-9<>:]+)\s+([A-z0-9]+)", RegexOptions.Compiled);

    public string Macro => MacroNames.Serializable;

    private readonly Dictionary<string, SerializableTypeDescriptor> _types = new();
    private readonly HashSet<string> _includes = new();

    public bool CanHandle(ref ParsingContext context)
    {
        var hasMacro = context.CurrentLine.StartsWith(Macro, StringComparison.InvariantCulture);
        var isHeader = context.File.EndsWith(".h");

        if (hasMacro && !isHeader)
        {
            Console.WriteLine(
                $"[WARNING] Macro {Macro} is found in non-header file: {context.File}:{context.LineNumber}");
            Console.WriteLine(context.CurrentLine);
            return false;
        }

        return hasMacro && isHeader;
    }

    public void Handle(ref ParsingContext context)
    {
        if (context.Scope.Count == 0)
        {
            Console.WriteLine(
                $"[ERROR] SERIALIZABLE cannot be used outside of struct/class! {context.File}:{context.LineNumber}");
        }

        var match = MemberNameRegex.Match(context.Lines[context.Index + 1]);
        var memberType = match.Groups[1].Value;
        var memberName = match.Groups[2].Value;

        var typeName = context.Scope.Peek().Split(' ')[1];
        var ns = GetNamespace(context.Scope);

        var descriptor = GetDescriptor(ref context, ns, typeName);
        descriptor.Fields.Add(new(memberType, memberName));
    }

    public void Flush()
    {
        var file = OutputFilesManager.Instance.GetFile(OutputFiles.Serialization);
        file.AppendLine("#include <nlohmann/json.hpp>");

        file.AppendLine();

        file.AppendLine("#include \"Core/Common/Codegen.h\"");

        foreach (var include in _includes)
        {
            file.AppendLine($"#include \"{include}\"");
        }

        file.AppendLine();
        file.AppendLine();

        foreach (var (fqn, descriptor) in _types)
        {
            GenerateJsonOperations(file, descriptor);
        }

        var totalFields = _types.Values.Sum(x => x.Fields.Count);
        Console.WriteLine($"[INFO] Generated serialization for {totalFields} field(s) in {_types.Count} type(s)");
    }

    private static void GenerateJsonOperations(StringBuilder output, SerializableTypeDescriptor descriptor)
    {
        output.AppendLine($"void to_json(nlohmann::json& output, const {descriptor.FullyQualifiedType}& input)");
        output.AppendLine("{");

        foreach (var field in descriptor.Fields)
        {
            output.AppendLine($"\toutput[\"{field.SerializableName}\"] = input.{field.Name};");
        }

        output.AppendLine("}");

        output.AppendLine();

        output.AppendLine($"void from_json(const nlohmann::json& input, {descriptor.FullyQualifiedType}& output)");
        output.AppendLine("{");

        foreach (var field in descriptor.Fields)
        {
            if (field.Type.Contains("optional<"))
            {
                var realType = $"TRealType<decltype(output.{field.Name})>::Type";

                output.AppendLine($"\tauto iterator{field.Name} = input.find(\"{field.SerializableName}\");");
                output.AppendLine($"\tif (iterator{field.Name} != input.end() && !iterator{field.Name}->is_null())");
                output.AppendLine("\t{");
                output.AppendLine($"\t\toutput.{field.Name} = iterator{field.Name}->template get<{realType}>();");
                output.AppendLine("\t}");
                continue;
            }

            output.AppendLine($"\tinput.at(\"{field.SerializableName}\").get_to(output.{field.Name});");
        }

        output.AppendLine("}");
    }

    private static string GetNamespace(Stack<string> stack)
    {
        var result = new List<string>();

        foreach (var s in stack)
        {
            if (!s.Trim().StartsWith("namespace", StringComparison.InvariantCulture))
            {
                continue;
            }

            var match = NamespaceRegex.Match(s);
            var namespaceName = match.Groups[1].Value;
            result.Add(namespaceName);
        }

        result.Reverse();

        return string.Join("::", result);
    }

    private SerializableTypeDescriptor GetDescriptor(ref ParsingContext context, string ns, string type)
    {
        var fqn = string.Join("::", ns, type);

        if (_types.TryGetValue(fqn, out var descriptor))
        {
            return descriptor;
        }

        var includePath = Path.GetRelativePath(context.SourceRoot, context.File);
        includePath = includePath.Replace('\\', '/');

        _includes.Add(includePath);

        _types.Add(fqn, descriptor = new SerializableTypeDescriptor(includePath, ns, type));
        return descriptor;
    }

    private class SerializableTypeDescriptor
    {
        public string IncludeFile { get; }

        public string Namespace { get; }
        public string Name { get; }

        public string FullyQualifiedType => string.Join("::", Namespace, Name);

        public List<SerializableFieldDescriptor> Fields { get; } = new();

        public SerializableTypeDescriptor(string includeFile, string ns, string name)
        {
            IncludeFile = includeFile;
            Namespace = ns;
            Name = name;
        }
    }

    private class SerializableFieldDescriptor
    {
        public string Type { get; }
        public string Name { get; }

        public string SerializableName => Name[..1].ToLower() + Name[1..];

        public SerializableFieldDescriptor(string type, string name)
        {
            Type = type;
            Name = name;
        }
    }
}