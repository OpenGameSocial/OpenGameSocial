namespace Builder.API;

public interface IPlatform
{
    string Name { get; }

    ICompiler GetCompiler();
    ILinker GetLinker();
}