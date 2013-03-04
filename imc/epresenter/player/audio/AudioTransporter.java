package imc.epresenter.player.audio;

import java.io.*;
import javax.sound.sampled.*;

import imc.epresenter.player.util.TimeSource;

public class AudioTransporter implements Runnable {
   static boolean DEBUG = false;
   static
   {
      if (System.getProperty("app.sounddebug") != null)
         DEBUG = true;
   }
	
	private AudioInputStream input;
	private SourceDataLine output;
	private byte[] buffer;
	private boolean transport;
	private int sleepTime;

	private EOFListener listener;


	public AudioTransporter(AudioInputStream ais, SourceDataLine sdl, EOFListener eofl) {
		input = ais;
		output = sdl;
		listener = eofl;
		AudioFormat format = input.getFormat();
		float bytesPerSecond = format.getFrameSize()*format.getFrameRate();
		
		// retrieve/calculate "parameters" and setup
		int capacity = (int)(bytesPerSecond*0.0625F); // currently 1000 bytes
		/*
		String bufferSize = System.getProperty("app.bufferSize");
		if (bufferSize != null) capacity = Integer.parseInt(bufferSize);
		System.out.println("AT: capacity="+capacity);
		*/
		buffer = new byte[capacity];
      if (DEBUG)
         System.out.println("Transporter bufferSize = "+capacity);
		sleepTime = (int)((capacity/bytesPerSecond)*1000); // currently 62 ms
      if (DEBUG)
         System.out.println("Transporter sleepTime = "+sleepTime);

		
      /*
		String sleepMillis = System.getProperty("app.sleepMillis");
		if (sleepMillis != null) {
			System.out.println("AT (before): sleepTime="+sleepTime);
			sleepTime = Integer.parseInt(sleepMillis);
			System.out.println("AT  (after): sleepTime="+sleepTime);
		}
      */
		

	}

	public void startOnce() {
		if (!output.isOpen()) throw new IllegalArgumentException(
			"Audio output port (SourceDataLine) is not open!");
		transport = true;
		new Thread(this, "AudioTransporter").start();
	}

	public void stopForever() {
		transport = false;
	}



	public void run() {
		long lastTime = TimeSource.currentTimeMillis();
		//
		// this method is never blocking (on input or output)
		// and after a stopForever() there will be no more data transferred
		//
		while(transport) {
         long currentTime = TimeSource.currentTimeMillis();

			if (output.available() >= buffer.length) {
				if (DEBUG)
            {
               System.out.print("+"+(currentTime-lastTime));
               if (output.available() > output.getBufferSize()*0.9)
                  System.out.print("!");
            }
            try {
					int read = input.read(buffer);
					if (read >= 0) {
						output.write(buffer, 0, read);
					} else {
                  output.drain();
                  listener.endOfMediaReached();
						transport = false;
					}
				} catch (IOException e) {
					e.printStackTrace();
					// little abuse:
					listener.endOfMediaReached();
					transport = false;
				}
			} else { // sleep as long as there is not enough place in output
				if (DEBUG) 
               System.out.print("-");
				try { Thread.sleep(sleepTime); } catch (InterruptedException e) {}
			}

         lastTime = currentTime;
		}
		if (DEBUG) System.out.println();
	}
}