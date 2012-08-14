package com.petroules.silverlock;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import com.petroules.Version;

public class Database extends Group
{
	private String password;
	private int compression;

	public Database()
	{
		this("", "");
	}

	public Database(String title)
	{
		this(title, "");
	}

	public Database(String title, String password)
	{
		super(title);
		this.password = password;
		this.compression = -1;
	}

	@Override
	public void destroy()
	{
	}

	public static Version version()
	{
		return new Version("1.0");
	}

	public String getPassword()
	{
		return this.password;
	}

	public void setPassword(String value)
	{
		if (!this.password.equals(value))
		{
			this.password = value;
			this.setModified();
		}
	}

	public int getCompression()
	{
		return this.compression;
	}

	public void setCompression(int value)
	{
		if (this.compression != value)
		{
			// If the compression is outside the proper range...
			if (compression < -1 || compression > 9)
			{
				// ...default to default compression
				compression = -1;
			}

			this.compression = value;
			this.setModified();
		}
	}

	@Override
	public void setParentNode(Group value)
	{
		/* do nothing */
	}

	@Override
	public Element toXml(Document document)
	{
		Element element = this.toXml(document, DatabaseKeys.XML_DATABASE);
		element.setAttribute(DatabaseKeys.XML_VERSION, Database.version().toString());
		element.setAttribute(DatabaseKeys.XML_DBPASSWORD, this.password);
		element.setAttribute(DatabaseKeys.XML_COMPRESSION, String.valueOf(this.compression));
		return element;
	}

	@Override
	protected void attachToList()
	{
	}

	@Override
	protected void detachFromList()
	{
	}

	@Override
	protected/* internal */void fromXml(Element element)
	{
		super.fromXml(element);

		this.setPassword(element.getAttribute(DatabaseKeys.XML_DBPASSWORD));
		try
		{
			this.setCompression(Integer.parseInt(element.getAttribute(DatabaseKeys.XML_COMPRESSION)));
		}
		catch (NumberFormatException e)
		{
			this.setCompression(-1);
		}
	}
}
