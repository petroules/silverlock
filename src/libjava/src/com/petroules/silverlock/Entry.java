package com.petroules.silverlock;

import java.net.URI;
import java.net.URISyntaxException;
import java.util.Collections;
import java.util.Hashtable;
import java.util.Map;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class Entry extends DatabaseNode
{
	private URI url;
	private String username;
	private String password;
	private String notes;
	private Hashtable<String, String> recoveryInfo = new Hashtable<String, String>();
	private Hashtable<String, String> customFields = new Hashtable<String, String>();

	public Entry()
	{
		this("");
	}

	public Entry(String title)
	{
		this(title, null);
	}

	public Entry(String title, Group parent)
	{
		super(title);
		this.setParentNode(parent);
	}

	@Override
	public void destroy()
	{
		this.detach();
	}

	public String getUsername()
	{
		return this.username;
	}

	public void setUsername(String value)
	{
		if (!this.username.equals(value))
		{
			this.username = value;
			this.setModified();
		}
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

	public URI getURL()
	{
		return this.url;
	}

	public void setURL(URI value)
	{
		if (!this.url.equals(value))
		{
			this.url = value;
			this.setModified();
		}
	}

	public String getNotes()
	{
		return this.notes;
	}

	public void setNotes(String value)
	{
		if (!this.notes.equals(value))
		{
			this.notes = value;
			this.setModified();
		}
	}

	public Map<String, String> getRecoveryInfo()
	{
		return Collections.unmodifiableMap(this.recoveryInfo);
	}

	public Map<String, String> getCustomFields()
	{
		return Collections.unmodifiableMap(this.customFields);
	}

	public void insertRecoveryInfo(String question, String answer)
	{
		// Blank questions are not accepted!
		if (question == null || question.isEmpty() || this.recoveryInfo.containsKey(question))
		{
			return;
		}

		this.recoveryInfo.put(question, answer);
		this.setModified();
	}

	public boolean removeRecoveryInfo(String question)
	{
		boolean removed = this.recoveryInfo.remove(question) != null;

		// If an item was actually removed...
		if (removed)
		{
			this.setModified();
		}

		return removed;
	}

	public void clearRecoveryInfo()
	{
		if (this.recoveryInfo.size() > 0)
		{
			this.recoveryInfo.clear();
			this.setModified();
		}
	}

	public void insertCustomField(String key, String value)
	{
		// Blank keys are not accepted!
		if (key == null || key.isEmpty() || this.customFields.containsKey(key))
		{
			return;
		}

		this.customFields.put(key, value);
		this.setModified();
	}

	public boolean removeCustomField(String key)
	{
		boolean removed = this.customFields.remove(key) != null;

		// If an item was actually removed...
		if (removed)
		{
			this.setModified();
		}

		return removed;
	}

	public void clearCustomFields()
	{
		if (this.customFields.size() > 0)
		{
			this.customFields.clear();
			this.setModified();
		}
	}

	@Override
	public Element toXml(Document document)
	{
		Element element = this.toXml(document, DatabaseKeys.XML_ENTRY);
		element.setAttribute(DatabaseKeys.XML_USERNAME, this.getUsername());
		element.setAttribute(DatabaseKeys.XML_PASSWORD, this.getPassword());
		element.setAttribute(DatabaseKeys.XML_URL, this.getURL().toString());
		element.setAttribute(DatabaseKeys.XML_NOTES, this.getNotes());

		// Add all the recovery info
		for (Map.Entry<String, String> i : this.getRecoveryInfo().entrySet())
		{
			Element pair = document.createElement(DatabaseKeys.XML_RECOVERYINFO);
			pair.setAttribute(DatabaseKeys.XML_QUESTION, i.getKey());
			pair.setAttribute(DatabaseKeys.XML_ANSWER, i.getValue());
			element.appendChild(pair);
		}

		// Add all the custom fields
		for (Map.Entry<String, String> i : this.getCustomFields().entrySet())
		{
			Element pair = document.createElement(DatabaseKeys.XML_CUSTOMFIELD);
			pair.setAttribute(DatabaseKeys.XML_CFNAME, i.getKey());
			pair.setAttribute(DatabaseKeys.XML_CFVALUE, i.getValue());
			element.appendChild(pair);
		}

		return element;
	}

	public Entry createCopy()
	{
		Entry entry = new Entry();
		entry.setTitle(this.getTitle());
		entry.setUsername(this.getUsername());
		entry.setPassword(this.getPassword());
		entry.setURL(this.getURL());
		entry.setNotes(this.getNotes());

		for (Map.Entry<String, String> i : this.getRecoveryInfo().entrySet())
		{
			entry.insertRecoveryInfo(i.getKey(), i.getValue());
		}

		for (Map.Entry<String, String> i : this.getCustomFields().entrySet())
		{
			entry.insertCustomField(i.getKey(), i.getValue());
		}

		return entry;
	}

	@Override
	protected void attachToList()
	{
		this.getParentNode().entries.add(this);
	}

	@Override
	protected void detachFromList()
	{
		this.getParentNode().entries.remove(this);
	}

	@Override
	protected/* internal */void fromXml(Element element)
	{
		super.fromXml(element);

		this.setUsername(element.getAttribute(DatabaseKeys.XML_USERNAME));
		this.setPassword(element.getAttribute(DatabaseKeys.XML_PASSWORD));
		try
		{
			this.setURL(new URI(element.getAttribute(DatabaseKeys.XML_URL)));
		}
		catch (URISyntaxException e)
		{
		}

		this.setNotes(element.getAttribute(DatabaseKeys.XML_NOTES));

		// This function should NEVER be called from a parented DatabaseNode
		// It is private and designed to be called in DatabaseReader only
		// immediately after an Entry is instantiated, therefore they should
		// always be empty at this point
		if (this.customFields.size() > 0)
		{
			throw new IllegalStateException("Cannot read an Entry from an XML element if it has custom fields.");
		}

		if (this.recoveryInfo.size() > 0)
		{
			throw new IllegalStateException("Cannot read an Entry from an XML element if it has recovery info.");
		}

		// Here we'll process any subnodes of the entry containing additional
		// information
		Node entryNode = element.getFirstChild();
		while (entryNode != null)
		{
			Element entryElement = entryNode instanceof Element ? (Element) entryNode : null;
			if (entryElement != null)
			{
				if (entryElement.getTagName().equals(DatabaseKeys.XML_CUSTOMFIELD))
				{
					this.customFields.put(entryElement.getAttribute(DatabaseKeys.XML_CFNAME), entryElement.getAttribute(DatabaseKeys.XML_CFVALUE));
				}
				else if (entryElement.getTagName().equals(DatabaseKeys.XML_RECOVERYINFO))
				{
					this.recoveryInfo.put(entryElement.getAttribute(DatabaseKeys.XML_QUESTION), entryElement.getAttribute(DatabaseKeys.XML_ANSWER));
				}
			}

			entryNode = entryNode.getNextSibling();
		}
	}
}
