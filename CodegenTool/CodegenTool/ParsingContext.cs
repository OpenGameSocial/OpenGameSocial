namespace CodegenTool;

public struct ParsingContext
{
    public string SourceRoot;

    public string File;

    public int Index;
    public string[] Lines;
    public Stack<string> Scope;

    public int LineNumber => Index + 1;
    public string CurrentLine => Lines[Index].Trim();
}