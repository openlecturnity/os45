package imc.epresenter.player;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.net.*;
import java.util.*;
import javax.swing.*;
import javax.swing.border.Border;

import imc.epresenter.filesdk.FileResources;
import imc.epresenter.player.*;
import imc.epresenter.player.util.*;

public class ApplicationPlayer implements SRStreamPlayer, ActionListener {
	public static void main(String[] args) throws Exception {
		/*
		ApplicationPlayer appp = new ApplicationPlayer();
		appp.init(new File(args[0]).getParentFile(), args);
		*/
	}
	
	
	private int currentTime;
	private AppEntry[] entries;
	private JButton[] buttons;
	private LED[] leds;
	private ArrayList activePrograms;
	private HashMap ledMap;
	private JPanel visualComponent;
	private Coordinator coordinator;
	private boolean started;
	
	
	public ApplicationPlayer() {
		activePrograms = new ArrayList();
	}
	
   public void setStartOffset(int millis)
   {
      throw new UnsupportedOperationException("Currently not implemented for this component.");
   }

	
	public void init(FileResources resources, String[] args, Coordinator c) {
		coordinator = c;

		String appevq = null;
		String appobj = null;
		
		for (int i=0; i<args.length; i++) {
			String argument = args[i].trim();
			if (argument.endsWith("appevq")) appevq = argument;
			if (argument.endsWith("appobj")) appobj = argument;
		}
		
		if (appevq == null || appobj == null) {
			System.err.println("Insufficient parameters for ApplicationPlayer.");
			// Abbruch ??
		}
		
		
		ArrayList time = new ArrayList(20);
		ArrayList what = new ArrayList(20);
		
		try {
			BufferedReader evqr = new BufferedReader(
				new InputStreamReader(resources.createInputStream(appevq)));
			BufferedReader objr = new BufferedReader(
				new InputStreamReader(resources.createInputStream(appobj)));
			
			String input = null;
			
			// read evq
			// append " " to each line and for latter 
			// looking for " "+i+" " and for easy cutout of time
			while((input = evqr.readLine()) != null) time.add(input+" ");
			
			//read obj
			while((input = objr.readLine()) != null) {
				if (input.trim().startsWith("<APP ")) what.add(input.trim());
			}
			
		} catch (IOException e) {
			throw new RuntimeException("Unable to init ApplicationPlayer ("+e+").");
		}
		

		entries = new AppEntry[what.size()];
		// extract entries
		for (int i=0; i<entries.length; i++) {
			SGMLParser content = new SGMLParser((String)what.get(i));
			String name = extractString(content, "filename");
			int start = -1;
			int end = -1;
			
			Iterator iter = time.iterator();
			// find the first line from evq containing an i
			while (iter.hasNext()) {
				String line = (String)iter.next();
				if (line.indexOf(" "+i+" ") != -1) {
					start = Integer.parseInt(line.substring(0, line.indexOf(' ')));
					break;
				}
			}
			
			// find the first line without an i
			while (iter.hasNext()) {
				String line = (String)iter.next();
				if (line.indexOf(" "+i+" ") == -1) {
					end = Integer.parseInt(line.substring(0, line.indexOf(' ')));
					//System.out.println(end);
					break;
				}
			}
			
			//entries[i] = new AppEntry(name, rootDir, start, end);
         entries[i] = new AppEntry(name, new File("."), start, end);
		}
		
		
		// setup UI
		if (entries.length > 0) {
			
			buttons = new JButton[entries.length];
			leds = new LED[entries.length];
			ledMap = new HashMap();
         
         
         /*
			JPanel signs = new JPanel(new GridLayout(0,1,0,2));
			JPanel names = new JPanel(new GridLayout(0,1,0,2));
			JPanel starts = new JPanel(new GridLayout(0,1,0,2));
			JPanel stops = new JPanel(new GridLayout(0,1,0,2));
			
			TimeFormat format = new TimeFormat();

			for (int i=0; i<entries.length; i++) {
				leds[i] = new LED();
				ledMap.put(entries[i], leds[i]);
				signs.add(leds[i]);
				buttons[i] = new JButton(entries[i].execName);
				buttons[i].addActionListener(this);
				names.add(buttons[i]);
				starts.add(new JLabel(format.format(entries[i].startMillis)+"-"));
				stops.add(new JLabel(format.format(entries[i].endMillis)));
			}

			JPanel times = new JPanel(new BorderLayout());
			times.add("West", starts);
			times.add("East", stops);

			JPanel total = new JPanel(new BorderLayout(10,0));
			total.add("West", signs);
			total.add("Center", names);
			total.add("East", times);
			total.setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));
         */
         
         
         JPanel inner = new JPanel();
         inner.setLayout(new BoxLayout(inner, BoxLayout.Y_AXIS));
         
         TimeFormat format = new TimeFormat();

         Dimension maximumButton = new Dimension(0,0);
			for (int i=0; i<entries.length; i++) {
            JPanel p1 = new JPanel(new FlowLayout());
            JPanel p2 = new JPanel(new FlowLayout(FlowLayout.CENTER, 0, 0));
            leds[i] = new LED();
				ledMap.put(entries[i], leds[i]);
				//signs.add(leds[i]);
				buttons[i] = new JButton(entries[i].execName);
				buttons[i].addActionListener(this);
            Dimension d = buttons[i].getPreferredSize();
            if (d.width > maximumButton.width) maximumButton = d;
				//names.add(buttons[i]);
            JLabel l1 = new JLabel(format.format(entries[i].startMillis)+"-");
            l1.setForeground(Color.black);
				JLabel l2 = new JLabel(format.format(entries[i].endMillis));
            l2.setForeground(Color.black);
            p2.add(l1);
				p2.add(l2);
            p1.add("West", leds[i]);
            //p1.add("South", p2);
            p1.add("Center", buttons[i]);

            inner.add(p1);
            inner.add(p2);
			}

         for (int i=0; i<buttons.length; i++)
         {
            buttons[i].setPreferredSize(maximumButton);
         }
			
         JPanel total = new JPanel(new BorderLayout());
         total.add("North", inner);
         
         visualComponent = total;
		}
	} // init
	
	
	
