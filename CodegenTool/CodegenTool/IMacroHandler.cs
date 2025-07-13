namespace CodegenTool;

public interface IMacroHandler
{
    string Macro { get; }

    bool CanHandle(string line);

    void Handle(string[] lines, int index);

    void Flush();
}