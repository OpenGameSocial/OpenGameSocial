using Builder.API;

namespace Builder.Platforms.Windows.X64;

public class PlatformWin64 : IPlatform
{
    public string Name => "Win64";

    private readonly ICompiler _compiler = new Win64Compiler();
    private readonly ILinker _linker = new Win64Linker();

    public ICompiler GetCompiler()
    {
        return _compiler;
    }

    public ILinker GetLinker()
    {
        return _linker;
    }
}