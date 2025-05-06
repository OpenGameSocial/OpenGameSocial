using System.Collections.Concurrent;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Logging;

namespace OpenGameSocial.Core.Dispatching;

internal class Dispatcher : IDispatcher
{
    private readonly IServiceProvider _serviceProvider;

    private readonly ILogger<IDispatcher> _logger;

    private readonly ConcurrentDictionary<Type, object> _opCache = new();
    private readonly ConcurrentDictionary<Type, object[]> _eventCache = new();

    public Dispatcher(IServiceProvider serviceProvider, ILogger<IDispatcher> logger)
    {
        _serviceProvider = serviceProvider;
        _logger = logger;
    }

    public Task RunCommand<TCommand>(TCommand cmd)
    {
        if (_opCache.TryGetValue(typeof(TCommand), out var op) &&
            op is ICommand<TCommand> cachedCommand)
        {
            return cachedCommand.Run(cmd);
        }

        cachedCommand = _serviceProvider.GetRequiredService<ICommand<TCommand>>();
        _opCache[typeof(TCommand)] = cachedCommand;
        return cachedCommand.Run(cmd);
    }

    public Task<TResult> RunQuery<TResult, TQuery>(TQuery query)
    {
        if (_opCache.TryGetValue(typeof(TQuery), out var op) &&
            op is IQuery<TQuery, TResult> cachedQuery)
        {
            return cachedQuery.Run(query);
        }

        cachedQuery = _serviceProvider.GetRequiredService<IQuery<TQuery, TResult>>();
        _opCache[typeof(TQuery)] = cachedQuery;
        return cachedQuery.Run(query);
    }

    public async Task RaiseEvent<TEvent>(TEvent evt)
    {
        if (_eventCache.TryGetValue(typeof(TEvent), out var eventList))
        {
            if (eventList.Length == 0)
            {
                return;
            }

            await RaiseEvents(evt, eventList.Cast<IEvent<TEvent>>(), eventList.Length);

            return;
        }

        var eventHandlers = _serviceProvider.GetServices<IEvent<TEvent>>().ToArray();
        // ReSharper disable once CoVariantArrayConversion
        _eventCache[typeof(TEvent)] = eventHandlers;

        await RaiseEvents(evt, eventHandlers, eventHandlers.Length);
    }

    private Task RaiseEvents<TEvent>(TEvent evt, IEnumerable<IEvent<TEvent>> events, int count)
    {
        if (count == 0)
        {
            return Task.CompletedTask;
        }

        var tasks = new List<Task>(count);

        foreach (var cachedEvent in events)
        {
            try
            {
                tasks.Add(cachedEvent.Handle(evt));
            }
            catch (Exception e)
            {
                _logger.LogError(e, "An error occured while raising an event {name}", typeof(TEvent).Name);
            }
        }

        return tasks.Count == 0 ? Task.CompletedTask : Task.WhenAll(tasks);
    }
}