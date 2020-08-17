package com.android.server;
import  android.os.IFarsightLedService;
import  android.util.Slog;


public class FarsightLedService extends IFarsightLedService.Stub{
	public FarsightLedService(){
		Slog.i("FarsightLed","Farsight Led init");
		ledInit();
	}
	
	public void farsightLedOn(){
		Slog.i("FarsightLed","Farsight led on");
		ledOn();
	}

	public void farsightLedOff(){
		Slog.i("FarsightLed","Farsight led off");
		ledOff();
	}
	
	public native void ledInit();
	public native void ledOn();
	public native void ledOff();
}
