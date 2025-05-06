using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using MongoDB.Driver;
using OpenGameSocial.Core.Config;
using OpenGameSocial.Core.Database;
using OpenGameSocial.Core.Dispatching;
using OpenGameSocial.Data.Account;
using OpenGameSocial.Services.Account;

namespace OpenGameSocial.TestApp;

internal static class Program
{
    public static async Task Main(string[] args)
    {
        var configuration = new ConfigurationBuilder()
            .AddJsonFile("appsettings.json")
            .AddJsonFile("appsettings.local.json", optional: true)
            .Build();

        var sc = new ServiceCollection()
            .AddDispatching()
            .RegisterDispatchingFromAssembly(typeof(Program).Assembly)
            .AddDefaultDatabaseProviders()
            .AddSingleton<IAccountRepository, AccountRepository>()
            .AddSingleton<ITitleAccountIdFactory, DefaultTitleAccountIdFactory>()
            .AddLogging()
            .AddOptions();

        var mongoSection = configuration.GetSection("MongoConfig");

        sc.AddSingleton<IMongoClient>(new MongoClient(mongoSection[nameof(MongoConfig.ConnectionString)]));

        sc.Configure<MongoConfig>(mongoSection);
    }
}