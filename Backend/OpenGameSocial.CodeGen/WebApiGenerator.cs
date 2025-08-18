using System.Text;
using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp.Syntax;
using Microsoft.CodeAnalysis.Text;

namespace OpenGameSocial.CodeGen
{
    [Generator]
    public class WebApiGenerator : IIncrementalGenerator
    {
        public void Initialize(IncrementalGeneratorInitializationContext context)
        {
            var interfaces = context.SyntaxProvider
                .CreateSyntaxProvider(
                    predicate: static (node, _) => node is InterfaceDeclarationSyntax ids && ids.AttributeLists.Count > 0,
                    transform: static (ctx, _) => GetInterfaceSymbolIfApiEndpoint(ctx))
                .Where(symbol => symbol is not null)! // фильтруем null
                .Select((symbol, _) => (INamedTypeSymbol)symbol!); // снимаем nullable

            context.RegisterSourceOutput(interfaces.Collect(), static (spc, allSymbols) =>
            {
                foreach (var symbol in allSymbols.Distinct(SymbolEqualityComparer.Default))
                {
                    GenerateController(spc, (INamedTypeSymbol)symbol!);
                }
            });
        }


        private static INamedTypeSymbol? GetInterfaceSymbolIfApiEndpoint(GeneratorSyntaxContext ctx)
        {
            var ifaceDecl = (InterfaceDeclarationSyntax)ctx.Node;
            var symbol = ctx.SemanticModel.GetDeclaredSymbol(ifaceDecl) as INamedTypeSymbol;
            if (symbol == null)
                return null;

            foreach (var attr in symbol.GetAttributes())
            {
                if (attr.AttributeClass?.Name == "ApiEndpointAttribute" ||
                    attr.AttributeClass?.ToDisplayString() == "ApiEndpointAttribute")
                {
                    return symbol;
                }
            }

            return null;
        }

        private static void GenerateController(SourceProductionContext spc, INamedTypeSymbol iface)
        {
            var ns = iface.ContainingNamespace.IsGlobalNamespace
                ? null
                : iface.ContainingNamespace.ToDisplayString();

            var className = iface.Name.TrimStart('I') + "Controller";
            var routeName = iface.Name.TrimStart('I');

            var sb = new StringBuilder();

            sb.AppendLine("using Microsoft.AspNetCore.Mvc;");
            sb.AppendLine("using System.Threading.Tasks;");
            if (!string.IsNullOrEmpty(ns))
                sb.AppendLine($"namespace {ns} {{");

            sb.AppendLine($"[Route(\"{routeName}\")]");
            sb.AppendLine("class " + className + " : ControllerBase");
            sb.AppendLine("{");
            sb.AppendLine($"    private readonly {iface.ToDisplayString()} _endpoint;");
            sb.AppendLine();
            sb.AppendLine($"    public {className}({iface.ToDisplayString()} endpoint)");
            sb.AppendLine("    {");
            sb.AppendLine("        _endpoint = endpoint;");
            sb.AppendLine("    }");

            foreach (var method in iface.GetMembers().OfType<IMethodSymbol>())
            {
                if (method.MethodKind != MethodKind.Ordinary)
                    continue;

                var httpAttr = method.Name switch
                {
                    "Get" => "[HttpGet]",
                    "Post" => "[HttpPost]",
                    "Put" => "[HttpPut]",
                    "Delete" => "[HttpDelete]",
                    "Patch" => "[HttpPatch]",
                    _ => null
                };
                if (httpAttr == null) continue;

                sb.AppendLine();
                sb.AppendLine($"    {httpAttr}");
                sb.Append($"    public {method.ReturnType.ToDisplayString()} {method.Name}(");

                var paramStrings = method.Parameters.Select(p =>
                {
                    var sourceAttr = p.GetAttributes()
                        .Select(a => a.AttributeClass?.Name switch
                        {
                            "FromQueryParamAttribute" => "[FromQuery]",
                            "FromBodyParamAttribute" => "[FromBody]",
                            _ => null
                        })
                        .FirstOrDefault(a => a != null);

                    var attrPrefix = sourceAttr != null ? sourceAttr + " " : "";
                    return $"{attrPrefix}{p.Type.ToDisplayString()} {p.Name}";
                });

                sb.Append(string.Join(", ", paramStrings));
                sb.AppendLine(")");
                sb.AppendLine("    {");
                sb.Append("        return _endpoint." + method.Name + "(");
                sb.Append(string.Join(", ", method.Parameters.Select(p => p.Name)));
                sb.AppendLine(");");
                sb.AppendLine("    }");
            }

            sb.AppendLine("}"); // class
            if (!string.IsNullOrEmpty(ns))
                sb.AppendLine("}"); // namespace

            spc.AddSource($"{className}.g.cs", SourceText.From(sb.ToString(), Encoding.UTF8));
        }
    }
}