namespace CodegenTool;

public class MacroHandlingManager
{
    private readonly Dictionary<string, IMacroHandler> _handlers = new();

    public static readonly MacroHandlingManager Instance = new();

    private MacroHandlingManager()
    {
        var handlers = GetType()
            .Assembly
            .GetTypes()
            .Where(x => x is { IsClass: true, IsAbstract: false } &&
                        typeof(IMacroHandler).IsAssignableFrom(x));

        foreach (var type in handlers)
        {
            _handlers.Add(type.Name, (IMacroHandler)Activator.CreateInstance(type)!);
        }
    }

    public void ProcessFile(string root, string filePath)
    {
        var lines = File.ReadAllLines(filePath);
        ParsingContext context = new()
        {
            SourceRoot = root,
            File = filePath,
            Lines = lines,
            Scope = new Stack<string>()
        };

        for (var i = 0; i < lines.Length; i++)
        {
            var line = lines[i];
            var trimmed = line.Trim();

            if (trimmed.StartsWith("//"))
            {
                continue;
            }
            
            context.Index = i;

            if (trimmed.Contains('{') && (trimmed.Contains('}') || trimmed.Contains("};")))
            {
                continue;
            }

            if (trimmed.StartsWith('{'))
            {
                var scopeLine = lines[i - 1].Trim();
                context.Scope.Push(scopeLine);
            }

            if (trimmed.EndsWith('}') || trimmed.EndsWith("};"))
            {
                context.Scope.Pop();
            }

            foreach (var handler in _handlers.Values)
            {
                if (!handler.CanHandle(ref context))
                {
                    continue;
                }

                handler.Handle(ref context);
                break;
            }
        }
    }

    public void Flush(string root)
    {
        foreach (var handler in _handlers.Values)
        {
            handler.Flush();
        }

        OutputFilesManager.Instance.Flush(root);
    }
}