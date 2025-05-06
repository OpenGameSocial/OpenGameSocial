namespace OpenGameSocial.Core.Dispatching;

public interface IDispatcher
{
    Task RunCommand<TCommand>(TCommand cmd);

    Task<TResult> RunQuery<TResult, TQuery>(TQuery cmd);

    Task RaiseEvent<TEvent>(TEvent evt);
}