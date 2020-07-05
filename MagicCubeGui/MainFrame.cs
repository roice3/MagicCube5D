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
        string[] GetMacroNames();

        void StartMacroRecord();

        void StopMacroRecord();

        void RenameMacro(int index, string name);

        void DeleteMacro(int index);

        void ExecuteMacro(int index, bool reverse);
    }

    public partial class MainFrame : Form
    {
        private const string DefaultSettingsName = "-- Default --";
        private readonly ICube cube;

        private GuiSettings lastSettings = new GuiSettings();

        public MainFrame(ICube cube)
        {
            InitializeComponent();

            // setup the drawing surface
            DrawSurface = new DrawSurface
            {
                Location = panelMain.Location,
                Size = panelMain.Size,
                Anchor = panelMain.Anchor
            };
            Controls.Add(DrawSurface);

            // setup tags for sliders
            track5dDist.Tag = CubeParameter.Perspective5d;
            track4dDist.Tag = CubeParameter.Perspective4d;
            traceFaceSeparation.Tag = CubeParameter.FaceSeparation;
            trackStickerSeparation.Tag = CubeParameter.StickerSeparation;
            trackStickerSize.Tag = CubeParameter.StickerSize;
            trackRotationRate.Tag = CubeParameter.RotationRate;
            trackStereoSeparation.Tag = CubeParameter.StereoSeparation;

            // setup tags for face checkboxes
            checkFace1.Tag = 0;
            checkFace2.Tag = 1;
            checkFace3.Tag = 2;
            checkFace4.Tag = 3;
            checkFace5.Tag = 4;
            checkFace6.Tag = 5;
            checkFace7.Tag = 6;
            checkFace8.Tag = 7;
            checkFace9.Tag = 8;
            checkFace10.Tag = 9;

            // setup tags for rotation buttons
            btnXY.Tag = new Axes(0, 1);
            btnXZ.Tag = new Axes(0, 2);
            btnXU.Tag = new Axes(0, 3);
            btnXV.Tag = new Axes(0, 4);
            btnYZ.Tag = new Axes(1, 2);
            btnYU.Tag = new Axes(1, 3);
            btnYV.Tag = new Axes(1, 4);
            btnZU.Tag = new Axes(2, 3);
            btnZV.Tag = new Axes(2, 4);
            btnUV.Tag = new Axes(3, 4);

            // default rotations to all
            comboFace.SelectedIndex = 10;

            // setup tags for puzzle type.
            menuPuzzle2.Tag = 2;
            menuPuzzle3.Tag = 3;
            menuPuzzle4.Tag = 4;
            menuPuzzle5.Tag = 5;
            menuPuzzle6.Tag = 6;
            menuPuzzle7.Tag = 7;

            // setup tags for hilight items.
            menuHilight1.Tag = 1;
            menuHilight2.Tag = 2;
            menuHilight3.Tag = 3;
            menuHilight4.Tag = 4;
            menuHilight5.Tag = 5;

            // setup tags for scramble menu items.
            menuScramble1.Tag = 1;
            menuScramble2.Tag = 2;
            menuScramble3.Tag = 3;
            menuScramble4.Tag = 4;
            menuScramble5.Tag = 5;
            menuScrambleFull.Tag = 100;

            // setup tags for stereo.
            menuStereoNone.Tag = 0;
            menuStereoCrossEyed.Tag = 1;
            menuStereoAnaglyph.Tag = 2;
            menuStereoAnaglyphNoColor.Tag = 3;

            btnStartMacro.Enabled = true;
            btnStopMacro.Enabled = false;

            this.cube = cube;
        }

        public DrawSurface DrawSurface { get; }

        private GuiSettings Settings
        {
            get
            {
                // refresh from input controls
                lastSettings.Perspective5d = track5dDist.Value;
                lastSettings.Perspective4d = track4dDist.Value;
                lastSettings.StickerSeparation = trackStickerSeparation.Value;
                lastSettings.StickerSize = trackStickerSize.Value;
                lastSettings.FaceSeparation = traceFaceSeparation.Value;
                lastSettings.RotationRate = trackRotationRate.Value;
                if (menuStereoNone.Checked) lastSettings.StereoMode = 0;
                else if (menuStereoCrossEyed.Checked) lastSettings.StereoMode = 1;
                else if (menuStereoAnaglyph.Checked) lastSettings.StereoMode = 2;
                else if (menuStereoAnaglyphNoColor.Checked) lastSettings.StereoMode = 3;
                lastSettings.StereoSeparation = trackStereoSeparation.Value;
                lastSettings.Face1 = checkFace1.Checked;
                lastSettings.Face2 = checkFace2.Checked;
                lastSettings.Face3 = checkFace3.Checked;
                lastSettings.Face4 = checkFace4.Checked;
                lastSettings.Face5 = checkFace5.Checked;
                lastSettings.Face6 = checkFace6.Checked;
                lastSettings.Face7 = checkFace7.Checked;
                lastSettings.Face8 = checkFace8.Checked;
                lastSettings.Face9 = checkFace9.Checked;
                lastSettings.Face10 = checkFace10.Checked;

                return lastSettings.Clone();
            }

            set
            {
                lastSettings = value.Clone();

                // update the input controls and the cube
                cube.EnableRedraw = false;
                track5dDist.Value = value.Perspective5d;
                track4dDist.Value = value.Perspective4d;
                trackStickerSeparation.Value = value.StickerSeparation;
                trackStickerSize.Value = value.StickerSize;
                traceFaceSeparation.Value = value.FaceSeparation;
                trackRotationRate.Value = value.RotationRate;
                menuStereoNone.Checked = 0 == value.StereoMode ? true : false;
                menuStereoCrossEyed.Checked = 1 == value.StereoMode ? true : false;
                menuStereoAnaglyph.Checked = 2 == value.StereoMode ? true : false;
                menuStereoAnaglyphNoColor.Checked = 3 == value.StereoMode ? true : false;
                trackStereoSeparation.Value = value.StereoSeparation;
                checkFace1.Checked = value.Face1;
                checkFace2.Checked = value.Face2;
                checkFace3.Checked = value.Face3;
                checkFace4.Checked = value.Face4;
                checkFace5.Checked = value.Face5;
                checkFace6.Checked = value.Face6;
                checkFace7.Checked = value.Face7;
                checkFace8.Checked = value.Face8;
                checkFace9.Checked = value.Face9;
                checkFace10.Checked = value.Face10;
                cube.ColorFace(0, value.Color1);
                cube.ColorFace(1, value.Color2);
                cube.ColorFace(2, value.Color3);
                cube.ColorFace(3, value.Color4);
                cube.ColorFace(4, value.Color5);
                cube.ColorFace(5, value.Color6);
                cube.ColorFace(6, value.Color7);
                cube.ColorFace(7, value.Color8);
                cube.ColorFace(8, value.Color9);
                cube.ColorFace(9, value.Color10);
                cube.ColorFace(10, value.ColorBg);
                cube.EnableRedraw = true;
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

            labelStickers.Text = string.Format("Stickers Solved: {0} of {1} ({2:F1}%)", ss, ns, 100.0 * ss / ns);
            labelCubies.Text = string.Format("Cubies Solved:   {0} of {1} ({2:F1}%)", sc, nc, 100.0 * sc / nc);
            labelZero.Text = string.Format("   0-coloreds:   {0} of {1} ({2:F1}%)", s0, n0, 100.0 * s0 / n0);
            labelOne.Text = string.Format("   1-coloreds:   {0} of {1} ({2:F1}%)", s1, n1, 100.0 * s1 / n1);
            labelTwo.Text = string.Format("   2-coloreds:   {0} of {1} ({2:F1}%)", s2, n2, 100.0 * s2 / n2);
            labelThree.Text = string.Format("   3-coloreds:   {0} of {1} ({2:F1}%)", s3, n3, 100.0 * s3 / n3);
            labelFour.Text = string.Format("   4-coloreds:   {0} of {1} ({2:F1}%)", s4, n4, 100.0 * s4 / n4);
            labelFive.Text = string.Format("   5-coloreds:   {0} of {1} ({2:F1}%)", s5, n5, 100.0 * s5 / n5);

            // Special case code for 2^5 here.
            if (0 == n0)
            {
                labelZero.Text = string.Format("   0-coloreds:   {0} of {1}", s0, n0);
                labelOne.Text = string.Format("   1-coloreds:   {0} of {1}", s1, n1);
                labelTwo.Text = string.Format("   2-coloreds:   {0} of {1}", s2, n2);
                labelThree.Text = string.Format("   3-coloreds:   {0} of {1}", s3, n3);
                labelFour.Text = string.Format("   4-coloreds:   {0} of {1}", s4, n4);
            }
        }

        private static bool ControlDown()
        {
            return (Form.ModifierKeys & Keys.Control) == Keys.Control;
        }

        private void MainFrame_Load(object sender, EventArgs e)
        {
            // setup initial values
            cube.EnableRedraw = false;
            menuHilight1.CheckState = CheckState.Unchecked;
            menuHilight2.CheckState = CheckState.Checked;
            menuHilight3.CheckState = CheckState.Unchecked;
            menuHilight4.CheckState = CheckState.Unchecked;
            menuHilight5.CheckState = CheckState.Unchecked;
            menuPuzzle3.Checked = true;
            cube.EnableRedraw = true;

            // load all of our our macros.
            foreach (var s in cube.GetMacroNames())
                _ = listMacros.Items.Add(s);

            // load all of our saved settings
            GuiSettings defSettings = null;
            foreach (var s in Directory.GetFiles(".", "*.settings"))
            {
                var settings = new GuiSettings();
                settings.Load(s);
                _ = comboSettings.Items.Add(settings);
                if (settings.Name == DefaultSettingsName)
                    defSettings = settings;
            }

            // make sure the "default" settings exist
            if (defSettings == null)
            {
                defSettings = new GuiSettings
                {
                    Name = DefaultSettingsName
                };
                _ = comboSettings.Items.Add(defSettings);
            }

            // select the default settings
            comboSettings.SelectedItem = defSettings;

            // give the draw surface the focus
            DrawSurface.Select();
        }

        private void trackItem_ValueChanged(object sender, EventArgs e)
        {
            var control = (TrackBar)sender;
            if (cube != null)
                cube.SetParameter((CubeParameter)control.Tag, control.Value);
        }

        private void checkFace_CheckedChanged(object sender, EventArgs e)
        {
            var control = (CheckBox)sender;
            if (cube != null)
                cube.EnableFace((int)control.Tag, control.Checked);
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

            if (cube != null)
            {
                var face = comboFace.SelectedIndex;
                if (10 == face)
                    face = -1;
                cube.Rotate(axes.a1, axes.a2, face, 1);
            }
        }

        private void menuSave_Click(object sender, EventArgs e)
        {
            cube.Save(false);
        }

        private void menuSaveAs_Click(object sender, EventArgs e)
        {
            cube.Save(true);
        }

        private void menuLoad_Click(object sender, EventArgs e)
        {
            cube.Load();
        }

        private void menuScramble_Click(object sender, EventArgs e)
        {
            var item = (ToolStripMenuItem)sender;
            var scrambleNumber = (int)item.Tag;
            cube.Scramble(scrambleNumber);
        }

        private void menuPuzzleType_Click(object sender, EventArgs e)
        {
            var item = (ToolStripMenuItem)sender;
            var puzzleType = (int)item.Tag;
            cube.SetParameter(CubeParameter.Puzzle, puzzleType);

            menuPuzzle2.Checked =
            menuPuzzle3.Checked =
            menuPuzzle4.Checked =
            menuPuzzle5.Checked =
            menuPuzzle6.Checked =
            menuPuzzle7.Checked = false;
            item.Checked = true;
        }

        private void menuSolve_Click(object sender, EventArgs e)
        {
            cube.Solve();
        }

        private void menuUndo_Click(object sender, EventArgs e)
        {
            cube.Undo();
        }

        private void menuFind_Click(object sender, EventArgs e)
        {
            using (var dlg = new FindDlg(Settings))
            {
                if (DialogResult.OK != dlg.ShowDialog(this))
                    return;

                var colors = dlg.getSelectedColors();
                cube.HighlightCubies(colors);
            }
        }

        private void menuRedo_Click(object sender, EventArgs e)
        {
            cube.Redo();
        }

        private void menuReset_Click(object sender, EventArgs e)
        {
            cube.Reset();
        }

        private void menuHilight_Click(object sender, EventArgs e)
        {
            var item = (ToolStripMenuItem)sender;
            cube.CycleStickerAccent((int)item.Tag);

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
            var face = comboFace.SelectedIndex;
            enableButton(ref btnXY, face);
            enableButton(ref btnXZ, face);
            enableButton(ref btnXU, face);
            enableButton(ref btnXV, face);
            enableButton(ref btnYZ, face);
            enableButton(ref btnYU, face);
            enableButton(ref btnYV, face);
            enableButton(ref btnZU, face);
            enableButton(ref btnZV, face);
            enableButton(ref btnUV, face);
        }

        private void btnStartMacro_Click(object sender, EventArgs e)
        {
            btnStartMacro.Enabled = false;
            btnStopMacro.Enabled = true;
            cube.StartMacroRecord();
        }

        private void btnStopMacro_Click(object sender, EventArgs e)
        {
            cube.StopMacroRecord();
            _ = listMacros.Items.Add("New Macro");
            btnStartMacro.Enabled = true;
            btnStopMacro.Enabled = false;
        }

        private void renameMacro_Click(object sender, EventArgs e)
        {
            if (1 == listMacros.SelectedIndices.Count)
            {
                var index = listMacros.SelectedIndices[0];
                var item = listMacros.Items[index];
                item.BeginEdit();
            }
        }

        private void deleteMacro_Click(object sender, EventArgs e)
        {
            if (1 == listMacros.SelectedIndices.Count)
            {
                var index = listMacros.SelectedIndices[0];
                listMacros.Items.RemoveAt(index);
                cube.DeleteMacro(index);
            }
        }

        private void listMacros_AfterLabelEdit(object sender, LabelEditEventArgs e)
        {
            if (1 == listMacros.SelectedIndices.Count)
            {
                var index = listMacros.SelectedIndices[0];
                if (e.Label != null)
                    cube.RenameMacro(index, e.Label);
            }
        }

        private void listMacros_DoubleClick(object sender, EventArgs e)
        {
            if (1 == listMacros.SelectedIndices.Count)
            {
                var index = listMacros.SelectedIndices[0];
                cube.ExecuteMacro(index, ControlDown() ? true : false);
            }
        }

        private void btnSettingsSave_Click(object sender, EventArgs e)
        {
            using (var dlg = new InputDlg())
            {
                // get a name for the settings
                dlg.Caption = "Save Settings";
                dlg.Prompt = "Please provide a name for the new saved settings.";
                if (comboSettings.SelectedItem != null)
                    dlg.Value = ((GuiSettings)comboSettings.SelectedItem).Name;
                if (DialogResult.OK != dlg.ShowDialog(this))
                    return;
                foreach (GuiSettings old in comboSettings.Items)
                {
                    if (old.Name == dlg.Value)
                    {
                        comboSettings.Items.Remove(old);
                        break;
                    }
                }

                // the new settings they are saving
                var s = Settings;
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
                _ = comboSettings.Items.Add(s);
                comboSettings.SelectedItem = s;
            }
        }

        private void btnSettingsDelete_Click(object sender, EventArgs e)
        {
            if (comboSettings.SelectedItem == null)
                return;
            var settings = (GuiSettings)comboSettings.SelectedItem;
            if (settings.Name == DefaultSettingsName)
                return;
            comboSettings.Items.Remove(settings);
            File.Delete(settings.Path);
        }

        private void comboSettings_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (comboSettings.SelectedItem == null)
                return;
            var settings = (GuiSettings)comboSettings.SelectedItem;
            btnSettingsDelete.Enabled = settings.Name != DefaultSettingsName;
            Settings = settings;
        }

        private void btnEditColors_Click(object sender, EventArgs e)
        {
            var current = Settings;
            var dlg = new PropertyDlg
            {
                EditObject = current
            };
            if (DialogResult.OK != dlg.ShowDialog(this))
                return;
            Settings = current;
        }

        private void menuStereo_Click(object sender, EventArgs e)
        {
            menuStereoNone.Checked =
            menuStereoCrossEyed.Checked =
            menuStereoAnaglyph.Checked =
            menuStereoAnaglyphNoColor.Checked = false;

            var item = (ToolStripMenuItem)sender;
            item.Checked = true;
        }

        private void menuStereo_CheckChanged(object sender, EventArgs e)
        {
            var item = (ToolStripMenuItem)sender;
            if (item.Checked)
            {
                var stereoMode = (int)item.Tag;
                cube.SetParameter(CubeParameter.StereoMode, stereoMode);
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
