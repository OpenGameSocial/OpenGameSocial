using OpenGameSocial.Data.Account;

namespace OpenGameSocial.Services.Account;

public class DefaultTitleAccountIdFactory : ITitleAccountIdFactory
{
    public TitleAccountId CreateTitleAccountId()
    {
        return new TitleAccountId(Guid.NewGuid().ToString());
    }
}