namespace IngestionEngine
{
    partial class Form_SmartDecimation
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
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.tbMain = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.labelX3 = new DevComponents.DotNetBar.LabelX();
            this.diElevation = new DevComponents.Editors.DoubleInput();
            this.labelX2 = new DevComponents.DotNetBar.LabelX();
            this.diSkipFactor = new DevComponents.Editors.DoubleInput();
            this.tbOUTFile = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.labelX1 = new DevComponents.DotNetBar.LabelX();
            this.diDistance = new DevComponents.Editors.DoubleInput();
            this.btX_Run = new DevComponents.DotNetBar.ButtonX();
            this.tbLASFile = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.btXLAS = new DevComponents.DotNetBar.ButtonX();
            this.tbGPSFile = new DevComponents.DotNetBar.Controls.TextBoxX();
            this.btXGPS = new DevComponents.DotNetBar.ButtonX();
            this.saveFileDialog1 = new System.Windows.Forms.SaveFileDialog();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.diElevation)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.diSkipFactor)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.diDistance)).BeginInit();
            this.SuspendLayout();
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.tbMain);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.labelX3);
            this.splitContainer1.Panel2.Controls.Add(this.diElevation);
            this.splitContainer1.Panel2.Controls.Add(this.labelX2);
            this.splitContainer1.Panel2.Controls.Add(this.diSkipFactor);
            this.splitContainer1.Panel2.Controls.Add(this.tbOUTFile);
            this.splitContainer1.Panel2.Controls.Add(this.labelX1);
            this.splitContainer1.Panel2.Controls.Add(this.diDistance);
            this.splitContainer1.Panel2.Controls.Add(this.btX_Run);
            this.splitContainer1.Panel2.Controls.Add(this.tbLASFile);
            this.splitContainer1.Panel2.Controls.Add(this.btXLAS);
            this.splitContainer1.Panel2.Controls.Add(this.tbGPSFile);
            this.splitContainer1.Panel2.Controls.Add(this.btXGPS);
            this.splitContainer1.Size = new System.Drawing.Size(1028, 507);
            this.splitContainer1.SplitterDistance = 100;
            this.splitContainer1.TabIndex = 0;
            // 
            // tbMain
            // 
            // 
            // 
            // 
            this.tbMain.Border.Class = "TextBoxBorder";
            this.tbMain.Border.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.tbMain.Location = new System.Drawing.Point(12, 12);
            this.tbMain.Multiline = true;
            this.tbMain.Name = "tbMain";
            this.tbMain.PreventEnterBeep = true;
            this.tbMain.Size = new System.Drawing.Size(846, 71);
            this.tbMain.TabIndex = 0;
            this.tbMain.Text = "Step 1. Choose a gps track\r\nStep 2. Choose the LAS file to process\r\nStep 3. Run t" +
    "he process";
            // 
            // labelX3
            // 
            this.labelX3.BackColor = System.Drawing.SystemColors.GradientActiveCaption;
            // 
            // 
            // 
            this.labelX3.BackgroundStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.labelX3.Location = new System.Drawing.Point(333, 163);
            this.labelX3.Name = "labelX3";
            this.labelX3.Size = new System.Drawing.Size(108, 23);
            this.labelX3.TabIndex = 11;
            this.labelX3.Text = "Elevation";
            // 
            // diElevation
            // 
            // 
            // 
            // 
            this.diElevation.BackgroundStyle.Class = "DateTimeInputBackground";
            this.diElevation.BackgroundStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.diElevation.ButtonFreeText.Shortcut = DevComponents.DotNetBar.eShortcut.F2;
            this.diElevation.DisplayFormat = "###.##########";
            this.diElevation.Increment = 1D;
            this.diElevation.Location = new System.Drawing.Point(333, 211);
            this.diElevation.MinValue = 77D;
            this.diElevation.Name = "diElevation";
            this.diElevation.ShowUpDown = true;
            this.diElevation.Size = new System.Drawing.Size(108, 22);
            this.diElevation.TabIndex = 10;
            this.diElevation.Value = 300D;
            // 
            // labelX2
            // 
            this.labelX2.BackColor = System.Drawing.SystemColors.GradientActiveCaption;
            // 
            // 
            // 
            this.labelX2.BackgroundStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.labelX2.Location = new System.Drawing.Point(533, 163);
            this.labelX2.Name = "labelX2";
            this.labelX2.Size = new System.Drawing.Size(108, 23);
            this.labelX2.TabIndex = 9;
            this.labelX2.Text = "Skip Factor";
            // 
            // diSkipFactor
            // 
            // 
            // 
            // 
            this.diSkipFactor.BackgroundStyle.Class = "DateTimeInputBackground";
            this.diSkipFactor.BackgroundStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.diSkipFactor.ButtonFreeText.Shortcut = DevComponents.DotNetBar.eShortcut.F2;
            this.diSkipFactor.Increment = 1D;
            this.diSkipFactor.Location = new System.Drawing.Point(533, 211);
            this.diSkipFactor.MaxValue = 101D;
            this.diSkipFactor.MinValue = 1D;
            this.diSkipFactor.Name = "diSkipFactor";
            this.diSkipFactor.ShowUpDown = true;
            this.diSkipFactor.Size = new System.Drawing.Size(108, 22);
            this.diSkipFactor.TabIndex = 8;
            this.diSkipFactor.Value = 1D;
            // 
            // tbOUTFile
            // 
            // 
            // 
            // 
            this.tbOUTFile.Border.Class = "TextBoxBorder";
            this.tbOUTFile.Border.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.tbOUTFile.Location = new System.Drawing.Point(148, 124);
            this.tbOUTFile.Name = "tbOUTFile";
            this.tbOUTFile.PreventEnterBeep = true;
            this.tbOUTFile.Size = new System.Drawing.Size(710, 22);
            this.tbOUTFile.TabIndex = 7;
            this.tbOUTFile.Visible = false;
            // 
            // labelX1
            // 
            this.labelX1.BackColor = System.Drawing.SystemColors.GradientActiveCaption;
            // 
            // 
            // 
            this.labelX1.BackgroundStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.labelX1.Location = new System.Drawing.Point(148, 163);
            this.labelX1.Name = "labelX1";
            this.labelX1.Size = new System.Drawing.Size(108, 23);
            this.labelX1.TabIndex = 6;
            this.labelX1.Text = "Distance";
            // 
            // diDistance
            // 
            // 
            // 
            // 
            this.diDistance.BackgroundStyle.Class = "DateTimeInputBackground";
            this.diDistance.BackgroundStyle.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.diDistance.ButtonFreeText.Shortcut = DevComponents.DotNetBar.eShortcut.F2;
            this.diDistance.DisplayFormat = "###.##########";
            this.diDistance.Increment = 0.001D;
            this.diDistance.Location = new System.Drawing.Point(148, 211);
            this.diDistance.MinValue = 2.748754E-05D;
            this.diDistance.Name = "diDistance";
            this.diDistance.ShowUpDown = true;
            this.diDistance.Size = new System.Drawing.Size(108, 22);
            this.diDistance.TabIndex = 5;
            this.diDistance.Value = 0.0002748754D;
            // 
            // btX_Run
            // 
            this.btX_Run.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btX_Run.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.btX_Run.Enabled = false;
            this.btX_Run.Location = new System.Drawing.Point(693, 185);
            this.btX_Run.Name = "btX_Run";
            this.btX_Run.Size = new System.Drawing.Size(144, 23);
            this.btX_Run.Style = DevComponents.DotNetBar.eDotNetBarStyle.StyleManagerControlled;
            this.btX_Run.TabIndex = 4;
            this.btX_Run.Text = "Run SLERP";
            this.btX_Run.Click += new System.EventHandler(this.btx_Run_Click);
            // 
            // tbLASFile
            // 
            // 
            // 
            // 
            this.tbLASFile.Border.Class = "TextBoxBorder";
            this.tbLASFile.Border.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.tbLASFile.Location = new System.Drawing.Point(148, 79);
            this.tbLASFile.Name = "tbLASFile";
            this.tbLASFile.PreventEnterBeep = true;
            this.tbLASFile.Size = new System.Drawing.Size(710, 22);
            this.tbLASFile.TabIndex = 3;
            // 
            // btXLAS
            // 
            this.btXLAS.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btXLAS.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.btXLAS.Location = new System.Drawing.Point(41, 79);
            this.btXLAS.Name = "btXLAS";
            this.btXLAS.Size = new System.Drawing.Size(75, 23);
            this.btXLAS.Style = DevComponents.DotNetBar.eDotNetBarStyle.StyleManagerControlled;
            this.btXLAS.TabIndex = 2;
            this.btXLAS.Text = "LAS File";
            this.btXLAS.Click += new System.EventHandler(this.btXLAS_Click);
            // 
            // tbGPSFile
            // 
            // 
            // 
            // 
            this.tbGPSFile.Border.Class = "TextBoxBorder";
            this.tbGPSFile.Border.CornerType = DevComponents.DotNetBar.eCornerType.Square;
            this.tbGPSFile.Location = new System.Drawing.Point(148, 28);
            this.tbGPSFile.Name = "tbGPSFile";
            this.tbGPSFile.PreventEnterBeep = true;
            this.tbGPSFile.Size = new System.Drawing.Size(710, 22);
            this.tbGPSFile.TabIndex = 1;
            // 
            // btXGPS
            // 
            this.btXGPS.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.btXGPS.ColorTable = DevComponents.DotNetBar.eButtonColor.OrangeWithBackground;
            this.btXGPS.Location = new System.Drawing.Point(41, 28);
            this.btXGPS.Name = "btXGPS";
            this.btXGPS.Size = new System.Drawing.Size(75, 23);
            this.btXGPS.Style = DevComponents.DotNetBar.eDotNetBarStyle.StyleManagerControlled;
            this.btXGPS.TabIndex = 0;
            this.btXGPS.Text = "GPS File";
            this.btXGPS.Click += new System.EventHandler(this.btXGPS_Click);
            // 
            // Form_SmartDecimation
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.Desktop;
            this.ClientSize = new System.Drawing.Size(1028, 507);
            this.Controls.Add(this.splitContainer1);
            this.Name = "Form_SmartDecimation";
            this.Text = "SLERP: Smart LAS Extraction Reduction Process";
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.diElevation)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.diSkipFactor)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.diDistance)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer1;
        private DevComponents.DotNetBar.Controls.TextBoxX tbMain;
        private DevComponents.DotNetBar.LabelX labelX1;
        private DevComponents.Editors.DoubleInput diDistance;
        private DevComponents.DotNetBar.ButtonX btX_Run;
        private DevComponents.DotNetBar.Controls.TextBoxX tbLASFile;
        private DevComponents.DotNetBar.ButtonX btXLAS;
        private DevComponents.DotNetBar.Controls.TextBoxX tbGPSFile;
        private DevComponents.DotNetBar.ButtonX btXGPS;
        private DevComponents.DotNetBar.LabelX labelX2;
        private DevComponents.Editors.DoubleInput diSkipFactor;
        private DevComponents.DotNetBar.Controls.TextBoxX tbOUTFile;
        private System.Windows.Forms.SaveFileDialog saveFileDialog1;
        private DevComponents.DotNetBar.LabelX labelX3;
        private DevComponents.Editors.DoubleInput diElevation;

    }
}