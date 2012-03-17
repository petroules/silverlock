package com.petroules.silverlock;

import java.io.IOException;
import java.io.InputStreamReader;
import java.net.MalformedURLException;
import java.net.URL;

import javax.xml.parsers.ParserConfigurationException;

import android.app.Activity;
import android.app.AlertDialog;
import android.os.AsyncTask;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import com.petroules.BufferedReader2;

public class Silverlock extends Activity {
	private EditText editTextURL;
	private EditText editTextPassword;
	private Button buttonOK;
	private Button buttonTests;
	
	private long start;
	private long counter;
	private boolean running = false;
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        this.editTextURL = (EditText)this.findViewById(R.id.editTextURL);
        this.editTextPassword = (EditText)this.findViewById(R.id.editTextPassword);
        this.buttonOK = (Button)this.findViewById(R.id.buttonOK);
        this.buttonTests = (Button)this.findViewById(R.id.buttonTests);
        
        this.buttonOK.setOnClickListener(new View.OnClickListener() { public void onClick(View v) { Silverlock.this.buttonOK_onClick(v); }});
        this.buttonTests.setOnClickListener(new View.OnClickListener() { public void onClick(View v) { Silverlock.this.buttonTests_onClick(v); }});
    }
    
    private void buttonTests_onClick(View v)
    {
    	try
    	{
    		TestData.setAssetManager(new AndroidAssetManager(getAssets()));
	    	//DatabaseCryptoTest test = new DatabaseCryptoTest();
	    	/*test.testTwoWayFileEncryption();
	    	test.testTwoWayEncryption();
	    	test.testTwoWayCompression();
	    	test.testStandardFileDecryptsToExpectedXml();
	    	test.testStandardFileDecryptsToInstance();*/
	    	//test.testHMAC();
	    	Native c = new Native();
	    	byte[] key = c.generateKeys("foo");
	    	String message = "null";
	    	if (key != null)
	    	{
	    		message = key.length + "bytes, (";
	    		for (int i = 0; i < key.length; i++)
	    		{
	    			message += key[i] + " ";
	    		}
	    		
	    		message = message.substring(0, message.length() - 1);
	    		message += ")";
	    	}
	    	
	    	new AlertDialog.Builder(this).setTitle("Success").setMessage(message).show();
    	}
    	catch (Exception e)
    	{
    		new AlertDialog.Builder(this).setTitle("Error").setMessage("error: " + e.getMessage()).show();
    	}
    }
    
    private void buttonOK_onClick(View v)
    {
    	// Only start the decryption process if we haven't already
    	if (!running)
    	{
    		// Decryption process has now started...
    		running = true;
    		
    		// Create a thread to perform the actual decryption process
    		start = System.currentTimeMillis();
			downloadAndDecryptJakeFile();
	    	
	    	// Create another thread to keep track of how long the decryption process has been running
	    	new Thread()
	    	{
	    		public void run()
	    		{
	    			while (running)
	    			{
	    				counter = System.currentTimeMillis() - start;
	    				try {
							sleep(1000);
						} catch (InterruptedException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
	    				
	    				System.out.println("Running for " + counter + "ms");
	    			}
	    		}
	    	}
	    	.start();
    	}
    }
    
    private void downloadAndDecryptJakeFile()
    {
    	URL url = null;
		try
		{
			url = new URL(this.editTextURL.getText().toString());
		}
		catch (MalformedURLException e)
		{
			new AlertDialog.Builder(this)
			    .setTitle("Error")
			    .setMessage("Invalid URL.")
			    /*.setPositiveButton("Yes", new DialogInterface.OnClickListener() {
			        public void onClick(DialogInterface dialog, int which) { 
			            // continue with delete
			        }
			     })
			    .setNegativeButton("No", new DialogInterface.OnClickListener() {
			        public void onClick(DialogInterface dialog, int which) { 
			            // do nothing
			        }
			     })*/
			    .show();
			return;
		}
		
        String pass = this.editTextPassword.getText().toString();
        
        new DecryptDatabaseTask().execute(new DatabaseSource(url, pass));
    }
    
    private class DatabaseSource
    {
    	public DatabaseSource(URL url, String pass)
    	{
    		this.url = url;
    		this.pass = pass;
    	}
    	
    	public URL url;
    	public String pass;
    }
    
    private class DecryptDatabaseTask extends AsyncTask<DatabaseSource, Integer, Database>
    {
    	DatabaseReader reader = new DatabaseReader();
    	
		@Override
		protected Database doInBackground(DatabaseSource... ds)
		{
			try {
				return reader.read(new BufferedReader2(new InputStreamReader(ds[0].url.openStream())), ds[0].pass);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (ParserConfigurationException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			return null;
		}
    	
		@Override
		protected void onPostExecute(Database db)
		{
			running = false;
			editTextURL.setText("Took " + counter + "ms");
			
			if (db == null)
			{
				System.out.println("db was null");
				new AlertDialog.Builder(Silverlock.this).setTitle("Error").setMessage(reader.getErrorString()).show();
				return;
			}
			
			System.out.println(db.getTitle());
			new AlertDialog.Builder(Silverlock.this).setTitle("Info").setMessage(db.getTitle());
		}
    }
}
