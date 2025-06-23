using CommandLine;

namespace Builder.Options;

[Verb("build")]
public class BuildVerb : CommandLine
{
    [Option('c', "configuration", Required = true, HelpText = "Configuration to build")]
    public required string Configuration { get; init; }
}