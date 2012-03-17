package com.petroules.silverlock;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.StringReader;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.security.SecureRandom;

/*import org.bouncycastle.crypto.CryptoException;
import org.bouncycastle.crypto.PBEParametersGenerator;
import org.bouncycastle.crypto.digests.SHA1Digest;
import org.bouncycastle.crypto.engines.AESEngine;
import org.bouncycastle.crypto.generators.PKCS5S2ParametersGenerator;
import org.bouncycastle.crypto.macs.HMac;
import org.bouncycastle.crypto.modes.CBCBlockCipher;
import org.bouncycastle.crypto.paddings.PKCS7Padding;
import org.bouncycastle.crypto.paddings.PaddedBufferedBlockCipher;
import org.bouncycastle.crypto.params.KeyParameter;
import org.bouncycastle.crypto.params.ParametersWithIV;
import org.bouncycastle.util.encoders.Base64;*/

import org.spongycastle.crypto.CryptoException;
import org.spongycastle.crypto.PBEParametersGenerator;
import org.spongycastle.crypto.digests.SHA1Digest;
import org.spongycastle.crypto.engines.AESEngine;
import org.spongycastle.crypto.generators.PKCS5S2ParametersGenerator;
import org.spongycastle.crypto.macs.HMac;
import org.spongycastle.crypto.modes.CBCBlockCipher;
import org.spongycastle.crypto.paddings.PKCS7Padding;
import org.spongycastle.crypto.paddings.PaddedBufferedBlockCipher;
import org.spongycastle.crypto.params.KeyParameter;
import org.spongycastle.crypto.params.ParametersWithIV;
import org.spongycastle.util.encoders.Base64;

import com.jcraft.jzlib.Deflater;
import com.jcraft.jzlib.DeflaterOutputStream;
import com.jcraft.jzlib.InflaterInputStream;
import com.jcraft.jzlib.JZlib;
import com.petroules.BufferedReader2;
import com.petroules.Version;

public class DatabaseCrypto
{
	static byte[] u2s(int[] unsignedArray) throws IOException
	{
		byte[] signedArray = new byte[unsignedArray.length];
		for (int i = 0; i < signedArray.length; i++)
		{
			if (unsignedArray[i] < 0 || unsignedArray[i] > 255)
			{
				throw new IOException("unsignedArray at " + i + " was not within the range 0 to 255.");
			}

			int subtraction = unsignedArray[i] > 128 ? 256 : 0;
			signedArray[i] = (byte) (unsignedArray[i] - subtraction);
		}

		return signedArray;
	}

	static int[] s2u(byte[] signedArray)
	{
		int[] unsignedArray = new int[signedArray.length];
		for (int i = 0; i < unsignedArray.length; i++)
		{
			int addition = signedArray[i] < 0 ? 256 : 0;
			unsignedArray[i] = (int) (signedArray[i] + addition);
		}

		return unsignedArray;
	}

	public static final String HEADER = "SILVERLOCK DATABASE FILE";
	public static final String COMPRESSED = "ZLIB";
	public static final String UNCOMPRESSED = "UNCOMPRESSED";

	public static class CryptoStatusWrapper
	{
		public CryptoStatus cryptoStatus;
	}

	public enum CryptoStatus
	{
		NoError, MissingHeader, VerificationFailed, DecodingError, UnsupportedVersion, UnknownError
	}

	public static String statusMessage(CryptoStatus status)
	{
		switch (status)
		{
			case MissingHeader:
				return "The file was missing its standard header.";
			case VerificationFailed:
				return "The message authentication codes were mismatched. The file may have been corrupted or tampered with.";
			case DecodingError:
				return "There was a problem decoding the file; either the password was invalid or the file may be corrupt.";
			case UnsupportedVersion:
				return "Unsupported database version.";
			case UnknownError:
				return "An unknown error occurred while decoding the file.";
			default:
				return "";
		}
	}

	public static String encrypt(String data, String password, int compressionLevel, CryptoStatusWrapper error)
	{
		try
		{
			EncryptedData encData = encrypt(password, data, compressionLevel);
			StringBuilder builder = new StringBuilder();
			builder.append(String.format("%1$s %2$s %3$s\n", HEADER, Database.version(), compressionLevel != 0 ? COMPRESSED : UNCOMPRESSED));
			builder.append(encData.getSaltString() + "\n");
			builder.append(encData.getMACString() + "\n");
			builder.append(encData.getDataString() + "\n");
			error.cryptoStatus = CryptoStatus.NoError;
			return builder.toString();
		}
		catch (Exception e)
		{
			error.cryptoStatus = CryptoStatus.UnknownError;
			return "";
		}
	}

