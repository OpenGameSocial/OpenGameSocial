using OpenGameSocial.Data.Account;

namespace OpenGameSocial.Services.Account;

public interface ITitleAccountIdFactory
{
    TitleAccountId CreateTitleAccountId();
}