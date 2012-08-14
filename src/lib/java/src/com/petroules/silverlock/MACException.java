package com.petroules.silverlock;

import java.io.IOException;

public class MACException extends IOException
{
	private static final long serialVersionUID = -2757412983313807994L;

	public MACException()
	{
		super();
	}

	public MACException(String message)
	{
		super(message);
	}
}
