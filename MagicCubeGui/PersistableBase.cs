namespace MagicCube5D
{
    using System;
    using System.Collections.Generic;
    using System.ComponentModel;
    using System.IO;

    class PersistableBase
    {
        private PropertyDescriptorCollection PersistProps
        {
            get { return TypeDescriptor.GetProperties(this, new Attribute[] { new PersistableAttribute() }); }
        }

		public virtual void Load( string file )
		{
			// read all of the key/value pairs
			Dictionary<string,string> values = new Dictionary<string,string>();
			foreach( string s in File.ReadAllLines( file ) )
			{
				int idx = s.IndexOf( '=' );
				string key = s.Substring( 0, idx );
				string value = s.Substring( idx + 1 );
				values[key] = value;
			}

			// convert each back to its original type
			foreach( PropertyDescriptor p in this.PersistProps )
			{
				if( ! values.ContainsKey( p.Name ) )
					continue;
				TypeConverter tc = TypeDescriptor.GetConverter( p.PropertyType );
				try
				{
					object o = tc.ConvertFromString( values[p.Name] );
					p.SetValue( this, o );
				}
				catch
				{
					System.Diagnostics.Debug.WriteLine( "Failed to reload " + p.Name );
				}
			}
		}

		private PropertyDescriptorCollection PersistProps
		{
			get { return TypeDescriptor.GetProperties( this, new Attribute[] { new PersistableAttribute() } ); }
		}
	}

	[global::System.AttributeUsage( AttributeTargets.Property, Inherited = true, AllowMultiple = false )]
	sealed class PersistableAttribute : Attribute
	{
		public PersistableAttribute()
		{
		}
	}

}
