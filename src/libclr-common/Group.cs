namespace Petroules.Silverlock
{
    using System;
    using System.Collections.Generic;
    using System.Collections.ObjectModel;
    using System.Text.RegularExpressions;
    using System.Xml;

    public class Group : DatabaseNode
    {
        internal List<Group> groups = new List<Group>();
        internal List<Entry> entries = new List<Entry>();

        public Group(string title = "", Group parent = null)
            : base(title)
        {
            this.ParentNode = parent;
        }

        public override void Destroy()
        {
            this.Detach();

            foreach (Entry entry in this.Entries)
            {
                entry.Destroy();
            }

            foreach (Group subgroup in this.Groups)
            {
                subgroup.Destroy();
            }
        }

        public ReadOnlyCollection<Group> Groups
        {
            get { return this.groups.AsReadOnly(); }
        }

        public ReadOnlyCollection<Entry> Entries
        {
            get { return this.entries.AsReadOnly(); }
        }

        public int CountGroups
        {
            get
            {
                int total = 0;

                foreach (Group group in this.Groups)
                {
                    total += group.CountGroups;
                }

                return total + this.Groups.Count;
            }
        }

        public int CountEntries
        {
            get
            {
                int total = 0;

                foreach (Group group in this.Groups)
                {
                    total += group.CountEntries;
                }

                return total + this.Entries.Count;
            }
        }

        public bool ContainsUUID(Guid uuid)
        {
            return this.IsGroup(uuid) || this.IsEntry(uuid);
        }

        public bool IsGroup(Guid uuid)
        {
            return this.FindGroup(uuid) != null;
        }

        public bool IsEntry(Guid uuid)
        {
            return this.FindEntry(uuid) != null;
        }

        public Group FindGroup(Guid uuid, bool includeThis = false)
        {
            if (includeThis && this.UUID == uuid)
            {
                return this;
            }
            else
            {
                return this.FindGroupInternal(uuid);
            }
        }

        public Entry FindEntry(Guid uuid)
        {
            foreach (Entry entry in this.Entries)
            {
                if (entry.UUID == uuid)
                {
                    return entry;
                }
            }

            foreach (Group group in this.Groups)
            {
                Entry candidate = group.FindEntry(uuid);
                if (candidate != null && candidate.UUID == uuid)
                {
                    return candidate;
                }
            }

            return null;
        }

        public List<Entry> FindEntries(SearchParameters @params)
        {
            if (!@params.FieldsSelected)
            {
                return new List<Entry>();
            }

            Regex regex = new Regex(@params.SearchPattern, @params.CaseSensitive ? RegexOptions.None : RegexOptions.IgnoreCase);
            List<Entry> entries = new List<Entry>();

            foreach (Entry entry in this.Entries)
            {
                List<string> fields = @params.GetDataList(entry);
                foreach (string field in fields)
                {
                    if (@params.UseRegex)
                    {
                        if (!entries.Contains(entry) && regex.IsMatch(field))
                        {
                            entries.Add(entry);

                            // Break out of fields loop so we go to the next entry
                            break;
                        }
                    }
                    else
                    {
                        // ToLowerInvariant effectively makes it a case-insensitive comparison
                        if (!entries.Contains(entry) &&
                            (@params.CaseSensitive
                            ? field.ToLowerInvariant().Contains(@params.SearchPattern.ToLowerInvariant())
                            : field.Contains(@params.SearchPattern)))
                        {
                            entries.Add(entry);

                            // Break out of fields loop so we go to the next entry
                            break;
                        }
                    }
                }
            }

            foreach (Group group in this.Groups)
            {
                entries.AddRange(group.FindEntries(@params));
            }

            return entries;
        }

        public override XmlElement ToXml(XmlDocument document)
        {
            XmlElement element = this.ToXml(document, DatabaseKeys.XML_GROUP);
            return element;
        }

        protected override void AttachToList()
        {
            this.ParentNode.groups.Add(this);
        }

        protected override void DetachFromList()
        {
            this.ParentNode.groups.Remove(this);
        }

        private Group FindGroupInternal(Guid uuid)
        {
            foreach (Group group in this.groups)
            {
                // If this group is a match, return it
                if (group.UUID == uuid)
                {
                    return group;
                }
                else
                {
                    // Otherwise search its children
                    Group candidate = group.FindGroup(uuid, true);
                    if (candidate != null && candidate.UUID == uuid)
                    {
                        return candidate;
                    }
                }
            }

            return null;
        }
    }
}
