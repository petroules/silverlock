package com.petroules.silverlock;

import java.io.IOException;
import java.io.InputStream;

import android.content.res.AssetManager;

public class AndroidAssetManager implements GenericAssetManager
{
	private AssetManager manager;
	
	public AndroidAssetManager(AssetManager manager)
	{
		this.manager = manager;
	}
	
	public InputStream open(String path)
	{
		try
		{
			return manager.open(path);
		}
		catch (IOException e)
		{
			e.printStackTrace();
			return null;
		}
	}
}
