package com.petroules.silverlock;

public class Native
{
	public native byte[] generateKeys(String password);
	
    static
    {
    	System.loadLibrary("native");
    }
}
