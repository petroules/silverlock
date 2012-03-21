namespace Petroules.Silverlock
{
    using System;
    using System.Xml;

    public abstract class DatabaseNode
    {
        private Group parent;
        private DateTime created;
        private DateTime accessed;
        private DateTime modified;
        private Guid uuid;
        private string title;

        protected DatabaseNode(string title = "")
        {
            this.created = DateTime.UtcNow;
            this.accessed = DateTime.UtcNow;
            this.modified = DateTime.UtcNow;
            this.uuid = Guid.NewGuid();
            this.title = title;
        }

        public abstract void Destroy();

        public DateTime Created
        {
            get
            {
                return this.created.ToUniversalTime();
            }

            private set
            {
                if (this.created != value)
                {
                    this.created = value.ToUniversalTime();
                }
            }
        }

        public DateTime Accessed
        {
            get
            {
                return this.accessed.ToUniversalTime();
            }

            private set
            {
                if (this.accessed != value)
                {
                    this.accessed = value.ToUniversalTime();
                }
            }
        }

        public DateTime Modified
        {
            get
            {
                return this.modified.ToUniversalTime();
            }

            protected set
            {
                if (this.modified != value)
                {
                    this.modified = value.ToUniversalTime();
                }
            }
        }

        public Guid UUID
        {
            get
            {
                return this.uuid;
            }

            protected set
            {
                if (this.uuid != value)
                {
                    this.uuid = value;
                    this.SetModified();
                }
            }
        }

        public string Title
        {
            get
            {
                return this.title;
            }

            set
            {
                if (this.title != value)
                {
                    this.title = value;
                    this.SetModified();
                }
            }
        }

        public virtual Group ParentNode
        {
            get
            {
                return this.parent;
            }

            set
            {
                // Cant't self-parent or parent to the same node
                if (this == value || this.parent == value)
                {
                    return;
                }

                this.Detach();
                this.parent = value;
                this.Attach();
            }
        }

        public Group RootNode
        {
            get
            {
                if (!this.IsAttached)
                {
                    return null;
                }

                Group parent = this.parent;
                while (parent.ParentNode != null)
                {
                    parent = parent.ParentNode;
                }

                return parent;
            }
        }

        public bool HasDatabase
        {
            get { return this.RootNode as Database != null; }
        }

        public bool IsAttached
        {
            get { return this.ParentNode != null; }
        }

        public event EventHandler TreeModified = delegate { };

        public void SetAccessed()
        {
            this.Accessed = DateTime.UtcNow;
        }

        public abstract XmlElement ToXml(XmlDocument document);

        protected XmlElement ToXml(XmlDocument document, string tagName)
        {
            XmlElement element = document.CreateElement(tagName);
            element.SetAttribute(DatabaseKeys.XML_CREATED, this.Created.ToUniversalTime().ToString("") + "Z");
            element.SetAttribute(DatabaseKeys.XML_ACCESSED, this.Accessed.ToUniversalTime().ToString("") + "Z");
            element.SetAttribute(DatabaseKeys.XML_MODIFIED, this.Modified.ToUniversalTime().ToString("") + "Z");
            element.SetAttribute(DatabaseKeys.XML_UUID, this.UUID.ToString());
            element.SetAttribute(DatabaseKeys.XML_TITLE, this.Title);
            return element;
        }

        protected void SetModified()
        {
            this.Modified = DateTime.UtcNow;
        }

        protected void Detach()
        {
            if (this.IsAttached)
            {
                this.DetachFromList();
                DatabaseNode parentTemp = this.parent;
                this.parent = null;

                // Notify the tree that it has been modified and
                // disconnect our modification event from the parent's
                this.TreeModified(this, EventArgs.Empty);
                this.TreeModified -= parentTemp.TreeModified;
            }
        }

        protected internal virtual void FromXml(XmlElement element)
        {
            if (this.parent != null)
            {
                throw new InvalidOperationException("Cannot read a DatabaseNode from an XML element if it has a parent");
            }

            this.Created = DateTime.Parse(element.Attributes[DatabaseKeys.XML_CREATED].Value);
            this.Accessed = DateTime.Parse(element.Attributes[DatabaseKeys.XML_ACCESSED].Value);
            this.Modified = DateTime.Parse(element.Attributes[DatabaseKeys.XML_MODIFIED].Value);
            this.UUID = Guid.Parse(element.Attributes[DatabaseKeys.XML_UUID].Value);
            this.Title = element.Attributes[DatabaseKeys.XML_TITLE].Value;
        }

        protected abstract void AttachToList();

        protected abstract void DetachFromList();

        private void Attach()
        {
            if (this.parent != null)
            {
                // Ensure UUID uniquity
                while (this.RootNode.ContainsUUID(this.UUID))
                {
                    this.UUID = Guid.NewGuid();
                }

                // When we add an entry to the parent, the DB was modified
                this.AttachToList();

                // Connect our modification event to the parent's
                // and notify the tree that it has been modified
                this.TreeModified += this.parent.TreeModified;
                this.TreeModified(this, EventArgs.Empty);
            }
        }
    }
}
