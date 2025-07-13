namespace CodegenTool;

public interface IMacroHandler
{
    string Macro { get; }

    bool CanHandle(ref ParsingContext context);

    void Handle(ref ParsingContext context);

    void Flush();
}