using System;
using System.ComponentModel;
using System.Drawing;

namespace MagicCube5D
{
	class GuiSettings : PersistableBase
	{
		private string name;
		private string path;

		[Persistable]
		[Browsable( false )]
		public string Name
		{
			get { return name; }
			set { name = value; }
		}

		[Browsable( false )]
		public string Path
		{
			get { return path; }
		}

		public override void Save( string file )
		{
			base.Save( file );
			this.path = file;
		}

		public override void Load( string file )
		{
			base.Load( file );
			this.path = file;
		}

		public override string ToString()
		{
			return this.name;
		}

		public GuiSettings Clone()
		{
			return (GuiSettings)MemberwiseClone();
		}

		private int perspective5d = 15;
		private int perspective4d = 75;
		private int faceSeparation = 50;
		private int stickerSeparation = 35;
		private int stickerSize = 13;
		private int rotationRate = 35;
		private int stereoMode = 0;
		private int stereoSeparation = 30;

		[Persistable]
		[Browsable( false )]
		public int Perspective5d
		{
			get { return perspective5d; }
			set { perspective5d = value; }
		}

		[Persistable]
		[Browsable( false )]
		public int Perspective4d
		{
			get { return perspective4d; }
			set { perspective4d = value; }
		}

		[Persistable]
		[Browsable( false )]
		public int FaceSeparation
		{
			get { return faceSeparation; }
			set { faceSeparation = value; }
		}

		[Persistable]
		[Browsable( false )]
		public int StickerSeparation
		{
			get { return stickerSeparation; }
			set { stickerSeparation = value; }
		}

		[Persistable]
		[Browsable( false )]
		public int StickerSize
		{
			get { return stickerSize; }
			set { stickerSize = value; }
		}

		[Persistable]
		[Browsable( false )]
		public int RotationRate
		{
			get { return rotationRate; }
			set { rotationRate = value; }
		}

		[Persistable]
		[Browsable( false )]
		public int StereoMode
		{
			get { return stereoMode; }
			set { stereoMode = value; }
		}

		[Persistable]
		[Browsable( false )]
		public int StereoSeparation
		{
			get { return stereoSeparation; }
			set { stereoSeparation = value; }
		}

		private bool face1 = true;
		private bool face2 = true;
		private bool face3 = true;
		private bool face4 = true;
		private bool face5 = true;
		private bool face6 = true;
		private bool face7 = true;
		private bool face8 = true;
		private bool face9 = true;
		private bool face10 = true;

		[Persistable]
		[Browsable( false )]
		public bool Face1
		{
			get { return face1; }
			set { face1 = value; }
		}

		[Persistable]
		[Browsable( false )]
		public bool Face2
		{
			get { return face2; }
			set { face2 = value; }
		}

		[Persistable]
		[Browsable( false )]
		public bool Face3
		{
			get { return face3; }
			set { face3 = value; }
		}

		[Persistable]
		[Browsable( false )]
		public bool Face4
		{
			get { return face4; }
			set { face4 = value; }
		}

		[Persistable]
		[Browsable( false )]
		public bool Face5
		{
			get { return face5; }
			set { face5 = value; }
		}

		[Persistable]
		[Browsable( false )]
		public bool Face6
		{
			get { return face6; }
			set { face6 = value; }
		}

		[Persistable]
		[Browsable( false )]
		public bool Face7
		{
			get { return face7; }
			set { face7 = value; }
		}

		[Persistable]
		[Browsable( false )]
		public bool Face8
		{
			get { return face8; }
			set { face8 = value; }
		}

		[Persistable]
		[Browsable( false )]
		public bool Face9
		{
			get { return face9; }
			set { face9 = value; }
		}

		[Persistable]
		[Browsable( false )]
		public bool Face10
		{
			get { return face10; }
			set { face10 = value; }
		}

		private Color color1 = ColorF( 1, 0, 0 );
		private Color color2 = ColorF( 0, 1, 0 );
		private Color color3 = ColorF( 0, 0, 1 );
		private Color color4 = ColorF( 1, 1, 0 );
		private Color color5 = ColorF( 0, 1, 1 );
		private Color color6 = ColorF( 1, 0, 1 );
		private Color color7 = ColorF( .5, .5, 1 );
		private Color color8 = ColorF( 1, 1, 1 );
		private Color color9 = ColorF( 1, 0, .5 );
		private Color color10 = ColorF( 1, .5, 0 );
		private Color colorBg = ColorF( 0, 0, 0 );

		// make a color from floats
		private static Color ColorF( double r, double g, double b )
		{
			return Color.FromArgb( (int)(r * 255), (int)(g * 255), (int)(b * 255) );
		}

		[Persistable]
		[DisplayName( "Face 1 Color" )]
		[Category( "Face Coloring" )]
		public Color Color1
		{
			get { return color1; }
			set { color1 = value; }
		}

		[Persistable]
		[DisplayName( "Face 2 Color" )]
		[Category( "Face Coloring" )]
		public Color Color2
		{
			get { return color2; }
			set { color2 = value; }
		}

		[Persistable]
		[DisplayName( "Face 3 Color" )]
		[Category( "Face Coloring" )]
		public Color Color3
		{
			get { return color3; }
			set { color3 = value; }
		}

		[Persistable]
		[DisplayName( "Face 4 Color" )]
		[Category( "Face Coloring" )]
		public Color Color4
		{
			get { return color4; }
			set { color4 = value; }
		}

		[Persistable]
		[DisplayName( "Face 5 Color" )]
		[Category( "Face Coloring" )]
		public Color Color5
		{
			get { return color5; }
			set { color5 = value; }
		}

		[Persistable]
		[DisplayName( "Face 6 Color" )]
		[Category( "Face Coloring" )]
		public Color Color6
		{
			get { return color6; }
			set { color6 = value; }
		}

		[Persistable]
		[DisplayName( "Face 7 Color" )]
		[Category( "Face Coloring" )]
		public Color Color7
		{
			get { return color7; }
			set { color7 = value; }
		}

		[Persistable]
		[DisplayName( "Face 8 Color" )]
		[Category( "Face Coloring" )]
		public Color Color8
		{
			get { return color8; }
			set { color8 = value; }
		}

		[Persistable]
		[DisplayName( "Face 9 Color" )]
		[Category( "Face Coloring" )]
		public Color Color9
		{
			get { return color9; }
			set { color9 = value; }
		}

		[Persistable]
		[DisplayName( "Face 10 Color" )]
		[Category( "Face Coloring" )]
		public Color Color10
		{
			get { return color10; }
			set { color10 = value; }
		}

		[Persistable]
		[DisplayName( "Background Color" )]
		[Category( "Face Coloring" )]
		public Color ColorBg
		{
			get { return colorBg; }
			set { colorBg = value; }
		}
	}


}
