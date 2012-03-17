package com.petroules.silverlock;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

public class SearchParameters
{
	private String searchPattern;

	public SearchParameters()
	{
		this.setDefaults();
	}

	public String getSearchPattern()
	{
		if (this.searchPattern == null)
		{
			return "";
		}

		return this.searchPattern;
	}

	public void setSearchPattern(String value)
	{
		this.searchPattern = value;
	}

	public boolean SearchTitle;
	public boolean SearchUsername;
	public boolean SearchPassword;
	public boolean SearchURL;
	public boolean SearchNotes;
	public boolean SearchRecovery;
	public boolean SearchCustomFields;
	public boolean SearchGroupTitle;
	public boolean SearchUUID;
	public boolean UseRegex;
	public boolean CaseSensitive;

	public void setDefaults()
	{
		this.SearchTitle = true;
		this.SearchUsername = true;
		this.SearchPassword = false;
		this.SearchURL = true;
		this.SearchNotes = true;
		this.SearchRecovery = false;
		this.SearchCustomFields = true;
		this.SearchGroupTitle = false;
		this.SearchUUID = false;
		this.UseRegex = false;
		this.CaseSensitive = false;
		this.setSearchPattern("");
	}

	public boolean areFieldsSelected()
	{
		return this.SearchTitle || this.SearchUsername || this.SearchPassword || this.SearchURL || this.SearchNotes || this.SearchRecovery || this.SearchCustomFields || this.SearchGroupTitle
				|| this.SearchUUID;
	}

	public List<String> getDataList(Entry entry)
	{
		ArrayList<String> dataItems = new ArrayList<String>();

		if (this.SearchTitle)
		{
			dataItems.add(entry.getTitle());
		}

		if (this.SearchUsername)
		{
			dataItems.add(entry.getUsername());
		}

		if (this.SearchPassword)
		{
			dataItems.add(entry.getPassword());
		}

		if (this.SearchURL)
		{
			dataItems.add(entry.getURL().toString());
		}

		if (this.SearchNotes)
		{
			dataItems.add(entry.getNotes());
		}

		if (this.SearchRecovery)
		{
			for (Map.Entry<String, String> i : entry.getRecoveryInfo().entrySet())
			{
				dataItems.add(i.getKey());
				dataItems.add(i.getValue());
			}
		}

		if (this.SearchCustomFields)
		{
			for (Map.Entry<String, String> i : entry.getCustomFields().entrySet())
			{
				dataItems.add(i.getKey());
				dataItems.add(i.getValue());
			}
		}

		// If the entry doesn't have a parent node we won't check its group
		// title
		if (this.SearchGroupTitle && entry.getParentNode() != null)
		{
			dataItems.add(entry.getParentNode().getTitle());
		}

		if (this.SearchUUID)
		{
			dataItems.add(entry.getUUID().toString());
		}

		return dataItems;
	}
}
