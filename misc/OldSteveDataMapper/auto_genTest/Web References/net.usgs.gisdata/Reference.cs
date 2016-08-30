﻿//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//     Runtime Version:4.0.30319.18444
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

// 
// This source code was auto-generated by Microsoft.VSDesigner, Version 4.0.30319.18444.
// 
#pragma warning disable 1591

namespace IngestionEngine.net.usgs.gisdata {
    using System;
    using System.Web.Services;
    using System.Diagnostics;
    using System.Web.Services.Protocols;
    using System.Xml.Serialization;
    using System.ComponentModel;
    
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Web.Services", "4.0.30319.18408")]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Web.Services.WebServiceBindingAttribute(Name="Elevation_ServiceSoap", Namespace="http://gisdata.usgs.gov/XMLWebServices2/")]
    public partial class Elevation_Service : System.Web.Services.Protocols.SoapHttpClientProtocol {
        
        private System.Threading.SendOrPostCallback getElevationOperationCompleted;
        
        private System.Threading.SendOrPostCallback getAllElevationsOperationCompleted;
        
        private bool useDefaultCredentialsSetExplicitly;
        
        /// <remarks/>
        public Elevation_Service() {
            this.Url = global::IngestionEngine.Properties.Settings.Default.AntzImporter_net_usgs_gisdata_Elevation_Service;
            if ((this.IsLocalFileSystemWebService(this.Url) == true)) {
                this.UseDefaultCredentials = true;
                this.useDefaultCredentialsSetExplicitly = false;
            }
            else {
                this.useDefaultCredentialsSetExplicitly = true;
            }
        }
        
        public new string Url {
            get {
                return base.Url;
            }
            set {
                if ((((this.IsLocalFileSystemWebService(base.Url) == true) 
                            && (this.useDefaultCredentialsSetExplicitly == false)) 
                            && (this.IsLocalFileSystemWebService(value) == false))) {
                    base.UseDefaultCredentials = false;
                }
                base.Url = value;
            }
        }
        
        public new bool UseDefaultCredentials {
            get {
                return base.UseDefaultCredentials;
            }
            set {
                base.UseDefaultCredentials = value;
                this.useDefaultCredentialsSetExplicitly = true;
            }
        }
        
        /// <remarks/>
        public event getElevationCompletedEventHandler getElevationCompleted;
        
        /// <remarks/>
        public event getAllElevationsCompletedEventHandler getAllElevationsCompleted;
        
        /// <remarks/>
        [System.Web.Services.Protocols.SoapDocumentMethodAttribute("http://gisdata.usgs.gov/XMLWebServices2/getElevation", RequestNamespace="http://gisdata.usgs.gov/XMLWebServices2/", ResponseNamespace="http://gisdata.usgs.gov/XMLWebServices2/", Use=System.Web.Services.Description.SoapBindingUse.Literal, ParameterStyle=System.Web.Services.Protocols.SoapParameterStyle.Wrapped)]
        public System.Xml.XmlNode getElevation(string X_Value, string Y_Value, string Elevation_Units, string Source_Layer, string Elevation_Only) {
            object[] results = this.Invoke("getElevation", new object[] {
                        X_Value,
                        Y_Value,
                        Elevation_Units,
                        Source_Layer,
                        Elevation_Only});
            return ((System.Xml.XmlNode)(results[0]));
        }
        
        /// <remarks/>
        public void getElevationAsync(string X_Value, string Y_Value, string Elevation_Units, string Source_Layer, string Elevation_Only) {
            this.getElevationAsync(X_Value, Y_Value, Elevation_Units, Source_Layer, Elevation_Only, null);
        }
        
        /// <remarks/>
        public void getElevationAsync(string X_Value, string Y_Value, string Elevation_Units, string Source_Layer, string Elevation_Only, object userState) {
            if ((this.getElevationOperationCompleted == null)) {
                this.getElevationOperationCompleted = new System.Threading.SendOrPostCallback(this.OngetElevationOperationCompleted);
            }
            this.InvokeAsync("getElevation", new object[] {
                        X_Value,
                        Y_Value,
                        Elevation_Units,
                        Source_Layer,
                        Elevation_Only}, this.getElevationOperationCompleted, userState);
        }
        
