using OpenGameSocial.Core.Dispatching;

namespace OpenGameSocial.TestApp;

public class MyTestQuery
{
    public class Result
    {
        public required string Text { get; init; }
    }
}

public class MyTestQueryHandler : IQuery<MyTestQuery, MyTestQuery.Result>
{
    public Task<MyTestQuery.Result> Run(MyTestQuery parameter)
    {
        return Task.FromResult(new MyTestQuery.Result
        {
            Text = "Hello World"
        });
    }
}