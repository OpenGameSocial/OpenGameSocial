using Microsoft.Extensions.Hosting;
using OpenGameSocial.Abstractions;

namespace OpenGameSocial.CodeGen;

public class WebApiPlugin : IPlugin
{
    public bool AlwaysEnabled => true;

    public void Initialize(IHostApplicationBuilder builder)
    {
        builder.Services.AddControllers();
        builder.Services.AddOpenApi();
    }

    public void Dispose()
    {
    }
}