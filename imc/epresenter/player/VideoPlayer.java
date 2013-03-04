package imc.epresenter.player;

import java.awt.*;
import java.awt.image.*;
import java.io.File;
import javax.swing.*;

import imc.epresenter.filesdk.FileResources;
import imc.epresenter.player.util.*;

public class VideoPlayer implements SRStreamPlayer, Runnable {
	private static boolean libraryLoaded;
	static {
		try {
			String libName = "divx2java";
			if (System.getProperty("app.library") != null)
				libName = System.getProperty("app.library");
			System.loadLibrary(libName);
			libraryLoaded = true;
		} catch (UnsatisfiedLinkError e) {
			libraryLoaded = false;
			e.printStackTrace();
		}
	}


	private static void main(String[] args) {
		/* due to new init method
		final VideoPlayer p = new VideoPlayer();
		p.init(new File("."), args, null);

		JFrame f = new JFrame("Video");
		if (System.getProperty("app.nodb") != null)
			RepaintManager.currentManager(f).setDoubleBufferingEnabled(false);
		f.setContentPane((Container)p.getVisualComponent());
		f.pack();
		f.setVisible(true);
		
		long time1 = System.currentTimeMillis();
		
		Thread t = new Thread() {
			public void run() {
				for (int i=0; i<5000; i++) {
					if (libraryLoaded) {
						p.p(0, i*40);
					} else {
						p.p(0xff000000 | (int)(i/1000.0*256), 0);
					}
					try { Thread.sleep(10); } catch (InterruptedException e) {}
				}
			}
		};
		t.start();
		try { t.join(); } catch (InterruptedException e) {} 
		
		long time2 = System.currentTimeMillis();


		//System.out.println((time2-time1)+" for "+counter+" frames ("+((time2-time1)/counter)+").");
	   */
	}


	private native void initDecoder(String fileName);
	private native int getVideoWidth();
	private native int getVideoHeight();
	private native void extractFrame(int millis, int[] pixels, boolean nextKeyFrame);
	private native void destroyDecoder();
	

	private int[] pixels;
	private int videoWidth;
	private int videoHeight;
	private MemoryImageSource source;
	private JComponent visualComponent;
	private boolean closed;
	private boolean running;
	private boolean started;
	private boolean scrolling;
	private int shownTime;
	private int requestedTime;
	
	//
	// Interface SRStreamPlayer
	//
   public void setStartOffset(int millis)
   {
      throw new UnsupportedOperationException("Currently not implemented for this component.");
   }
	
   public void init(FileResources resources, String[] args, Coordinator c) {
		if (libraryLoaded) {
			File video = resources.getFile(args[0]);
			if (video == null || !video.exists()) throw new IllegalArgumentException(
				"Specified video file ("+video+") does not exist.");
			
			initDecoder(video.getAbsolutePath());
			videoWidth = getVideoWidth();
			videoHeight = getVideoHeight();
			if (videoWidth < 1 || videoHeight < 1) throw new RuntimeException(
				"Illegal video size ("+videoWidth+"x"+videoWidth+").");
		} else {
			videoWidth=352;
			videoHeight=288;
		}

		pixels = new int[videoWidth*videoHeight];
		java.util.Arrays.fill(pixels, 0xff0000ff);
		DirectColorModel dcm = new DirectColorModel(
			32, 0x00ff0000, 0x0000ff00, 0x000000ff);
		source = new MemoryImageSource(videoWidth, videoHeight, dcm, pixels, 0, videoWidth);
		source.setAnimated(true);
		source.setFullBufferUpdates(true);
		 
		//visualComponent = new TLabel(new ImageIcon(
		//	Toolkit.getDefaultToolkit().createImage(source)));
		visualComponent = new VideoPanel(
			Toolkit.getDefaultToolkit().createImage(source), videoWidth, videoHeight);
		
		shownTime = -1;
		requestedTime = 0;
	} // init
	
	
	
	public void enableVisualComponents(boolean bEnable)
   {
      // not implemented here
   }

