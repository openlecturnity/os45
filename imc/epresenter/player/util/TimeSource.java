package imc.epresenter.player.util;

public class TimeSource implements Runnable {
    private static boolean libraryLoaded;
	static {
		try {
			System.loadLibrary("time2java");
			libraryLoaded = true;
		} catch (UnsatisfiedLinkError e) {
			libraryLoaded = false;
		}
	}

	private native static long nativeMillis();
	private native void initNativeTimer(Timeable sink, int millis);
	private native void startNativeTimer(Timeable sink);
	private native void stopNativeTimer(Timeable sink);
	private native void destroyNativeTimer(Timeable sink);


	public static long currentTimeMillis() {
		if (libraryLoaded) return nativeMillis();
		else return System.currentTimeMillis();
	}


	private int delay;
	private Timeable sink;
	private boolean ticking;
	
	public TimeSource(Timeable t) { this(t,40); }
	public TimeSource(Timeable t, int d) {
		sink = t;
		delay = d;
		if (libraryLoaded) initNativeTimer(sink, delay);
	}
	
	public void start() {
		ticking = true;
		if (libraryLoaded) startNativeTimer(sink);
		else new Thread(this, "TimeSource").start();
	}
	
	public void run() {
		while (ticking) {
			try { Thread.sleep(delay); } catch (InterruptedException e) {}
			sink.tickTime();
		}
	}
	
	public boolean isRunning() {
		return ticking;
	}
	
	public void stop() {
		ticking = false;
		if (libraryLoaded) stopNativeTimer(sink);
	}

	protected void finalize() {
		if (libraryLoaded) destroyNativeTimer(sink);
	}
}