	//
	// Interface SRStreamPlayer
	//
	public void enableVisualComponents(boolean bEnable)
   {
      // not implemented here
   }

	public Component[] getVisualComponents() {
		return new Component[] { visualComponent };
	}

	public String getTitle() {
		return "ApplicationPlayer";
	}	

	public String getDescription() {
		return "Replay of an application stream.";
	}
	

	// pause ist beim derzeitigen Modell mit scrollen identisch
	public void pause(EventInfo info) { started = false; }
	public void start(EventInfo info) { started = true; }
	public void stop(EventInfo info) { started = false; }
	
	public synchronized void setMediaTime(int millis, EventInfo info) {
		if (started) {
			ArrayList list = findProgramsFor(millis);
			if (list.size() > 0 || activePrograms.size() > 0) {
				ArrayList toStop = cutOut(activePrograms, list);
				ArrayList toStart = cutOut(list, activePrograms);

				Iterator iter1 = toStop.iterator();
				while(iter1.hasNext()) {
					AppEntry e = (AppEntry)iter1.next();
					//System.out.println("AP: stop: "+e);
					LED led = (LED)ledMap.get(e);
					if (led.isFlashing()) led.stopFlashing();
					if (e.started) {
						e.stop();
						led.setActive(false);
					}
					activePrograms.remove(activePrograms.indexOf(e));
				}

				Iterator iter2 = toStart.iterator();
				while(iter2.hasNext()) {
					AppEntry e = (AppEntry)iter2.next();
					//System.out.println("AP: start: "+e);
					LED led = (LED)ledMap.get(e);
					if (millis > e.startMillis-500 && millis <= e.startMillis+1000) {
						if (e.executable) {
							e.start();
							// C-style error check:
							if (e.executable) {
								led.setActive(true);
							} else { // problems with starting:
								led.setColor(Color.red);
								led.startFlashing();
							}
						} else { // should be started but is not executable
							led.startFlashing();
						}
					} else { // should be started but too late
						led.startFlashing();
					}
					activePrograms.add(e);
				}
			}
		}
	}
	
