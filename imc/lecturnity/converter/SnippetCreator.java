package imc.lecturnity.converter;

import java.util.Vector;

import imc.epresenter.filesdk.Metadata;
import imc.lecturnity.util.NativeUtils;

public class SnippetCreator
{
   private Vector snippets_;

   public SnippetCreator()
   {
      super();

      snippets_ = new Vector();
   }

   public Snippet[] getSnippets()
   {
      Snippet[] snippets = new Snippet[snippets_.size()];
      snippets_.copyInto(snippets);
      return snippets;
   }

   public void dumpSnippets()
   {
      for (int i=0; i<snippets_.size(); ++i)
      {
         Snippet snippet = (Snippet) snippets_.elementAt(i);
         System.out.println(snippet.keyword + "='" + snippet.replace + "'");
      }
   }

   protected void addSnippet(String keyword, String replace)
   {
      addSnippet(new Snippet(keyword, replace));
   }

   protected void addSnippet(Snippet snippet)
   {
      snippets_.addElement(snippet);
   }

   protected String msToTime(long msTime)
   {
      long seconds = msTime / 1000;
      long hours = seconds / 3600;
      seconds = seconds - hours * 3600;
      long minutes = seconds / 60;
      seconds = seconds - minutes * 60;
      
      StringBuffer tmp = new StringBuffer();
      tmp.append("" + hours).append(":");
      if (minutes < 10)
         tmp.append("0");
      tmp.append("" + minutes).append(":");
      if (seconds < 10)
         tmp.append("0");
      tmp.append("" + seconds);
      
      return tmp.toString();
   }

   protected String truncateTo(String s, int maxLen)
   {
      String tmp = s;
      if (s.length() > maxLen)
         tmp = s.substring(0, maxLen - 3) + "...";
      return tmp;
   }
   
   protected String getMetadataKeywords(Metadata metadata)
   {
      StringBuffer buffer = new StringBuffer(1024);
      
      buffer.append("imc, lecturnity, version: " + NativeUtils.getVersionStringShortShort());
      if (metadata != null)
      {
         for (int i = 0; i < metadata.keywords.length; ++i)
         {
            buffer.append(", ");
            buffer.append(metadata.keywords[i]);
         }
      }
      
      return buffer.toString();
   }

   /***************************************************************************/

   /**
    * Check a string for HTML specific and/or special characters and replace them.
    * 
    *    @param string strText: The String to be checked.
    *    @return string: the String with special chararacters replaced by HTML names
    */
   protected String checkHtmlString(String strText)
   {
      // Debug info
      //System.out.println(strText);

      if (strText.length() == 0)
         return strText;

      // Little "hack": A '"' at the end of the string is not detected correctly, 
      // so we append a whitespace, which we remove after the conversion
      String strHtmlText = strText + " ";
      
      for (int i = (strText.length()-1); i >= 0; --i)
      {
         char charTmp = strText.charAt(i);
         int nTmp = (int)(charTmp);
         //System.out.print(nTmp + " ");

         // HTML specific characters
         switch(nTmp)
         {
            case 34: // '"'
               strHtmlText = replaceCharAtPosWithHtmlString(strHtmlText, i, "&quot;");
               break;
            case 38: // '&'
               strHtmlText = replaceCharAtPosWithHtmlString(strHtmlText, i, "&amp;");
               break;
            case 60: // '<'
               strHtmlText = replaceCharAtPosWithHtmlString(strHtmlText, i, "&lt;");
               break;
            case 62: // '>'
               strHtmlText = replaceCharAtPosWithHtmlString(strHtmlText, i, "&gt;");
               break;
            default:
               // do nothing
         }
         
         // Not needed anymore from 1.7.0.p3 on: UTF-8 is used
         // characters with ASCII code >= 128
         //if (nTmp >= 128)
         //{
         //   String strHtml = "&#" + nTmp + ";";
         //   strHtmlText = replaceCharAtPosWithHtmlString(strHtmlText, i, strHtml);
         //}
      }
      //System.out.println("");
      
      // Remove the 'extra' whitespace
      strHtmlText = strHtmlText.substring(0, strHtmlText.length()-1);

      // Debug info
      //System.out.println(strHtmlText);

      return strHtmlText;
   }
   
   /***************************************************************************/

   /**
    * Remove a character from a string at a specific position and replace it with HTML syntax.
    * 
    *    @param String strText: The input string.
    *    @param int pos: The position where to remove one character. 
    *                    If pos exceeds the length of the string, nothing is removed.
    *    @param String strHtml: The string with HTML syntax.
    *    @return String: the string with the specified character removed.
    */
   protected String replaceCharAtPosWithHtmlString(String strText, int pos, String strHtml)
   {
      if ( (pos < 0) || (pos >= strText.length()-1) )
         return strText;

      //System.out.println(strHtml);
      StringBuffer strNew = new StringBuffer("");
      strNew.append(strText.substring(0, pos));
      strNew.append(strHtml);
      strNew.append(strText.substring(pos+1)); 
      
      return strNew.toString();
   }
}
