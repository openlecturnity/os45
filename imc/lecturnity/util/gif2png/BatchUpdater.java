package imc.lecturnity.util.gif2png;

import java.io.*;

import imc.epresenter.filesdk.FileResources;

public class BatchUpdater
{
   public static void main(String[] args)
   {
      new BatchUpdater(args);
   }

   public BatchUpdater(String[] args)
   {
      System.out.println("Lecturnity Document Optimizer 1.4.0");
      System.out.println();
      System.out.println("Copyright (c) 2002 by imc AG");
      System.out.println("Saarbrucken, Berlin, Cologne, Freiburg - Germany, Zurich - Switzerland");
      System.out.println("imc AG, Office Freiburg, Germany");
      System.out.println("http://www.im-c.de");
      System.out.println();

      // check the arguments:
      String arg1 = null;
      String arg2 = null;
      boolean argsOk = false;

      if (args.length == 1)
      {
         if (!args[0].equalsIgnoreCase("-help") &&
             !args[0].equalsIgnoreCase("/?") &&
             !args[0].equalsIgnoreCase("--help"))
         {
            // assume args[0] to be a LRD file
            arg1 = args[0];

            try
            {
               if (!FileResources.hasNewFormat(arg1))
                  argsOk = true;
            }
            catch (IOException e)
            {
               System.out.println("Error: " + e.getMessage());
            }
         }
      }
      else if (args.length == 2)
      {
         // assume args[0] to be an LPD file
         arg1 = args[0];
         arg2 = args[1];

         try
         {
            if (FileResources.hasNewFormat(arg1))
               argsOk = true;
         }
         catch (IOException e)
         {
            System.out.println("Error: " + e.getMessage());
         }
      }

      if (!argsOk)
      {
         System.out.println("Usage:");
         System.out.println("  docoptb.exe <input.lrd>, or");
         System.out.println("  docoptb.exe <input.lpd> <output.lpd>");
      }
      else
      {
         try
         {
            String args1 = null;
            if (args.length > 1)
               args1 = args[1];
            Gif2PngEngine engine = new Gif2PngEngine(args[0], args1, null, null);
            engine.update();
         }
         catch (Exception e)
         {
            System.out.println("Error: " + e.getMessage());
            
            try
            {
               PrintStream out = new PrintStream(new FileOutputStream("_docopt.txt"));
               e.printStackTrace(out);
               out.flush();
               out.close();
            }
            catch (FileNotFoundException e2)
            {
               e2.printStackTrace();
               System.err.println("*** FATAL ERROR");
               System.exit(1);
            }
            
            System.exit(1);
         }
      }

      System.exit(0);
   }
}

