namespace MagicCube5D
{
    using System.Windows.Forms;

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
