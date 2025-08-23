namespace OpenGameSocial.Abstractions.DependencyInjection;

[AttributeUsage(AttributeTargets.Class)]
public class RegisterConfigurationAttribute : Attribute
{
    public readonly string Section;

    public RegisterConfigurationAttribute(string section)
    {
        Section = section;
    }
}