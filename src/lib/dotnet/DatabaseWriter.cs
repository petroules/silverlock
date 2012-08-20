namespace Petroules.Silverlock
{
    using System.IO;
    using System.Xml;

    public class DatabaseWriter
    {
        public DatabaseWriter()
        {
            this.ErrorString = string.Empty;
        }

        public string ErrorString
        {
            get;
            private set;
        }

        public bool Write(Database database, StreamWriter stream, bool encrypt = true)
        {
            // Clear the last error
            this.ErrorString = string.Empty;

            XmlDocument document = new XmlDocument();
            this.Append(document, document, database);

            StringWriter sw = new StringWriter();
            XmlTextWriter xw = new XmlTextWriter(sw);
            xw.Formatting = Formatting.Indented;
            xw.Indentation = 4;
            document.WriteTo(xw);
            string fileDataString = sw.ToString();

            if (encrypt)
            {
                DatabaseCrypto.CryptoStatus status;
                fileDataString = DatabaseCrypto.Encrypt(fileDataString, database.Password, database.Compression, out status);
                if (status != DatabaseCrypto.CryptoStatus.NoError)
                {
                    this.ErrorString = DatabaseCrypto.StatusMessage(status);
                    return false;
                }
            }

            stream.Write(fileDataString);
            return true;
        }

        private void Append(XmlDocument document, XmlNode node, Group group)
        {
            if (group == null)
            {
                return;
            }

            XmlElement element = group.ToXml(document);
            node.AppendChild(element);

            foreach (Group childGroup in group.Groups)
            {
                this.Append(document, element, childGroup);
            }

            foreach (Entry entry in group.Entries)
            {
                element.AppendChild(entry.ToXml(document));
            }
        }
    }
}
