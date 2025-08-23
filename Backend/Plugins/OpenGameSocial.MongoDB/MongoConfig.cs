using OpenGameSocial.Abstractions.DependencyInjection;

namespace OpenGameSocial.MongoDB;

[RegisterConfiguration("MongoConfig")]
public class MongoConfig
{
    public required string ConnectionString { get; init; }

    public required string DatabaseName { get; init; }
}