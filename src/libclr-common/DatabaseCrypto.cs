namespace Petroules.Silverlock
{
    using System;
    using System.Globalization;
    using System.IO;
    using System.Runtime.Serialization;
    using System.Security.Cryptography;
    using System.Text;
    using ComponentAce.Compression.Libs.zlib;

    [Serializable]
    public class MACException : Exception
    {
        public MACException()
        {
        }

        public MACException(string message)
            : base(message)
        {
        }

        public MACException(string message, Exception inner)
            : base(message, inner)
        {
        }

        protected MACException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
        }
    }

    public static class DatabaseCrypto
    {
        public const string HEADER = "SILVERLOCK DATABASE FILE";
        public const string COMPRESSED = "ZLIB";
        public const string UNCOMPRESSED = "UNCOMPRESSED";

        public enum CryptoStatus
        {
            NoError,
            MissingHeader,
            VerificationFailed,
            DecodingError,
            UnsupportedVersion,
            UnknownError
        }

        public static string StatusMessage(CryptoStatus status)
        {
            switch (status)
            {
                case CryptoStatus.MissingHeader:
                    return "The file was missing its standard header.";
                case CryptoStatus.VerificationFailed:
                    return "The message authentication codes were mismatched. The file may have been corrupted or tampered with.";
                case CryptoStatus.DecodingError:
                    return "There was a problem decoding the file; either the password was invalid or the file may be corrupt.";
                case CryptoStatus.UnsupportedVersion:
                    return "Unsupported database version.";
                case CryptoStatus.UnknownError:
                    return "An unknown error occurred while decoding the file.";
                default:
                    return string.Empty;
            }
        }

        public static string Encrypt(string data, string password, int compressionLevel, out CryptoStatus error)
        {
            try
            {
                EncryptedData encData = Encrypt(password, data, compressionLevel);
                StringBuilder builder = new StringBuilder();
                builder.AppendFormat(CultureInfo.InvariantCulture, "{0} {1} {2}\n", HEADER, Database.Version, compressionLevel != 0 ? COMPRESSED : UNCOMPRESSED);
                builder.Append(encData.SaltString + "\n");
                builder.Append(encData.MACString + "\n");
                builder.Append(encData.DataString + "\n");
                error = CryptoStatus.NoError;
                return builder.ToString();
            }
            catch
            {
                error = CryptoStatus.UnknownError;
                return string.Empty;
            }
        }

        public static string Decrypt(string data, string password, out CryptoStatus error)
        {
            try
            {
                using (StringReader reader = new StringReader(data))
                {
                    // Read the actual header line and store the expected prefix
                    string actualHeaderLine = reader.ReadLine();
                    string headerLine = string.Format(CultureInfo.InvariantCulture, "{0} ", HEADER);

                    // If the actual header is less than (or equal!) to our expected one, there is no version
                    // and is thus invalid, OR if the actual header doesn't start with our header, it's invalid
                    if (actualHeaderLine.Length <= headerLine.Length || !actualHeaderLine.StartsWith(headerLine))
                    {
                        error = CryptoStatus.MissingHeader;
                        return string.Empty;
                    }

                    // Everything after the "SILVERLOCK DATABASE FILE"
                    string[] theRest = actualHeaderLine.Substring(actualHeaderLine.Length - (actualHeaderLine.Length - headerLine.Length)).Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
                    if (theRest.Length == 2)
                    {
                        Version version = new Version(theRest[0]);
                        if (version != Database.Version)
                        {
                            error = CryptoStatus.UnsupportedVersion;
                            return string.Empty;
                        }
                    }
                    else
                    {
                        error = CryptoStatus.MissingHeader;
                        return string.Empty;
                    }

                    error = CryptoStatus.NoError;
                    return Decrypt(password, new EncryptedData(reader.ReadLine(), reader.ReadLine(), reader.ReadToEnd()));
                }
            }
            catch (MACException)
            {
                error = CryptoStatus.VerificationFailed;
                return string.Empty;
            }
            catch (CryptographicException)
            {
                error = CryptoStatus.DecodingError;
                return string.Empty;
            }
            catch
            {
                error = CryptoStatus.UnknownError;
                return string.Empty;
            }
        }

        public static EncryptedData Encrypt(string password, string data, int compressionLevel = zlibConst.Z_DEFAULT_COMPRESSION)
        {
            return DatabaseCrypto.Transform(true, password, data, null, null, compressionLevel) as EncryptedData;
        }

        public static string Decrypt(string password, EncryptedData data)
        {
            return DatabaseCrypto.Transform(false, password, data.DataString, data.SaltString, data.MACString) as string;
        }

        private static object Transform(bool encrypt, string password, string data, string saltString, string verificationMacString, int compressionLevel = zlibConst.Z_DEFAULT_COMPRESSION)
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

                byte[] rawData = encrypt ? Encoding.UTF8.GetBytes(data) : Convert.FromBase64String(data);

                byte[] bcKey = new Rfc2898DeriveBytes("BLK" + password, salt, keyTransformationRounds).GetBytes(keyLengthBytes);
                byte[] iv = new Rfc2898DeriveBytes("IVL" + password, salt, keyTransformationRounds).GetBytes(ivLengthBytes);
                byte[] macKey = new Rfc2898DeriveBytes("MAC" + password, salt, keyTransformationRounds).GetBytes(macLengthBytes);

                aes.Key = bcKey;
                aes.IV = iv;

                using (ICryptoTransform transform = encrypt ? aes.CreateEncryptor() : aes.CreateDecryptor())
                using (MemoryStream cryptoMemoryStream = encrypt ? new MemoryStream() : new MemoryStream(rawData))
                using (CryptoStream cryptoStream = new CryptoStream(cryptoMemoryStream, transform, encrypt ? CryptoStreamMode.Write : CryptoStreamMode.Read))
                {
                    if (encrypt)
                    {
                        byte[] compressedRawData = Compress(rawData, compressionLevel);

                        cryptoStream.Write(compressedRawData, 0, compressedRawData.Length);
                        cryptoStream.FlushFinalBlock();

                        return new EncryptedData(salt, new HMACSHA1(macKey, true).ComputeHash(compressedRawData), cryptoMemoryStream.ToArray());
                    }
                    else
                    {
                        byte[] originalData = new byte[rawData.Length];
                        int count = cryptoStream.Read(originalData, 0, originalData.Length);
                        byte[] originalDataFixed = new byte[count];
                        Buffer.BlockCopy(originalData, 0, originalDataFixed, 0, count);

                        byte[] decompressedOriginalData = Decompress(originalDataFixed);

                        byte[] macKeyFiltered = new HMACSHA1(macKey, true).ComputeHash(originalDataFixed);
                        string macString = Convert.ToBase64String(macKeyFiltered);
                        if (!encrypt && macString != verificationMacString)
                        {
                            throw new MACException("Unmatched MACs");
                        }

                        return Encoding.UTF8.GetString(decompressedOriginalData, 0, decompressedOriginalData.Length);
                    }
                }
            }
        }

        /// <summary>
        /// Compresses an array of bytes using the DEFLATE algorithm from zlib.
        /// </summary>
        /// <param name="data">The array of bytes to compress.</param>
        /// <param name="compression">The compression level to use when compressing the data. -1 for the default, 0 for none, and 1 to 9 for fastest/worst to slowest/best.</param>
        /// <returns>The compressed byte array.</returns>
        /// <exception cref="ArgumentNullException"><paramref name="data"/> is <c>null</c>.</exception>
        public static byte[] Compress(byte[] data, int compression = zlibConst.Z_DEFAULT_COMPRESSION)
        {
            if (data == null)
            {
                throw new ArgumentNullException("data");
            }

            // Create a memory stream to write our compressed data to
            using (MemoryStream memoryStream = new MemoryStream())
            {
                // Create a GZIP stream and write our byte array to compress it
                using (ZOutputStream gzip = new ZOutputStream(memoryStream, compression))
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
                if (BitConverter.IsLittleEndian)
                {
                    Array.Reverse(length);
                }

                // Copy the length of the compressed data array into the first 4 bytes our our new array
                Buffer.BlockCopy(length, 0, buffer, 0, sizeof(int));

                return buffer;
            }
        }

        /// <summary>
        /// Decompresses an array of bytes using the DEFLATE algorithm from zlib.
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
                throw new ArgumentOutOfRangeException("data", "The length of data must be at least 4 bytes.");
            }

            using (MemoryStream memoryStream = new MemoryStream())
            {
                // Get decompressed length
                byte[] length = new byte[sizeof(int)];
                Buffer.BlockCopy(data, 0, length, 0, sizeof(int));
                if (BitConverter.IsLittleEndian)
                {
                    Array.Reverse(length);
                }

                int decompressedLength = BitConverter.ToInt32(length, 0);

                // Write the compressed data to our memory stream
                memoryStream.Write(data, sizeof(int), data.Length - sizeof(int));
                memoryStream.Position = 0;

                // Create a new array the same as the length of the compressed data minus 4 bytes for the size
                byte[] buffer = new byte[data.Length - sizeof(int)];

                // Copy the compressed bytes we got earlier into our new array at offset 4
                Buffer.BlockCopy(data, sizeof(int), buffer, 0, data.Length - sizeof(int));

                using (ZOutputStream gzip = new ZOutputStream(memoryStream))
                {
                    gzip.Write(buffer, 0, buffer.Length);
                }

                return memoryStream.ToArray();
            }
        }
    }
}
