namespace OpenGameSocial.Core.Dispatching;

public interface IQuery<in TArg, TResult>
{
    Task<TResult> Run(TArg parameter);
}