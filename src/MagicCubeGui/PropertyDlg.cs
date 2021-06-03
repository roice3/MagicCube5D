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
            get { return properties.SelectedObject; }
            set { properties.SelectedObject = value; }
        }
    }
}
