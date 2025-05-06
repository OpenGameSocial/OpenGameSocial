using OpenGameSocial.Core.Dispatching;

namespace OpenGameSocial.TestApp;

public class MyTestCommand
{
}

internal class MyTestCommandHandler : ICommand<MyTestCommand>
{
    private readonly IDispatcher _dispatcher;

    public MyTestCommandHandler(IDispatcher dispatcher)
    {
        _dispatcher = dispatcher;
    }

    public async Task Run(MyTestCommand cmd)
    {
        var queryResult = await _dispatcher.RunQuery<MyTestQuery.Result, MyTestQuery>(new());

        await _dispatcher.RaiseEvent(new MyTestEvent
        {
            Text = queryResult.Text
        });
    }
}