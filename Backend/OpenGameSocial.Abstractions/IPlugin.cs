using Microsoft.Extensions.Hosting;

namespace OpenGameSocial.Abstractions;

public interface IPlugin : IDisposable
{
    bool AlwaysEnabled { get; }
    
    void Initialize(IHostApplicationBuilder builder);
}