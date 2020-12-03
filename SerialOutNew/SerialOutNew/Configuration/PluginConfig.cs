
using System.Runtime.CompilerServices;
using IPA.Config.Stores;

[assembly: InternalsVisibleTo(GeneratedStore.AssemblyVisibilityTarget)]
namespace SerialOutNew.Configuration
{
    internal class PluginConfig
    {
        public virtual SerialConfig SerialConfig { get; set; } = new SerialConfig();
    }
}
