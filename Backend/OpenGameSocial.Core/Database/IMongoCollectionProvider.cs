using MongoDB.Driver;

namespace OpenGameSocial.Core.Database;

public interface IMongoCollectionProvider
{
    IMongoCollection<TDocument> GetCollection<TDocument>(string name);
}