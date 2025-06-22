using Builder.API;
using Builder.Modules;

namespace Builder.Extensions;

public static class StringExtensions
{
    public static string AsModulePath(this string path, ModuleInfo info)
    {
        return Path.Combine(info.BaseDir, path);
    }
}