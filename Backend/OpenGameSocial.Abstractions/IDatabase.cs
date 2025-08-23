using System.Linq.Expressions;

namespace OpenGameSocial.Abstractions;

public interface IDatabase
{
    Task<T> GetDocumentAsync<T>(Expression<Func<T, bool>> expression);
    Task<T> UpdateDocumentAsync<T>(T document, params Expression<Func<T, object>>[] fields);
}