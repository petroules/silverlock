package com.petroules;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.Reader;

/**
 * Extends {@code BufferedReader} with a {@code readToEnd()} method.
 */
public class BufferedReader2 extends BufferedReader
{
	/**
	 * Creates a buffering character-input stream that uses a default-sized
	 * input buffer.
	 * 
	 * @param in
	 *            A Reader
	 */
	public BufferedReader2(Reader in)
	{
		super(in);
	}

	/**
	 * Creates a buffering character-input stream that uses an input buffer of
	 * the specified size.
	 * 
	 * @param in
	 *            A Reader
	 * @param sz
	 *            Input-buffer size
	 */
	public BufferedReader2(Reader in, int sz)
	{
		super(in, sz);
	}

	/**
	 * Continually calls {@code readLine()} until the end of the file, joining
	 * the contents into one string.
	 * 
	 * @return A String object containing the contents of the file from the
	 *         position when the method was called, to the end.
	 * @throws IOException
	 */
	public String readToEnd() throws IOException
	{
		StringBuilder str = new StringBuilder();
		String line;
		while ((line = this.readLine()) != null)
		{
			str.append(line + "\n");
		}

		return str.toString();
	}
}
