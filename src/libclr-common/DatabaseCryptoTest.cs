namespace Petroules.Silverlock.Test
{
    using System;
    using System.Collections.Generic;
    using System.Diagnostics;
    using System.Globalization;
    using System.IO;
    using System.Linq;
    using System.Text;
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using Petroules.Silverlock.Test.Properties;

    [TestClass]
    public class DatabaseCryptoTest
    {
        [TestMethod]
        public void TestTwoWayFileEncryption()
        {
            const string password = "MyS3cr3tP4$$w0rd";
            byte[] encryptedBytes = null;
            string encryptedString = null;

            using (MemoryStream stream = new MemoryStream())
            using (StreamWriter writer = new StreamWriter(stream, Encoding.UTF8))
            {
                WriteEncryptedData(writer, TestData.LoremIpsum, password);

                encryptedBytes = stream.ToArray();
                encryptedString = Encoding.UTF8.GetString(encryptedBytes);
                Trace.WriteLine(string.Format(CultureInfo.InvariantCulture, "Encrypted file ({0} bytes, {1} characters): ", encryptedBytes.Length, encryptedString.Length));
                Trace.WriteLine(encryptedString);
            }

            byte[] encryptedBytesFromString = Encoding.UTF8.GetBytes(encryptedString);
            Assert.IsTrue(encryptedBytes.SequenceEqual(encryptedBytesFromString), "Byte array to string and back caused loss of data");

            using (MemoryStream stream = new MemoryStream(encryptedBytesFromString))
            using (StreamReader reader = new StreamReader(stream, Encoding.UTF8))
            {
                string decryptedString = ReadEncryptedData(reader, password);

                Trace.WriteLine("Decrypted file:");
                Trace.WriteLine(decryptedString);

                Assert.IsTrue(TestData.LoremIpsum == decryptedString, "Data did not decrypt back to original");
            }
        }

        [TestMethod]
        public void TestTwoWayEncryption()
        {
            const string password = "MyS3cr3tP4$$w0rd";

            EncryptedData encryptedData = DatabaseCrypto.Encrypt(password, TestData.LoremIpsum);
            Trace.WriteLine("Encrypted data:");
            Trace.WriteLine("Salt: " + encryptedData.SaltString);
            Trace.WriteLine("MAC: " + encryptedData.MACString);
            Trace.WriteLine("Encrypted data: " + encryptedData.DataString);

            string decryptedData = DatabaseCrypto.Decrypt(password, encryptedData);
            Trace.WriteLine("Decrypted data: " + decryptedData);

            Assert.IsTrue(TestData.LoremIpsum == decryptedData, "Data did not decrypt back to original");
        }

        [TestMethod]
        public void TestTwoWayCompression()
        {
            byte[] bytes = Encoding.UTF8.GetBytes(TestData.LoremIpsum);

            byte[] compressed = DatabaseCrypto.Compress(bytes);
            byte[] decompressed = DatabaseCrypto.Decompress(compressed);
            Assert.IsTrue(bytes.SequenceEqual(decompressed), "Data did not decompress back to original");
        }

        [TestMethod]
        public void TestStandardSilverlockDatabaseDecryption()
        {
            List<Tuple<string, string, string>> databases = new List<Tuple<string, string, string>>();
            databases.Add(new Tuple<string, string, string>(TestData.TestDatabasePassword, "test", TestData.TestDatabasePasswordXml));
            databases.Add(new Tuple<string, string, string>(TestData.TestDatabaseNoPassword, string.Empty, TestData.TestDatabaseNoPasswordXml));

            foreach (var item in databases)
            {
                /*using (MemoryStream stream = new MemoryStream(Encoding.UTF8.GetBytes(item.Item1)))
                using (StreamReader reader = new StreamReader(stream, Encoding.UTF8))
                {
                    string decryptedData = ReadEncryptedData(reader, item.Item2);
                    Assert.AreEqual(item.Item3, decryptedData);
                    Trace.WriteLine(decryptedData);
                }*/

                DatabaseCrypto.CryptoStatus status;
                string decryptedData = DatabaseCrypto.Decrypt(item.Item1, item.Item2, out status);
                Trace.WriteLine(decryptedData);
                Assert.AreEqual(DatabaseCrypto.CryptoStatus.NoError, status);
                Assert.AreEqual(item.Item3, decryptedData);
            }
        }

        private string ReadEncryptedData(TextReader stream, string password)
        {
            EncryptedData enc = new EncryptedData();
            stream.ReadLine(); // reads the header, we just ignore this
            enc.SaltString = stream.ReadLine();
            enc.MACString = stream.ReadLine();
            enc.DataString = stream.ReadToEnd();

            return DatabaseCrypto.Decrypt(password, enc);
        }

        private void WriteEncryptedData(TextWriter stream, string data, string password)
        {
            EncryptedData enc = DatabaseCrypto.Encrypt(password, data);
            stream.WriteLine("SILVERLOCK DATABASE FILE 1.0 ZLIB");
            stream.WriteLine(enc.SaltString);
            stream.WriteLine(enc.MACString);
            stream.WriteLine(enc.DataString);

            // VERY important... I wonder if this should be called by the caller though?
            stream.Flush();
        }
    }
}
