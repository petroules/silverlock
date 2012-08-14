namespace Petroules.Silverlock
{
    using System;
    using System.Globalization;
    using System.IO;
    using System.Xml;
    using System.Text;

    public class DatabaseReader
    {
        public DatabaseReader()
        {
            this.ErrorString = string.Empty;
        }

        public string ErrorString
        {
            get;
            private set;
        }

        public Database Read(StreamReader stream, string password)
        {
            // Clear the last error
            this.ErrorString = string.Empty;

            // Check if the stream is unreadable
            if (!stream.EndOfStream)
            {
                this.ErrorString = "Stream was not readable.";
                return null;
            }

            // Get the contents of the database file and create a new XML document
            string fileDataString = stream.ReadToEnd();

            XmlDocument doc = new XmlDocument();

            // Try and see if it's a valid XML document; if it is, we'll simply continue on and read
            // it (unencrypted mode), if it's not, we'll try to decrypt it (encrypted mode)
            try
            {
                doc.LoadXml(fileDataString);

                DatabaseCrypto.CryptoStatus status;
                fileDataString = DatabaseCrypto.Decrypt(fileDataString, password, out status);
                if (status != DatabaseCrypto.CryptoStatus.NoError)
                {
                    this.ErrorString = DatabaseCrypto.StatusMessage(status);
                    return null;
                }
            }
            catch (XmlException)
            {
            }

            try
            {
                doc.LoadXml(fileDataString);

                XmlElement root = doc.DocumentElement;
                if (root.Name != DatabaseKeys.XML_DATABASE)
                {
                    this.ErrorString = "Missing root XML element.";
                    return null;
                }

                Version version = new Version(root.Attributes[DatabaseKeys.XML_VERSION].Value);
                if (version != Database.Version)
                {
                    this.ErrorString = string.Format(CultureInfo.InvariantCulture, "Unsupported database version: {0}", version);
                    return null;
                }

                Database db = new Database();
                db.FromXml(root);

                // Recursively read in all the groups and their subnodes
                this.ReadGroup(db, root);

                return db;
            }
            catch (XmlException)
            {
                this.ErrorString = "Unable to parse the XML tree.";
                return null;
            }
        }

        private void ReadGroup(Group group, XmlElement element)
        {
            if (group == null)
            {
                return;
            }

            // Find the first child of the element we were given to process
            XmlNode node = element.FirstChild;
            while (node != null)
            {
                // Convert the node into an element, which should be either a group or entry tag
                XmlElement e = node as XmlElement;
                if (e != null)
                {
                    if (e.Name == DatabaseKeys.XML_GROUP)
                    {
                        Group ourGroup = new Group();
                        ourGroup.FromXml(e);
                        ourGroup.ParentNode = group;

                        // Call the method again to recursively add child nodes to the one we just added
                        this.ReadGroup(ourGroup, e);
                    }
                    else if (e.Name == DatabaseKeys.XML_ENTRY)
                    {
                        // If we found an entry, the process here is simply - read all the
                        // properties and then add it to the group's list of entries
                        Entry entry = new Entry();
                        entry.FromXml(e);
                        entry.ParentNode = group;
                    }
                }

                // Continue on with the loop to process the next sibling node (which can be a group or entry)
                node = node.NextSibling;
            }
        }
    }
}
