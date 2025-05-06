namespace OpenGameSocial.Data.Account;

public class UserAccount
{
    public TitleAccountId Id { get; set; }

    public string Name { get; set; }

    public DateTime? BannedUntil { get; set; }

    public List<TitleAccountId> Blocklist { get; set; } = new();
    
    public List<TitleAccountId> FriendList { get; set; } = new();

    public List<LinkedAccountId> LinkedAccounts { get; set; } = new();

    public Dictionary<string, string> Properties { get; set; } = new();

    public UserAccount(TitleAccountId id, string name, LinkedAccountId linkedAccount)
    {
        Id = id;
        Name = name;
        
        LinkedAccounts.Add(linkedAccount);
    }
}