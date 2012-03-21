namespace Petroules.Silverlock
{
    using System.Collections.Generic;

    public class SearchParameters
    {
        private string searchPattern;

        public SearchParameters()
        {
            this.SetDefaults();
        }

        public string SearchPattern
        {
            get { return this.searchPattern ?? string.Empty; }
            set { this.searchPattern = value; }
        }

        public bool SearchTitle;
        public bool SearchUsername;
        public bool SearchPassword;
        public bool SearchURL;
        public bool SearchNotes;
        public bool SearchRecovery;
        public bool SearchCustomFields;
        public bool SearchGroupTitle;
        public bool SearchUUID;
        public bool UseRegex;
        public bool CaseSensitive;

        public void SetDefaults()
        {
            this.SearchTitle = true;
            this.SearchUsername = true;
            this.SearchPassword = false;
            this.SearchURL = true;
            this.SearchNotes = true;
            this.SearchRecovery = false;
            this.SearchCustomFields = true;
            this.SearchGroupTitle = false;
            this.SearchUUID = false;
            this.UseRegex = false;
            this.CaseSensitive = false;
            this.SearchPattern = string.Empty;
        }

        public bool FieldsSelected
        {
            get
            {
                return this.SearchTitle || this.SearchUsername || this.SearchPassword ||
                    this.SearchURL || this.SearchNotes || this.SearchRecovery ||
                    this.SearchCustomFields || this.SearchGroupTitle || this.SearchUUID;
            }
        }

        public List<string> GetDataList(Entry entry)
        {
            List<string> dataItems = new List<string>();

            if (this.SearchTitle)
            {
                dataItems.Add(entry.Title);
            }

            if (this.SearchUsername)
            {
                dataItems.Add(entry.Username);
            }

            if (this.SearchPassword)
            {
                dataItems.Add(entry.Password);
            }

            if (this.SearchURL)
            {
                dataItems.Add(entry.URL.ToString());
            }

            if (this.SearchNotes)
            {
                dataItems.Add(entry.Notes);
            }

            if (this.SearchRecovery)
            {
                foreach (var i in entry.RecoveryInfo)
                {
                    dataItems.Add(i.Key);
                    dataItems.Add(i.Value);
                }
            }

            if (this.SearchCustomFields)
            {
                foreach (var i in entry.CustomFields)
                {
                    dataItems.Add(i.Key);
                    dataItems.Add(i.Value);
                }
            }

            // If the entry doesn't have a parent node we won't check its group title
            if (this.SearchGroupTitle && entry.ParentNode != null)
            {
                dataItems.Add(entry.ParentNode.Title);
            }

            if (this.SearchUUID)
            {
                dataItems.Add(entry.UUID.ToString());
            }

            return dataItems;
        }
    }
}
