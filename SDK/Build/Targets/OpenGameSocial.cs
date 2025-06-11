using Builder.API.Target;

namespace Build.Targets;

public class OpenGameSocial : TargetBase<OpenGameSocial>
{
    public override TargetType Type => TargetType.StaticLibrary;
    public override string SourcesDir => "Source";
    public override string BaseDir => GetCurrentBaseDirectory();
    
    public override void Setup()
    {
        IncludeDirs.Add("Include");
    }
}