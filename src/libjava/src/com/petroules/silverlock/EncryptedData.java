package com.petroules.silverlock;

import java.io.UnsupportedEncodingException;
import org.spongycastle.util.encoders.Base64;

public class EncryptedData
{
	/**
	 * The salt.
	 */
	private byte[] salt;

	/**
	 * The message authentication code.
	 */
	private byte[] mac;

	/**
	 * The encrypted data.
	 */
	private byte[] data;

	/**
	 * Initializes a new instance of the {@code EncryptedData} class.
	 */
	public EncryptedData()
	{
	}

	/**
	 * Initializes a new instance of the {@code EncryptedData} class with the
	 * specified salt, MAC and data.
	 * 
	 * @param salt
	 *            The salt.
	 * @param mac
	 *            The message authentication code.
	 * @param data
	 *            The encrypted data.
	 */
	public EncryptedData(byte[] salt, byte[] mac, byte[] data)
	{
		this.salt = salt;
		this.mac = mac;
		this.data = data;
	}

	/**
	 * Initializes a new instance of the {@code EncryptedData} class with the
	 * specified salt, MAC and data.
	 * 
	 * @param salt
	 *            The salt as a base-64 encoded string.
	 * @param mac
	 *            The message authentication code as a base-64 encoded string.
	 * @param data
	 *            The encrypted data as a base-64 encoded string.
	 */
	public EncryptedData(String salt, String mac, String data)
	{
		this.setSaltString(salt);
		this.setMACString(mac);
		this.setDataString(data);
	}

	/**
	 * Gets the salt.
	 */
	public byte[] getSalt()
	{
		return this.salt;
	}

	/**
	 * Sets the salt to {@code salt}.
	 * 
	 * @param salt
	 *            The salt to set.
	 */
	public void setSalt(byte[] salt)
	{
		this.salt = salt;
	}

	/**
	 * Gets the salt as a base-64 encoded string.
	 */
	public String getSaltString()
	{
		try
		{
			return new String(Base64.encode(this.salt), "UTF-8");
		}
		catch (UnsupportedEncodingException e)
		{
			return null;
		}
	}

	/**
	 * Sets the salt to {@code salt}.
	 * 
	 * @param salt
	 *            The salt as a base-64 encoded string.
	 */
	public void setSaltString(String salt)
	{
		this.salt = Base64.decode(salt);
	}

	/**
	 * Gets the message authentication code.
	 */
	public byte[] getMAC()
	{
		return this.mac;
	}

	/**
	 * Sets the message authentication code to {@code mac}.
	 * 
	 * @param mac
	 *            The message authentication code to set.
	 */
	public void setMAC(byte[] mac)
	{
		this.mac = mac;
	}

	/**
	 * Gets the message authentication code as a base-64 encoded string.
	 */
	public String getMACString()
	{
		try
		{
			return new String(Base64.encode(this.mac), "UTF-8");
		}
		catch (UnsupportedEncodingException e)
		{
			return null;
		}
	}

	/**
	 * Sets the message authentication code to {@code mac}.
	 * 
	 * @param mac
	 *            The message authentication code as a base-64 encoded string.
	 */
	public void setMACString(String mac)
	{
		this.mac = Base64.decode(mac);
	}

	/**
	 * Gets the encrypted data.
	 */
	public byte[] getData()
	{
		return this.data;
	}

	/**
	 * Sets the encrypted data to {@code data}.
	 * 
	 * @param data
	 *            The encrypted data to set.
	 */
	public void setData(byte[] data)
	{
		this.data = data;
	}

	/**
	 * Gets the encrypted data as a base-64 encoded string.
	 */
	public String getDataString()
	{
		try
		{
			return new String(Base64.encode(this.data), "UTF-8");
		}
		catch (UnsupportedEncodingException e)
		{
			return null;
		}
	}

	/**
	 * Sets the encrypted data to {@code data}.
	 * 
	 * @param data
	 *            The encrypted data as a base-64 encoded string.
	 */
	public void setDataString(String data)
	{
		this.data = Base64.decode(data);
	}
}