        private void OngetElevationOperationCompleted(object arg) {
            if ((this.getElevationCompleted != null)) {
                System.Web.Services.Protocols.InvokeCompletedEventArgs invokeArgs = ((System.Web.Services.Protocols.InvokeCompletedEventArgs)(arg));
                this.getElevationCompleted(this, new getElevationCompletedEventArgs(invokeArgs.Results, invokeArgs.Error, invokeArgs.Cancelled, invokeArgs.UserState));
            }
        }
        
        /// <remarks/>
        [System.Web.Services.Protocols.SoapDocumentMethodAttribute("http://gisdata.usgs.gov/XMLWebServices2/getAllElevations", RequestNamespace="http://gisdata.usgs.gov/XMLWebServices2/", ResponseNamespace="http://gisdata.usgs.gov/XMLWebServices2/", Use=System.Web.Services.Description.SoapBindingUse.Literal, ParameterStyle=System.Web.Services.Protocols.SoapParameterStyle.Wrapped)]
        public System.Xml.XmlNode getAllElevations(string X_Value, string Y_Value, string Elevation_Units) {
            object[] results = this.Invoke("getAllElevations", new object[] {
                        X_Value,
                        Y_Value,
                        Elevation_Units});
            return ((System.Xml.XmlNode)(results[0]));
        }
        
        /// <remarks/>
        public void getAllElevationsAsync(string X_Value, string Y_Value, string Elevation_Units) {
            this.getAllElevationsAsync(X_Value, Y_Value, Elevation_Units, null);
        }
        
        /// <remarks/>
        public void getAllElevationsAsync(string X_Value, string Y_Value, string Elevation_Units, object userState) {
            if ((this.getAllElevationsOperationCompleted == null)) {
                this.getAllElevationsOperationCompleted = new System.Threading.SendOrPostCallback(this.OngetAllElevationsOperationCompleted);
            }
            this.InvokeAsync("getAllElevations", new object[] {
                        X_Value,
                        Y_Value,
                        Elevation_Units}, this.getAllElevationsOperationCompleted, userState);
        }
        
        private void OngetAllElevationsOperationCompleted(object arg) {
            if ((this.getAllElevationsCompleted != null)) {
                System.Web.Services.Protocols.InvokeCompletedEventArgs invokeArgs = ((System.Web.Services.Protocols.InvokeCompletedEventArgs)(arg));
                this.getAllElevationsCompleted(this, new getAllElevationsCompletedEventArgs(invokeArgs.Results, invokeArgs.Error, invokeArgs.Cancelled, invokeArgs.UserState));
            }
        }
        
        /// <remarks/>
        public new void CancelAsync(object userState) {
            base.CancelAsync(userState);
        }
        
        private bool IsLocalFileSystemWebService(string url) {
            if (((url == null) 
                        || (url == string.Empty))) {
                return false;
            }
            System.Uri wsUri = new System.Uri(url);
            if (((wsUri.Port >= 1024) 
                        && (string.Compare(wsUri.Host, "localHost", System.StringComparison.OrdinalIgnoreCase) == 0))) {
                return true;
            }
            return false;
        }
    }
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Web.Services", "4.0.30319.18408")]
    public delegate void getElevationCompletedEventHandler(object sender, getElevationCompletedEventArgs e);
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Web.Services", "4.0.30319.18408")]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    public partial class getElevationCompletedEventArgs : System.ComponentModel.AsyncCompletedEventArgs {
        
        private object[] results;
        
        internal getElevationCompletedEventArgs(object[] results, System.Exception exception, bool cancelled, object userState) : 
                base(exception, cancelled, userState) {
            this.results = results;
        }
        
        /// <remarks/>
        public System.Xml.XmlNode Result {
            get {
                this.RaiseExceptionIfNecessary();
                return ((System.Xml.XmlNode)(this.results[0]));
            }
        }
    }
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Web.Services", "4.0.30319.18408")]
    public delegate void getAllElevationsCompletedEventHandler(object sender, getAllElevationsCompletedEventArgs e);
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Web.Services", "4.0.30319.18408")]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    public partial class getAllElevationsCompletedEventArgs : System.ComponentModel.AsyncCompletedEventArgs {
        
        private object[] results;
        
        internal getAllElevationsCompletedEventArgs(object[] results, System.Exception exception, bool cancelled, object userState) : 
                base(exception, cancelled, userState) {
            this.results = results;
        }
        
        /// <remarks/>
        public System.Xml.XmlNode Result {
            get {
                this.RaiseExceptionIfNecessary();
                return ((System.Xml.XmlNode)(this.results[0]));
            }
        }
    }
}

#pragma warning restore 1591