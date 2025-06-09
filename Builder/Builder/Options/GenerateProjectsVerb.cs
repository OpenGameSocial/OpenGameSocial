using CommandLine;

namespace Builder.Options;

[Verb("projgen")]
public class GenerateProjectsVerb : CommandLine
{
    [Option('t', "targetIde", Required = true)]
    public required string TargetIde { get; set; }
}