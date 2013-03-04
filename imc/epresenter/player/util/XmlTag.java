package imc.epresenter.player.util;

import java.util.*;
import java.io.*;

import imc.lecturnity.util.FileUtils;

public class XmlTag {
   private String label;
   private Vector content;
   private boolean m_bIsUtf8 = false; // copied during init
   
   /**
    * Konstruktot für allgemeine Tags, denen noch beliebige andere Objekte
    * (meist weitere Tags) hinzugefuegt werden koennen.
    */
   public XmlTag(String s, boolean bIsUtf8) {
      this(s,null,bIsUtf8);
   }
   
   /**
    * Konstruktor fuer 'primitive' Tags, die nur ein einziges Content-Objekt
    * enthalten.
    */
   public XmlTag(String s, Object o, boolean bIsUtf8) {
      label = s;
      
      content = new Vector(4,12);
      if (o!=null) content.addElement(o);
      
      m_bIsUtf8 = bIsUtf8;
   }
   
   public boolean IsUtf8()
   {
      return m_bIsUtf8;
   }
   
   public void addContent(Object o) {
      content.addElement(o);
   }
   public void addContent(Object[] os) {
      // grumpf!
      for (int i=0; i<os.length; i++) content.addElement(os[i]);
   }
   
   public String getLabel() {
      return label;
   }
   
   /**
    * Searches for this labelname in this tag and all possible subtags and
    * if found returns its content objects.
    */
   public Object[] getValues(String name) {
      // return this content
      if (name.equals(label)) {
         Object[] os = new Object[content.size()];
         content.copyInto(os);
         return os;
      }
      
      // or search in all subtags and store all hits
      ArraySaver saver = new ArraySaver();
      for (int i=0; i<content.size(); i++) {
         try {
            XmlTag inner = (XmlTag)content.elementAt(i);
            Object[] result = inner.getValues(name);
            if (result != null) saver.addElements(result);
         } catch (ClassCastException e) {}
      }
      
      Object[] os = saver.getElements();
      if (os.length > 0) return os;
      else return null; // no match found
   }
   
   /**
    * Returns the labels of the first level of inner <code>XmlTag</code>s.
    * Only for <code>DublinCore</code> <code>XmlTag</code>s.
    */
   protected String[] getContentLabels() {
      ArraySaver saver = new ArraySaver();
      for (int i=0; i<content.size(); i++) {
         try {
            XmlTag inner = (XmlTag)content.elementAt(i);
            saver.addElement(inner.getLabel());
         } catch (ClassCastException e) {}
      }
      
      // convert to String array
      Object[] eles = saver.getElements();
      String[] names = new String[saver.size()];
      for (int i=0; i<names.length; i++) {
         names[i] = (String)eles[i];
      }
      
      return names;
   }
   
   public String toString() {
      StringBuffer result = new StringBuffer();
      result.append("<"+label+">");
      Enumeration e = content.elements();
      while(e.hasMoreElements()) {
         result.append(e.nextElement().toString());
      }
      result.append("</"+label+">");
      return result.toString();
   }
   
   //public static XmlTag[] parse(String string) {
   //   return parse(new StringBuffer(string));
   //}
   
   public static XmlTag[] parse(InputStream input) 
   {
      StringBuffer fileBuffer = new StringBuffer();
      
      boolean bIsUtf8 = false;
      try
      {
         // auto-detects and handles utf-8
         BufferedReader reader = FileUtils.createBufferedReader(input, null);
         bIsUtf8 = FileUtils.WasUtf8Reader();
         
         while (reader.ready()) 
            fileBuffer.append(reader.readLine());
        
      } catch (IOException ioe) {
         ioe.printStackTrace();
      } catch (NullPointerException e) {}
      
      return parse(fileBuffer, bIsUtf8);
   }
   
   private static XmlTag[] parse(StringBuffer buffer, boolean bIsUtf8) 
   {
      return parse(buffer, 0, buffer.length(), bIsUtf8);
   }
   
