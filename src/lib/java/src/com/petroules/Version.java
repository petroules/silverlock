package com.petroules;

public class Version implements Comparable<Version>
{
	private String version;

	public Version(String version)
	{
		this.version = version;
	}

	@Override
	public int compareTo(Version other)
	{
		return this.version.compareTo(other.version);
	}

	@Override
	public boolean equals(Object obj)
	{
		if (this == obj)
		{
			return true;
		}

		if (obj == null)
		{
			return false;
		}

		if (this.getClass() != obj.getClass())
		{
			return false;
		}

		final Version other = (Version) obj;
		if (!this.version.equals(other.version))
		{
			return false;
		}

		return true;
	}

	@Override
	public String toString()
	{
		return this.version;
	}
}
