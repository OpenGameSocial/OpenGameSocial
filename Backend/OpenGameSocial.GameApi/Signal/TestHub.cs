using Microsoft.AspNetCore.SignalR;

namespace OpenGameSocial.GameApi.Signal;

public class TestHub : Hub
{
    public override async Task OnConnectedAsync()
    {
        var name = Context.User?.Identity?.Name ?? "Anonymous";

        await Clients.All.SendAsync("UserConnected", name);
    }

    public async Task SendMessage(string user, string message)
    {
        await Clients.All.SendAsync("ReceiveMessage", user, message);
    }
}