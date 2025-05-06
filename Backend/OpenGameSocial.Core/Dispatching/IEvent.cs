namespace OpenGameSocial.Core.Dispatching;

public interface IEvent<in T>
{
    Task Handle(T evt);
}