	public Component[] getVisualComponents() {
		return new Component[] { visualComponent };
	}

	public String getTitle() {
		return "VideoPlayer";
	}	

	public String getDescription() {
		return "Replay of an video stream.";
	}
	

	public void start(EventInfo info) { started = true; scrolling = false;}
	public void pause(EventInfo info) { scrolling = true; }
	public void stop(EventInfo info) { started = false; }
	

	public void p(int color, int time) {
		//visualComponent.repaint();
		
		if (libraryLoaded) {
			long time1 = System.currentTimeMillis();
			extractFrame(time, pixels, false);
			long time2 = System.currentTimeMillis();
			if (System.getProperty("app.videoTiming") != null)
				System.out.print("vt"+(time2-time1)+" ");
		} else {
			java.util.Arrays.fill(pixels, color);
		}
		source.newPixels(0,0,videoWidth,videoHeight);
	}

	long accessStart;
	public void setMediaTime(int millis, EventInfo info) {
	    requestedTime = millis;
		accessStart = System.currentTimeMillis();
		if (!running && libraryLoaded) new Thread(this).start(); // we have an inactivity timer
	}
	
	public int getMediaTime() { return 0; }
	public int getDuration() { return 0; }
	public void setDuration(int millis) { }
	public void setInfo(String key, String value) {}

	public void close(EventInfo info) {
		closed = true;
		if (libraryLoaded) destroyDecoder();
	}

	public void run() {
		running = true;
		int inactivityCounter = 0;


		while(!closed && inactivityCounter < 64) {
			if (shownTime != requestedTime) {
				inactivityCounter = 0;
				long time1 = System.currentTimeMillis();
				extractFrame(requestedTime, pixels, !started || scrolling);
				long time2 = System.currentTimeMillis();
				if (System.getProperty("app.videoTiming") != null)
					System.out.print("vt"+(time2-time1)+" ");
				source.newPixels(0,0,videoWidth,videoHeight);
				shownTime = requestedTime;
			} else {
				inactivityCounter++;
			}
			try { Thread.sleep(20); } catch (InterruptedException e) {}
		}
		running = false;
		if (System.getProperty("app.videoTiming") != null)
			System.out.println();
				
	}
	



	static int counter;

	class VideoPanel extends JPanel {
		private Image video;
		private FrameCounter frameCounter;
		private long lastMillis;

		public VideoPanel(Image v, int w, int h) {
			setOpaque(true);
			video = v;
			frameCounter = new FrameCounter();
			setPreferredSize(new Dimension(w,h));
			lastMillis = TimeSource.currentTimeMillis();
		}

		protected void paintComponent(Graphics g) {
			Dimension d = getSize();
			Dimension s = getPreferredSize();
			int posX=0, posY=0;
			if (s.width != d.width || s.height != d.height) {
				if (d.width > s.width || d.height > s.height) {
					// chane me: clear only unused spaces (-> no flicker)
					g.clearRect(0,0,d.width,d.height);
				}
				posX = (d.width-s.width)/2;
				posY = (d.height-s.height)/2;
			}
			
			g.drawImage(video, posX, posY, this);
			
			String fpsString = frameCounter.getCurrentFrames()+"";
			g.setColor(Color.black);
			g.drawString(fpsString, 6, d.height-4);
			g.setColor(Color.white);
			g.drawString(fpsString, 4, d.height-6);
			
			long currentMillis = TimeSource.currentTimeMillis();
			frameCounter.add((int)(currentMillis-lastMillis));
			lastMillis = currentMillis;
			
			counter++;
		}
	}
	
	class TLabel extends JLabel {
		boolean ACCESS = System.getProperty("app.access") != null;

		public TLabel(ImageIcon ii) {
			super(ii);
		}
		protected void paintComponent(Graphics g) {
			super.paintComponent(g);
			counter++;
			if (ACCESS) System.out.println("Time for retrieval and paint: "
										   +(System.currentTimeMillis()-accessStart));
		}
	}


}