using OpenGameSocial.Core.DependencyInjection;
using OpenGameSocial.Plugins;
using OpenGameSocial.Vendor;

namespace OpenGameSocial.AccountService;

internal class Program
{
    public static async Task Main(string[] args)
    {
        await PluginManager.Instance.Load(args);

        var builder = VendorStartup<Program>.Create(args);
        builder.AddOpenGameSocial();
        builder.RegisterServices();

        PluginManager.Instance.RunInit(builder);

        var app = VendorStartup<Program>.Build(builder);

        await VendorStartup<Program>.Run(app);
    }
}