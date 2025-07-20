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
        
        foreach (var (file, sb) in _files)
        {
            var path = Path.Combine(root, file);
            var contents = sb.ToString();

            if (File.Exists(path))
            {
                var oldContents = File.ReadAllText(path);

                if (contents.Equals(oldContents, StringComparison.InvariantCulture))
                {
                    Console.WriteLine($"[INFO] {Path.GetFileName(path)} is unchanged. Skipping.");
                    continue;
                }
                
                File.Delete(path);
            }

            File.WriteAllText(path, contents);
            File.SetLastWriteTime(path, DateTime.Now);
        }
    }
}