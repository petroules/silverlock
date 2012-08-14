package com.petroules.silverlock;

import java.text.DateFormat;
import java.text.ParseException;
import java.util.Date;
import java.util.Observable;
import java.util.Observer;
import java.util.UUID;
import org.w3c.dom.Document;
import org.w3c.dom.Element;

public abstract class DatabaseNode extends Observable implements Observer
{
	private Group parent;
	private Date created;
	private Date accessed;
	private Date modified;
	private UUID uuid;
	private String title;

	protected DatabaseNode()
	{
		this("");
	}

	protected DatabaseNode(String title)
	{
		this.created = new Date();
		this.accessed = new Date();
		this.modified = new Date();
		this.uuid = UUID.randomUUID();
		this.title = title;
	}

	public abstract void destroy();

	public Date getCreated()
	{
		return this.created;
	}

	private void setCreated(Date value)
	{
		if (!this.created.equals(value))
		{
			this.created = value;
		}
	}

	public Date getAccessed()
	{
		return this.accessed;
	}

	private void setAccessed(Date value)
	{
		if (!this.accessed.equals(value))
		{
			this.accessed = value;
		}
	}

	public Date getModified()
	{
		return this.modified;
	}

	protected void setModified(Date value)
	{
		if (!this.modified.equals(value))
		{
			this.modified = value;
		}
	}

	public UUID getUUID()
	{
		return this.uuid;
	}

	protected void setUUID(UUID value)
	{
		if (!this.uuid.equals(value))
		{
			this.uuid = value;
			this.setModified();
		}
	}

	public String getTitle()
	{
		return this.title;
	}

	public void setTitle(String value)
	{
		if (!this.title.equals(value))
		{
			this.title = value;
			this.setModified();
		}
	}

	public Group getParentNode()
	{
		return this.parent;
	}

	public void setParentNode(Group value)
	{
		// Cant't self-parent or parent to the same node
		if (this == value || this.parent == value)
		{
			return;
		}

		this.detach();
		this.parent = value;
		this.attach();
	}

	public Group getRootNode()
	{
		if (!this.isAttached())
		{
			return null;
		}

		Group parent = this.parent;
		while (parent.getParentNode() != null)
		{
			parent = parent.getParentNode();
		}

		return parent;
	}

	public boolean hasDatabase()
	{
		return this.getRootNode() instanceof Database;
	}

	public boolean isAttached()
	{
		return this.getParentNode() != null;
	}

	public void setAccessed()
	{
		this.setAccessed(new Date());
	}

	public abstract Element toXml(Document document);

	protected Element toXml(Document document, String tagName)
	{
		Element element = document.createElement(tagName);
		element.setAttribute(DatabaseKeys.XML_CREATED, this.getCreated().toString() + "Z");
		element.setAttribute(DatabaseKeys.XML_ACCESSED, this.getAccessed().toString() + "Z");
		element.setAttribute(DatabaseKeys.XML_MODIFIED, this.getModified().toString() + "Z");
		element.setAttribute(DatabaseKeys.XML_UUID, uuidToString(this.getUUID()));
		element.setAttribute(DatabaseKeys.XML_TITLE, this.getTitle());
		return element;
	}

	protected void setModified()
	{
		this.setModified(new Date());
	}

	protected void detach()
	{
		if (this.isAttached())
		{
			this.detachFromList();
			DatabaseNode parentTemp = this.parent;
			this.parent = null;

			// Notify the tree that it has been modified and
			// disconnect our modification event from the parent's
			this.setChanged();
			this.notifyObservers();
			this.deleteObserver(parentTemp);
		}
	}

	protected/* internal */void fromXml(Element element)
	{
		if (this.parent != null)
		{
			throw new IllegalStateException("Cannot read a DatabaseNode from an XML element if it has a parent");
		}

		DateFormat dateFormat = DateFormat.getDateTimeInstance();
		try
		{
			this.setCreated(dateFormat.parse(element.getAttribute(DatabaseKeys.XML_CREATED)));
		}
		catch (ParseException e)
		{
		}

		try
		{
			this.setAccessed(dateFormat.parse(element.getAttribute(DatabaseKeys.XML_ACCESSED)));
		}
		catch (ParseException e)
		{
		}

		try
		{
			this.setModified(dateFormat.parse(element.getAttribute(DatabaseKeys.XML_MODIFIED)));
		}
		catch (ParseException e)
		{
		}

		this.setUUID(uuidFromString(element.getAttribute(DatabaseKeys.XML_UUID)));
		this.setTitle(element.getAttribute(DatabaseKeys.XML_TITLE));
	}

	protected abstract void attachToList();

	protected abstract void detachFromList();

	private void attach()
	{
		if (this.parent != null)
		{
			// Ensure UUID uniquity
			while (this.getRootNode().containsUUID(this.getUUID()))
			{
				this.setUUID(UUID.randomUUID());
			}

			// When we add an entry to the parent, the DB was modified
			this.attachToList();

			// Connect our modification event to the parent's
			// and notify the tree that it has been modified
			this.addObserver(this.parent);
			this.setChanged();
			this.notifyObservers();
		}
	}

	@Override
	public void update(Observable o, Object arg)
	{
	}
	
	private String uuidToString(UUID u)
	{
		return "{" + u + "}";
	}
	
	private UUID uuidFromString(String s)
	{
		return UUID.fromString(s.replace("{", "").replace("}", ""));
	}
}
