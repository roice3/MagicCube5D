namespace MagicCube5D
{
    using System.ComponentModel;
    using System.Drawing;

    internal class GuiSettings : PersistableBase
    {
        [Persistable]
        [Browsable(false)]
        public string Name { get; set; }

        [Browsable(false)]
        public string Path { get; private set; }

        [Persistable]
        [Browsable(false)]
        public int Perspective5d { get; set; } = 15;

        [Persistable]
        [Browsable(false)]
        public int Perspective4d { get; set; } = 75;

        [Persistable]
        [Browsable(false)]
        public int FaceSeparation { get; set; } = 50;

        [Persistable]
        [Browsable(false)]
        public int StickerSeparation { get; set; } = 35;

        [Persistable]
        [Browsable(false)]
        public int StickerSize { get; set; } = 13;

        [Persistable]
        [Browsable(false)]
        public int RotationRate { get; set; } = 35;

        [Persistable]
        [Browsable(false)]
        public int StereoMode { get; set; } = 0;

        [Persistable]
        [Browsable(false)]
        public int StereoSeparation { get; set; } = 30;

        [Persistable]
        [Browsable(false)]
        public bool Face1 { get; set; } = true;

        [Persistable]
        [Browsable(false)]
        public bool Face2 { get; set; } = true;

        [Persistable]
        [Browsable(false)]
        public bool Face3 { get; set; } = true;

        [Persistable]
        [Browsable(false)]
        public bool Face4 { get; set; } = true;

        [Persistable]
        [Browsable(false)]
        public bool Face5 { get; set; } = true;

        [Persistable]
        [Browsable(false)]
        public bool Face6 { get; set; } = true;

        [Persistable]
        [Browsable(false)]
        public bool Face7 { get; set; } = true;

        [Persistable]
        [Browsable(false)]
        public bool Face8 { get; set; } = true;

        [Persistable]
        [Browsable(false)]
        public bool Face9 { get; set; } = true;

        [Persistable]
        [Browsable(false)]
        public bool Face10 { get; set; } = true;

        [Persistable]
        [DisplayName("Face 1 Color")]
        [Category("Face Coloring")]
        public Color Color1 { get; set; } = ColorF(1, 0, 0);

        [Persistable]
        [DisplayName("Face 2 Color")]
        [Category("Face Coloring")]
        public Color Color2 { get; set; } = ColorF(0, 1, 0);

        [Persistable]
        [DisplayName("Face 3 Color")]
        [Category("Face Coloring")]
        public Color Color3 { get; set; } = ColorF(0, 0, 1);

        [Persistable]
        [DisplayName("Face 4 Color")]
        [Category("Face Coloring")]
        public Color Color4 { get; set; } = ColorF(1, 1, 0);

        [Persistable]
        [DisplayName("Face 5 Color")]
        [Category("Face Coloring")]
        public Color Color5 { get; set; } = ColorF(0, 1, 1);

        [Persistable]
        [DisplayName("Face 6 Color")]
        [Category("Face Coloring")]
        public Color Color6 { get; set; } = ColorF(1, 0, 1);

        [Persistable]
        [DisplayName("Face 7 Color")]
        [Category("Face Coloring")]
        public Color Color7 { get; set; } = ColorF(.5, .5, 1);

        [Persistable]
        [DisplayName("Face 8 Color")]
        [Category("Face Coloring")]
        public Color Color8 { get; set; } = ColorF(1, 1, 1);

        [Persistable]
        [DisplayName("Face 9 Color")]
        [Category("Face Coloring")]
        public Color Color9 { get; set; } = ColorF(1, 0, .5);

        [Persistable]
        [DisplayName("Face 10 Color")]
        [Category("Face Coloring")]
        public Color Color10 { get; set; } = ColorF(1, .5, 0);

        [Persistable]
        [DisplayName("Background Color")]
        [Category("Face Coloring")]
        public Color ColorBg { get; set; } = ColorF(0, 0, 0);

        public override void Save(string file)
        {
            base.Save(file);
            Path = file;
        }

        public override void Load(string file)
        {
            base.Load(file);
            Path = file;
        }

        public override string ToString()
        {
            return Name;
        }

        public GuiSettings Clone()
        {
            return (GuiSettings)MemberwiseClone();
        }

        // make a color from floats
        private static Color ColorF(double r, double g, double b)
        {
            return Color.FromArgb((int)(r * 255), (int)(g * 255), (int)(b * 255));
        }
    }
}
