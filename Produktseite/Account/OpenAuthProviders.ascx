<%@ Control Language="C#" AutoEventWireup="true" CodeBehind="OpenAuthProviders.ascx.cs" Inherits="Produktseite.Account.OpenAuthProviders" %>

<div id="socialLoginList">
    <h4>Einen anderen Dienst zum Anmelden verwenden.</h4>
    <hr />
    <asp:ListView runat="server" ID="providerDetails" ItemType="System.String"
        SelectMethod="GetProviderNames" ViewStateMode="Disabled">
        <ItemTemplate>
            <p>
                <button type="submit" class="btn btn-default" name="provider" value="<%#: Item %>"
                    title="Anmelden mithilfe Ihres <%#: Item %> Kontos.">
                    <%#: Item %>
                </button>
            </p>
        </ItemTemplate>
        <EmptyDataTemplate>
            <div>
                <p>Es sind keine externen Authentifizierungsdienste konfiguriert. In <a href="http://go.microsoft.com/fwlink/?LinkId=252803">diesem Artikel</a> finden Sie weitere Informationen zum Einrichten dieser ASP.NET-Anwendung für die Unterstützung der Anmeldung über externe Dienste.</p>
            </div>
        </EmptyDataTemplate>
    </asp:ListView>
</div>
