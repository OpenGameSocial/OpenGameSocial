using Builder.API;

namespace Builder.Platforms;

public class PlatformWin64 : IPlatform
{
    public string Name => "Win64";

    public ICompiler GetCompiler()
    {
        throw new NotImplementedException();
    }

    public ILinker GetLinker()
    {
        throw new NotImplementedException();
    }
}