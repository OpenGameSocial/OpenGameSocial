namespace OpenGameSocial.Data.Account;

public sealed class TitleAccountId : AccountId
{
    public const string TitleAccountType = "title";
    
    public TitleAccountId(string id) : base(id, TitleAccountType)
    {
    }
}