   private static XmlTag[] parse(StringBuffer buffer, int start, int end, boolean bIsUtf8) 
   {
      // count tags
      int tagcount = countTags(buffer, start, end);
      if (tagcount==0) System.err.println("XmlTag.parse(): No tags found!");
      
            // hm wird zweimal geparst
            // wenigstens die Endindizes sollten gespeichert werden
            // \n müssen aus Tagname und Contents entfernt werden
      
      // make new array
      XmlTag[] tags = new XmlTag[tagcount];
      
      // parse the content, maybe many subtags
      int index = start;
      for (int i=0; i<tags.length; i++) {
         // parse label
         String name = parseLabel(buffer, index, end);
         int tagbegin = buffer.toString().indexOf("<"+name+">", index);
         
         XmlTag tag = new XmlTag(name, bIsUtf8);
         int tagend = findTagEnd(buffer, name, index);
         
         // parse content
         int contentstart = tagbegin+name.length()+2;
         int contentend = tagend-name.length()-3;
         int subtagcount = countTags(buffer, contentstart, contentend);
         if (subtagcount == 0)
         {
            String realContent = substring(buffer, contentstart, contentend, true);
            tag.addContent(realContent);
            // possibly substring().trim() makes sense?
         }
         else {
            tag.addContent(XmlTag.parse(buffer, contentstart, contentend, bIsUtf8));
         }
         
         tags[i] = tag;
         
         index = tagend+1;
      }
      
      return tags;
   }
   
   /**
    * Counts all hierarchical equal tags from the first level (tags and not their subtags).
    */
   private static int countTags(StringBuffer buffer, int start,  int end) {
      int tagcount = 0;
      int index = start;
      while(index < end) {
         String name = parseLabel(buffer, index, end);
         if (name != null) {
            index = findTagEnd(buffer, name, index)+1;
            tagcount++;
         } else break;
      }
      return tagcount;
   }
   
   private static String parseLabel(StringBuffer buffer, int start, int end) {
      int begin = -1;
      int stop = -1;
      for (int i=start; i<end; i++) {
         if (buffer.charAt(i) == '\\') { i++; continue; }
         if (buffer.charAt(i) == '<') begin = i;
         if (buffer.charAt(i) == '>') stop = i;
         
         if (begin != -1 && stop != -1 && stop-begin>1) {
            String result = substring(buffer, begin+1, stop-1, false);
            return result;
         }
      }
      
      // kein Tag gefunden
      return null;
   }
   
   // Fehler! FALLS <tag><tag></tag></tag> so wird immer erstes zurückgegeben!
   private static int findTagEnd(StringBuffer buffer, String tagname, int tagbegin) {
      // search for endtag
      int maybebegin = -1;
      int begin = -1;
      int end = -1;
      for (int i=tagbegin; i<buffer.length(); i++) {
         if (buffer.charAt(i) == '<') maybebegin = i;
         if (buffer.charAt(i) == '/' && maybebegin == i-1) begin = i; 
         if (buffer.charAt(i) == '>' && begin != -1) end = i;
         
         // das finden von / ist wichtig und muss sichergestellt werden -> maybebegin
         
         if (begin != -1 && end != -1 && end-begin>1) { // irgendeinen Tag gefunden
            if (tagname.equals(substring(buffer, begin+1, end-1, false))) return end;
            else { begin=-1; end=-1; } // found another tag, reseting
         }
      }
      
      // FEHLER! Tagende nicht gefunden
      return -1;
   }
   
   private static String substring(StringBuffer buffer, int start, int end, boolean deshlashify) {
      char[] cs = new char[end-start+1];
      buffer.getChars(start, end+1, cs, 0);
      
      if (deshlashify)
      {
         // buffer might contain '\'; especially in file names with strange
         // symbols: then remove (correctly) some or all of em
         int slashCount = 0;
         for (int i=0; i<cs.length; i++)
            if (cs[i] == '\\' && (i == 0 || cs[i-1] != '\\'))
               slashCount++;
         
         if (slashCount > 0)
         {
            char[] csNew = new char[cs.length-slashCount];
            int counterNew = 0;
            for (int i=0; i<cs.length; i++)
            {
               if (cs[i] == '\\' && (i == 0 || cs[i-1] != '\\'))
                  ; // do not copy; this is a "protecting" backslash
               else
                  csNew[counterNew++] = cs[i];
            }
            
            cs = csNew;
         }
      }
      
      return new String(cs);
   }
   
   public class ArraySaver {
      private Object[] content = new Object[0];
      
      public Object[] getElements() { return content; }
      public void addElement(Object o) {
         Object[] os = {o};
         addElements(os);
      }
      public void addElements(Object[] os) {
         if (content.length==0) content=os;
         else {
            Object[] newcontent = new Object[content.length+os.length];
            System.arraycopy(content, 0, newcontent, 0, content.length);
            System.arraycopy(os, 0, newcontent, content.length, os.length);
            content = newcontent;
         }
      }
      public int size() {
         return content.length;
      }
   }
}
