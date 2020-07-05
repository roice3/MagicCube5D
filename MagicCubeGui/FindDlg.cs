namespace MagicCube5D
{
    using System;
    using System.Collections.Generic;
    using System.Drawing;
    using System.Windows.Forms;

    partial class FindDlg : Form
    {
        private GuiSettings m_settings;

        public FindDlg(GuiSettings settings)
        {
            InitializeComponent();
            m_settings = settings;
            fillBox1();
        }

        /// <summary>
        /// Gets the colors selected (as physical face integers).
        /// </summary>
        public List<int> getSelectedColors()
        {
            List<int> ret = new List<int>();
            if (this.colorBox1.SelectedItem != null)
                ret.Add((int)this.colorBox1.SelectedItem);
            if (this.colorBox2.SelectedItem != null)
                ret.Add((int)this.colorBox2.SelectedItem);
            if (this.colorBox3.SelectedItem != null)
                ret.Add((int)this.colorBox3.SelectedItem);
            if (this.colorBox4.SelectedItem != null)
                ret.Add((int)this.colorBox4.SelectedItem);
            if (this.colorBox5.SelectedItem != null)
                ret.Add((int)this.colorBox5.SelectedItem);
            return ret;
        }

        private void updateEnabled()
        {
            this.colorBox2.Enabled = (this.colorBox1.SelectedItem != null);
            this.colorBox3.Enabled = (this.colorBox2.SelectedItem != null);
            this.colorBox4.Enabled = (this.colorBox3.SelectedItem != null);
            this.colorBox5.Enabled = (this.colorBox4.SelectedItem != null);
            this.btnOK.Enabled = this.colorBox2.Enabled;
        }

        private Color getColor(int face)
        {
            switch (face)
            {
            case 0:
                return m_settings.Color1;

            case 1:
                return m_settings.Color2;

            case 2:
                return m_settings.Color3;

            case 3:
                return m_settings.Color4;

            case 4:
                return m_settings.Color5;

            case 5:
                return m_settings.Color6;

            case 6:
                return m_settings.Color7;

            case 7:
                return m_settings.Color8;

            case 8:
                return m_settings.Color9;

            case 9:
                return m_settings.Color10;
            }

            return Color.White;
        }

        private void drawItem(object sender, DrawItemEventArgs e)
        {
            ComboBox box = (ComboBox)sender;

            // -1 index references what to draw for the selected item.
            int index = e.Index;
            if (index == -1 && box.SelectedItem == null)
                return;

            bool selected = (e.State & DrawItemState.Selected) == DrawItemState.Selected;
            bool focused = (e.State & DrawItemState.Focus) == DrawItemState.Focus;

            e.DrawBackground();
            if (focused)
                e.DrawFocusRectangle();

            Color c = getColor(-1 == index ? (int)box.SelectedItem : (int)box.Items[e.Index]);

            Rectangle rect = e.Bounds;
            rect.Inflate(-2, -2);
            e.Graphics.FillRectangle(new SolidBrush(c), rect);
            e.Graphics.DrawRectangle(SystemPens.WindowText, rect);
        }

        private void colorBox1_DrawItem(object sender, DrawItemEventArgs e)
        {
            drawItem(sender, e);
        }

        private void colorBox2_DrawItem(object sender, DrawItemEventArgs e)
        {
            drawItem(sender, e);
        }

        private void colorBox3_DrawItem(object sender, DrawItemEventArgs e)
        {
            drawItem(sender, e);
        }

        private void colorBox4_DrawItem(object sender, DrawItemEventArgs e)
        {
            drawItem(sender, e);
        }

        private void colorBox5_DrawItem(object sender, DrawItemEventArgs e)
        {
            drawItem(sender, e);
        }

        private int findOppositeFace(int face)
        {
            switch (face)
            {
            case 0:
                return 1;

            case 1:
                return 0;

            case 2:
                return 3;

            case 3:
                return 2;

            case 4:
                return 5;

            case 5:
                return 4;

            case 6:
                return 7;

            case 7:
                return 6;

            case 8:
                return 9;

            case 9:
                return 8;
            }

            System.Diagnostics.Debug.Assert(false);
            return -1;
        }

        private void fillBoxHelper(List<int> used, ComboBox box)
        {
            box.Items.Clear();

            for (int i = 0; i < 10; i++)
            {
                bool add = true;
                for (int j = 0; j < used.Count; j++)
                {
                    if (i == used[j])
                    {
                        add = false;
                        break;
                    }
                }

                if (add)
                    box.Items.Add(i);
            }
        }

        private void fillBox1()
        {
            for (int i = 0; i < 10; i++)
                this.colorBox1.Items.Add(i);
            updateEnabled();
        }

        private void fillBox2()
        {
            List<int> used = new List<int>();
            used.Add((int)this.colorBox1.SelectedItem);
            used.Add(findOppositeFace((int)this.colorBox1.SelectedItem));
            fillBoxHelper(used, this.colorBox2);
            updateEnabled();
        }

        private void fillBox3()
        {
            List<int> used = new List<int>();
            used.Add((int)this.colorBox1.SelectedItem);
            used.Add(findOppositeFace((int)this.colorBox1.SelectedItem));
            used.Add((int)this.colorBox2.SelectedItem);
            used.Add(findOppositeFace((int)this.colorBox2.SelectedItem));
            fillBoxHelper(used, this.colorBox3);
            updateEnabled();
        }

        private void fillBox4()
        {
            List<int> used = new List<int>();
            used.Add((int)this.colorBox1.SelectedItem);
            used.Add(findOppositeFace((int)this.colorBox1.SelectedItem));
            used.Add((int)this.colorBox2.SelectedItem);
            used.Add(findOppositeFace((int)this.colorBox2.SelectedItem));
            used.Add((int)this.colorBox3.SelectedItem);
            used.Add(findOppositeFace((int)this.colorBox3.SelectedItem));
            fillBoxHelper(used, this.colorBox4);
            updateEnabled();
        }

        private void fillBox5()
        {
            List<int> used = new List<int>();
            used.Add((int)this.colorBox1.SelectedItem);
            used.Add(findOppositeFace((int)this.colorBox1.SelectedItem));
            used.Add((int)this.colorBox2.SelectedItem);
            used.Add(findOppositeFace((int)this.colorBox2.SelectedItem));
            used.Add((int)this.colorBox3.SelectedItem);
            used.Add(findOppositeFace((int)this.colorBox3.SelectedItem));
            used.Add((int)this.colorBox4.SelectedItem);
            used.Add(findOppositeFace((int)this.colorBox4.SelectedItem));
            fillBoxHelper(used, this.colorBox5);
            updateEnabled();
        }

        private void colorBox1_SelectedValueChanged(object sender, EventArgs e)
        {
            this.colorBox3.Items.Clear();
            this.colorBox4.Items.Clear();
            this.colorBox5.Items.Clear();
            fillBox2();
        }

        private void colorBox2_SelectedValueChanged(object sender, EventArgs e)
        {
            this.colorBox4.Items.Clear();
            this.colorBox5.Items.Clear();
            fillBox3();
        }

        private void colorBox3_SelectedValueChanged(object sender, EventArgs e)
        {
            this.colorBox5.Items.Clear();
            fillBox4();
        }

        private void colorBox4_SelectedValueChanged(object sender, EventArgs e)
        {
            fillBox5();
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.OK;
            Close();
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
            Close();
        }
    }
}
