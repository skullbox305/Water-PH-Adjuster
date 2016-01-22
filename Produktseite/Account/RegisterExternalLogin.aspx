<%@ Page Title="Externe Anmeldung registrieren" Language="C#" MasterPageFile="~/Site.Master" AutoEventWireup="true" CodeBehind="RegisterExternalLogin.aspx.cs" Inherits="Produktseite.Account.RegisterExternalLogin" Async="true" %>

<asp:Content runat="server" ID="BodyContent" ContentPlaceHolderID="MainContent">
<h3>Mit Ihrem <%: ProviderName %>-Konto registrieren</h3>

    <asp:PlaceHolder runat="server">
        <div class="form-horizontal">
            <h4>Zuordnungsformular</h4>
            <hr />
            <asp:ValidationSummary runat="server" ShowModelStateErrors="true" CssClass="text-danger" />
            <p class="text-info">
                Sie haben sich authentifiziert mit<strong><%: ProviderName %></strong>. Bitte geben Sie unten eine E-Mail-Adresse für die aktuelle Website ein,
                und klicken Sie dann auf die Schaltfläche "Anmelden".
            </p>

            <div class="form-group">
                <asp:Label runat="server" AssociatedControlID="email" CssClass="col-md-2 control-label">E-Mail</asp:Label>
                <div class="col-md-10">
                    <asp:TextBox runat="server" ID="email" CssClass="form-control" TextMode="Email" />
                    <asp:RequiredFieldValidator runat="server" ControlToValidate="email"
                        Display="Dynamic" CssClass="text-danger" ErrorMessage="Die E-Mail-Adresse ist erforderlich." />
                    <asp:ModelErrorMessage runat="server" ModelStateKey="email" CssClass="text-error" />
                </div>
            </div>

            <div class="form-group">
                <div class="col-md-offset-2 col-md-10">
                    <asp:Button runat="server" Text="Anmelden" CssClass="btn btn-default" OnClick="LogIn_Click" />
                </div>
            </div>
        </div>
    </asp:PlaceHolder>
</asp:Content>
