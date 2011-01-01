namespace Petroules.Silverlock
{
    using System;
    using System.Text;

    public class EncryptedData
    {
        public EncryptedData()
        {
        }

        public EncryptedData(byte[] salt, byte[] mac, byte[] data)
        {
            this.Salt = salt;
            this.MAC = mac;
            this.Data = data;
        }

        public EncryptedData(string salt, string mac, string data)
        {
            this.SaltString = salt;
            this.MACString = mac;
            this.DataString = data;
        }

        public byte[] Salt
        {
            get;
            set;
        }

        public string SaltString
        {
            get { return Convert.ToBase64String(this.Salt); }
            set { this.Salt = Convert.FromBase64String(value); }
        }

        public byte[] MAC
        {
            get;
            set;
        }

        public string MACString
        {
            get { return Convert.ToBase64String(this.MAC); }
            set { this.MAC = Convert.FromBase64String(value); }
        }

        public byte[] Data
        {
            get;
            set;
        }

        public string DataString
        {
            get { return Convert.ToBase64String(this.Data, Base64FormattingOptions.InsertLineBreaks); }
            set { this.Data = Convert.FromBase64String(value); }
        }
    }
}