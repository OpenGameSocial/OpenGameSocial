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

    public void ProcessFile(string filePath)
    {
        var lines = File.ReadAllLines(filePath);

        for (var i = 0; i < lines.Length; i++)
        {
            var line = lines[i];

            foreach (var handler in _handlers.Values)
            {
                if (!handler.CanHandle(line))
                {
                    continue;
                }

                handler.Handle(lines, i);
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