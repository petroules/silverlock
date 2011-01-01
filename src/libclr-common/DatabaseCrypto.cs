namespace Petroules.Silverlock
{
    using System;
    using System.IO;
    using System.Security.Cryptography;
    using System.Text;
    using System.IO.Compression;
    using System.Linq;

    public static class DatabaseCrypto
    {
        public static EncryptedData Encrypt(string password, string data)
        {
            return DatabaseCrypto.Transform(true, password, data, null, null) as EncryptedData;
        }

        public static string Decrypt(string password, EncryptedData data)
        {
            return DatabaseCrypto.Transform(false, password, data.DataString, data.SaltString, data.MACString) as string;
        }

        private static object Transform(bool encrypt, string password, string data, string saltString, string macString)
        {
            using (AesManaged aes = new AesManaged())
            {
#if !SILVERLIGHT
                // For Silverlight, the cipher mode is always CBC,
                // and the padding mode is always PKCS7 - we only
                // need to specify this for desktop .NET
                aes.Mode = CipherMode.CBC;
                aes.Padding = PaddingMode.PKCS7;
#endif
                int keyLengthBytes = aes.KeySize / 8;
                int ivLengthBytes = aes.BlockSize / 8;
                const int saltLengthBytes = 8;
                const int macLengthBytes = 16;
                const int keyTransformationRounds = 8192;

                byte[] salt = encrypt ? new byte[saltLengthBytes] : Convert.FromBase64String(saltString);
                if (encrypt)
                {
                    new RNGCryptoServiceProvider().GetBytes(salt);
                }

                byte[] bcKey = new Rfc2898DeriveBytes("BLK" + password, salt, keyTransformationRounds).GetBytes(keyLengthBytes);
                byte[] iv = new Rfc2898DeriveBytes("IV" + password, salt, keyTransformationRounds).GetBytes(ivLengthBytes);
                byte[] macKey = new Rfc2898DeriveBytes("MAC" + password, salt, keyTransformationRounds).GetBytes(macLengthBytes);

                if (!encrypt && Convert.ToBase64String(macKey) != macString)
                {
                    throw new IOException("Unmatched MACs");
                }

                aes.Key = bcKey;
                aes.IV = iv;

                byte[] rawData = encrypt ? Encoding.UTF8.GetBytes(data) : Convert.FromBase64String(data);

                using (ICryptoTransform transform = encrypt ? aes.CreateEncryptor() : aes.CreateDecryptor())
                using (MemoryStream cryptoMemoryStream = encrypt ? new MemoryStream() : new MemoryStream(rawData))
                using (CryptoStream cryptoStream = new CryptoStream(cryptoMemoryStream, transform, encrypt ? CryptoStreamMode.Write : CryptoStreamMode.Read))
                {
                    if (encrypt)
                    {
                        byte[] compressedRawData = Compress(rawData);

                        cryptoStream.Write(compressedRawData, 0, compressedRawData.Length);
                        cryptoStream.FlushFinalBlock();

                        return new EncryptedData(salt, macKey, cryptoMemoryStream.ToArray());
                    }
                    else
                    {
                        byte[] originalData = new byte[rawData.Length];
                        int count = cryptoStream.Read(originalData, 0, originalData.Length);
                        byte[] originalDataFixed = new byte[count];
                        Buffer.BlockCopy(originalData, 0, originalDataFixed, 0, count);

                        byte[] decompressedOriginalData = Decompress(originalDataFixed);

                        return Encoding.UTF8.GetString(decompressedOriginalData, 0, decompressedOriginalData.Length);
                    }
                }
            }
        }

        /// <summary>
        /// Compresses an array of bytes using the GZIP algorithm.
        /// </summary>
        /// <param name="data">The array of bytes to compress.</param>
        /// <returns>The compressed byte array.</returns>
        /// <exception cref="ArgumentNullException"><paramref name="data"/> is <c>null</c>.</exception>
        public static byte[] Compress(byte[] data)
        {
            if (data == null)
            {
                throw new ArgumentNullException("data");
            }

            // Create a memory stream to write our compressed data to
            using (MemoryStream memoryStream = new MemoryStream())
            {
                // Create a GZIP stream and write our byte array to compress it
                using (DeflateStream gzip = new DeflateStream(memoryStream, CompressionMode.Compress, true))
                {
                    gzip.Write(data, 0, data.Length);
                }

                // Read compressed bytes back
                byte[] compressed = memoryStream.ToArray();

                // Create a new array the same as the length of the compressed data plus 4 bytes for the size
                byte[] buffer = new byte[compressed.Length + sizeof(int)];

                // Copy the compressed bytes we got earlier into our new array at offset 4
                Buffer.BlockCopy(compressed, 0, buffer, sizeof(int), compressed.Length);

                // Get the length of the compressed data array, making sure to adjust for endianness
                byte[] length = BitConverter.GetBytes(data.Length);
                if (!BitConverter.IsLittleEndian)
                {
                    Array.Reverse(length);
                }

                // Copy the length of the compressed data array into the first 4 bytes our our new array
                Buffer.BlockCopy(length, 0, buffer, 0, sizeof(int));

                return buffer;
            }
        }

        /// <summary>
        /// Decompresses an array of bytes using the GZIP algorithm.
        /// </summary>
        /// <param name="data">The array of bytes to decompress.</param>
        /// <returns>The decompressed byte array.</returns>
        /// <exception cref="ArgumentNullException"><paramref name="data"/> is <c>null</c>.</exception>
        /// <exception cref="IndexOutOfRangeException">The length of <paramref name="data"/> was less than 4 bytes.</exception>
        public static byte[] Decompress(byte[] data)
        {
            if (data == null)
            {
                throw new ArgumentNullException("data");
            }

            if (data.Length < 4)
            {
                throw new IndexOutOfRangeException("The length of data must be at least 4 bytes.");
            }

            using (MemoryStream memoryStream = new MemoryStream())
            {
                // Get decompressed length
                byte[] length = new byte[sizeof(int)];
                Buffer.BlockCopy(data, 0, length, 0, sizeof(int));
                if (!BitConverter.IsLittleEndian)
                {
                    Array.Reverse(length);
                }

                int decompressedLength = BitConverter.ToInt32(length, 0);

                // Write the compressed data to our memory stream
                memoryStream.Write(data, sizeof(int), data.Length - sizeof(int));
                memoryStream.Position = 0;

                // Create buffer to hold decompressed bytes and read them out
                byte[] buffer = new byte[decompressedLength];
                using (DeflateStream gzip = new DeflateStream(memoryStream, CompressionMode.Decompress, true))
                {
                    gzip.Read(buffer, 0, buffer.Length);
                }

                return buffer;
            }
        }
    }
}
