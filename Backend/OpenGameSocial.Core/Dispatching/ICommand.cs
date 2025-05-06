namespace OpenGameSocial.Core.Dispatching;

public interface ICommand<in TArg>
{
    Task Run(TArg cmd);
}