using MongoDB.Driver;
using OpenGameSocial.Core.Database;
using OpenGameSocial.Data.Account;

namespace OpenGameSocial.Services.Account;

public class AccountRepository : IAccountRepository
{
    private const string CollectionName = "Accounts";

    private readonly IMongoCollection<UserAccount> _accountsCollection;

    public AccountRepository(IMongoCollectionProvider provider)
    {
        _accountsCollection = provider.GetCollection<UserAccount>(CollectionName);
    }

    public async Task<UserAccount?> GetUserAccountById(TitleAccountId accountId)
    {
        var result = await _accountsCollection.FindAsync(x => x.Id.Id == accountId.Id && x.Id.Type == accountId.Type,
            new FindOptions<UserAccount>
            {
                Limit = 1
            });

        return result.FirstOrDefault();
    }

    public async Task<UserAccount?> GetUserAccountByLinkedId(LinkedAccountId accountId)
    {
        var result = await _accountsCollection.FindAsync(x =>
                x.LinkedAccounts.Contains(accountId),
            new FindOptions<UserAccount>
            {
                Limit = 1
            });

        return result.FirstOrDefault();
    }

    public Task AddUserAccount(UserAccount account)
    {
        return _accountsCollection.InsertOneAsync(account);
    }
}