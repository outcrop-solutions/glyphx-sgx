namespace IngestionEngine
{
    partial class Palette_Manager
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
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Palette_Manager));
            this.LowColorButton = new System.Windows.Forms.Button();
            this.HighColorButton = new System.Windows.Forms.Button();
            this.palette_man = new System.Windows.Forms.Panel();
            this.colorDialog1 = new System.Windows.Forms.ColorDialog();
            this.LowValueSlider = new System.Windows.Forms.TrackBar();
            this.labelLow = new System.Windows.Forms.Label();
            this.labelHigh = new System.Windows.Forms.Label();
            this.HighValueSlider = new System.Windows.Forms.TrackBar();
            this.pal_List = new System.Windows.Forms.ListBox();
            this.paletteGlyphClassBindingSource = new System.Windows.Forms.BindingSource(this.components);
            this.paletteSave = new System.Windows.Forms.Button();
            this.paletteShow = new System.Windows.Forms.Button();
            this.paletteDelete = new System.Windows.Forms.Button();
            this.paletteName = new System.Windows.Forms.TextBox();
            this.button1 = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.LowValueSlider)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.HighValueSlider)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.paletteGlyphClassBindingSource)).BeginInit();
            this.SuspendLayout();
            // 
            // LowColorButton
            // 
            this.LowColorButton.Location = new System.Drawing.Point(88, 261);
            this.LowColorButton.Name = "LowColorButton";
            this.LowColorButton.Size = new System.Drawing.Size(171, 23);
            this.LowColorButton.TabIndex = 0;
            this.LowColorButton.Text = "Low Color";
            this.LowColorButton.UseVisualStyleBackColor = true;
            this.LowColorButton.Click += new System.EventHandler(this.StartButton_Click);
            // 
            // HighColorButton
            // 
            this.HighColorButton.Location = new System.Drawing.Point(35, 26);
            this.HighColorButton.Name = "HighColorButton";
            this.HighColorButton.Size = new System.Drawing.Size(171, 23);
            this.HighColorButton.TabIndex = 1;
            this.HighColorButton.Text = "High Color";
            this.HighColorButton.UseVisualStyleBackColor = true;
            this.HighColorButton.Click += new System.EventHandler(this.EndButton_Click);
            // 
            // palette_man
            // 
            this.palette_man.Location = new System.Drawing.Point(88, 55);
            this.palette_man.Name = "palette_man";
            this.palette_man.Size = new System.Drawing.Size(118, 200);
            this.palette_man.TabIndex = 2;
            this.palette_man.Paint += new System.Windows.Forms.PaintEventHandler(this.palette_man_Paint);
            this.palette_man.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.palette_man_MouseDoubleClick);
            // 
            // LowValueSlider
            // 
            this.LowValueSlider.LargeChange = 100;
            this.LowValueSlider.Location = new System.Drawing.Point(39, 145);
            this.LowValueSlider.Maximum = 999;
            this.LowValueSlider.Name = "LowValueSlider";
            this.LowValueSlider.Orientation = System.Windows.Forms.Orientation.Vertical;
            this.LowValueSlider.Size = new System.Drawing.Size(53, 110);
            this.LowValueSlider.TabIndex = 3;
            this.LowValueSlider.TickFrequency = 100;
            this.LowValueSlider.Visible = false;
            this.LowValueSlider.Scroll += new System.EventHandler(this.LowValueSlider_Scroll);
            // 
            // labelLow
            // 
            this.labelLow.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.labelLow.Location = new System.Drawing.Point(32, 257);
            this.labelLow.MaximumSize = new System.Drawing.Size(50, 30);
            this.labelLow.MinimumSize = new System.Drawing.Size(50, 30);
            this.labelLow.Name = "labelLow";
            this.labelLow.Size = new System.Drawing.Size(50, 30);
            this.labelLow.TabIndex = 4;
            this.labelLow.Text = "0";
            this.labelLow.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.labelLow.Visible = false;
            // 
            // labelHigh
            // 
            this.labelHigh.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.labelHigh.Location = new System.Drawing.Point(209, 22);
            this.labelHigh.MaximumSize = new System.Drawing.Size(50, 30);
            this.labelHigh.MinimumSize = new System.Drawing.Size(50, 30);
            this.labelHigh.Name = "labelHigh";
            this.labelHigh.Size = new System.Drawing.Size(50, 30);
            this.labelHigh.TabIndex = 5;
            this.labelHigh.Text = "1000";
            this.labelHigh.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.labelHigh.Visible = false;
            // 
            // HighValueSlider
            // 
            this.HighValueSlider.LargeChange = 100;
            this.HighValueSlider.Location = new System.Drawing.Point(212, 55);
            this.HighValueSlider.Maximum = 1000;
            this.HighValueSlider.Minimum = 1;
            this.HighValueSlider.Name = "HighValueSlider";
            this.HighValueSlider.Orientation = System.Windows.Forms.Orientation.Vertical;
            this.HighValueSlider.Size = new System.Drawing.Size(53, 110);
            this.HighValueSlider.TabIndex = 6;
            this.HighValueSlider.TickFrequency = 100;
            this.HighValueSlider.TickStyle = System.Windows.Forms.TickStyle.TopLeft;
            this.HighValueSlider.Value = 1000;
            this.HighValueSlider.Visible = false;
            this.HighValueSlider.Scroll += new System.EventHandler(this.trackBar2_Scroll);
            // 
            // pal_List
            // 
            this.pal_List.DataSource = this.paletteGlyphClassBindingSource;
            this.pal_List.DisplayMember = "name";
            this.pal_List.FormattingEnabled = true;
            this.pal_List.ItemHeight = 16;
            this.pal_List.Location = new System.Drawing.Point(444, 26);
            this.pal_List.Name = "pal_List";
            this.pal_List.Size = new System.Drawing.Size(219, 260);
            this.pal_List.TabIndex = 7;
            this.pal_List.ValueMember = "name";
            this.pal_List.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.pal_man_DoubleClick);
            // 
            // paletteGlyphClassBindingSource
            // 
            this.paletteGlyphClassBindingSource.DataSource = typeof(IngestionEngine.Palette_Glyph_Class);
            // 
            // paletteSave
            // 
            this.paletteSave.Enabled = false;
            this.paletteSave.Location = new System.Drawing.Point(325, 170);
            this.paletteSave.Name = "paletteSave";
            this.paletteSave.Size = new System.Drawing.Size(113, 23);
            this.paletteSave.TabIndex = 8;
            this.paletteSave.Text = "Save Palatte";
            this.paletteSave.UseVisualStyleBackColor = true;
            this.paletteSave.Click += new System.EventHandler(this.paletteSave_Click);
            // 
            // paletteShow
            // 
            this.paletteShow.Location = new System.Drawing.Point(325, 90);
            this.paletteShow.Name = "paletteShow";
            this.paletteShow.Size = new System.Drawing.Size(113, 23);
            this.paletteShow.TabIndex = 9;
            this.paletteShow.Text = "Show Palatte";
            this.paletteShow.UseVisualStyleBackColor = true;
            this.paletteShow.Click += new System.EventHandler(this.paletteShow_Click);
            // 
            // paletteDelete
            // 
            this.paletteDelete.Enabled = false;
            this.paletteDelete.Location = new System.Drawing.Point(325, 130);
            this.paletteDelete.Name = "paletteDelete";
            this.paletteDelete.Size = new System.Drawing.Size(113, 23);
            this.paletteDelete.TabIndex = 10;
            this.paletteDelete.Text = "Delete Palatte";
            this.paletteDelete.UseVisualStyleBackColor = true;
            this.paletteDelete.Click += new System.EventHandler(this.paletteDelete_Click);
            // 
            // paletteName
            // 
            this.paletteName.Enabled = false;
            this.paletteName.Location = new System.Drawing.Point(325, 210);
            this.paletteName.MaxLength = 128;
            this.paletteName.Name = "paletteName";
            this.paletteName.Size = new System.Drawing.Size(113, 22);
            this.paletteName.TabIndex = 11;
            this.paletteName.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.paletteName_KeyPress);
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(325, 55);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(113, 23);
            this.button1.TabIndex = 12;
            this.button1.Text = "Demo Palatte";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // Palette_Manager
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(685, 306);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.paletteName);
            this.Controls.Add(this.paletteDelete);
            this.Controls.Add(this.paletteShow);
            this.Controls.Add(this.paletteSave);
            this.Controls.Add(this.pal_List);
            this.Controls.Add(this.HighValueSlider);
            this.Controls.Add(this.labelHigh);
            this.Controls.Add(this.labelLow);
            this.Controls.Add(this.palette_man);
            this.Controls.Add(this.HighColorButton);
            this.Controls.Add(this.LowColorButton);
            this.Controls.Add(this.LowValueSlider);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "Palette_Manager";
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.Text = "Color Palette Editor";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Palette_Manager_FormClosing);
            ((System.ComponentModel.ISupportInitialize)(this.LowValueSlider)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.HighValueSlider)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.paletteGlyphClassBindingSource)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button LowColorButton;
        private System.Windows.Forms.Button HighColorButton;
        private System.Windows.Forms.Panel palette_man;
        private System.Windows.Forms.ColorDialog colorDialog1;
        private System.Windows.Forms.TrackBar LowValueSlider;
        private System.Windows.Forms.Label labelLow;
        private System.Windows.Forms.Label labelHigh;
        private System.Windows.Forms.TrackBar HighValueSlider;
        private System.Windows.Forms.ListBox pal_List;
        private System.Windows.Forms.BindingSource paletteGlyphClassBindingSource;
        private System.Windows.Forms.Button paletteSave;
        private System.Windows.Forms.Button paletteShow;
        private System.Windows.Forms.Button paletteDelete;
        private System.Windows.Forms.TextBox paletteName;
        private System.Windows.Forms.Button button1;
    }
}