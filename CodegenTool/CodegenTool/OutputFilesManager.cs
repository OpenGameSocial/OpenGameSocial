using System.Text;

namespace CodegenTool;

public class OutputFilesManager
{
    private readonly Dictionary<string, StringBuilder> _files = new();

    public static readonly OutputFilesManager Instance = new();

    public void AppendLine(string file, string content)
    {
        GetFile(file).AppendLine(content);
    }

    public void Append(string file, string content)
    {
        GetFile(file).Append(content);
    }

    public StringBuilder GetFile(string file)
    {
        if (_files.TryGetValue(file, out var result))
        {
            return result;
        }

        _files.Add(file, result = new StringBuilder());

        return result;
    }

    public void Flush(string root)
    {
        Directory.CreateDirectory(root);
        
        foreach (var (file, content) in _files)
        {
            var path = Path.Combine(root, file);

            if (File.Exists(path))
            {
                File.Delete(path);
            }

            File.WriteAllText(path, content.ToString());
            File.SetLastWriteTime(path, DateTime.Now);
        }
    }
}