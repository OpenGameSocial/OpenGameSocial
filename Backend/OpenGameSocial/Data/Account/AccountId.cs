namespace OpenGameSocial.Data.Account;

public abstract class AccountId
{
    public string Id { get; }
    
    public string Type { get; }

    protected AccountId(string id, string type)
    {
        Id = id;
        Type = type;
    }
}