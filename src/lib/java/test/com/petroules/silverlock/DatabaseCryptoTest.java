package com.petroules.silverlock;

import java.io.BufferedWriter;
import java.io.IOException;
import java.io.StringReader;
import java.io.StringWriter;
import java.util.Vector;

import javax.xml.parsers.ParserConfigurationException;

import org.junit.Assert;
import org.junit.BeforeClass;
import org.junit.Test;
import org.spongycastle.crypto.CryptoException;
import org.spongycastle.crypto.PBEParametersGenerator;
import org.spongycastle.crypto.generators.PKCS5S2ParametersGenerator;
import org.spongycastle.crypto.params.KeyParameter;

import com.petroules.BufferedReader2;
import com.petroules.silverlock.DatabaseCrypto.CryptoStatus;
import com.petroules.silverlock.DatabaseCrypto.CryptoStatusWrapper;

public class DatabaseCryptoTest
{
	@BeforeClass
	public static void init()
	{
		TestData.setAssetManager(new FilesystemAssetManager("test-data"));
	}
	
	@Test
	public void testTwoWayFileEncryption() throws IOException, CryptoException
	{
		System.out.println(getTestAnnouncement("testTwoWayFileEncryption"));

		final String password = "MyS3cr3tP4$$w0rd";
		byte[] encryptedBytes = null;
		String encryptedString = null;

		StringWriter w = new StringWriter();
		BufferedWriter writer = new BufferedWriter(w);

		writeEncryptedData(writer, TestData.loremIpsum(), password);

		encryptedBytes = w.toString().getBytes("UTF-8");
		encryptedString = new String(encryptedBytes, "UTF-8");
		
		System.out.println(String.format("Encrypted file (%1$d bytes, %2$d characters): ", encryptedBytes.length, encryptedString.length()));
		System.out.println(encryptedString);

		byte[] encryptedBytesFromString = encryptedString.getBytes("UTF-8");
		Assert.assertArrayEquals("Byte array to string and back caused loss of data", encryptedBytes, encryptedBytesFromString);

		BufferedReader2 reader = new BufferedReader2(new StringReader(new String(encryptedBytesFromString, "UTF-8")));

		System.out.println("Decrypting...");
		String decryptedString = readEncryptedData(reader, password);

		System.out.println("Decrypted file:");
		System.out.println(decryptedString);

		Assert.assertEquals("Data did not decrypt back to original", TestData.loremIpsum(), decryptedString);
	}

	@Test
	public void testTwoWayEncryption() throws IOException, CryptoException
	{
		System.out.println(getTestAnnouncement("testTwoWayEncryption"));
		
		final String password = "MyS3cr3tP4$$w0rd";

		EncryptedData encryptedData = DatabaseCrypto.encrypt(password, TestData.loremIpsum());
		System.out.println("Encrypted data:");
		System.out.println("Salt: " + encryptedData.getSaltString());
		System.out.println("MAC: " + encryptedData.getMACString());
		System.out.println("Encrypted data: " + encryptedData.getDataString());

		String decryptedData = DatabaseCrypto.decrypt(password, encryptedData);
		System.out.println("Decrypted data: " + decryptedData);

		Assert.assertEquals("Data did not decrypt back to original", TestData.loremIpsum(), decryptedData);
	}

	@Test
	public void testTwoWayCompression() throws IOException
	{
		System.out.println(getTestAnnouncement("testTwoWayCompression"));
		
		byte[] bytes = TestData.loremIpsum().getBytes("UTF-8");

		try
		{
			byte[] compressed = DatabaseCrypto.compress(bytes);
			byte[] decompressed = DatabaseCrypto.decompress(compressed);
			Assert.assertArrayEquals("Data did not decompress back to original", bytes, decompressed);
		}
		catch (IOException e)
		{
			Assert.fail(e.getMessage());
		}
	}

	@Test
	public void testStandardFileDecryptsToExpectedXml()
	{
		System.out.println(getTestAnnouncement("testStandardFileDecryptsToExpectedXml"));
		
		Vector<TestDatabaseInfo> databases = TestData.testDatabases();
		for (TestDatabaseInfo item : databases)
		{
			DatabaseCrypto.CryptoStatusWrapper status = new CryptoStatusWrapper();
			String decryptedData = DatabaseCrypto.decrypt(item.databaseContents, item.password, status);
			System.out.println(decryptedData);
			Assert.assertEquals(CryptoStatus.NoError, status.cryptoStatus);
			Assert.assertEquals(item.xmlDatabaseContents, decryptedData);
		}
	}
	
	@Test
	public void testStandardFileDecryptsToInstance() throws IOException, ParserConfigurationException
	{
		System.out.println(getTestAnnouncement("testStandardFileDecryptsToInstance"));
		
		DatabaseReader reader = new DatabaseReader();
		
		Vector<TestDatabaseInfo> databases = TestData.testDatabases();
		for (TestDatabaseInfo item : databases)
		{
			Database db = reader.read(TestData.getAssetManager().open(item.fullPath), item.password);
			Assert.assertNotNull(reader.getErrorString(), db);
		}
	}
	
	@Test
	public void testHMAC()
	{
		System.out.println(getTestAnnouncement("testHMAC"));
		
		long start = System.currentTimeMillis();
		
		final PBEParametersGenerator generator = new PKCS5S2ParametersGenerator();
		generator.init(PBEParametersGenerator.PKCS5PasswordToBytes(("mypassword").toCharArray()), new byte[] { 1, 2, 3, 4, 5, 6, 7, 8 }, 8192);
		byte[] key = ((KeyParameter)generator.generateDerivedParameters(256)).getKey();
		
		long time = System.currentTimeMillis() - start;
		Assert.assertTrue("Generated " + key.length + "-byte key, took " + time + "ms", true);
	}

	private String readEncryptedData(BufferedReader2 stream, String password) throws IOException, CryptoException
	{
		EncryptedData enc = new EncryptedData();
		stream.readLine(); // reads the header, we just ignore this
		enc.setSaltString(stream.readLine());
		enc.setMACString(stream.readLine());
		enc.setDataString(stream.readToEnd());

		return DatabaseCrypto.decrypt(password, enc);
	}

	private void writeEncryptedData(BufferedWriter stream, String data, String password) throws IOException, CryptoException
	{
		EncryptedData enc = DatabaseCrypto.encrypt(password, data);
		stream.write("SILVERLOCK DATABASE FILE 1.0 ZLIB");
		stream.newLine();
		stream.write(enc.getSaltString());
		stream.newLine();
		stream.write(enc.getMACString());
		stream.newLine();
		stream.write(enc.getDataString());
		stream.newLine();

		// VERY important... I wonder if this should be called by the caller
		// though?
		stream.flush();
	}
	
	private String getTestAnnouncement(String message)
	{
		final int lineLength = 80;
		
		StringBuilder builder = new StringBuilder();
		builder.append(getDashes(lineLength) + "\n");
		
		// The number of dashes to print after the length of
		// the message plus padding spaces are taken into account
		int dashCount = (lineLength - (message.length() + 2));
		
		builder.append(
				getDashes(dashCount / 2) +
				" " + message.toUpperCase() + " " +
				getDashes((dashCount % 2 == 0) ? (dashCount / 2) : (dashCount / 2 + 1)) +
				"\n");
		
		builder.append(getDashes(lineLength) + "\n");
		
		return builder.toString();
	}
	
	private String getDashes(int count)
	{
		StringBuilder builder = new StringBuilder();
		for (int i = 0; i < count; i++)
		{
			builder.append("-");
		}
		
		return builder.toString();
	}
}
