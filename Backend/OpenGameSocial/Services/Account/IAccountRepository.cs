using OpenGameSocial.Data.Account;

namespace OpenGameSocial.Services.Account;

public interface IAccountRepository
{
    Task<UserAccount?> GetUserAccountById(TitleAccountId accountId);
    Task<UserAccount?> GetUserAccountByLinkedId(LinkedAccountId accountId);

    Task AddUserAccount(UserAccount account);
}