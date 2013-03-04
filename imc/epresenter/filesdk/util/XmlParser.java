package imc.epresenter.filesdk.util;

/*
 * File:             XmlParser.java
 * Code conventions: 18.09.2001
 * Author:           Martin Danielsson
 *
 * $Id: XmlParser.java,v 1.10 2007-08-01 14:59:59 kuhn Exp $
 */

import java.io.*;

import imc.lecturnity.util.FileUtils;

/**
 * This class implements a very simple xml file parser. Given a
 * <tt>Reader</tt>, an <tt>InputStream</tt> or a file name, it gives a
 * programmer the possibility to read out the tags one by one. For
 * this purpose, the method {@link #getNextTag() getNextTag} should be
 * used. Depending on the kind of tag or token that was read, this
 * method returns different constants:
 * <ul>
 *   <li><tt>TAG_START</tt> is returned at a start tag,
 * e.g. <tt>&lt;app&gt;</tt>, using {@link #getToken() getToken}, the name of
 * the tag can be retrieved (here <tt>app</tt>). <b>Note:</b> It is
 * not said that this tag does not have any additional properties.</li>
 *   <li><tt>TAG_END</tt> is returned at a closing tag, like
 * <tt>&lt;/app&gt;</tt></li>
 *   <li><tt>PROPERTY</tt> is returned if a tag property of the form
 * <tt>property=value</tt> is found. The name of the property may be
 * retrieved with {@link #getToken() getToken} and the value with
 * {@link #getPropertyOfToken() getPropertyOfToken}.</li>
 *   <li><tt>BOOL_PROPERTY</tt> is returned if a property is found
 * which has no value, like in <tt>&lt;app noresize&gt;</tt> (here
 * <tt>noresize</tt> is a <tt>BOOL_PROPERTY</tt>). The name of the
 * boolean property may be retrieved with the {@link #getToken()
 * getToken} method.</li>
 *   <li><tt>CONTENT</tt> is returned if a word is found which does
 * not belong to a tag in any way.</li>
 *   <li>Finally, <tt>EOF</tt> is returned if the end of the input
 * stream has been reached.</li>
 * </ul>
 *
 * In CONTENT, the backslash character '\' is interpreted as an escape
 * character: any character following a backslash will be interpreted
 * literally. To get a backslash, use '\\'.
 *
 * @see XmlFormatException
 * @author <a href="mailto:martin.danielsson@im-c.de">Martin
 * Danielsson</a>
 */
public class XmlParser 
{
   /** @see XmlParser */
   public static final int EOF           = -1;
   /** @see XmlParser */
   public static final int TAG_START     = 0;
   /** @see XmlParser */
   public static final int TAG_END       = 1;
   /** @see XmlParser */
   public static final int PROPERTY      = 2;
   /** @see XmlParser */
   public static final int BOOL_PROPERTY = 3;
   /** @see XmlParser */
   public static final int CONTENT       = 4;

   private StreamTokenizer tokenizer_;
   private Reader          reader_;

   private boolean isInsideTag_  = false;
   private boolean tokenPending_ = false;
   //    private boolean nextLineAsContent_ = false;
   //    private boolean doNextLineAsContent_ = false;

   private String token_ = "";
   private String propertyOfToken_ = "";
   
   private boolean m_bIsUtf8;

   /**
    * Creates a new <tt>XmlParser</tt> from an input stream.
    * @see XmlParser
    */
   public XmlParser(InputStream in) throws IOException
   {
      // auto-detects and handles utf-8
      Reader reader = FileUtils.createBufferedReader(in, null);
      if (FileUtils.WasUtf8Reader())
         m_bIsUtf8 = true;
    
      init(reader);
   }
  
   /**
    * Creates a new <tt>XmlParser</tt> from a <tt>Reader</tt>. The
    * character encoding parameters are taken from the <tt>Reader</tt>
    * instance and are not altered.
    * @see XmlParser
    */
   public XmlParser(Reader reader)
   {
      init(reader);
   }
   
   public boolean IsUtf8()
   {
      return m_bIsUtf8;
   }
   
   private void init(Reader reader)
   {
      reader_ = reader;

      tokenizer_ = new StreamTokenizer(reader_);
      setCorrectSyntax();
   }

   private void setCorrectSyntax()
   {
      tokenizer_.resetSyntax();
      // tokenizer_.slashSlashComments(false);
      // tokenizer_.slashStarComments(false);
      // tokenizer_.eolIsSignificant(false);
      tokenizer_.whitespaceChars(0, 32);
      // tokenizer_.ordinaryChars('<', '>'); // <, =, >
      // tokenizer_.ordinaryChar('/');
      tokenizer_.quoteChar('"');
      tokenizer_.quoteChar('\'');
      tokenizer_.wordChars('!', '!');
      tokenizer_.wordChars('#', '&');
      tokenizer_.wordChars('(', ';');
      tokenizer_.wordChars('?', '[');
      tokenizer_.wordChars(']', 255);
      //       tokenizer_.eolIsSignificant(true);
   }

