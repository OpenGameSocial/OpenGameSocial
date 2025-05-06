using Microsoft.Extensions.Options;
using MongoDB.Driver;
using OpenGameSocial.Core.Config;

namespace OpenGameSocial.Core.Database;

internal class MongoCollectionProvider : IMongoCollectionProvider
{
    private readonly IMongoDatabase _database;

    public MongoCollectionProvider(IMongoClient client, IOptions<MongoConfig> config)
    {
        _database = client.GetDatabase(config.Value.DatabaseName);
    }

    public IMongoCollection<TDocument> GetCollection<TDocument>(string name)
    {
        return _database.GetCollection<TDocument>(name);
    }
}