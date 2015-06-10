using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace MagicCube5D
{
	public partial class PropertyDlg : Form
	{
		public PropertyDlg()
		{
			InitializeComponent();
		}

		public object EditObject
		{
			get { return this.properties.SelectedObject; }
			set { this.properties.SelectedObject = value; }
		}
	}
}