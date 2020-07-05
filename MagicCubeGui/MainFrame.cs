namespace MagicCube5D
{
    using System;
    using System.Collections.Generic;
    using System.Drawing;
    using System.IO;
    using System.Windows.Forms;

    public enum CubeParameter
    {
        Puzzle,
        Perspective5d,
        Perspective4d,
        FaceSeparation,
        StickerSeparation,
        StickerSize,
        RotationRate,
        StereoMode,
        StereoSeparation
    }

    public interface ICube
    {
        // Basic commands.
        bool EnableRedraw { get; set; }

        void SetParameter(CubeParameter param, int value);

        void EnableFace(int face, bool enable);

        void ColorFace(int face, Color c);

        void Rotate(int axis1, int axis2, int face, int slicemask);

        void CycleStickerAccent(int colors);

        void Save(bool saveas);

        void Load();

        void Scramble(int number);

        void Solve();

        void Undo();

        void Redo();

        void Reset();

        void HighlightCubies(List<int> colors);

        // Macro control.
        String[] GetMacroNames();

        void StartMacroRecord();

        void StopMacroRecord();

        void RenameMacro(int index, String name);

        void DeleteMacro(int index);

        void ExecuteMacro(int index, bool reverse);
    }

    public partial class MainFrame : Form
    {
        private const string DefaultSettingsName = "-- Default --";
        private ICube cube;

        private GuiSettings lastSettings = new GuiSettings();

        public MainFrame(ICube cube)
        {
            InitializeComponent();

            // setup the drawing surface
            this.DrawSurface = new DrawSurface();
            this.DrawSurface.Location = this.panelMain.Location;
            this.DrawSurface.Size = this.panelMain.Size;
            this.DrawSurface.Anchor = this.panelMain.Anchor;
            this.Controls.Add(this.DrawSurface);

            // setup tags for sliders
            this.track5dDist.Tag = CubeParameter.Perspective5d;
            this.track4dDist.Tag = CubeParameter.Perspective4d;
            this.traceFaceSeparation.Tag = CubeParameter.FaceSeparation;
            this.trackStickerSeparation.Tag = CubeParameter.StickerSeparation;
            this.trackStickerSize.Tag = CubeParameter.StickerSize;
            this.trackRotationRate.Tag = CubeParameter.RotationRate;
            this.trackStereoSeparation.Tag = CubeParameter.StereoSeparation;

            // setup tags for face checkboxes
            this.checkFace1.Tag = 0;
            this.checkFace2.Tag = 1;
            this.checkFace3.Tag = 2;
            this.checkFace4.Tag = 3;
            this.checkFace5.Tag = 4;
            this.checkFace6.Tag = 5;
            this.checkFace7.Tag = 6;
            this.checkFace8.Tag = 7;
            this.checkFace9.Tag = 8;
            this.checkFace10.Tag = 9;

            // setup tags for rotation buttons
            this.btnXY.Tag = new Axes(0, 1);
            this.btnXZ.Tag = new Axes(0, 2);
            this.btnXU.Tag = new Axes(0, 3);
            this.btnXV.Tag = new Axes(0, 4);
            this.btnYZ.Tag = new Axes(1, 2);
            this.btnYU.Tag = new Axes(1, 3);
            this.btnYV.Tag = new Axes(1, 4);
            this.btnZU.Tag = new Axes(2, 3);
            this.btnZV.Tag = new Axes(2, 4);
            this.btnUV.Tag = new Axes(3, 4);

            // default rotations to all
            this.comboFace.SelectedIndex = 10;

            // setup tags for puzzle type.
            this.menuPuzzle2.Tag = 2;
            this.menuPuzzle3.Tag = 3;
            this.menuPuzzle4.Tag = 4;
            this.menuPuzzle5.Tag = 5;
            this.menuPuzzle6.Tag = 6;
            this.menuPuzzle7.Tag = 7;

            // setup tags for hilight items.
            this.menuHilight1.Tag = 1;
            this.menuHilight2.Tag = 2;
            this.menuHilight3.Tag = 3;
            this.menuHilight4.Tag = 4;
            this.menuHilight5.Tag = 5;

            // setup tags for scramble menu items.
            this.menuScramble1.Tag = 1;
            this.menuScramble2.Tag = 2;
            this.menuScramble3.Tag = 3;
            this.menuScramble4.Tag = 4;
            this.menuScramble5.Tag = 5;
            this.menuScrambleFull.Tag = 100;

            // setup tags for stereo.
            this.menuStereoNone.Tag = 0;
            this.menuStereoCrossEyed.Tag = 1;
            this.menuStereoAnaglyph.Tag = 2;
            this.menuStereoAnaglyphNoColor.Tag = 3;

            this.btnStartMacro.Enabled = true;
            this.btnStopMacro.Enabled = false;

            this.cube = cube;
        }

        public DrawSurface DrawSurface { get; }

        private GuiSettings Settings
        {
            get
            {
                // refresh from input controls
                this.lastSettings.Perspective5d = this.track5dDist.Value;
                this.lastSettings.Perspective4d = this.track4dDist.Value;
                this.lastSettings.StickerSeparation = this.trackStickerSeparation.Value;
                this.lastSettings.StickerSize = this.trackStickerSize.Value;
                this.lastSettings.FaceSeparation = this.traceFaceSeparation.Value;
                this.lastSettings.RotationRate = this.trackRotationRate.Value;
                if (this.menuStereoNone.Checked) this.lastSettings.StereoMode = 0;
                else if (this.menuStereoCrossEyed.Checked) this.lastSettings.StereoMode = 1;
                else if (this.menuStereoAnaglyph.Checked) this.lastSettings.StereoMode = 2;
                else if (this.menuStereoAnaglyphNoColor.Checked) this.lastSettings.StereoMode = 3;
                this.lastSettings.StereoSeparation = this.trackStereoSeparation.Value;
                this.lastSettings.Face1 = this.checkFace1.Checked;
                this.lastSettings.Face2 = this.checkFace2.Checked;
                this.lastSettings.Face3 = this.checkFace3.Checked;
                this.lastSettings.Face4 = this.checkFace4.Checked;
                this.lastSettings.Face5 = this.checkFace5.Checked;
                this.lastSettings.Face6 = this.checkFace6.Checked;
                this.lastSettings.Face7 = this.checkFace7.Checked;
                this.lastSettings.Face8 = this.checkFace8.Checked;
                this.lastSettings.Face9 = this.checkFace9.Checked;
                this.lastSettings.Face10 = this.checkFace10.Checked;

                return this.lastSettings.Clone();
            }

            set
            {
                this.lastSettings = value.Clone();

                // update the input controls and the cube
                this.cube.EnableRedraw = false;
                this.track5dDist.Value = value.Perspective5d;
                this.track4dDist.Value = value.Perspective4d;
                this.trackStickerSeparation.Value = value.StickerSeparation;
                this.trackStickerSize.Value = value.StickerSize;
                this.traceFaceSeparation.Value = value.FaceSeparation;
                this.trackRotationRate.Value = value.RotationRate;
                this.menuStereoNone.Checked = 0 == value.StereoMode ? true : false;
                this.menuStereoCrossEyed.Checked = 1 == value.StereoMode ? true : false;
                this.menuStereoAnaglyph.Checked = 2 == value.StereoMode ? true : false;
                this.menuStereoAnaglyphNoColor.Checked = 3 == value.StereoMode ? true : false;
                this.trackStereoSeparation.Value = value.StereoSeparation;
                this.checkFace1.Checked = value.Face1;
                this.checkFace2.Checked = value.Face2;
                this.checkFace3.Checked = value.Face3;
                this.checkFace4.Checked = value.Face4;
                this.checkFace5.Checked = value.Face5;
                this.checkFace6.Checked = value.Face6;
                this.checkFace7.Checked = value.Face7;
                this.checkFace8.Checked = value.Face8;
                this.checkFace9.Checked = value.Face9;
                this.checkFace10.Checked = value.Face10;
                this.cube.ColorFace(0, value.Color1);
                this.cube.ColorFace(1, value.Color2);
                this.cube.ColorFace(2, value.Color3);
                this.cube.ColorFace(3, value.Color4);
                this.cube.ColorFace(4, value.Color5);
                this.cube.ColorFace(5, value.Color6);
                this.cube.ColorFace(6, value.Color7);
                this.cube.ColorFace(7, value.Color8);
                this.cube.ColorFace(8, value.Color9);
                this.cube.ColorFace(9, value.Color10);
                this.cube.ColorFace(10, value.ColorBg);
                this.cube.EnableRedraw = true;
            }
        }

        public void updateProgressPage(
            int s0, int n0,
            int s1, int n1,
            int s2, int n2,
            int s3, int n3,
            int s4, int n4,
            int s5, int n5,
            int ss, int ns)
        {
            var sc = s0 + s1 + s2 + s3 + s4 + s5;
            var nc = n0 + n1 + n2 + n3 + n4 + n5;

            this.labelStickers.Text = string.Format("Stickers Solved: {0} of {1} ({2:F1}%)", ss, ns, 100.0 * ss / ns);
            this.labelCubies.Text = string.Format("Cubies Solved:   {0} of {1} ({2:F1}%)", sc, nc, 100.0 * sc / nc);
            this.labelZero.Text = string.Format("   0-coloreds:   {0} of {1} ({2:F1}%)", s0, n0, 100.0 * s0 / n0);
            this.labelOne.Text = string.Format("   1-coloreds:   {0} of {1} ({2:F1}%)", s1, n1, 100.0 * s1 / n1);
            this.labelTwo.Text = string.Format("   2-coloreds:   {0} of {1} ({2:F1}%)", s2, n2, 100.0 * s2 / n2);
            this.labelThree.Text = string.Format("   3-coloreds:   {0} of {1} ({2:F1}%)", s3, n3, 100.0 * s3 / n3);
            this.labelFour.Text = string.Format("   4-coloreds:   {0} of {1} ({2:F1}%)", s4, n4, 100.0 * s4 / n4);
            this.labelFive.Text = string.Format("   5-coloreds:   {0} of {1} ({2:F1}%)", s5, n5, 100.0 * s5 / n5);

            // Special case code for 2^5 here.
            if (0 == n0)
            {
                this.labelZero.Text = string.Format("   0-coloreds:   {0} of {1}", s0, n0);
                this.labelOne.Text = string.Format("   1-coloreds:   {0} of {1}", s1, n1);
                this.labelTwo.Text = string.Format("   2-coloreds:   {0} of {1}", s2, n2);
                this.labelThree.Text = string.Format("   3-coloreds:   {0} of {1}", s3, n3);
                this.labelFour.Text = string.Format("   4-coloreds:   {0} of {1}", s4, n4);
            }
        }

        private static bool ControlDown()
        {
            return (Form.ModifierKeys & Keys.Control) == Keys.Control;
        }

        private void MainFrame_Load(object sender, EventArgs e)
        {
            // setup initial values
            this.cube.EnableRedraw = false;
            this.menuHilight1.CheckState = CheckState.Unchecked;
            this.menuHilight2.CheckState = CheckState.Checked;
            this.menuHilight3.CheckState = CheckState.Unchecked;
            this.menuHilight4.CheckState = CheckState.Unchecked;
            this.menuHilight5.CheckState = CheckState.Unchecked;
            this.menuPuzzle3.Checked = true;
            this.cube.EnableRedraw = true;

            // load all of our our macros.
            foreach (var s in this.cube.GetMacroNames())
                this.listMacros.Items.Add(s);

            // load all of our saved settings
            GuiSettings defSettings = null;
            foreach (var s in Directory.GetFiles(".", "*.settings"))
            {
                var settings = new GuiSettings();
                settings.Load(s);
                this.comboSettings.Items.Add(settings);
                if (settings.Name == DefaultSettingsName)
                    defSettings = settings;
            }

            // make sure the "default" settings exist
            if (defSettings == null)
            {
                defSettings = new GuiSettings();
                defSettings.Name = DefaultSettingsName;
                this.comboSettings.Items.Add(defSettings);
            }

            // select the default settings
            this.comboSettings.SelectedItem = defSettings;

            // give the draw surface the focus
            this.DrawSurface.Select();
        }

        private void trackItem_ValueChanged(object sender, EventArgs e)
        {
            var control = (TrackBar)sender;
            if (this.cube != null)
                this.cube.SetParameter((CubeParameter)control.Tag, control.Value);
        }

        private void checkFace_CheckedChanged(object sender, EventArgs e)
        {
            var control = (CheckBox)sender;
            if (this.cube != null)
                this.cube.EnableFace((int)control.Tag, control.Checked);
        }

        private void btnRotate_MouseClick(object sender, MouseEventArgs e)
        {
            var b = (Button)sender;
            var axes = (Axes)b.Tag;

            // swap axes on right side of button
            if (e.X > b.Width / 2)
            {
                var temp = axes.a1;
                axes.a1 = axes.a2;
                axes.a2 = temp;
            }

            if (this.cube != null)
            {
                var face = this.comboFace.SelectedIndex;
                if (10 == face)
                    face = -1;
                this.cube.Rotate(axes.a1, axes.a2, face, 1);
            }
        }

        private void menuSave_Click(object sender, EventArgs e)
        {
            this.cube.Save(false);
        }

        private void menuSaveAs_Click(object sender, EventArgs e)
        {
            this.cube.Save(true);
        }

        private void menuLoad_Click(object sender, EventArgs e)
        {
            this.cube.Load();
        }

        private void menuScramble_Click(object sender, EventArgs e)
        {
            var item = (ToolStripMenuItem)sender;
            var scrambleNumber = (int)item.Tag;
            this.cube.Scramble(scrambleNumber);
        }

        private void menuPuzzleType_Click(object sender, EventArgs e)
        {
            var item = (ToolStripMenuItem)sender;
            var puzzleType = (int)item.Tag;
            this.cube.SetParameter(CubeParameter.Puzzle, puzzleType);

            this.menuPuzzle2.Checked =
            this.menuPuzzle3.Checked =
            this.menuPuzzle4.Checked =
            this.menuPuzzle5.Checked =
            this.menuPuzzle6.Checked =
            this.menuPuzzle7.Checked = false;
            item.Checked = true;
        }

        private void menuSolve_Click(object sender, EventArgs e)
        {
            this.cube.Solve();
        }

        private void menuUndo_Click(object sender, EventArgs e)
        {
            this.cube.Undo();
        }

        private void menuFind_Click(object sender, EventArgs e)
        {
            using (var dlg = new FindDlg(this.Settings))
            {
                if (DialogResult.OK != dlg.ShowDialog(this))
                    return;

                var colors = dlg.getSelectedColors();
                this.cube.HighlightCubies(colors);
            }
        }

        private void menuRedo_Click(object sender, EventArgs e)
        {
            this.cube.Redo();
        }

        private void menuReset_Click(object sender, EventArgs e)
        {
            this.cube.Reset();
        }

        private void menuHilight_Click(object sender, EventArgs e)
        {
            var item = (ToolStripMenuItem)sender;
            this.cube.CycleStickerAccent((int)item.Tag);

            // Cycle the check state.
            if (item.CheckState == CheckState.Indeterminate)
                item.CheckState = CheckState.Unchecked;
            else if (item.CheckState == CheckState.Unchecked)
                item.CheckState = CheckState.Checked;
            else if (item.CheckState == CheckState.Checked)
                item.CheckState = CheckState.Indeterminate;
        }

        // Get the axis index for a face.
        private int getFaceAxisIndex(int face)
        {
            if (10 == face)
                return -1;
            else
                return face / 2;
        }

        // Helper for enabling buttons.
        private void enableButton(ref Button button, int face)
        {
            var invalidAxis = getFaceAxisIndex(face);
            var axes = (Axes)button.Tag;
            if (axes.a1 == invalidAxis ||
                axes.a2 == invalidAxis)
                button.Enabled = false;
            else
                button.Enabled = true;
        }

        private void comboFace_SelectedIndexChanged(object sender, EventArgs e)
        {
            // Update which buttons are enabled.
            var face = this.comboFace.SelectedIndex;
            enableButton(ref this.btnXY, face);
            enableButton(ref this.btnXZ, face);
            enableButton(ref this.btnXU, face);
            enableButton(ref this.btnXV, face);
            enableButton(ref this.btnYZ, face);
            enableButton(ref this.btnYU, face);
            enableButton(ref this.btnYV, face);
            enableButton(ref this.btnZU, face);
            enableButton(ref this.btnZV, face);
            enableButton(ref this.btnUV, face);
        }

        private void btnStartMacro_Click(object sender, EventArgs e)
        {
            this.btnStartMacro.Enabled = false;
            this.btnStopMacro.Enabled = true;
            this.cube.StartMacroRecord();
        }

        private void btnStopMacro_Click(object sender, EventArgs e)
        {
            this.cube.StopMacroRecord();
            this.listMacros.Items.Add("New Macro");
            this.btnStartMacro.Enabled = true;
            this.btnStopMacro.Enabled = false;
        }

        private void renameMacro_Click(object sender, EventArgs e)
        {
            if (1 == this.listMacros.SelectedIndices.Count)
            {
                var index = this.listMacros.SelectedIndices[0];
                var item = this.listMacros.Items[index];
                item.BeginEdit();
            }
        }

        private void deleteMacro_Click(object sender, EventArgs e)
        {
            if (1 == this.listMacros.SelectedIndices.Count)
            {
                var index = this.listMacros.SelectedIndices[0];
                this.listMacros.Items.RemoveAt(index);
                this.cube.DeleteMacro(index);
            }
        }

        private void listMacros_AfterLabelEdit(object sender, LabelEditEventArgs e)
        {
            if (1 == this.listMacros.SelectedIndices.Count)
            {
                var index = this.listMacros.SelectedIndices[0];
                if (e.Label != null)
                    this.cube.RenameMacro(index, e.Label);
            }
        }

        private void listMacros_DoubleClick(object sender, EventArgs e)
        {
            if (1 == this.listMacros.SelectedIndices.Count)
            {
                var index = this.listMacros.SelectedIndices[0];
                this.cube.ExecuteMacro(index, ControlDown() ? true : false);
            }
        }

        private void btnSettingsSave_Click(object sender, EventArgs e)
        {
            using (var dlg = new InputDlg())
            {
                // get a name for the settings
                dlg.Caption = "Save Settings";
                dlg.Prompt = "Please provide a name for the new saved settings.";
                if (this.comboSettings.SelectedItem != null)
                    dlg.Value = ((GuiSettings)this.comboSettings.SelectedItem).Name;
                if (DialogResult.OK != dlg.ShowDialog(this))
                    return;
                foreach (GuiSettings old in this.comboSettings.Items)
                {
                    if (old.Name == dlg.Value)
                    {
                        this.comboSettings.Items.Remove(old);
                        break;
                    }
                }

                // the new settings they are saving
                var s = this.Settings;
                s.Name = dlg.Value;

                // find where we can save it
                var baseFile = s.Name;
                foreach (var c in Path.GetInvalidFileNameChars())
                    baseFile = baseFile.Replace(c, '_');
                var final = baseFile + ".settings";
                var next = 1;
                while (System.IO.File.Exists(final))
                    final = baseFile + next++ + ".settings";
                s.Save(final);

                // add it to the list
                this.comboSettings.Items.Add(s);
                this.comboSettings.SelectedItem = s;
            }
        }

        private void btnSettingsDelete_Click(object sender, EventArgs e)
        {
            if (this.comboSettings.SelectedItem == null)
                return;
            var settings = (GuiSettings)this.comboSettings.SelectedItem;
            if (settings.Name == DefaultSettingsName)
                return;
            this.comboSettings.Items.Remove(settings);
            File.Delete(settings.Path);
        }

        private void comboSettings_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (this.comboSettings.SelectedItem == null)
                return;
            var settings = (GuiSettings)this.comboSettings.SelectedItem;
            this.btnSettingsDelete.Enabled = settings.Name != DefaultSettingsName;
            this.Settings = settings;
        }

        private void btnEditColors_Click(object sender, EventArgs e)
        {
            var current = this.Settings;
            var dlg = new PropertyDlg();
            dlg.EditObject = current;
            if (DialogResult.OK != dlg.ShowDialog(this))
                return;
            this.Settings = current;
        }

        private void menuStereo_Click(object sender, EventArgs e)
        {
            this.menuStereoNone.Checked =
            this.menuStereoCrossEyed.Checked =
            this.menuStereoAnaglyph.Checked =
            this.menuStereoAnaglyphNoColor.Checked = false;

            var item = (ToolStripMenuItem)sender;
            item.Checked = true;
        }

        private void menuStereo_CheckChanged(object sender, EventArgs e)
        {
            var item = (ToolStripMenuItem)sender;
            if (item.Checked)
            {
                var stereoMode = (int)item.Tag;
                this.cube.SetParameter(CubeParameter.StereoMode, stereoMode);
            }
        }

        private struct Axes
        {
            public int a1, a2;

            public Axes(int a1, int a2)
            {
                this.a1 = a1;
                this.a2 = a2;
            }
        }
    }

    public class DrawSurface : Control
    {
        public DrawSurface()
        {
            SetStyle(ControlStyles.UserMouse, true);
            SetStyle(ControlStyles.UserPaint, true);
            SetStyle(ControlStyles.AllPaintingInWmPaint, true);
            SetStyle(ControlStyles.Selectable, true);
        }

        protected override bool IsInputKey(Keys keyData)
        {
            if (keyData == Keys.Left || keyData == Keys.Right || keyData == Keys.Up || keyData == Keys.Down)
                return true;
            return base.IsInputKey(keyData);
        }

        protected override void OnPaintBackground(PaintEventArgs pevent)
        {
            // do nothing, otherwise we get flicker-city
        }
    }
}
