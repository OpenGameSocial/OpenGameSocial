using System.Linq.Expressions;
using Microsoft.Extensions.Options;
using OpenGameSocial.Abstractions;
using OpenGameSocial.Abstractions.DependencyInjection;

namespace OpenGameSocial.MongoDB;

[RegisterService]
public class MongoDatabase : IDatabase
{
    private readonly IOptions<MongoConfig> _config;

    public MongoDatabase(IOptions<MongoConfig> config)
    {
        _config = config;
    }

    public Task<T> GetDocumentAsync<T>(Expression<Func<T, bool>> expression)
    {
        throw new NotImplementedException();
    }

    public Task<T> UpdateDocumentAsync<T>(T document, params Expression<Func<T, object>>[] fields)
    {
        throw new NotImplementedException();
    }
}