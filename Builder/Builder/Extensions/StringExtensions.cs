using Builder.API;

namespace Builder.Extensions;

public static class StringExtensions
{
    public static string AsTargetPath(this string path, ITarget target)
    {
        return Path.Combine(target.BaseDir, path);
    }
}