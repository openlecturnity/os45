package imc.epresenter.player.whiteboard;

import java.awt.Graphics;
import java.awt.Point;
import java.awt.Rectangle;
import java.io.BufferedReader;
import java.util.ArrayList;
import java.util.StringTokenizer;

import imc.epresenter.player.util.SGMLParser;

public class InteractionArea extends OutlineRectangle
{
   public static final int ACTION_MOUSE_CLICK = 1;
   public static final int ACTION_MOUSE_DOWN = 2;
   public static final int ACTION_MOUSE_UP = 3;
   public static final int ACTION_MOUSE_ENTER = 4;
   public static final int ACTION_MOUSE_LEAVE = 5;
   
   protected boolean m_bIsActivated = false;
   
   private boolean m_bIsLongtermDisabled = false;
   
   private String m_strClickAction = null;
   private String m_strDownAction = null;
   private String m_strUpAction = null;
   private String m_strEnterAction = null;
   private String m_strLeaveAction = null;
   
   private boolean m_bHandCursor = false;

   
   
	public InteractionArea(String strLine, BufferedReader in)
   {
      super(strLine, in);
      
		SGMLParser parser = new SGMLParser(strLine);
      
      // read action definitions
      m_strClickAction = parser.getValue("onClick");
      m_strDownAction = parser.getValue("onDown");
      m_strUpAction = parser.getValue("onUp");
      m_strEnterAction = parser.getValue("onEnter");
      m_strLeaveAction = parser.getValue("onLeave");
      
      // these actions should remain null if they are undefined or empty
      if (m_strClickAction != null && m_strClickAction.length() == 0)
         m_strClickAction = null;
      if (m_strDownAction != null && m_strDownAction.length() == 0)
         m_strDownAction = null;
      if (m_strUpAction != null && m_strUpAction.length() == 0)
         m_strUpAction = null;
      if (m_strEnterAction != null && m_strEnterAction.length() == 0)
         m_strEnterAction = null;
      if (m_strLeaveAction != null && m_strLeaveAction.length() == 0)
         m_strLeaveAction = null;
      
      String strCursor = parser.getValue("cursor");
      if (strCursor != null && strCursor.equals("link"))
         m_bHandCursor = true;
      
      super.ParseActivityVisibility(parser);
   }
   
   void paint(Graphics g, int cwidth, int cheight, double scale)
   {
      // do nothing 
      // this object is interactive but invisible
   }
   
   public boolean HasHandCursor()
   {
      return m_bHandCursor;
   }
   
   /**
    * @returns true if the state was (had to be) changed.
    */
   public boolean SetActive(boolean bYes)
   {
      if (m_bIsActivated != bYes)
      {
         m_bIsActivated = bYes;
         return true;
      }
      else
         return false;
   }
   
   public boolean DisableForever()
   {
      boolean bChange = SetActive(false);
      m_bIsLongtermDisabled = true;
      return bChange;
   }
   
   boolean IsActiveAt(int iTime)
   {
      if (!m_bIsLongtermDisabled)
         return super.IsActiveAt(iTime);
      else
         return false;
   }
   
   public Rectangle GetActivityArea()
   {
      return m_rcSize.getBounds();
   }
   
   /**
    * @returns true if this actions region was matched.
    */
   public boolean HitTest(int x, int y)
   {
      return x >= m_rcSize.x && x < m_rcSize.x + m_rcSize.width && y >= m_rcSize.y && y < m_rcSize.y + m_rcSize.height;
   }
   
   /**
    * @returns a String defining the action to execute. Or null if 
    *          either the region was not matched or no such action type
    *          was defined.
    */
   public String HitTestAction(int x, int y, int iActionType)
   {
      if (HitTest(x, y))
         return GetActionString(iActionType);
      
      return null;
   }
   
   public String GetActionString(int iActionType)
   {
      switch(iActionType)
      {
         case ACTION_MOUSE_CLICK:
            return m_strClickAction;
         case ACTION_MOUSE_DOWN:
            return m_strDownAction;
         case ACTION_MOUSE_UP:
            return m_strUpAction;
         case ACTION_MOUSE_ENTER:
            return m_strEnterAction;
         case ACTION_MOUSE_LEAVE:
            return m_strLeaveAction;
         default:
            System.err.println("Action type not recognized: "+iActionType);
      }
     
      return null;
   }
   
   public boolean IsQuestionObject()
   {
      return false;
   }
 
   public String toString()
   {
      return "AREA "+m_rcSize.x+","+m_rcSize.y+" "+m_rcSize.width+"x"+m_rcSize.height;
   }

}	