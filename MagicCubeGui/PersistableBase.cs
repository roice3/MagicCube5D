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

        public virtual void Save(string file)
        {
            // write all of our props
            var outFile = new StreamWriter(File.OpenWrite(file));
            foreach (PropertyDescriptor p in this.PersistProps)
            {
                var value = p.GetValue(this);
                var tc = TypeDescriptor.GetConverter(value);
                outFile.WriteLine("{0}={1}", p.Name, tc.ConvertToString(value));
            }
            outFile.Close();
        }

        public virtual void Load(string file)
        {
            // read all of the key/value pairs
            var values = new Dictionary<string, string>();
            foreach (var s in File.ReadAllLines(file))
            {
                var idx = s.IndexOf('=');
                var key = s.Substring(0, idx);
                var value = s.Substring(idx + 1);
                values[key] = value;
            }

            // convert each back to its original type
            foreach (PropertyDescriptor p in this.PersistProps)
            {
                if (!values.ContainsKey(p.Name))
                    continue;
                var tc = TypeDescriptor.GetConverter(p.PropertyType);
                try
                {
                    var o = tc.ConvertFromString(values[p.Name]);
                    p.SetValue(this, o);
                }
                catch
                {
                    System.Diagnostics.Debug.WriteLine("Failed to reload " + p.Name);
                }
            }
        }
    }

}
