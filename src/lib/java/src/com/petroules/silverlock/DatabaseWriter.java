package com.petroules.silverlock;

import java.io.BufferedWriter;
import java.io.IOException;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import com.petroules.silverlock.DatabaseCrypto.CryptoStatusWrapper;

public class DatabaseWriter
{
	private String errorString;
	
    public DatabaseWriter()
    {
        this.errorString = "";
    }

    public String getErrorString()
    {
    	return this.errorString;
    }
    
    public boolean write(Database database, BufferedWriter stream) throws IOException, ParserConfigurationException
    {
    	return this.write(database, stream, true);
    }

    public boolean write(Database database, BufferedWriter stream, boolean encrypt) throws IOException, ParserConfigurationException
    {
        // Clear the last error
        this.errorString = "";

        DocumentBuilderFactory dbfac = DocumentBuilderFactory.newInstance();
		DocumentBuilder docBuilder = dbfac.newDocumentBuilder();
        Document document = docBuilder.newDocument();
        this.append(document, document, database);

        String fileDataString = document.getTextContent();

        if (encrypt)
        {
            DatabaseCrypto.CryptoStatusWrapper status = new CryptoStatusWrapper();
            fileDataString = DatabaseCrypto.encrypt(fileDataString, database.getPassword(), database.getCompression(), status);
            if (status.cryptoStatus != DatabaseCrypto.CryptoStatus.NoError)
            {
                this.errorString = DatabaseCrypto.statusMessage(status.cryptoStatus);
                return false;
            }
        }

        stream.write(fileDataString);
        return true;
    }

    private void append(Document document, Node node, Group group)
    {
        if (group == null)
        {
            return;
        }

        Element element = group.toXml(document);
        node.appendChild(element);

        for (Group childGroup : group.getGroups())
        {
            this.append(document, element, childGroup);
        }

        for (Entry entry : group.getEntries())
        {
            element.appendChild(entry.toXml(document));
        }
    }
}