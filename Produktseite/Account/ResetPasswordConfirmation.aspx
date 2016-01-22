<%@ Page Title="Das Kennwort wurde geändert." Language="C#" MasterPageFile="~/Site.Master" AutoEventWireup="true" CodeBehind="ResetPasswordConfirmation.aspx.cs" Inherits="Produktseite.Account.ResetPasswordConfirmation" Async="true" %>

<asp:Content runat="server" ID="BodyContent" ContentPlaceHolderID="MainContent">
    <h2><%: Title %>.</h2>
    <div>
        <p>Ihr Kennwort wurde geändert. Klicken Sie <asp:HyperLink ID="login" runat="server" NavigateUrl="~/Account/Login">hier</asp:HyperLink> um sich anzumelden. </p>
    </div>
</asp:Content>
