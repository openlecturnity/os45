package imc.epresenter.player;

import java.io.*;
import java.util.Random;

import imc.lecturnity.util.NativeUtils;

public class SpeedCheck 
{
   public static int testSpeedOf(String location)
   {
      File f = new File(location);

      if (!f.exists())
         throw new IllegalArgumentException("File ("+f+") does not exist.");

      if (f.length() < 10*1024)
         return Integer.MAX_VALUE;


      RandomAccessFile file = null;
      try
      {
         file = new RandomAccessFile(location, "r");
      }
      catch (FileNotFoundException e)
      {
         // may not occur
      }

      Object notifyTarget = new Object();

      AccessThread at = new AccessThread(file, notifyTarget);
      TimeoutThread tt = new TimeoutThread(notifyTarget);
      tt.start();
      at.start();

      try {
         synchronized(notifyTarget)
         {
            notifyTarget.wait();
         }
      }
      catch (InterruptedException exc)
      {
      }

      tt.stop();
      at.stop();

      try
      {
         file.close();
      }
      catch (IOException exc)
      {
         // hm ...
      }

      // in case of a failure or access timeout this may be -1
      return at.getMinimumSpeed();


   }


   static class AccessThread implements Runnable
   {
      private int TIME_OUT = 1000*1000; // us for read time after the first block
      private int BYTE_OUT = 250*1024; // max number of bytes to read
      private Object notifyTarget_;
      private RandomAccessFile testFile_;
      private boolean isCancelled_;
      private int minimumSpeed_ = -1;

      AccessThread(RandomAccessFile raf, Object target)
      {
         testFile_ = raf;
         notifyTarget_ = target;

      }

      public void start()
      {
         new Thread(this, "SpeedCheck: AccessThread").start();
      }

      public void run()
      {
         long length = 0;
         try
         {
            length = testFile_.length();
         }
         catch (IOException exc)
         {
            // can this occur?
            exc.printStackTrace();
         }


         Random m = new Random();

         double[] samples = new double[3];
         for (int i=0; i<samples.length; i++)
         {
            samples[i] = samples[i > 0 ? i-1 : 0] + m.nextDouble();
         }
         double total = 
            samples[samples.length-1] + m.nextDouble() + samples[samples.length-1]/10;

         long[] seekPoint = new long[samples.length];
         for (int i=0; i<seekPoint.length; i++)
            seekPoint[i] = (long)((samples[i]/total)*length);



         int[] bytesPerSecond = new int[samples.length];

         byte[] buffer = new byte[4096];
            
         for (int i=0; i<seekPoint.length; i++)
         {
            int totalRead = 0;
            long startTime = NativeUtils.getCurrentMicroseconds();
            long firstData = 0;

            try
            {
               testFile_.seek(seekPoint[i]);

               while(true)
               {
                  int r = testFile_.read(buffer);
                  if (r == -1) // end of file
                     break;
                  if (isCancelled_)
                     return;
                  totalRead += r;
                  if (firstData == 0)
                     firstData = NativeUtils.getCurrentMicroseconds();
                  if (totalRead > BYTE_OUT)
                     break;
                  if (NativeUtils.getCurrentMicroseconds()-firstData > TIME_OUT)
                     break;
               }
            }
            catch (IOException exc)
            {
               // minimumSpeed_ remains -1
               return;
            }

            long stopTime = NativeUtils.getCurrentMicroseconds();

            /*
            System.out.println((startTime/1000)+" "
                               +(firstData/1000)+" "
                               +(stopTime/1000)+"; "+totalRead);
                               */

            if (firstData == stopTime)
               firstData = stopTime-1;

            bytesPerSecond[i] = (int)(totalRead/((stopTime-firstData)/1000000.0));
           
         }
          
         if (!isCancelled_)
         {
            for (int i=0; i<samples.length; i++)
               if (minimumSpeed_ == -1 || bytesPerSecond[i] < minimumSpeed_)
                  minimumSpeed_ = bytesPerSecond[i];
         


            synchronized(notifyTarget_) 
            {
               notifyTarget_.notify();
            }
         }

      }

      public void stop()
      {
         isCancelled_ = true;
      }

      public int getMinimumSpeed()
      {
         return minimumSpeed_;
      }

   }

   static class TimeoutThread implements Runnable
   {
      private int TIME_OUT = 15*1000; // ms for the first block 
      private Object notifyTarget_;
      private boolean isCancelled_;


      TimeoutThread(Object target)
      {
         notifyTarget_ = target;
      }



      public void start()
      {
         new Thread(this, "SpeedCheck: TimeoutThread").start();
      }

      public void run()
      {
         try { Thread.sleep(TIME_OUT); } catch (InterruptedException e) {}
         if (!isCancelled_)
         {
            synchronized(notifyTarget_) 
            {
               notifyTarget_.notify();
            }
         }

      }

      public void stop()
      {
         isCancelled_ = true;
      }


   }



}
