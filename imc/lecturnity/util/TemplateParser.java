package imc.lecturnity.util;

import java.io.*;

import java.util.PropertyResourceBundle;
import java.util.Enumeration;
import java.util.HashSet;
import java.util.HashMap;

public class TemplateParser
{
   /*
   public static void main(String[] args)
      throws IOException
   {
      TemplateParser tp = new TemplateParser(args);
      tp.doParse();
      tp.closeStreams();
   }
   */

   private InputStream in_;
   private OutputStream out_;

   //private PropertyResourceBundle resources_;
   //private HashSet keys_;
   private HashMap map_;

   private String charEncoding_;
   private String outputString_;

   /*
   public TemplateParser(String[] args)
      throws IOException
   {
      this(args[0], args[1], args[2]);
   }

   public TemplateParser(String inputFileName, String outputFileName, 
                         String resourceFileName)
      throws IOException
   {
      this(new FileInputStream(inputFileName),
           new FileOutputStream(outputFileName),
           new BufferedInputStream(new FileInputStream(resourceFileName)));

      System.out.println("Reading from '" + inputFileName + "'.");
      System.out.println("Writing to '" + outputFileName + "'.");
   }

   public TemplateParser(InputStream in, OutputStream out,
                         InputStream resources)
      throws IOException
   {
      super();

      in_ = in;
      out_ = out;

      resources_ = new PropertyResourceBundle(resources);
//          (new BufferedInputStream(new FileInputStream(resourceFileName)));
      keys_ = new HashSet();
      Enumeration enum = resources_.getKeys();
      while (enum.hasMoreElements())
         keys_.add(enum.nextElement());
   }
   */

   public TemplateParser(String inputFileName, String outputFileName,
                         HashMap map)
      throws IOException
   {
      this(inputFileName, outputFileName, map, "UTF-8");
   }

   public TemplateParser(String inputFileName, String outputFileName,
                         HashMap map, String charEncoding)
      throws IOException
   {
      this(new FileInputStream(inputFileName),
           new FileOutputStream(outputFileName),
           map,
           charEncoding);

      System.out.println("Reading from '" + inputFileName + "'.");
      System.out.println("Writing to '" + outputFileName + "'.");
   }

   public TemplateParser(InputStream in, OutputStream out,
                         HashMap map)
      throws IOException
   {
      this(in, out, map, "UTF-8");
   }

   public TemplateParser(String inputFileName, HashMap map)
      throws IOException
   {
      this(inputFileName, map, "UTF-8");
   }

   public TemplateParser(String inputFileName, HashMap map, String charEncoding)
      throws IOException
   {
      this(new FileInputStream(inputFileName), map, charEncoding);
   }

   public TemplateParser(InputStream in, HashMap map)
      throws IOException
   {
      this(in, map, "UTF-8");
   }

   public TemplateParser(InputStream in, HashMap map, String charEncoding)
      throws IOException
   {
      this(in, null, map, charEncoding);
   }

   public TemplateParser(InputStream in, OutputStream out,
                         HashMap map, String charEncoding)
      throws IOException
   {
      in_ = in;
      out_ = out;
      map_ = map;
      charEncoding_ = charEncoding;
      outputString_ = null;
   }


   public void doParse() throws IOException
   {
//       BufferedReader in = new BufferedReader(in_);
//       PrintWriter out = new PrintWriter(new BufferedWriter(out_));
//      LineInputStream in = new LineInputStream(new BufferedInputStream(in_));
//      PrintStream out = new PrintStream(new BufferedOutputStream(out_));
      BufferedReader in = new BufferedReader(new InputStreamReader(in_, charEncoding_));
      StringWriter stringOut = null;
      PrintWriter out = null;
      if (out_ != null)
      {
         out = new PrintWriter(new BufferedWriter(new OutputStreamWriter(out_, charEncoding_)));
      }
      else
      {
         stringOut = new StringWriter();
         out = new PrintWriter(stringOut);
      }

      String line;
      String newLine;
      while ((line = in.readLine()) != null)
      {
         String rest = line;
         newLine = "";

         int percentPos = 0;
         int nextPos = -1;

         while ((percentPos = rest.indexOf('%')) >= 0)
         {
            nextPos = rest.indexOf('%', percentPos + 1);
            boolean keyFound = false;
            if (nextPos >= 0)
            {
               String key = rest.substring(percentPos + 1, nextPos);
               //if (keys_.contains(key))
               if (map_.containsKey(key))
               {
                  String left = rest.substring(0, percentPos);
                  //String middle = resources_.getString(key);
                  String middle = (String) map_.get(key);
                  newLine += left + middle;
                  rest = rest.substring(nextPos + 1);
                  keyFound = true;
               }
            }

            if (!keyFound)
            {
               newLine += rest.substring(0, percentPos + 1);
               rest = rest.substring(percentPos + 1);
            }
         }

         newLine += rest;
         out.println(newLine);
      }

//          newLine = line;
//          int percentPos = 0;
//          int nextPos = -1;
//          while ((percentPos = line.indexOf('%', nextPos + 1)) >= 0)
//          {
// //          if (percentPos >= 0)
// //          {
//             nextPos = line.indexOf('%', percentPos + 1);
//             if (nextPos >= 0)
//             {
//                String key = line.substring(percentPos + 1, nextPos);
//                if (keys_.contains(key))
//                {
//                   String left = line.substring(0, percentPos);
//                   String right = line.substring(nextPos + 1);
//                   String middle = resources_.getString(key);

//                   newLine = left+middle+right;
//                }
//             }
//          }
//          out.println(newLine);

      out.flush();

      if (stringOut != null)
         outputString_ = stringOut.toString();
   }

   public String getOutputString()
   {
      return outputString_;
   }

   public void closeStreams()
   {
      try
      {
         out_.flush();
         out_.close();
         in_.close();
      }
      catch (IOException e)
      {
         e.printStackTrace();
      }
   }
}
