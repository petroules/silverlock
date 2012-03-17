package com.petroules.silverlock;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.UUID;
import java.util.regex.Pattern;
import org.w3c.dom.Document;
import org.w3c.dom.Element;

public class Group extends DatabaseNode
{
	ArrayList<Group> groups = new ArrayList<Group>();
	ArrayList<Entry> entries = new ArrayList<Entry>();

	public Group()
	{
		this("");
	}

	public Group(String title)
	{
		this(title, null);
	}

	public Group(String title, Group parent)
	{
		super(title);
		this.setParentNode(parent);
	}

	@Override
	public void destroy()
	{
		this.detach();

		for (Entry entry : this.getEntries())
		{
			entry.destroy();
		}

		for (Group subgroup : this.getGroups())
		{
			subgroup.destroy();
		}
	}

	public List<Group> getGroups()
	{
		return Collections.unmodifiableList(this.groups);
	}

	public List<Entry> getEntries()
	{
		return Collections.unmodifiableList(this.entries);
	}

	public int getCountGroups()
	{
		int total = 0;

		for (Group group : this.getGroups())
		{
			total += group.getCountGroups();
		}

		return total + this.getGroups().size();
	}

	public int getCountEntries()
	{
		int total = 0;

		for (Group group : this.getGroups())
		{
			total += group.getCountEntries();
		}

		return total + this.getEntries().size();
	}

	public boolean containsUUID(UUID uuid)
	{
		return this.isGroup(uuid) || this.isEntry(uuid);
	}

	public boolean isGroup(UUID uuid)
	{
		return this.findGroup(uuid) != null;
	}

	public boolean isEntry(UUID uuid)
	{
		return this.findEntry(uuid) != null;
	}

	public Group findGroup(UUID uuid)
	{
		return this.findGroup(uuid, false);
	}

	public Group findGroup(UUID uuid, boolean includeThis)
	{
		if (includeThis && this.getUUID().equals(uuid))
		{
			return this;
		}
		else
		{
			return this.findGroupInternal(uuid);
		}
	}

	public Entry findEntry(UUID uuid)
	{
		for (Entry entry : this.getEntries())
		{
			if (entry.getUUID().equals(uuid))
			{
				return entry;
			}
		}

		for (Group group : this.getGroups())
		{
			Entry candidate = group.findEntry(uuid);
			if (candidate != null && candidate.getUUID().equals(uuid))
			{
				return candidate;
			}
		}

		return null;
	}

	public List<Entry> findEntries(SearchParameters params)
	{
		if (!params.areFieldsSelected())
		{
			return new ArrayList<Entry>();
		}

		Pattern regex = Pattern.compile(params.getSearchPattern(), params.CaseSensitive ? 0 : Pattern.CASE_INSENSITIVE);
		ArrayList<Entry> entries = new ArrayList<Entry>();

		for (Entry entry : this.getEntries())
		{
			List<String> fields = params.getDataList(entry);
			for (String field : fields)
			{
				if (params.UseRegex)
				{
					if (!entries.contains(entry) && regex.matcher(field).matches())
					{
						entries.add(entry);

						// Break out of fields loop so we go to the next entry
						break;
					}
				}
				else
				{
					// ToLowerInvariant effectively makes it a case-insensitive
					// comparison
					if (!entries.contains(entry) && (params.CaseSensitive ? field.toLowerCase().contains(params.getSearchPattern().toLowerCase()) : field.contains(params.getSearchPattern())))
					{
						entries.add(entry);

						// Break out of fields loop so we go to the next entry
						break;
					}
				}
			}
		}

		for (Group group : this.getGroups())
		{
			entries.addAll(group.findEntries(params));
		}

		return entries;
	}

	@Override
	public Element toXml(Document document)
	{
		Element element = this.toXml(document, DatabaseKeys.XML_GROUP);
		return element;
	}

	@Override
	protected void attachToList()
	{
		this.getParentNode().groups.add(this);
	}

	@Override
	protected void detachFromList()
	{
		this.getParentNode().groups.remove(this);
	}

	private Group findGroupInternal(UUID uuid)
	{
		for (Group group : this.groups)
		{
			// If this group is a match, return it
			if (group.getUUID().equals(uuid))
			{
				return group;
			}
			else
			{
				// Otherwise search its children
				Group candidate = group.findGroup(uuid, true);
				if (candidate != null && candidate.getUUID().equals(uuid))
				{
					return candidate;
				}
			}
		}

		return null;
	}
}