   /**
    * Retrieve the next tag in the stream and return its type. The
    * read token can be retrieved using the {@link #getToken()
    * getToken} method.
    *
    * @return the type of the read token
    * @see #EOF
    * @see #TAG_START
    * @see #TAG_END
    * @see #PROPERTY
    * @see #BOOL_PROPERTY
    * @see #CONTENT
    * @see #getToken()
    * @see #getPropertyOfToken()
    * @see XmlParser
    */
   public int getNextTag() throws XmlFormatException, IOException
   {
      int result = EOF;
      boolean isFirstWordInTag = false;
      boolean isWaitingForProperty = false;
      boolean isWaitingForEscChar = false;
      boolean isEndTag = false;
      char expectedChar = 0;

      String tempContent = "";
      boolean isPeekingForMoreContent = false;

      boolean ready = false;
      while (!ready)
      {
         int ttype;
         if (!tokenPending_)
            ttype = tokenizer_.nextToken();
         else
         {
            ttype = tokenizer_.ttype;
            tokenPending_ = false;
         }

         switch (ttype)
         {
            case StreamTokenizer.TT_EOF:
               if (isWaitingForEscChar)
                  throw new XmlFormatException
               ("Expected character (after backslash), got TT_EOF in " +
                "line " + tokenizer_.lineno() + ".");
       
               // were we peeking for more content?
               if (isPeekingForMoreContent)
               {
                  result = CONTENT;
                  setToken(tempContent);
                  isPeekingForMoreContent = false;
                  tokenPending_ = true;
                  ready = true;
                  break;
               }
       
               if (expectedChar != 0)
                  throw new XmlFormatException
               ("Expected character '" + expectedChar + "', got TT_EOF " +
                "in line " + tokenizer_.lineno() + ".");
               if (isWaitingForProperty)
                  throw new XmlFormatException
               ("Expected property, got TT_EOF in line " + 
                tokenizer_.lineno() + ".");
               result = EOF;
               ready = true;
               break;

            case '"':
            case '\'':
            case StreamTokenizer.TT_WORD:
               if (isWaitingForEscChar)
               {
                  if (ttype == '"' || ttype == '\'')
                  {
                     // NOTE: double code (also in case "default:")
                     isWaitingForEscChar = false;
                     tempContent += ((char) ttype);
                     setToken(tempContent);
                     isPeekingForMoreContent = true;
             
                     setCorrectSyntax();
                     tokenizer_.wordChars('/', '/');
                     tokenizer_.wordChars('=', '=');
                     tokenizer_.ordinaryChar(' ');
                     tokenizer_.wordChars(' ', ' ');
             
                     break;
                  }
                  else
                     throw new XmlFormatException
                  ("Expected character (after backslash), got TT_WORD in " +
                   "line " + tokenizer_.lineno() + ".");
               }
       
               if (expectedChar != 0)
               {
                  if (expectedChar == '=')
                  {
                     result = BOOL_PROPERTY;
                     tokenPending_ = true;
                     break;
                  }
                  throw new XmlFormatException
                  ("Expected character '" + expectedChar + "', got TT_WORD" +
                   " in line " + tokenizer_.lineno() + ".");
               }

               if (isFirstWordInTag)
               {
                  if (!isEndTag)
                  {
                     result = TAG_START;
                     setToken(tokenizer_.sval);
                     ready = true;
                  }
                  else
                  {
                     result = TAG_END;
                     setToken(tokenizer_.sval);
                     ready = true;
                     isEndTag = false;
                  }
                  isFirstWordInTag = false;
               }
               else 
               {
                  if (isInsideTag_)
                  {
                     // any property
	       
                     if (!isWaitingForProperty)
                     {
                        setToken(tokenizer_.sval);
                        expectedChar = '=';
                     }
                     else
                     {
                        setPropertyOfToken(tokenizer_.sval);
                        isWaitingForProperty = false;
                        result = PROPERTY;
                        ready = true;
                     }
                  }
                  else
                  {
                     result = CONTENT;
                     tempContent += tokenizer_.sval;
                     isPeekingForMoreContent = true;
                     setToken(tempContent);
                     // ready = true;
                  }
               }
               break;
       
            case StreamTokenizer.TT_NUMBER:
               if (isWaitingForEscChar)
                  throw new XmlFormatException
               ("Expected character (after backslash), got TT_EOF in " +
                "line " + tokenizer_.lineno() + ".");
       
               if (expectedChar != 0)
                  throw new XmlFormatException
               ("Expected character '" + expectedChar + "', got TT_NUMBER " +
                "in line " + tokenizer_.lineno() + ".");
               if (isWaitingForProperty)
                  throw new XmlFormatException
               ("Expected property, got TT_NUMBER in line " + 
                tokenizer_.lineno() + ".");
       
               break;
       
            case StreamTokenizer.TT_EOL:
               if (expectedChar != 0)
                  throw new XmlFormatException
               ("Expected character '" + expectedChar + "', got TT_EOL " + 
                "in line " + tokenizer_.lineno() + ".");
               if (isWaitingForProperty)
                  throw new XmlFormatException
               ("Expected property, got TT_EOL in line " + 
                tokenizer_.lineno() + ".");
       
               break;

            default: // "ordinary char"
               // was last char a backslash?
               if (isWaitingForEscChar)
               {
                  isWaitingForEscChar = false;
                  tempContent += ((char) ttype);
                  setToken(tempContent);
                  isPeekingForMoreContent = true;
          
                  setCorrectSyntax();
                  tokenizer_.wordChars('/', '/');
                  tokenizer_.wordChars('=', '=');
                  tokenizer_.ordinaryChar(' ');
                  tokenizer_.wordChars(' ', ' ');

                  break;
               }

               // were we peeking for more content and ttype is not a backslash?
               if (isPeekingForMoreContent && ttype != '\\')
               {
                  result = CONTENT;
                  setToken(tempContent);
                  isPeekingForMoreContent = false;
                  tokenPending_ = true;
                  ready = true;
                  break;
               }

               if (isWaitingForProperty)
                  throw new XmlFormatException("Expected property, got character '" +
                                               ((char) ttype) + "' in line " + 
                                               tokenizer_.lineno() + ".");
	    
               if (expectedChar == '=' && ((char) ttype) == '>')
               {
                  result = BOOL_PROPERTY;
                  expectedChar = 0;
                  ready = true;
               }

               if (expectedChar != 0 && 
                   expectedChar != ((char) ttype))
                  throw new XmlFormatException
               ("Expected character '" + expectedChar + "', got '" +
                ((char) ttype) + "' in line " + tokenizer_.lineno());
       
               switch (ttype)
               {
                  case '<': // open tag
                     isInsideTag_ = true;
                     isFirstWordInTag = true;
                     tokenizer_.ordinaryChar('/');
                     tokenizer_.ordinaryChar('=');
                     tokenizer_.ordinaryChar(' ');
                     tokenizer_.whitespaceChars(' ', ' ');
                     break;
	       
                  case '=': // property
                     expectedChar = 0;
                     isWaitingForProperty = true;
                     break;

                  case '>': // end tag
                     isInsideTag_ = false;

                     tokenizer_.wordChars('/', '/');
                     tokenizer_.wordChars('=', '=');
                     tokenizer_.ordinaryChar(' ');
                     tokenizer_.wordChars(' ', ' ');

                     break;

                  case '/': // end of tag group?
                     if (isInsideTag_)
                        isEndTag = true;
                     break;

                  case '\\': // escape character
                     isWaitingForEscChar = true;
                     // make all chars ordinary.
                     tokenizer_.resetSyntax();
                     break;

                  default:
                     throw new XmlFormatException("Received unknown ordinary character '" + 
                                                  ((char) ttype) + "' (#" + ttype + ") " +
                                                  "in line " + tokenizer_.lineno() + ".");
               }
               break;
         }
      }
      return result;
   }

   private void setToken(String token)
   {
      token_ = token;
   }

   /**
    * Returns the last read token. After initialisation, this method
    * will return an empty <tt>String</tt> object.
    * 
    * @see #getNextTag()
    * @see XmlParser
    */
   public String getToken()
   {
      return token_;
   }

   private void setPropertyOfToken(String propertyOfToken)
   {
      propertyOfToken_ = propertyOfToken;
   }

   /**
    * If the {@link #getNextTag() getNextTag} returns {@link
    * #PROPERTY PROPERTY}, the value of the property may be retrieved
    * using this method.
    *
    * @return the value of the property in {@link #getToken()
    * getToken}
    * @see #getToken()
    * @see #getNextTag()
    * @see #PROPERTY
    * @see XmlParser
    */
   public String getPropertyOfToken() 
   {
      return propertyOfToken_;
   }

   //    public void returnNextLineAsContent()
   //    {
   //       System.out.println("returnNextLineAsContent()");
   //       System.out.flush();
   //       nextLineAsContent_ = true;
   //    }
}
