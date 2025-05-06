using Microsoft.Extensions.DependencyInjection;

namespace OpenGameSocial.Core.Database;

public static class DiExtensions
{
    public static IServiceCollection AddDefaultDatabaseProviders(this IServiceCollection services)
    {
        return services.AddSingleton<IMongoCollectionProvider, MongoCollectionProvider>();
    }
}