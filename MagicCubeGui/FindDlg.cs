namespace MagicCube5D
{
    using System;
    using System.Collections.Generic;
    using System.Drawing;
    using System.Windows.Forms;

    internal partial class FindDlg : Form
    {
        private readonly GuiSettings m_settings;

        public FindDlg(GuiSettings settings)
        {
            InitializeComponent();
            m_settings = settings;
            FillBox1();
        }

        /// <summary>
        /// Gets the colors selected (as physical face integers).
        /// </summary>
        public List<int> GetSelectedColors()
        {
            var ret = new List<int>();
            if (colorBox1.SelectedItem != null)
            {
                ret.Add((int)colorBox1.SelectedItem);
            }

            if (colorBox2.SelectedItem != null)
            {
                ret.Add((int)colorBox2.SelectedItem);
            }

            if (colorBox3.SelectedItem != null)
            {
                ret.Add((int)colorBox3.SelectedItem);
            }

            if (colorBox4.SelectedItem != null)
            {
                ret.Add((int)colorBox4.SelectedItem);
            }

            if (colorBox5.SelectedItem != null)
            {
                ret.Add((int)colorBox5.SelectedItem);
            }

            return ret;
        }

        private void UpdateEnabled()
        {
            colorBox2.Enabled = colorBox1.SelectedItem != null;
            colorBox3.Enabled = colorBox2.SelectedItem != null;
            colorBox4.Enabled = colorBox3.SelectedItem != null;
            colorBox5.Enabled = colorBox4.SelectedItem != null;
            btnOK.Enabled = colorBox2.Enabled;
        }

        private Color GetColor(int face)
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

        private void DrawItem(object sender, DrawItemEventArgs e)
        {
            var box = (ComboBox)sender;

            // -1 index references what to draw for the selected item.
            var index = e.Index;
            if (index == -1 && box.SelectedItem == null)
            {
                return;
            }

            var focused = (e.State & DrawItemState.Focus) == DrawItemState.Focus;

            e.DrawBackground();
            if (focused)
            {
                e.DrawFocusRectangle();
            }

            var c = GetColor(index == -1 ? (int)box.SelectedItem : (int)box.Items[e.Index]);

            var rect = e.Bounds;
            rect.Inflate(-2, -2);
            e.Graphics.FillRectangle(new SolidBrush(c), rect);
            e.Graphics.DrawRectangle(SystemPens.WindowText, rect);
        }

        private void ColorBox1_DrawItem(object sender, DrawItemEventArgs e)
        {
            DrawItem(sender, e);
        }

        private void ColorBox2_DrawItem(object sender, DrawItemEventArgs e)
        {
            DrawItem(sender, e);
        }

        private void ColorBox3_DrawItem(object sender, DrawItemEventArgs e)
        {
            DrawItem(sender, e);
        }

        private void ColorBox4_DrawItem(object sender, DrawItemEventArgs e)
        {
            DrawItem(sender, e);
        }

        private void ColorBox5_DrawItem(object sender, DrawItemEventArgs e)
        {
            DrawItem(sender, e);
        }

        private int FindOppositeFace(int face)
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

        private void FillBoxHelper(List<int> used, ComboBox box)
        {
            box.Items.Clear();

            for (var i = 0; i < 10; i++)
            {
                var add = true;
                for (var j = 0; j < used.Count; j++)
                {
                    if (i == used[j])
                    {
                        add = false;
                        break;
                    }
                }

                if (add)
                {
                    _ = box.Items.Add(i);
                }
            }
        }

        private void FillBox1()
        {
            for (var i = 0; i < 10; i++)
            {
                _ = colorBox1.Items.Add(i);
            }

            UpdateEnabled();
        }

        private void FillBox2()
        {
            var used = new List<int>
            {
                (int)colorBox1.SelectedItem,
                FindOppositeFace((int)colorBox1.SelectedItem)
            };
            FillBoxHelper(used, colorBox2);
            UpdateEnabled();
        }

        private void FillBox3()
        {
            var used = new List<int>
            {
                (int)colorBox1.SelectedItem,
                FindOppositeFace((int)colorBox1.SelectedItem),
                (int)colorBox2.SelectedItem,
                FindOppositeFace((int)colorBox2.SelectedItem)
            };
            FillBoxHelper(used, colorBox3);
            UpdateEnabled();
        }

        private void FillBox4()
        {
            var used = new List<int>
            {
                (int)colorBox1.SelectedItem,
                FindOppositeFace((int)colorBox1.SelectedItem),
                (int)colorBox2.SelectedItem,
                FindOppositeFace((int)colorBox2.SelectedItem),
                (int)colorBox3.SelectedItem,
                FindOppositeFace((int)colorBox3.SelectedItem)
            };
            FillBoxHelper(used, colorBox4);
            UpdateEnabled();
        }

        private void FillBox5()
        {
            var used = new List<int>
            {
                (int)colorBox1.SelectedItem,
                FindOppositeFace((int)colorBox1.SelectedItem),
                (int)colorBox2.SelectedItem,
                FindOppositeFace((int)colorBox2.SelectedItem),
                (int)colorBox3.SelectedItem,
                FindOppositeFace((int)colorBox3.SelectedItem),
                (int)colorBox4.SelectedItem,
                FindOppositeFace((int)colorBox4.SelectedItem)
            };
            FillBoxHelper(used, colorBox5);
            UpdateEnabled();
        }

        private void ColorBox1_SelectedValueChanged(object sender, EventArgs e)
        {
            colorBox3.Items.Clear();
            colorBox4.Items.Clear();
            colorBox5.Items.Clear();
            FillBox2();
        }

        private void ColorBox2_SelectedValueChanged(object sender, EventArgs e)
        {
            colorBox4.Items.Clear();
            colorBox5.Items.Clear();
            FillBox3();
        }

        private void ColorBox3_SelectedValueChanged(object sender, EventArgs e)
        {
            colorBox5.Items.Clear();
            FillBox4();
        }

        private void ColorBox4_SelectedValueChanged(object sender, EventArgs e)
        {
            FillBox5();
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
    }
}
