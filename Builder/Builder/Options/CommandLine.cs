using CommandLine;

namespace Builder.Options;

public class CommandLine
{
    public static CommandLine Instance = null!;

    [Option('p', "platform", Required = true, HelpText = "Platform to build")]
    public required string Platform { get; init; }

    [Option('r', "root", Required = true, HelpText = "Project root path")]
    public required string Root { get; init; }
}