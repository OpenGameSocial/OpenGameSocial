using OpenGameSocial.Abstractions;
using OpenGameSocial.Abstractions.API;

namespace OpenGameSocial.AccountService;

[ApiEndpoint]
public class TestEndpoint
{
    private readonly IDatabase _database;

    public TestEndpoint(IDatabase database)
    {
        _database = database;
    }

    public Task<string> Get()
    {
        var m = new TestModel(string.Empty, string.Empty);
        _database.UpdateDocumentAsync(m,
            x => x.Id,
            x => x.Name);
        return Task.FromResult("Hello World!");
    }
}