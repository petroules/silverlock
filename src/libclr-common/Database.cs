namespace Petroules.Silverlock
{
    using System;
    using System.Xml;

    public class Database : Group
    {
        private string password;
        private int compression;

        public Database(string title = "", string password = "")
            : base(title)
        {
            this.password = password;
            this.compression = -1;
        }

        public override void Destroy()
        {
        }

        public static Version Version
        {
            get { return new Version("1.0"); }
        }

        public string Password
        {
            get
            {
                return this.password;
            }

            set
            {
                if (this.password != value)
                {
                    this.password = value;
                    this.SetModified();
                }
            }
        }

        public int Compression
        {
            get
            {
                return this.compression;
            }

            set
            {
                if (this.compression != value)
                {
                    // If the compression is outside the proper range...
                    if (compression < -1 || compression > 9)
                    {
                        // ...default to default compression
                        compression = -1;
                    }

                    this.compression = value;
                    this.SetModified();
                }
            }
        }

        public override Group ParentNode
        {
            get { return base.ParentNode; }
            set { /* do nothing */ }
        }

        public override XmlElement ToXml(XmlDocument document)
        {
            XmlElement element = this.ToXml(document, DatabaseKeys.XML_DATABASE);
            element.SetAttribute(DatabaseKeys.XML_VERSION, Database.Version.ToString());
            element.SetAttribute(DatabaseKeys.XML_DBPASSWORD, this.password);
            element.SetAttribute(DatabaseKeys.XML_COMPRESSION, this.compression.ToString());
            return element;
        }

        protected override void AttachToList()
        {
        }

        protected override void DetachFromList()
        {
        }

        protected internal override void FromXml(XmlElement element)
        {
            base.FromXml(element);

            this.Password = element.Attributes[DatabaseKeys.XML_DBPASSWORD].Value;
            try
            {
                this.Compression = Convert.ToInt32(element.Attributes[DatabaseKeys.XML_COMPRESSION].Value);
            }
            catch (FormatException)
            {
                this.Compression = -1;
            }
        }
    }
}
