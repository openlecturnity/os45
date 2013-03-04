package imc.epresenter.player.master;

import javax.media.*;
import java.util.*;

/**
 * Funktionstuechtige Minimalimplementation des JMF-Interface Controller.
 * @author Ulrich Kuhn <kuhnu@uni-freiburg.de>
 * @version 0.1, 12.04.00
*/
public abstract class BaseController implements Controller, Runnable {
	private boolean debug = false;
	protected Vector listeners = new Vector(3,3);
	protected int oldstate = Controller.Unrealized;
	protected int currentstate = Controller.Unrealized;
	protected int targetstate = Controller.Unrealized;
	protected TimeBase timeBase;
	protected final int Starting = 560;
	private Time nulltime = new Time(0.0);
	
	protected void sendEvent(ControllerEvent e) {
		Enumeration enum_ = listeners.elements();
		while (enum_.hasMoreElements()) {
			((ControllerListener)enum_.nextElement()).controllerUpdate(e);
		}
	}
	protected void stateChange(int newstate) {
		stateChange(newstate, newstate);
	}
	protected void stateChange(int newstate, int target) {
		oldstate = currentstate;
		currentstate = newstate;
		targetstate = target;
	}
	
	//
   	// Interface Controller
   	//
	public void addControllerListener(ControllerListener listener) {
		listeners.addElement(listener);
	}
    public void removeControllerListener(ControllerListener listener) {
    	listeners.removeElement(listener);
    }
    public void close() { }
	public void deallocate() {
		if (debug) System.out.print("deallocate ");
		stateChange(Realized);
		sendEvent(new StopEvent(this, oldstate, currentstate, targetstate, nulltime));
	}
	public Control getControl(String forName) { return null; }
    public Control[] getControls() { return null; }
    public Time getStartLatency() { return nulltime; }
    	/*
    	  100 - Unrealized
    	  200 - Realizing
    	  300 - Realized
    	  400 - Prefetching
    	  500 - Prefetched
    	  600 - Started
    	*/
    public int getState() {
    	if (debug) System.out.print("getState:"+currentstate+" ");
    	return currentstate;
    }
    public int getTargetState() {
    	if (debug) System.out.print("getTargetState ");
    	return targetstate;
    }
    public void prefetch() {
    	if (debug) System.out.print("prefetch ");
    	stateChange(Prefetching, Prefetched);
   		new Thread(this).start();
	}
	public void realize() {
		stateChange(Realized);
   		if (debug) System.out.print("realize ");
	}
   	
    // 
    // Interface Clock (representing the current media time)
    //
	public long getMediaNanoseconds() { return 0; }
    public Time getMediaTime() { return nulltime; }
    public float getRate() { return 1; }
    public Time getStopTime() { return nulltime; }
    public Time getSyncTime() { return nulltime; }
	public TimeBase getTimeBase() { return timeBase; }
    public Time mapToTimeBase(Time t) { return null; }
    public void setMediaTime(Time now) {
    	if (debug) System.out.print("setMediaTime:"+(float)(now.getSeconds())+" ");
    }
    public float setRate(float factor) { return 1; }
    public void setStopTime(Time stopTime) { }
    public void setTimeBase(TimeBase master) {
    	timeBase = master;
    	if (debug) System.out.print("setTimeBase ");
    }
	public void stop() {
		if (debug) System.out.print("stop ");
		stateChange(Prefetched);
		sendEvent(new StopEvent(this, oldstate, currentstate, targetstate, nulltime));
	}
	public void syncStart(Time at) {
		if (debug) System.out.print("syncStart:"+(float)(at.getSeconds())+" ");
		stateChange(Starting, Started);
		new Thread(this).start();
	}
	
    // 
    // Interface Duration
    //
	public Time getDuration() {
		if (debug) System.out.print("getDuration ");
		return nulltime;
	}
	
	//
	// Interface Runnable
	//
	public void run() {
    	if (currentstate == Controller.Prefetching) {
    		stateChange(Prefetched);
    		sendEvent(new PrefetchCompleteEvent(this, oldstate, currentstate, targetstate));
    	} else if (currentstate == Starting) {
    		stateChange(Started);
    		sendEvent(new StartEvent(this, Controller.Prefetched,
				currentstate, targetstate, timeBase.getTime(), nulltime));
		}
	}
	
}
