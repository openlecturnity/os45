package imc.epresenter.player.util;

import java.util.*;

/** Used to parse lines of the type 'key1=value1 key2=value2 ...' */
public class SGMLParser {
	private Hashtable entries = new Hashtable();
	private static String delimiters = " \">=\\";

	private String input;
	
   public SGMLParser(String toDo) 
   {
      this(toDo, false);
   }
   
	public SGMLParser(String toDo, boolean bRemoveEscape) 
   {
		input = toDo;

		StringTokenizer st = new StringTokenizer(toDo, delimiters, true);
		st.nextToken(); // ommit component name
		st.nextToken(); // ommit " " after component name
		String tok = null;
      while(st.hasMoreTokens())
      {
         // parse a key=value portion
         
			tok = st.nextToken();
			String key = tok;
			tok = st.nextToken();
			if (!tok.equals("=")) 
         {
            System.err.println("SGMLParser: something went wrong (tok="+tok+")!");
            System.err.println("  "+input);
         }
			String value = st.nextToken();
			if (value.equals("\"")) 
         {
            // there can different sorts of escaped characters
            boolean bLastEscape = false;
		
				value = "";
            do
            {
               tok = st.nextToken();
               if (tok.equals("\"") && !bLastEscape)
                  break;
               else
                  bLastEscape = tok.equals("\\");
               
               value += tok;
            }
            while (st.hasMoreTokens()); // break condition just for safety
             
            if (bRemoveEscape && value.indexOf('\\') > -1)
            {
               bLastEscape = false;
               
               StringBuffer sb = new StringBuffer();
               for (int i=0; i<value.length(); ++i)
               {
                  char c = value.charAt(i);
       
                  if (!bLastEscape)
                  {
                     if (c == '\\')
                        bLastEscape = true;
                     else
                        sb.append(c);
                  }
                  else
                     sb.append(c);   
               }
               
               value = sb.toString();
            }
			}
			entries.put(key, value);
			tok = st.nextToken(); // normally " "
			if (tok.equals(">")) break;
		}
	}
	
	public String getValue(String key) {
		return (String)entries.get(key);
	}

    public String toString() {
		return input;
    }

	public static void main(String[] args) {
		System.out.println(args[0]);
		SGMLParser test = new SGMLParser(args[0]);
		System.out.println(test.entries);
	}
}