	public static String decrypt(String data, String password, CryptoStatusWrapper error)
	{
		try
		{
			BufferedReader2 reader = new BufferedReader2(new StringReader(data));

			// Read the actual header line and store the expected prefix
			String actualHeaderLine = reader.readLine();
			String headerLine = String.format("%1$s ", HEADER);

			// If the actual header is less than (or equal!) to our expected
			// one, there is no version
			// and is thus invalid, OR if the actual header doesn't start with
			// our header, it's invalid
			if (actualHeaderLine.length() <= headerLine.length() || !actualHeaderLine.startsWith(headerLine))
			{
				error.cryptoStatus = CryptoStatus.MissingHeader;
				return "";
			}

			// Everything after the "SILVERLOCK DATABASE FILE"
			String[] theRest = actualHeaderLine.substring(actualHeaderLine.length() - (actualHeaderLine.length() - headerLine.length())).split(" ");
			if (theRest.length == 2)
			{
				Version version = new Version(theRest[0]);
				if (!version.equals(Database.version()))
				{
					error.cryptoStatus = CryptoStatus.UnsupportedVersion;
					return "";
				}
			}
			else
			{
				error.cryptoStatus = CryptoStatus.MissingHeader;
				return "";
			}

			error.cryptoStatus = CryptoStatus.NoError;
			return decrypt(password, new EncryptedData(reader.readLine(), reader.readLine(), reader.readToEnd()));
		}
		catch (MACException e)
		{
			error.cryptoStatus = CryptoStatus.VerificationFailed;
			return "";
		}
		catch (CryptoException e)
		{
			error.cryptoStatus = CryptoStatus.DecodingError;
			return "";
		}
		catch (Exception e)
		{
			error.cryptoStatus = CryptoStatus.UnknownError;
			return "";
		}
	}

	public static byte[] compress(byte[] data) throws IOException
	{
		return DatabaseCrypto.compress(data, JZlib.Z_DEFAULT_COMPRESSION);
	}

	public static byte[] compress(byte[] data, int compressionLevel) throws IOException
	{
		if (data == null)
		{
			throw new NullPointerException("data is null.");
		}

		// Create a stream to write our compressed data to
		ByteArrayOutputStream out = new ByteArrayOutputStream();

		// Create a zlib stream and write our byte array to compress it
		DeflaterOutputStream zOut = new DeflaterOutputStream(out, new Deflater(compressionLevel));
		zOut.write(data);
		//zOut.flush(); // wasn't here before but maybe it should be
		zOut.close();

		// Read compressed bytes back
		byte[] compressed = out.toByteArray();

		// Create a new array the same as the length of the compressed data plus
		// 4 bytes for the size
		byte[] buffer = new byte[compressed.length + 4];

		// Copy the compressed bytes we got earlier into our new array at offset
		// 4
		System.arraycopy(compressed, 0, buffer, 4, compressed.length);

		// Get the length of the compressed data array
		byte[] length = ByteBuffer.allocate(4).order(ByteOrder.BIG_ENDIAN).putInt(data.length).array();

		// Copy the length of the compressed data array into the first 4 bytes
		// our our new array
		System.arraycopy(length, 0, buffer, 0, 4);

		return buffer;
	}

	public static byte[] decompress(byte[] data) throws IOException
	{
		if (data == null)
		{
			throw new NullPointerException("data is null.");
		}

		if (data.length < 4)
		{
			throw new IllegalArgumentException("The length of data must be at least 4 bytes.");
		}

		// Get decompressed length
		byte[] length = new byte[4];
		System.arraycopy(data, 0, length, 0, 4);

		int decompressedLength = ByteBuffer.wrap(length).order(ByteOrder.BIG_ENDIAN).getInt();

		// Write the compressed data to our memory stream
		byte[] compressedData = new byte[data.length - 4];
		System.arraycopy(data, 4, compressedData, 0, data.length - 4);
		ByteArrayInputStream in = new ByteArrayInputStream(compressedData);

		// Read the uncompressed data back
		InflaterInputStream zIn = new InflaterInputStream(in);
		byte[] out = new byte[decompressedLength];
		int actual = zIn.read(out);
		zIn.close();

		if (actual != decompressedLength)
		{
			throw new IOException(String.format("Actual number of bytes read should match the expected decompressed length. Expected: %1$d, Actual: %2$d", decompressedLength, actual));
		}

		// Create a new array of the correct size
		byte[] actualOut = new byte[decompressedLength];
		System.arraycopy(out, 0, actualOut, 0, actual);

		return actualOut;
	}

