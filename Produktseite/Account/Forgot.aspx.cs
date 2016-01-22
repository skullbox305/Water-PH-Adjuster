using System;
using System.Web;
using System.Web.UI;
using Microsoft.AspNet.Identity;
using Microsoft.AspNet.Identity.Owin;
using Owin;
using Produktseite.Models;

namespace Produktseite.Account
{
    public partial class ForgotPassword : Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
        }

        protected void Forgot(object sender, EventArgs e)
        {
            if (IsValid)
            {
                // E-Mail-Adresse des Benutzers überprüfen
                var manager = Context.GetOwinContext().GetUserManager<ApplicationUserManager>();
                ApplicationUser user = manager.FindByName(Email.Text);
                if (user == null || !manager.IsEmailConfirmed(user.Id))
                {
                    FailureText.Text = "Der Benutzer ist nicht vorhanden oder wurde nicht bestätigt.";
                    ErrorMessage.Visible = true;
                    return;
                }
                // Weitere Informationen zum Aktivieren der Kontobestätigung und Kennwortzurücksetzung finden Sie unter "http://go.microsoft.com/fwlink/?LinkID=320771".
                // E-Mail mit dem Code und die Umleitung zur Seite zum Zurücksetzen des Kennworts senden
                //string code = manager.GeneratePasswordResetToken(user.Id);
                //string callbackUrl = IdentityHelper.GetResetPasswordRedirectUrl(code, Request);
                //manager.SendEmail(user.Id, "Kennwort zurücksetzen", "Bitte setzen Sie Ihr Kennwort zurück. Klicken Sie dazu <a href=\"" + callbackUrl + "\">hier</a>.");
                loginForm.Visible = false;
                DisplayEmail.Visible = true;
            }
        }
    }
}