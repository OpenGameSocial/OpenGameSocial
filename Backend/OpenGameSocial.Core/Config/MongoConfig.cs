namespace OpenGameSocial.Core.Config;

public class MongoConfig
{
    public required string ConnectionString { get; init; }

    public required string DatabaseName { get; init; }
}