	public static EncryptedData encrypt(String password, String data) throws IOException, CryptoException
	{
		return DatabaseCrypto.encrypt(password, data, -1);
	}

	public static EncryptedData encrypt(String password, String data, int compressionLevel) throws IOException, CryptoException
	{
		return (EncryptedData) DatabaseCrypto.transform(true, password, data, null, null, compressionLevel);
	}

	public static String decrypt(String password, EncryptedData data) throws IOException, CryptoException
	{
		return (String) DatabaseCrypto.transform(false, password, data.getDataString(), data.getSaltString(), data.getMACString());
	}

	private static Object transform(boolean encrypt, String password, String data, String saltString, String verificationMacString) throws IOException, CryptoException
	{
		return DatabaseCrypto.transform(encrypt, password, data, saltString, verificationMacString, -1);
	}

	private static Object transform(boolean encrypt, String password, String data, String saltString, String verificationMacString, int compressionLevel) throws IOException, CryptoException
	{
		/*try
		{*/
			final PaddedBufferedBlockCipher cipher = new PaddedBufferedBlockCipher(new CBCBlockCipher(new AESEngine()), new PKCS7Padding());

			final int keyLengthBits = 256; // cipher.getKeySize() / 8;
			final int ivLengthBits = cipher.getBlockSize() * 8;
			final int saltLengthBytes = 8;
			final int macLengthBits = 16 * 8;
			final int keyTransformationRounds = 8192;

			final byte[] salt = encrypt ? new byte[saltLengthBytes] : Base64.decode(saltString);
			if (encrypt)
			{
				SecureRandom random = new SecureRandom();
				random.nextBytes(salt);
			}

			byte[] rawData = encrypt ? data.getBytes("UTF-8") : Base64.decode(data);

			final PBEParametersGenerator generator = new PKCS5S2ParametersGenerator();
			generator.init(PBEParametersGenerator.PKCS5PasswordToBytes(("BLK" + password).toCharArray()), salt, keyTransformationRounds);
			byte[] bcKey = ((KeyParameter) generator.generateDerivedParameters(keyLengthBits)).getKey();
			generator.init(PBEParametersGenerator.PKCS5PasswordToBytes(("IVL" + password).toCharArray()), salt, keyTransformationRounds);
			byte[] iv = ((KeyParameter) generator.generateDerivedParameters(ivLengthBits)).getKey();
			generator.init(PBEParametersGenerator.PKCS5PasswordToBytes(("MAC" + password).toCharArray()), salt, keyTransformationRounds);
			byte[] macKey = ((KeyParameter) generator.generateDerivedParameters(macLengthBits)).getKey();

			cipher.init(encrypt, new ParametersWithIV(new KeyParameter(bcKey, 0, keyLengthBits / 8), iv, 0, ivLengthBits / 8));

			if (encrypt)
			{
				rawData = compress(rawData, compressionLevel);
			}

			// Create a byte array to hold the output data - the initial size is
			// the
			// smallest multiple of the block size greater than or equal to the
			// input length
			final byte[] processed = new byte[cipher.getOutputSize(rawData.length)];

			// Output length holds the *actual* length of the output data
			int outputLength = cipher.processBytes(rawData, 0, rawData.length, processed, 0);
			/*try
			{*/
				outputLength += cipher.doFinal(processed, outputLength);

				// Copy the output array to an array of the correct size
				// (otherwise we'll potentially have trailing null bytes)
				byte[] processedFinal = new byte[outputLength];
				System.arraycopy(processed, 0, processedFinal, 0, outputLength);

				if (encrypt)
				{
					return new EncryptedData(salt, mac(macKey, rawData), processedFinal);
				}
				else
				{
					String macString = new String(Base64.encode(mac(macKey, processedFinal)), "UTF-8");
					if (!macString.equals(verificationMacString))
					{
						throw new MACException("Unmatched MACs");
					}

					return new String(decompress(processedFinal), "UTF-8");
				}
			/*}
			catch (CryptoException ce)
			{
				System.err.println(ce);
				return null;
			}*/
		/*}
		catch (UnsupportedEncodingException e)
		{
			return null;
		}*/
	}

	private static byte[] mac(byte[] macKey, byte[] data)
	{
		HMac hmac = new HMac(new SHA1Digest());
		hmac.init(new KeyParameter(macKey));
		hmac.update(data, 0, data.length);
		byte[] mac = new byte[hmac.getMacSize()];
		hmac.doFinal(mac, 0);
		return mac;
	}
}
