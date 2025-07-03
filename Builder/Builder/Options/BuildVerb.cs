using CommandLine;

namespace Builder.Options;

[Verb("build")]
public class BuildVerb : CommandLine
{
    public new static BuildVerb Instance = null!;
    
    [Option('c', "configuration", Required = true, HelpText = "Configuration to build")]
    public required string Configuration { get; init; }
    
    [Option('p', "platform", Required = true, HelpText = "Platform to build")]
    public required string Platform { get; init; }
    
    [Option(longName: "dry")]
    public bool DryRun { get; init; }
}