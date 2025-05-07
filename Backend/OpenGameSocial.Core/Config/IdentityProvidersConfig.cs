namespace OpenGameSocial.Core.Config;

public class IdentityProvidersConfig
{
    public List<OpenIdIdentityProviderConfig> OpenIdProviders { get; set; } = new();
}