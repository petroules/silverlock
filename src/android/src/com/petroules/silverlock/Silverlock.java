package com.petroules.silverlock;

import com.petroules.silverlock.DatabaseCrypto;
import com.petroules.silverlock.R;

import android.app.Activity;
import android.os.Bundle;

public class Silverlock extends Activity
{
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		this.setContentView(R.layout.main);
		
		DatabaseCrypto c;
	}
}
