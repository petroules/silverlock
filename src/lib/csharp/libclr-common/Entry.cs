namespace Petroules.Silverlock
{
    using System;
    using System.Collections.Generic;
    using System.Xml;

    public class Entry : DatabaseNode
    {
        private Uri url;
        private string username;
        private string password;
        private string notes;
        private SortedDictionary<string, string> recoveryInfo = new SortedDictionary<string, string>();
        private SortedDictionary<string, string> customFields = new SortedDictionary<string, string>();

        public Entry(string title = "", Group parent = null)
            : base(title)
        {
            this.ParentNode = parent;
        }

        public override void Destroy()
        {
            this.Detach();
        }

        public string Username
        {
            get
            {
                return this.username;
            }

            set
            {
                if (this.username != value)
                {
                    this.username = value;
                    this.SetModified();
                }
            }
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

        public Uri URL
        {
            get
            {
                return this.url;
            }

            set
            {
                if (this.url != value)
                {
                    this.url = value;
                    this.SetModified();
                }
            }
        }

        public string Notes
        {
            get
            {
                return this.notes;
            }

            set
            {
                if (this.notes != value)
                {
                    this.notes = value;
                    this.SetModified();
                }
            }
        }

        public ReadOnlyDictionary<string, string> RecoveryInfo
        {
            get { return new ReadOnlyDictionary<string, string>(this.recoveryInfo); }
        }

        public ReadOnlyDictionary<string, string> CustomFields
        {
            get { return new ReadOnlyDictionary<string, string>(this.customFields); }
        }

        public void InsertRecoveryInfo(string question, string answer)
        {
            // Blank questions are not accepted!
            if (string.IsNullOrEmpty(question) || this.recoveryInfo.ContainsKey(question))
            {
                return;
            }

            this.recoveryInfo.Add(question, answer);
            this.SetModified();
        }

        public bool RemoveRecoveryInfo(string question)
        {
            bool removed = this.recoveryInfo.Remove(question);

            // If an item was actually removed...
            if (removed)
            {
                this.SetModified();
            }

            return removed;
        }

        public void ClearRecoveryInfo()
        {
            if (this.recoveryInfo.Count > 0)
            {
                this.recoveryInfo.Clear();
                this.SetModified();
            }
        }

        public void InsertCustomField(string key, string value)
        {
            // Blank keys are not accepted!
            if (string.IsNullOrEmpty(key) || this.customFields.ContainsKey(key))
            {
                return;
            }

            this.customFields.Add(key, value);
            this.SetModified();
        }

        public bool RemoveCustomField(string key)
        {
            bool removed = this.customFields.Remove(key);

            // If an item was actually removed...
            if (removed)
            {
                this.SetModified();
            }

            return removed;
        }

        public void ClearCustomFields()
        {
            if (this.customFields.Count > 0)
            {
                this.customFields.Clear();
                this.SetModified();
            }
        }

        public override XmlElement ToXml(XmlDocument document)
        {
            XmlElement element = this.ToXml(document, DatabaseKeys.XML_ENTRY);
            element.SetAttribute(DatabaseKeys.XML_USERNAME, this.Username);
            element.SetAttribute(DatabaseKeys.XML_PASSWORD, this.Password);
            element.SetAttribute(DatabaseKeys.XML_URL, this.URL.ToString());
            element.SetAttribute(DatabaseKeys.XML_NOTES, this.Notes);

            // Add all the recovery info
            foreach (var i in this.RecoveryInfo)
            {
                XmlElement pair = document.CreateElement(DatabaseKeys.XML_RECOVERYINFO);
                pair.SetAttribute(DatabaseKeys.XML_QUESTION, i.Key);
                pair.SetAttribute(DatabaseKeys.XML_ANSWER, i.Value);
                element.AppendChild(pair);
            }

            // Add all the custom fields
            foreach (var i in this.CustomFields)
            {
                XmlElement pair = document.CreateElement(DatabaseKeys.XML_CUSTOMFIELD);
                pair.SetAttribute(DatabaseKeys.XML_CFNAME, i.Key);
                pair.SetAttribute(DatabaseKeys.XML_CFVALUE, i.Value);
                element.AppendChild(pair);
            }

            return element;
        }

        public Entry CreateCopy()
        {
            Entry entry = new Entry();
            entry.Title = this.Title;
            entry.Username = this.Username;
            entry.Password = this.Password;
            entry.URL = this.URL;
            entry.Notes = this.Notes;

            foreach (var i in this.RecoveryInfo)
            {
                entry.InsertRecoveryInfo(i.Key, i.Value);
            }

            foreach (var i in this.CustomFields)
            {
                entry.InsertCustomField(i.Key, i.Value);
            }

            return entry;
        }

        protected override void AttachToList()
        {
            this.ParentNode.entries.Add(this);
        }

        protected override void DetachFromList()
        {
            this.ParentNode.entries.Remove(this);
        }

        protected internal override void FromXml(XmlElement element)
        {
            base.FromXml(element);

            this.Username = element.Attributes[DatabaseKeys.XML_USERNAME].Value;
            this.Password = element.Attributes[DatabaseKeys.XML_PASSWORD].Value;
            this.URL = new Uri(element.Attributes[DatabaseKeys.XML_URL].Value);
            this.Notes = element.Attributes[DatabaseKeys.XML_NOTES].Value;

            // This function should NEVER be called from a parented DatabaseNode
            // It is private and designed to be called in DatabaseReader only
            // immediately after an Entry is instantiated, therefore they should
            // always be empty at this point
            if (this.customFields.Count > 0)
            {
                throw new InvalidOperationException("Cannot read an Entry from an XML element if it has custom fields.");
            }

            if (this.recoveryInfo.Count > 0)
            {
                throw new InvalidOperationException("Cannot read an Entry from an XML element if it has recovery info.");
            }

            // Here we'll process any subnodes of the entry containing additional information
            XmlNode entryNode = element.FirstChild;
            while (entryNode != null)
            {
                XmlElement entryElement = entryNode as XmlElement;
                if (entryElement != null)
                {
                    if (entryElement.Name == DatabaseKeys.XML_CUSTOMFIELD)
                    {
                        this.customFields.Add(entryElement.Attributes[DatabaseKeys.XML_CFNAME].Value, entryElement.Attributes[DatabaseKeys.XML_CFVALUE].Value);
                    }
                    else if (entryElement.Name == DatabaseKeys.XML_RECOVERYINFO)
                    {
                        this.recoveryInfo.Add(entryElement.Attributes[DatabaseKeys.XML_QUESTION].Value, entryElement.Attributes[DatabaseKeys.XML_ANSWER].Value);
                    }
                }

                entryNode = entryNode.NextSibling;
            }
        }
    }
}