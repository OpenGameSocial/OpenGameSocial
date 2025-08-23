using System.Reflection;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Hosting;
using OpenGameSocial.Abstractions;
using OpenGameSocial.Core.DependencyInjection;

namespace OpenGameSocial.Plugins;

public class PluginManager
{
    private const string FileName = "plugins.list";

    private readonly Dictionary<string, PluginContainer> _plugins = new();

    public static readonly PluginManager Instance = new();

    private PluginManager()
    {
    }

    public async Task Load(string[] args)
    {
        var searchPathList = GetSearchPathList(args);

        if (!File.Exists(FileName))
        {
            Console.WriteLine("No plugins found.");
            return;
        }

        var enabledPlugins = await File.ReadAllLinesAsync(FileName);

        foreach (var enabledPlugin in enabledPlugins.Select(x => x.Trim()))
        {
            if (enabledPlugin.StartsWith('#'))
            {
                continue;
            }

            var pluginDir = searchPathList.FirstOrDefault(x => File.Exists(Path.Combine(x, enabledPlugin)));

            if (string.IsNullOrEmpty(pluginDir))
            {
                pluginDir = Environment.CurrentDirectory;
            }

            var assemblyFileName = Path.GetFullPath(Path.Combine(pluginDir, enabledPlugin.Replace("\\", "/")));
            var symbolsFileName = Path.ChangeExtension(assemblyFileName, ".pdb");
            var hasSymbols = File.Exists(symbolsFileName);
            Console.WriteLine($"Loading plugin from {enabledPlugin} [has symbols: {hasSymbols}]");

            byte[]? rawSymbols = null;

            if (!File.Exists(assemblyFileName))
            {
                Console.WriteLine($"Plugin {enabledPlugin} not found.");
                continue;
            }

            var rawAssembly = await File.ReadAllBytesAsync(assemblyFileName);

            if (hasSymbols)
            {
                rawSymbols = await File.ReadAllBytesAsync(symbolsFileName);
            }

            var assembly = Assembly.Load(rawAssembly, rawSymbols);

            foreach (var pluginType in assembly.GetTypes().Where(x => typeof(IPlugin).IsAssignableFrom(x)))
            {
                if (Activator.CreateInstance(pluginType) is not IPlugin plugin)
                {
                    Console.WriteLine($"Cannot create instance of {pluginType}");
                    continue;
                }

                var pluginName = Path.GetFileNameWithoutExtension(assemblyFileName);

                _plugins.Add(pluginName, new PluginContainer(pluginDir, assembly, plugin));

                Console.WriteLine($"Loaded plugin {plugin}");
            }
        }
    }

    private static string[] GetSearchPathList(string[] args)
    {
        var result = new List<string>();

        foreach (var arg in args)
        {
            if (arg.StartsWith("--plugin-path="))
            {
                result.Add(arg["--plugin-path=".Length..].Trim('"'));
            }
        }

        return result.ToArray();
    }

    public void RunInit(IHostApplicationBuilder builder)
    {
        foreach (var (name, container) in _plugins)
        {
            builder.Configuration
                .AddJsonFile(Path.Join(container.PluginDir, $"{name}.json"), optional: true)
                .AddJsonFile(Path.Join(container.PluginDir, $"{name}.Development.json"), optional: true);

            builder.RegisterConfigurations(container.Assembly);
            builder.RegisterServices(container.Assembly);
            
            container.Plugin.Initialize(builder);
        }
    }

    private struct PluginContainer
    {
        public readonly string PluginDir;
        public readonly Assembly Assembly;
        public readonly IPlugin Plugin;

        public PluginContainer(string pluginDir, Assembly assembly, IPlugin plugin)
        {
            PluginDir = pluginDir;
            Assembly = assembly;
            Plugin = plugin;
        }
    }
}