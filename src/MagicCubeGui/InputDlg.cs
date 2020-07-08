namespace MagicCube5D
{
    using System;
    using System.Windows.Forms;

    public partial class InputDlg : Form
    {
        public InputDlg()
        {
            InitializeComponent();
        }

        public string Caption
        {
            get { return Text; }
            set { Text = value; }
        }

        public string Prompt
        {
            get { return labelPrompt.Text; }
            set { labelPrompt.Text = value; }
        }

        public string Value
        {
            get { return textInput.Text; }
            set { textInput.Text = value; }
        }

        private void OK_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.OK;
            Close();
        }

        private void Cancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            Close();
        }

        private void Input_TextChanged(object sender, EventArgs e)
        {
            btnOK.Enabled = textInput.Text != string.Empty;
        }
    }
}
