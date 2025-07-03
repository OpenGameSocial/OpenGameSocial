using CommandLine;

namespace Builder.Options;

public class CommandLine
{
    public static CommandLine Instance = null!;

    [Option('r', "root", Required = true, HelpText = "Project root path")]
    public required string Root { get; init; }
}