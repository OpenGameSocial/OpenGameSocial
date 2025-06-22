using System.Xml.Serialization;

namespace Builder;

[XmlRoot(ElementName = "PropertyGroup")]
public class PropertyGroupModel
{
    [XmlElement(ElementName = "TargetFramework")]
    public string TargetFramework { get; set; } = "net9.0";

    [XmlElement(ElementName = "ImplicitUsings")]
    public string ImplicitUsings { get; set; } = "enable";

    [XmlElement(ElementName = "Nullable")]
    public string Nullable { get; set; } = "enable";

    [XmlElement(ElementName = "EnableDefaultItems")]
    public string EnableDefaultItems { get; set; } = "false";
    
    [XmlElement(ElementName = "AppendTargetFrameworkToOutputPath")]
    public string AppendTargetFrameworkToOutputPath { get; set; } = "false";
    
    [XmlElement(ElementName = "DebugType")]
    public string DebugType { get; set; } = "portable";
    
    [XmlElement(ElementName = "DebugSymbols")]
    public string DebugSymbols { get; set; } = "true";
    
    [XmlElement(ElementName = "Optimize")]
    public string Optimize { get; set; } = "false";
}

[XmlRoot(ElementName = "Compile")]
public class CompileModel
{
    [XmlAttribute(AttributeName = "Include")]
    public required string Include { get; set; }
}

[XmlRoot(ElementName = "Reference")]
public class ReferenceModel
{
    [XmlElement(ElementName = "HintPath")]
    public required string HintPath { get; set; }

    [XmlAttribute(AttributeName = "Include")]
    public required string Include { get; set; }
}

[XmlRoot(ElementName = "ItemGroup")]
public class ItemGroupModel
{
    [XmlElement(ElementName = "Compile")]
    public List<CompileModel>? Compile { get; set; }

    [XmlElement(ElementName = "Reference")]
    public List<ReferenceModel>? Reference { get; set; }
}

[XmlRoot(ElementName = "Project")]
public class CsharpProjectModel
{
    [XmlElement(ElementName = "PropertyGroup")]
    public PropertyGroupModel PropertyGroup { get; set; } = new();

    [XmlElement(ElementName = "ItemGroup")]
    public ItemGroupModel ItemGroup { get; set; } = new();

    [XmlAttribute(AttributeName = "Sdk")]
    public string Sdk { get; set; } = "Microsoft.NET.Sdk";
}