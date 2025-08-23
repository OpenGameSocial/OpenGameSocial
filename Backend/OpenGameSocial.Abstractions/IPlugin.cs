using Microsoft.Extensions.Hosting;

namespace OpenGameSocial.Abstractions;

public interface IPlugin : IDisposable
{
    void Initialize(IHostApplicationBuilder builder);
}