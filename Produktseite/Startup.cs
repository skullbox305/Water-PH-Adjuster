using Microsoft.Owin;
using Owin;

[assembly: OwinStartupAttribute(typeof(Produktseite.Startup))]
namespace Produktseite
{
    public partial class Startup {
        public void Configuration(IAppBuilder app) {
            ConfigureAuth(app);
        }
    }
}
