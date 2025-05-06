using OpenGameSocial.Core.Dispatching;

namespace OpenGameSocial.TestApp;

public class MyTestEvent
{
    public required string Text { get; set; }
}

public class MyTestEventHandler : IEvent<MyTestEvent>
{
    public Task Handle(MyTestEvent evt)
    {
        Console.WriteLine($"Received {evt.Text}");
        return Task.CompletedTask;
    }
}