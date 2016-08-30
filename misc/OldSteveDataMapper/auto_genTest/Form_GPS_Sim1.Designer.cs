namespace IngestionEngine
{
    partial class Form_GPS_Sim1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.panelEx1 = new DevComponents.DotNetBar.PanelEx();
            this.panelEx2 = new DevComponents.DotNetBar.PanelEx();
            this.lbWayPts = new System.Windows.Forms.ListBox();
            this.buttonX1 = new DevComponents.DotNetBar.ButtonX();
            this.groupPanel1 = new DevComponents.DotNetBar.Controls.GroupPanel();
            this.button1 = new System.Windows.Forms.Button();
            this.label7 = new System.Windows.Forms.Label();
            this.diCal = new DevComponents.Editors.DoubleInput();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.diVo2 = new DevComponents.Editors.DoubleInput();
            this.diCad = new DevComponents.Editors.DoubleInput();
            this.diHrt = new DevComponents.Editors.DoubleInput();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.diElv = new DevComponents.Editors.DoubleInput();
            this.diLon = new DevComponents.Editors.DoubleInput();
            this.diLat = new DevComponents.Editors.DoubleInput();
            this.groupPanel2 = new DevComponents.DotNetBar.Controls.GroupPanel();
            this.panelEx3 = new DevComponents.DotNetBar.PanelEx();
            this.layoutGroup1 = new DevComponents.DotNetBar.Layout.LayoutGroup();
            this.layoutGroup2 = new DevComponents.DotNetBar.Layout.LayoutGroup();
            this.layoutSpacerItem1 = new DevComponents.DotNetBar.Layout.LayoutSpacerItem();
            this.layoutGroup3 = new DevComponents.DotNetBar.Layout.LayoutGroup();
            this.tbResults = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.diStp = new DevComponents.Editors.DoubleInput();
            this.btReadRoute = new DevComponents.DotNetBar.ButtonX();
            this.panelEx2.SuspendLayout();
            this.groupPanel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.diCal)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.diVo2)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.diCad)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.diHrt)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.diElv)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.diLon)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.diLat)).BeginInit();
            this.groupPanel2.SuspendLayout();
            this.panelEx3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.diStp)).BeginInit();
            this.SuspendLayout();
            // 
            // panelEx1
            // 
            this.panelEx1.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx1.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.StyleManagerControlled;
            this.panelEx1.DisabledBackColor = System.Drawing.Color.Empty;
            this.panelEx1.Dock = System.Windows.Forms.DockStyle.Top;
            this.panelEx1.Location = new System.Drawing.Point(0, 0);
            this.panelEx1.Name = "panelEx1";
            this.panelEx1.Size = new System.Drawing.Size(1247, 53);
            this.panelEx1.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx1.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.panelEx1.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.panelEx1.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.panelEx1.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.panelEx1.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.panelEx1.Style.GradientAngle = 90;
            this.panelEx1.TabIndex = 0;
            this.panelEx1.Text = "panelEx1";
            // 
            // panelEx2
            // 
            this.panelEx2.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx2.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.StyleManagerControlled;
            this.panelEx2.Controls.Add(this.lbWayPts);
            this.panelEx2.Controls.Add(this.buttonX1);
            this.panelEx2.DisabledBackColor = System.Drawing.Color.Empty;
            this.panelEx2.Dock = System.Windows.Forms.DockStyle.Left;
            this.panelEx2.Location = new System.Drawing.Point(0, 53);
            this.panelEx2.Name = "panelEx2";
            this.panelEx2.Size = new System.Drawing.Size(271, 661);
            this.panelEx2.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx2.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.panelEx2.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.panelEx2.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.panelEx2.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.panelEx2.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.panelEx2.Style.GradientAngle = 90;
            this.panelEx2.TabIndex = 1;
            // 
            // lbWayPts
            // 
            this.lbWayPts.FormattingEnabled = true;
            this.lbWayPts.ItemHeight = 16;
            this.lbWayPts.Location = new System.Drawing.Point(13, 76);
            this.lbWayPts.MultiColumn = true;
            this.lbWayPts.Name = "lbWayPts";
            this.lbWayPts.Size = new System.Drawing.Size(240, 500);
            this.lbWayPts.TabIndex = 1;
            // 
            // buttonX1
            // 
            this.buttonX1.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.buttonX1.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.buttonX1.Location = new System.Drawing.Point(101, 21);
            this.buttonX1.Name = "buttonX1";
            this.buttonX1.Size = new System.Drawing.Size(152, 24);
            this.buttonX1.Style = DevComponents.DotNetBar.eDotNetBarStyle.StyleManagerControlled;
            this.buttonX1.TabIndex = 0;
            this.buttonX1.Text = "Simulate Real Data";
            this.buttonX1.Click += new System.EventHandler(this.buttonX1_Click);
            // 
            // groupPanel1
            // 
            this.groupPanel1.CanvasColor = System.Drawing.SystemColors.Control;
            this.groupPanel1.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.groupPanel1.Controls.Add(this.label8);
            this.groupPanel1.Controls.Add(this.diStp);
            this.groupPanel1.Controls.Add(this.button1);
            this.groupPanel1.Controls.Add(this.label7);
            this.groupPanel1.Controls.Add(this.diCal);
            this.groupPanel1.Controls.Add(this.label4);
            this.groupPanel1.Controls.Add(this.label5);
            this.groupPanel1.Controls.Add(this.label6);
            this.groupPanel1.Controls.Add(this.diVo2);
            this.groupPanel1.Controls.Add(this.diCad);
            this.groupPanel1.Controls.Add(this.diHrt);
            this.groupPanel1.Controls.Add(this.label3);
            this.groupPanel1.Controls.Add(this.label2);
            this.groupPanel1.Controls.Add(this.label1);
            this.groupPanel1.Controls.Add(this.diElv);
            this.groupPanel1.Controls.Add(this.diLon);
            this.groupPanel1.Controls.Add(this.diLat);
            this.groupPanel1.DisabledBackColor = System.Drawing.Color.Empty;
            this.groupPanel1.Location = new System.Drawing.Point(9, 12);
            this.groupPanel1.Margin = new System.Windows.Forms.Padding(0);
            this.groupPanel1.Name = "groupPanel1";
            this.groupPanel1.Size = new System.Drawing.Size(934, 89);
            // 
            // 
            // 
            this.groupPanel1.Style.BackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.groupPanel1.Style.BackColorGradientAngle = 90;
            this.groupPanel1.Style.BackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.groupPanel1.Style.BorderBottom = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel1.Style.BorderBottomWidth = 1;
            this.groupPanel1.Style.BorderColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.groupPanel1.Style.BorderLeft = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel1.Style.BorderLeftWidth = 1;
            this.groupPanel1.Style.BorderRight = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel1.Style.BorderRightWidth = 1;
            this.groupPanel1.Style.BorderTop = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel1.Style.BorderTopWidth = 1;
            this.groupPanel1.Style.CornerDiameter = 4;
            this.groupPanel1.Style.CornerType = DevComponents.DotNetBar.eCornerType.Rounded;
            this.groupPanel1.Style.TextAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Center;
            this.groupPanel1.Style.TextColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.groupPanel1.Style.TextLineAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Near;
            // 
            // 
            // 
            this.groupPanel1.StyleMouseDown.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            // 
            // 
            // 
            this.groupPanel1.StyleMouseOver.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.groupPanel1.TabIndex = 0;
            this.groupPanel1.Text = "GPS Simulator";
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(3, 8);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(64, 45);
            this.button1.TabIndex = 14;
            this.button1.Text = "Add Point";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(833, 8);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(59, 17);
            this.label7.TabIndex = 13;
            this.label7.Text = "Calories";
            // 
            // diCal
            // 
            // 
            // 
            // 
            this.diCal.BackgroundStyle.Class = "DateTimeInputBackground";
            this.diCal.BackgroundStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.diCal.ButtonFreeText.Shortcut = DevComponents.DotNetBar.eShortcut.F2;
            this.diCal.Increment = 1D;
            this.diCal.Location = new System.Drawing.Point(824, 31);
            this.diCal.Name = "diCal";
            this.diCal.ShowUpDown = true;
            this.diCal.Size = new System.Drawing.Size(94, 22);
            this.diCal.TabIndex = 12;
            this.diCal.Value = 2D;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(723, 8);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(38, 17);
            this.label4.TabIndex = 11;
            this.label4.Text = "vO2 ";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(610, 8);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(64, 17);
            this.label5.TabIndex = 10;
            this.label5.Text = "Cadance";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(498, 8);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(77, 17);
            this.label6.TabIndex = 9;
            this.label6.Text = "Heart Rate";
            // 
            // diVo2
            // 
            // 
            // 
            // 
            this.diVo2.BackgroundStyle.Class = "DateTimeInputBackground";
            this.diVo2.BackgroundStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.diVo2.ButtonFreeText.Shortcut = DevComponents.DotNetBar.eShortcut.F2;
            this.diVo2.DisplayFormat = "###.######";
            this.diVo2.Increment = 1D;
            this.diVo2.Location = new System.Drawing.Point(714, 31);
            this.diVo2.Name = "diVo2";
            this.diVo2.ShowUpDown = true;
            this.diVo2.Size = new System.Drawing.Size(94, 22);
            this.diVo2.TabIndex = 8;
            this.diVo2.Value = 12.99D;
            // 
            // diCad
            // 
            // 
            // 
            // 
            this.diCad.BackgroundStyle.Class = "DateTimeInputBackground";
            this.diCad.BackgroundStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.diCad.ButtonFreeText.Shortcut = DevComponents.DotNetBar.eShortcut.F2;
            this.diCad.Increment = 1D;
            this.diCad.Location = new System.Drawing.Point(601, 31);
            this.diCad.Name = "diCad";
            this.diCad.ShowUpDown = true;
            this.diCad.Size = new System.Drawing.Size(93, 22);
            this.diCad.TabIndex = 7;
            this.diCad.Value = 45D;
            // 
            // diHrt
            // 
            // 
            // 
            // 
            this.diHrt.BackgroundStyle.Class = "DateTimeInputBackground";
            this.diHrt.BackgroundStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.diHrt.ButtonFreeText.Shortcut = DevComponents.DotNetBar.eShortcut.F2;
            this.diHrt.Increment = 1D;
            this.diHrt.Location = new System.Drawing.Point(491, 31);
            this.diHrt.Name = "diHrt";
            this.diHrt.ShowUpDown = true;
            this.diHrt.Size = new System.Drawing.Size(94, 22);
            this.diHrt.TabIndex = 6;
            this.diHrt.Value = 85D;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(316, 8);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(66, 17);
            this.label3.TabIndex = 5;
            this.label3.Text = "Elevation";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(202, 8);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(71, 17);
            this.label2.TabIndex = 4;
            this.label2.Text = "Longitude";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(103, 8);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(55, 17);
            this.label1.TabIndex = 3;
            this.label1.Text = "Latiude";
            // 
            // diElv
            // 
            // 
            // 
            // 
            this.diElv.BackgroundStyle.Class = "DateTimeInputBackground";
            this.diElv.BackgroundStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.diElv.ButtonFreeText.Shortcut = DevComponents.DotNetBar.eShortcut.F2;
            this.diElv.DisplayFormat = "####.########";
            this.diElv.Increment = 1D;
            this.diElv.Location = new System.Drawing.Point(307, 31);
            this.diElv.Name = "diElv";
            this.diElv.Size = new System.Drawing.Size(75, 22);
            this.diElv.TabIndex = 2;
            this.diElv.Value = 12.21D;
            // 
            // diLon
            // 
            // 
            // 
            // 
            this.diLon.BackgroundStyle.Class = "DateTimeInputBackground";
            this.diLon.BackgroundStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.diLon.ButtonFreeText.Shortcut = DevComponents.DotNetBar.eShortcut.F2;
            this.diLon.DisplayFormat = "####.########";
            this.diLon.Increment = 1D;
            this.diLon.Location = new System.Drawing.Point(194, 31);
            this.diLon.Name = "diLon";
            this.diLon.Size = new System.Drawing.Size(93, 22);
            this.diLon.TabIndex = 1;
            this.diLon.Value = -118.314D;
            // 
            // diLat
            // 
            // 
            // 
            // 
            this.diLat.BackgroundStyle.Class = "DateTimeInputBackground";
            this.diLat.BackgroundStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.diLat.ButtonFreeText.Shortcut = DevComponents.DotNetBar.eShortcut.F2;
            this.diLat.DisplayFormat = "####.########";
            this.diLat.Increment = 1D;
            this.diLat.Location = new System.Drawing.Point(84, 31);
            this.diLat.Name = "diLat";
            this.diLat.Size = new System.Drawing.Size(93, 22);
            this.diLat.TabIndex = 0;
            this.diLat.Value = 34.78694D;
            // 
            // groupPanel2
            // 
            this.groupPanel2.CanvasColor = System.Drawing.SystemColors.Control;
            this.groupPanel2.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.Office2007;
            this.groupPanel2.Controls.Add(this.tbResults);
            this.groupPanel2.DisabledBackColor = System.Drawing.Color.Empty;
            this.groupPanel2.Location = new System.Drawing.Point(9, 142);
            this.groupPanel2.Margin = new System.Windows.Forms.Padding(0);
            this.groupPanel2.Name = "groupPanel2";
            this.groupPanel2.Size = new System.Drawing.Size(934, 519);
            // 
            // 
            // 
            this.groupPanel2.Style.BackColor2SchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.groupPanel2.Style.BackColorGradientAngle = 90;
            this.groupPanel2.Style.BackColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.groupPanel2.Style.BorderBottom = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel2.Style.BorderBottomWidth = 1;
            this.groupPanel2.Style.BorderColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.groupPanel2.Style.BorderLeft = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel2.Style.BorderLeftWidth = 1;
            this.groupPanel2.Style.BorderRight = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel2.Style.BorderRightWidth = 1;
            this.groupPanel2.Style.BorderTop = DevComponents.DotNetBar.eStyleBorderType.Solid;
            this.groupPanel2.Style.BorderTopWidth = 1;
            this.groupPanel2.Style.CornerDiameter = 4;
            this.groupPanel2.Style.CornerType = DevComponents.DotNetBar.eCornerType.Rounded;
            this.groupPanel2.Style.TextColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.groupPanel2.Style.TextLineAlignment = DevComponents.DotNetBar.eStyleTextAlignment.Near;
            // 
            // 
            // 
            this.groupPanel2.StyleMouseDown.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            // 
            // 
            // 
            this.groupPanel2.StyleMouseOver.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.groupPanel2.TabIndex = 2;
            this.groupPanel2.Text = "Results";
            // 
            // panelEx3
            // 
            this.panelEx3.CanvasColor = System.Drawing.SystemColors.Control;
            this.panelEx3.ColorSchemeStyle = DevComponents.DotNetBar.eDotNetBarStyle.StyleManagerControlled;
            this.panelEx3.Controls.Add(this.btReadRoute);
            this.panelEx3.Controls.Add(this.groupPanel1);
            this.panelEx3.Controls.Add(this.groupPanel2);
            this.panelEx3.DisabledBackColor = System.Drawing.Color.Empty;
            this.panelEx3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelEx3.Location = new System.Drawing.Point(271, 53);
            this.panelEx3.Name = "panelEx3";
            this.panelEx3.Size = new System.Drawing.Size(976, 661);
            this.panelEx3.Style.Alignment = System.Drawing.StringAlignment.Center;
            this.panelEx3.Style.BackColor1.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground;
            this.panelEx3.Style.BackColor2.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBackground2;
            this.panelEx3.Style.Border = DevComponents.DotNetBar.eBorderType.SingleLine;
            this.panelEx3.Style.BorderColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelBorder;
            this.panelEx3.Style.ForeColor.ColorSchemePart = DevComponents.DotNetBar.eColorSchemePart.PanelText;
            this.panelEx3.Style.GradientAngle = 90;
            this.panelEx3.TabIndex = 4;
            this.panelEx3.Text = "panelEx3";
            // 
            // layoutGroup1
            // 
            this.layoutGroup1.Height = 100;
            this.layoutGroup1.MinSize = new System.Drawing.Size(120, 32);
            this.layoutGroup1.Name = "layoutGroup1";
            this.layoutGroup1.TextPosition = DevComponents.DotNetBar.Layout.eLayoutPosition.Top;
            this.layoutGroup1.Width = 200;
            // 
            // layoutGroup2
            // 
            this.layoutGroup2.Appearance = DevComponents.DotNetBar.Layout.eGroupAppearance.GroupPanel;
            this.layoutGroup2.CaptionHeight = 18;
            this.layoutGroup2.Height = 100;
            this.layoutGroup2.MinSize = new System.Drawing.Size(120, 32);
            this.layoutGroup2.Name = "layoutGroup2";
            this.layoutGroup2.Text = "Caption";
            this.layoutGroup2.TextAlignment = DevComponents.DotNetBar.Layout.eTextAlignment.Center;
            this.layoutGroup2.TextLineAlignment = DevComponents.DotNetBar.Layout.eTextLineAlignment.Middle;
            this.layoutGroup2.TextPosition = DevComponents.DotNetBar.Layout.eLayoutPosition.Top;
            this.layoutGroup2.Width = 200;
            // 
            // layoutSpacerItem1
            // 
            this.layoutSpacerItem1.Height = 32;
            this.layoutSpacerItem1.Name = "layoutSpacerItem1";
            this.layoutSpacerItem1.Width = 32;
            // 
            // layoutGroup3
            // 
            this.layoutGroup3.Height = 100;
            this.layoutGroup3.MinSize = new System.Drawing.Size(120, 32);
            this.layoutGroup3.Name = "layoutGroup3";
            this.layoutGroup3.TextPosition = DevComponents.DotNetBar.Layout.eLayoutPosition.Top;
            this.layoutGroup3.Width = 200;
            // 
            // tbResults
            // 
            this.tbResults.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbResults.Location = new System.Drawing.Point(0, 0);
            this.tbResults.Multiline = true;
            this.tbResults.Name = "tbResults";
            this.tbResults.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.tbResults.Size = new System.Drawing.Size(928, 496);
            this.tbResults.TabIndex = 0;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(431, 8);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(44, 17);
            this.label8.TabIndex = 16;
            this.label8.Text = "Steps";
            // 
            // diStp
            // 
            // 
            // 
            // 
            this.diStp.BackgroundStyle.Class = "DateTimeInputBackground";
            this.diStp.BackgroundStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.diStp.ButtonFreeText.Shortcut = DevComponents.DotNetBar.eShortcut.F2;
            this.diStp.DisplayFormat = "####.########";
            this.diStp.Increment = 1D;
            this.diStp.Location = new System.Drawing.Point(400, 31);
            this.diStp.MinValue = 1D;
            this.diStp.Name = "diStp";
            this.diStp.Size = new System.Drawing.Size(75, 22);
            this.diStp.TabIndex = 15;
            this.diStp.Value = 2D;
            // 
            // btReadRoute
            // 
            this.btReadRoute.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btReadRoute.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.btReadRoute.Location = new System.Drawing.Point(15, 105);
            this.btReadRoute.Name = "btReadRoute";
            this.btReadRoute.Size = new System.Drawing.Size(102, 23);
            this.btReadRoute.Style = DevComponents.DotNetBar.eDotNetBarStyle.StyleManagerControlled;
            this.btReadRoute.TabIndex = 3;
            this.btReadRoute.Text = "buttonX2";
            this.btReadRoute.Click += new System.EventHandler(this.btReadRoute_Click);
            // 
            // Form_GPS_Sim1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1247, 714);
            this.Controls.Add(this.panelEx3);
            this.Controls.Add(this.panelEx2);
            this.Controls.Add(this.panelEx1);
            this.Name = "Form_GPS_Sim1";
            this.Text = "Form_GPS_Sim1";
            this.panelEx2.ResumeLayout(false);
            this.groupPanel1.ResumeLayout(false);
            this.groupPanel1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.diCal)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.diVo2)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.diCad)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.diHrt)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.diElv)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.diLon)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.diLat)).EndInit();
            this.groupPanel2.ResumeLayout(false);
            this.groupPanel2.PerformLayout();
            this.panelEx3.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.diStp)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private DevComponents.DotNetBar.PanelEx panelEx1;
        private DevComponents.DotNetBar.PanelEx panelEx2;
        private DevComponents.DotNetBar.ButtonX buttonX1;
        private DevComponents.DotNetBar.Controls.GroupPanel groupPanel1;
        private DevComponents.Editors.DoubleInput diElv;
        private DevComponents.Editors.DoubleInput diLon;
        private DevComponents.Editors.DoubleInput diLat;
        private DevComponents.DotNetBar.Controls.GroupPanel groupPanel2;
        private DevComponents.DotNetBar.PanelEx panelEx3;
        private DevComponents.DotNetBar.Layout.LayoutGroup layoutGroup1;
        private DevComponents.DotNetBar.Layout.LayoutGroup layoutGroup2;
        private DevComponents.DotNetBar.Layout.LayoutSpacerItem layoutSpacerItem1;
        private DevComponents.DotNetBar.Layout.LayoutGroup layoutGroup3;
        private System.Windows.Forms.ListBox lbWayPts;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Label label7;
        private DevComponents.Editors.DoubleInput diCal;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private DevComponents.Editors.DoubleInput diVo2;
        private DevComponents.Editors.DoubleInput diCad;
        private DevComponents.Editors.DoubleInput diHrt;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox tbResults;
        private System.Windows.Forms.Label label8;
        private DevComponents.Editors.DoubleInput diStp;
        private DevComponents.DotNetBar.ButtonX btReadRoute;

    }
}