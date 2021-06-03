namespace MagicCube5D
{
	partial class FindDlg
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose( bool disposing )
		{
			if( disposing && (components != null) )
			{
				components.Dispose();
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.btnCancel = new System.Windows.Forms.Button();
			this.btnOK = new System.Windows.Forms.Button();
			this.colorBox1 = new System.Windows.Forms.ComboBox();
			this.colorBox2 = new System.Windows.Forms.ComboBox();
			this.colorBox3 = new System.Windows.Forms.ComboBox();
			this.colorBox4 = new System.Windows.Forms.ComboBox();
			this.colorBox5 = new System.Windows.Forms.ComboBox();
			this.label1 = new System.Windows.Forms.Label();
			this.SuspendLayout();
			// 
			// btnCancel
			// 
			this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.btnCancel.Location = new System.Drawing.Point( 244, 213 );
			this.btnCancel.Name = "btnCancel";
			this.btnCancel.Size = new System.Drawing.Size( 80, 24 );
			this.btnCancel.TabIndex = 5;
			this.btnCancel.Text = "Cancel";
			this.btnCancel.UseVisualStyleBackColor = true;
			this.btnCancel.Click += new System.EventHandler( this.Cancel_Click );
			// 
			// btnOK
			// 
			this.btnOK.Enabled = false;
			this.btnOK.Location = new System.Drawing.Point( 156, 213 );
			this.btnOK.Name = "btnOK";
			this.btnOK.Size = new System.Drawing.Size( 80, 24 );
			this.btnOK.TabIndex = 4;
			this.btnOK.Text = "OK";
			this.btnOK.UseVisualStyleBackColor = true;
			this.btnOK.Click += new System.EventHandler( this.OK_Click );
			// 
			// colorBox1
			// 
			this.colorBox1.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawVariable;
			this.colorBox1.DropDownHeight = 175;
			this.colorBox1.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.colorBox1.FormattingEnabled = true;
			this.colorBox1.IntegralHeight = false;
			this.colorBox1.Location = new System.Drawing.Point( 12, 61 );
			this.colorBox1.Name = "colorBox1";
			this.colorBox1.Size = new System.Drawing.Size( 312, 21 );
			this.colorBox1.TabIndex = 6;
			this.colorBox1.DrawItem += new System.Windows.Forms.DrawItemEventHandler( this.ColorBox1_DrawItem );
			this.colorBox1.SelectedValueChanged += new System.EventHandler( this.ColorBox1_SelectedValueChanged );
			// 
			// colorBox2
			// 
			this.colorBox2.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawVariable;
			this.colorBox2.DropDownHeight = 175;
			this.colorBox2.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.colorBox2.FormattingEnabled = true;
			this.colorBox2.IntegralHeight = false;
			this.colorBox2.Location = new System.Drawing.Point( 12, 88 );
			this.colorBox2.Name = "colorBox2";
			this.colorBox2.Size = new System.Drawing.Size( 312, 21 );
			this.colorBox2.TabIndex = 7;
			this.colorBox2.DrawItem += new System.Windows.Forms.DrawItemEventHandler( this.ColorBox2_DrawItem );
			this.colorBox2.SelectedValueChanged += new System.EventHandler( this.ColorBox2_SelectedValueChanged );
			// 
			// colorBox3
			// 
			this.colorBox3.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawVariable;
			this.colorBox3.DropDownHeight = 175;
			this.colorBox3.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.colorBox3.FormattingEnabled = true;
			this.colorBox3.IntegralHeight = false;
			this.colorBox3.Location = new System.Drawing.Point( 12, 115 );
			this.colorBox3.Name = "colorBox3";
			this.colorBox3.Size = new System.Drawing.Size( 312, 21 );
			this.colorBox3.TabIndex = 8;
			this.colorBox3.DrawItem += new System.Windows.Forms.DrawItemEventHandler( this.ColorBox3_DrawItem );
			this.colorBox3.SelectedValueChanged += new System.EventHandler( this.ColorBox3_SelectedValueChanged );
			// 
			// colorBox4
			// 
			this.colorBox4.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawVariable;
			this.colorBox4.DropDownHeight = 175;
			this.colorBox4.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.colorBox4.FormattingEnabled = true;
			this.colorBox4.IntegralHeight = false;
			this.colorBox4.Location = new System.Drawing.Point( 12, 142 );
			this.colorBox4.Name = "colorBox4";
			this.colorBox4.Size = new System.Drawing.Size( 312, 21 );
			this.colorBox4.TabIndex = 9;
			this.colorBox4.DrawItem += new System.Windows.Forms.DrawItemEventHandler( this.ColorBox4_DrawItem );
			this.colorBox4.SelectedValueChanged += new System.EventHandler( this.ColorBox4_SelectedValueChanged );
			// 
			// colorBox5
			// 
			this.colorBox5.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawVariable;
			this.colorBox5.DropDownHeight = 175;
			this.colorBox5.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.colorBox5.FormattingEnabled = true;
			this.colorBox5.IntegralHeight = false;
			this.colorBox5.Location = new System.Drawing.Point( 12, 169 );
			this.colorBox5.Name = "colorBox5";
			this.colorBox5.Size = new System.Drawing.Size( 312, 21 );
			this.colorBox5.TabIndex = 10;
			this.colorBox5.DrawItem += new System.Windows.Forms.DrawItemEventHandler( this.ColorBox5_DrawItem );
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point( 13, 13 );
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size( 311, 45 );
			this.label1.TabIndex = 11;
			this.label1.Text = "Please select the colors in the cubie(s) you would like to find.  After clicking " +
				"OK, you can revert to the normal display by pressing escape.";
			// 
			// FindDlg
			// 
			this.AcceptButton = this.btnOK;
			this.AutoScaleDimensions = new System.Drawing.SizeF( 6F, 13F );
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.CancelButton = this.btnCancel;
			this.ClientSize = new System.Drawing.Size( 336, 249 );
			this.ControlBox = false;
			this.Controls.Add( this.label1 );
			this.Controls.Add( this.colorBox5 );
			this.Controls.Add( this.colorBox4 );
			this.Controls.Add( this.colorBox3 );
			this.Controls.Add( this.colorBox2 );
			this.Controls.Add( this.colorBox1 );
			this.Controls.Add( this.btnCancel );
			this.Controls.Add( this.btnOK );
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "FindDlg";
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "Find Cubies";
			this.ResumeLayout( false );

		}

		#endregion

		private System.Windows.Forms.Button btnCancel;
		private System.Windows.Forms.Button btnOK;
		private System.Windows.Forms.ComboBox colorBox1;
		private System.Windows.Forms.ComboBox colorBox2;
		private System.Windows.Forms.ComboBox colorBox3;
		private System.Windows.Forms.ComboBox colorBox4;
		private System.Windows.Forms.ComboBox colorBox5;
		private System.Windows.Forms.Label label1;
	}
}