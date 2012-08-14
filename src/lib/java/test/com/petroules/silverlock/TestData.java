package com.petroules.silverlock;

import java.io.IOException;
import java.io.InputStream;
import java.util.Vector;

import com.petroules.BufferedReader2;
import com.petroules.UnicodeReader;

public class TestData
{
	private static GenericAssetManager assetManager;
	
	public static GenericAssetManager getAssetManager()
	{
		return TestData.assetManager;
	}
	
	public static void setAssetManager(GenericAssetManager manager)
	{
		TestData.assetManager = manager;
	}

	/**
	 * Gets the contents of a lorem ipsum file.
	 */
	public static String loremIpsum()
	{
		return getFileAsString("LoremIpsum.txt");
	}
	
	/**
	 * Gets an array of structures containing information about all test databases, including:
	 * * The full path to the test file.
	 * * The file contents as a string.
	 * * The actual password to the file.
	 * * The internal (expected) XML contents as a string.
	 */
	public static Vector<TestDatabaseInfo> testDatabases()
	{
		Vector<TestDatabaseInfo> databases = new Vector<TestDatabaseInfo>();
		databases.add(testDatabasePassword());
		databases.add(testDatabaseNoPassword());
		return databases;
	}
	
	/**
	 * Gets information about a test database encrypted with a password.
	 */
	public static TestDatabaseInfo testDatabasePassword()
	{
		TestDatabaseInfo tuple = new TestDatabaseInfo();
		tuple.fullPath = "TestDatabasePassword.sdbx";
		tuple.databaseContents = getFileAsString(tuple.fullPath);
		tuple.password = "test";
		tuple.xmlDatabaseContents = getFileAsString(tuple.fullPath + ".xml");
		return tuple;
	}
	
	/**
	 * Gets information about a test database without a password.
	 */
	public static TestDatabaseInfo testDatabaseNoPassword()
	{
		TestDatabaseInfo tuple = new TestDatabaseInfo();
		tuple.fullPath = "TestDatabaseNoPassword.sdbx";
		tuple.databaseContents = getFileAsString(tuple.fullPath);
		tuple.password = "";
		tuple.xmlDatabaseContents = getFileAsString(tuple.fullPath + ".xml");
		return tuple;
	}

	private static String getFileAsString(String uri) throws RuntimeException
	{
		if (assetManager == null)
		{
			throw new NullPointerException("assetManager has not been set!");
		}
		
		InputStream is = null;
		try
		{
			is = assetManager.open(uri);
			BufferedReader2 reader = new BufferedReader2(new UnicodeReader(is, "UTF-8"));
			return reader.readToEnd();
		}
		catch (IOException e)
		{
			throw new RuntimeException(e);
		}
		finally
		{
			if (is != null)
			{
				try
				{
					is.close();
				}
				catch (IOException e)
				{
					e.printStackTrace();
				}
			}
		}
	}
}