	public int getMediaTime() { return currentTime; }
	public int getDuration() { return 0; }
	public void setDuration(int millis) { }
	public void setInfo(String key, String value) {}

	public void close(EventInfo info) { 
		// Programm-Stop ???
	}
	
	private String extractString(SGMLParser p, String key) {
		String s = p.getValue(key);
		return s != null ? s : "";
	}
	
	private ArrayList findProgramsFor(int millis) {
		ArrayList result = new ArrayList(2);
		if (entries != null && entries.length > 0) {
			for (int i=0; i<entries.length; i++) {
				AppEntry e = entries[i];
				if (millis > e.startMillis-500 && millis < e.endMillis+1500)
					result.add(e);
			}
		}
		return result;
	}
	
	public void actionPerformed(ActionEvent e) {
		// find appropriate AppEntry
		int idx = -1;
		for (int i=0; i<buttons.length; i++) {
			if (e.getSource() == buttons[i]) {
				idx = i;
				break;
			}
		}

		// start application implicitly with next invocation of setMediaTime
		coordinator.requestTime(entries[idx].startMillis-1000, this);
		coordinator.requestStart(this);
	}

	private ArrayList cutOut(ArrayList cut, ArrayList out) {
		ArrayList result = new ArrayList(cut);
		Iterator iter = out.iterator();
		while(iter.hasNext()) {
			int idx = result.indexOf(iter.next());
			if (idx != -1) result.remove(idx);
		}
		return result;
	}
	
	
	
	private class AppEntry {
		boolean started = false;
		boolean executable = true;
		int startMillis;
		int endMillis;
		String execName;
		File programDir;
		Process handle;
		ProcessOutputRedirector redirector;
		
		AppEntry(String name, File dir, int start, int end) {
			execName = name;
			programDir = dir;
			startMillis = start;
			endMillis = end;
		}
		
		void start() {
			if (started || !executable) return;
			
			try {
				handle = Runtime.getRuntime().exec(execName);
					//programDir+File.separator+execName,
					//new String[] {}, programDir);
            int exitCode = 0;
            try
            {
               exitCode = handle.exitValue();
            }
            catch(IllegalThreadStateException e)
            {
               // normal case: do nothing
            }
            if (exitCode != 0)
            {
               executable = false;
               return;
            }
				redirector = new ProcessOutputRedirector(handle);
            try
            {
               Thread.sleep(500);
            }
            catch (InterruptedException e)
            {
            }
            if (redirector.hasError())
            {
               executable = false;
               return;
            }
			} catch (IOException e) {
				executable = false;
				System.err.println(execName+" has generated the exception "+e);
				//e.printStackTrace();
			}
			started = true;
		}
		
		void stop() {
			if (handle != null) 
         {
            handle.destroy();
            System.out.println(execName+" destroyed");
         }
			if (redirector != null) redirector.stopOutput();
			started = false;
		}
		
		String errorOutput() {
			return null;
		}
	}
	
	private class ProcessOutputRedirector implements Runnable {
		private InputStream in;
		private InputStream err;
		private boolean reading;
      private boolean errored;

		ProcessOutputRedirector(Process p) {
			in = p.getInputStream();
			err = p.getErrorStream();
			reading = true;
			new Thread(this).start();
		}

		public void run() {
			byte[] buffer = new byte[1024];
			while(reading) {
				try {
					while (in.available() > 0) {
						int read = in.read(buffer, 0, Math.min(in.available(), buffer.length));
						System.out.write(buffer, 0, read);
					}
					while (err.available() > 0) {
                  errored = true;
						int read = err.read(buffer, 0, Math.min(err.available(), buffer.length));
						System.err.write(buffer, 0, read);
					}
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}

		public void stopOutput() {
			reading = false;
		}

      public boolean hasError()
      {
         return errored;
      }
	}
}