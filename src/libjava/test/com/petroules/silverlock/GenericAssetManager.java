package com.petroules.silverlock;

import java.io.InputStream;

public interface GenericAssetManager
{
	/**
	 * Opens an {@link InputStream} to the resource at {@code path}.
	 * @param path The path of the resource to open an input stream for.
	 * @return An {@link InputStream} to read the resource, or null if the resource is not found.
	 */
	public InputStream open(String path);
}
