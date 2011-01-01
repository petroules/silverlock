package com.petroules.silverlock;

import net.rim.device.api.ui.DrawStyle;
import net.rim.device.api.ui.Field;
import net.rim.device.api.ui.UiApplication;
import net.rim.device.api.ui.component.Dialog;
import net.rim.device.api.ui.component.LabelField;
import net.rim.device.api.ui.component.RichTextField;
import net.rim.device.api.ui.container.MainScreen;

public class Silverlock extends UiApplication
{
	public static void main(String[] args)
	{
		Silverlock theApp = new Silverlock();
		theApp.enterEventDispatcher();
	}

	public Silverlock()
	{
		this.pushScreen(new HelloWorldScreen());
	}
}

class HelloWorldScreen extends MainScreen
{
	public HelloWorldScreen()
	{
		super();

		LabelField title = new LabelField("Silverlock", DrawStyle.ELLIPSIS | Field.USE_ALL_WIDTH);
		this.setTitle(title);
		
		this.add(new RichTextField("Hello World!"));
	}

	public boolean onClose()
	{
		Dialog.alert("Goodbye!");
		System.exit(0);
		return true;
	}
}
