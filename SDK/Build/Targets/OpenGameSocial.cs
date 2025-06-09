using Builder.API.Target;

namespace Build.Targets;

public class OpenGameSocial : TargetBase<OpenGameSocial>
{
    public override TargetType Type => TargetType.StaticLibrary;
    public override string SourcesDir => "Source";
}