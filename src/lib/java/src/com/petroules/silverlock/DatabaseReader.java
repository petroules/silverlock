package com.petroules.silverlock;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.StringReader;
import java.io.StringWriter;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.OutputKeys;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import com.petroules.BufferedReader2;
import com.petroules.Version;
import com.petroules.silverlock.DatabaseCrypto.CryptoStatusWrapper;

public class DatabaseReader
{
	private String errorString;

	public DatabaseReader()
	{
		this.errorString = "";
	}

	public String getErrorString()
	{
		return this.errorString;
	}
	
	public Database readFile(String filename, String password) throws IOException, ParserConfigurationException
	{
		return read(new BufferedReader2(new InputStreamReader(new FileInputStream(filename))), password);
	}
	
	public Database read(InputStream stream, String password) throws IOException, ParserConfigurationException
	{
		return read(new BufferedReader2(new InputStreamReader(stream)), password);
	}

	public Database read(BufferedReader2 stream, String password) throws IOException, ParserConfigurationException
	{
		// Clear the last error
		this.errorString = "";

		// Get the contents of the database file and create a new XML document
		String fileDataString = stream.readToEnd();

		DocumentBuilderFactory dbfac = DocumentBuilderFactory.newInstance();
		DocumentBuilder docBuilder = dbfac.newDocumentBuilder();
		Document doc = null;

		// Try and see if it's a valid XML document; if it is, we'll simply
		// continue on and read
		// it (unencrypted mode), if it's not, we'll try to decrypt it
		// (encrypted mode)
		try
		{
			doc = docBuilder.parse(new InputSource(new StringReader(fileDataString)));
			if (doc.getDocumentElement() == null)
			{
				throw new SAXException("not a valid XML document");
			}
		}
		catch (SAXException e)
		{
			DatabaseCrypto.CryptoStatusWrapper status = new CryptoStatusWrapper();
			fileDataString = DatabaseCrypto.decrypt(
					//doc.getTextContent(),
					/*getStringFromDocument(doc),*/ fileDataString,
					password, status);
			if (status.cryptoStatus != DatabaseCrypto.CryptoStatus.NoError)
			{
				this.errorString = DatabaseCrypto.statusMessage(status.cryptoStatus);
				return null;
			}
		}

		try
		{
			doc = docBuilder.parse(new InputSource(new StringReader(fileDataString)));

			Element root = doc.getDocumentElement();
			if (root == null || !root.getTagName().equals(DatabaseKeys.XML_DATABASE))
			{
				this.errorString = "Missing root XML element.";
				return null;
			}
			
			root.normalize();

			Version version = new Version(root.getAttribute(DatabaseKeys.XML_VERSION));
			if (!version.equals(Database.version()))
			{
				this.errorString = String.format("Unsupported database version: %1$s", version);
				return null;
			}

			Database db = new Database();
			db.fromXml(root);

			// Recursively read in all the groups and their subnodes
			this.readGroup(db, root);

			return db;
		}
		catch (SAXException e)
		{
			this.errorString = "Unable to parse the XML tree.";
			return null;
		}
	}

	private void readGroup(Group group, Element element)
	{
		if (group == null)
		{
			return;
		}

		// Find the first child of the element we were given to process
		Node node = element.getFirstChild();
		while (node != null)
		{
			// Convert the node into an element, which should be either a group
			// or entry tag
			Element e = node instanceof Element ? (Element) node : null;
			if (e != null)
			{
				if (e.getTagName() == DatabaseKeys.XML_GROUP)
				{
					Group ourGroup = new Group();
					ourGroup.fromXml(e);
					ourGroup.setParentNode(group);

					// Call the method again to recursively add child nodes to
					// the one we just added
					this.readGroup(ourGroup, e);
				}
				else if (e.getTagName() == DatabaseKeys.XML_ENTRY)
				{
					// If we found an entry, the process here is simply - read
					// all the
					// properties and then add it to the group's list of entries
					Entry entry = new Entry();
					entry.fromXml(e);
					entry.setParentNode(group);
				}
			}

			// Continue on with the loop to process the next sibling node (which
			// can be a group or entry)
			node = node.getNextSibling();
		}
	}
	
	public String getStringFromDocument(Document doc)
	{
		try
		{
			DOMSource domSource = new DOMSource(doc);
			StringWriter writer = new StringWriter();
			StreamResult result = new StreamResult(writer);
			TransformerFactory tf = TransformerFactory.newInstance();
			Transformer transformer = tf.newTransformer();
			transformer.setOutputProperty(OutputKeys.METHOD, "text");
			transformer.setOutputProperty(OutputKeys.STANDALONE, "yes");
			transformer.transform(domSource, result);
			return writer.toString();
		}
		catch (TransformerException ex)
		{
			ex.printStackTrace();
			return null;
		}
	}
}
