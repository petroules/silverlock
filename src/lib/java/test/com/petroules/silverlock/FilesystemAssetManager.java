package com.petroules.silverlock;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.InputStream;

public class FilesystemAssetManager implements GenericAssetManager
{
	private File resourceDirectory;
	
	public FilesystemAssetManager(String path)
	{
		this.resourceDirectory = new File(path);
	}
	
	@Override
	public InputStream open(String path)
	{
		try
		{
			return new FileInputStream(new File(this.resourceDirectory, path));
		}
		catch (FileNotFoundException e)
		{
			e.printStackTrace();
			return null;
		}
	}
}
