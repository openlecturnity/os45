package imc.epresenter.player.whiteboard;

import java.awt.*;
import java.awt.event.*;
import java.awt.print.*;
import java.io.*;
import java.net.*;
import java.util.*;
import javax.swing.*;
import javax.swing.border.*;

import imc.epresenter.filesdk.Feedback;
import imc.epresenter.filesdk.FileResources;
import imc.epresenter.filesdk.Question;
import imc.epresenter.filesdk.Questionnaire;
import imc.epresenter.player.*;
import imc.epresenter.player.master.TestEngine;
import imc.epresenter.player.util.*;
import imc.lecturnity.util.ui.LayoutUtils;
					 
/** Reads the evq and obj files and displays the appropriate objects for a given time. */
public class WhiteBoardPlayer implements SRStreamPlayer, Timeable
{	
	private ObjectQueue m_ObjectQueue;
	private EventQueue m_EventQueue;
   private FileResources m_Resources;
	private int m_iCurrentTime;
   private int m_iLectureLength;
   private int[] m_aiPageChanges;
	private Board m_Board;
   private Dimension m_dimBoardSize;
	private Border m_ButtonBorder;
	private JPanel m_VisualComponent;
	private Coordinator m_Coordinator;
   private boolean m_bConsoleOperation;
   private TestEngine m_TestEngine = null;
   private TimeSource m_Timer = null;
   private boolean m_bStarted = false;
   
   private boolean m_structuredClips = false;
   
   private boolean m_bHasRandomTest = false;
   private int m_iJumpRandomTimestamp = -1;
	
	public WhiteBoardPlayer()
   {
      this(false);
   }
   
   public WhiteBoardPlayer(boolean bConsoleOperation)
   {
      m_bConsoleOperation = bConsoleOperation;
      
      if (!m_bConsoleOperation)
         m_VisualComponent = new JPanel(new BorderLayout());
	}
   
   public void setStartOffset(int millis)
   {
   }
   
	public void init(FileResources resources, String[] args, Coordinator c)
   {
	   m_Coordinator = c;
      m_Resources = resources;

		String events = null, objects = null;
		int width = -1, height = -1;
      float zoomFactor = 1.0f;
      String strCodepage = "cp1252";
		InputStream objectInputStream = null, eventInputStream = null;

		for (int j=0; j<args.length; j++)
      {
			String opt = (String)args[j];
			if (opt.indexOf(".obj") != -1)
         {
				objects = opt;
			}
         else if (opt.indexOf(".evq") != -1)
         {
				events = opt;
			}
         else if (opt.indexOf("x") != -1)
         {
				try
            {
					width = Integer.parseInt(
						opt.substring(0,opt.indexOf("x")));
					height = Integer.parseInt(
						opt.substring(opt.indexOf("x")+1));
				}
            catch (NumberFormatException nfe)
            {
					nfe.printStackTrace();
				}
			}
         else if (opt.equals("-Z"))
         {
            j++;
            opt = (String)args[j];
            int zoom = Integer.parseInt(opt);
            zoomFactor = zoom/100.0f;
         }
         else if (opt.equals("-cp"))
         {
            j++;
            strCodepage = args[j];
         }
         else if (opt.equals("-structuredClips"))
         {
            // PZI: disable painting of whiteboard behind structured clips to avoid parts of wb to become visible
            m_structuredClips = true;
         }
		}
		
      m_dimBoardSize = new Dimension(width, height);
      
		if (events != null && objects != null)
      {
			try
         {
				objectInputStream = new BufferedInputStream(
					resources.createInputStream(objects));
				eventInputStream = new BufferedInputStream(
					resources.createInputStream(events));
			}
         catch (IOException ioe)
         {
				ioe.printStackTrace();
			}
			
      }
      else
      {
			Manager.showError("WhiteBoardPlayer: "+Manager.getLocalized("eveobjFail"), Manager.ERROR, null);
		}
	
      try
      {
         m_ObjectQueue = new ObjectQueue(objectInputStream, strCodepage, resources, m_bConsoleOperation);
         m_ObjectQueue.startParsing(null);
         m_EventQueue = new EventQueue(eventInputStream, m_ObjectQueue);
			m_EventQueue.startParsing(null);
      }
      catch (Exception e)
      {
         e.printStackTrace();
         throw new RuntimeException("Unable to init WhiteBoardPlayer ("+e+").");
      }
      finally
      {
         try
         {
            objectInputStream.close();
            eventInputStream.close();
         }
         catch (IOException exc)
         {
         }
      }
      
      if (!m_bConsoleOperation)
      {
         if (width == -1 || height == -1)
            m_Board = new Board();
         else
            m_Board = new Board(width, height, zoomFactor);
		
         m_VisualComponent = m_Board;
         m_VisualComponent.setOpaque(true);
         
         m_Board.setLayout(null); // dynamics are placed absolutely
         
         
         ButtonGroup group = null;
         // Note: you probably don't need to store these group objects
         // as they are bound to the added objects with their listeners.
         // And as long as the added objects are not set to null also the
         // group object lives on.
         DynamicComponent dcLast = null;
         
         ArrayList alDynamics = new ArrayList(40);
         m_ObjectQueue.GetDynamics(alDynamics);
         for (int i=0; i<alDynamics.size(); ++i)
         {
            DynamicComponent dc = (DynamicComponent)alDynamics.get(i);
            m_Board.AddNative(dc.GetNative(), dc.GetActivityArea());
                  
            if (dc instanceof RadioDynamic && !((RadioDynamic)dc).IsCheckBox())
            {
               if (group == null || dcLast != null && !dcLast.EqualsActivity(dc))
               {
                  group = new ButtonGroup();
                  JRadioButton btnDummy = new JRadioButton();
                  btnDummy.setVisible(false);
                  group.add(btnDummy); // invisible, empty, can be selected to "reset"
               }
                     
               dc.AddRadioTo(group);
               dcLast = dc;
            }
               
         }
         
         Questionnaire[] aQuestions = m_ObjectQueue.GetQuestionnaires(true);
         if (aQuestions != null && aQuestions.length > 0)
         {
            m_TestEngine = new TestEngine(aQuestions, m_Coordinator, this);
            
            if (m_TestEngine.IsTestRandom())
            {
            	//System.out.println("!!!!!!!!!! e test random baaaaaa");
            	m_bHasRandomTest = true;
            	m_iJumpRandomTimestamp = m_TestEngine.GetFirstRandomQuestionMs();
            	
            }
            
            // TODO limit timer and repaint to changeable question texts
            m_Timer = new TimeSource(this, 500);
            m_Timer.start();
         }
      }


      //if (!m_bConsoleOperation)
      //   m_Board.setBackground(m_EventQueue.getFirstBackgroundColor());
      
      //m_aiPageChanges = m_EventQueue.pageChangeTimes();
      m_aiPageChanges = m_EventQueue.getAllPageChangeTimes();

	} // init

   public ObjectQueue getObjectQueue()
   {
      return m_ObjectQueue;
   }

   public EventQueue getEventQueue()
   {
      return m_EventQueue;
   }


	
	public void enableVisualComponents(boolean bEnable)
   {
      // not implemented here
   }

	public Component[] getVisualComponents()
   {
			return new Component[] { m_VisualComponent };
	}
   
   public Dimension GetBoardSize()
   {
      return m_dimBoardSize;
   }

	public String getTitle()
   {
		return "WhiteBoardPlayer";
	}	

	public String getDescription()
   {
		return "Replay of a whiteboard stream.";
	}

	public void nextPage()
   {
		int newTime = m_iLectureLength;
      
      // first look for the page we are on (double code)
      int idx = 0;
      for (int i=m_aiPageChanges.length-1; i>-1; i--)
      {
         if (m_aiPageChanges[i] <= m_iCurrentTime)
         {
            idx = i;
            break;
         }
      }

      newTime = idx < m_aiPageChanges.length-1 ? m_aiPageChanges[idx+1] : m_iLectureLength;

      if (newTime != m_iCurrentTime)
         m_Coordinator.requestTime(newTime, this);
	}
	
	public void previousPage(boolean bRealPrevious)
   {
      int newTime = 0;

      // first look for the page we are on (double code)
      int idx = 0;
      for (int i=m_aiPageChanges.length-1; i>-1; i--)
      {
         if (m_aiPageChanges[i] <= m_iCurrentTime)
         {
            idx = i;
            break;
         }
      }

      // now do a cd-player-like jump
      if (m_iCurrentTime-m_aiPageChanges[idx] > 2000 && !bRealPrevious)
      {
         newTime = m_aiPageChanges[idx];
      }
      else
      {
         newTime = m_aiPageChanges[idx > 0 ? idx-1 : idx];
      }
      
      if (newTime != m_iCurrentTime)
         m_Coordinator.requestTime(newTime, this);
	}
	
	public int GetPreviousPageStartTimestamp(int iTimestamp)
	{
		int newTime = 0;

        // first look for the page we are on (double code)
        int idx = 0;
        for (int i=m_aiPageChanges.length-1; i>-1; i--)
        {
           if (m_aiPageChanges[i] <= iTimestamp)
           {
              idx = i;
              break;
           }
        }
        
        newTime = m_aiPageChanges[idx > 0 ? idx - 1 : idx];
        return newTime;
	}

   public void tickTime()
   {
      if (!m_bStarted)
         setMediaTime(m_iCurrentTime, null);
   }

	
	// Interface SRStreamPlayer
	public void setMediaTime(int time, EventInfo info)
   {
   	
   //	System.out.print("my time = " + time + "\r");
   		int iPrevTime = m_iCurrentTime;
      m_iCurrentTime = time;
      if(m_bHasRandomTest)
      {
      		if(m_TestEngine.IsQuestionOnFirstPage())
            {
            	m_TestEngine.ResetBeforeFirstQuestionTimestamp();
            	m_TestEngine.ResetIsQuestionOnFirstPage();
            	ExecuteJumpToCurrentRandomQuestion();
            	return;
            	
            }

      		if(IsJumpToCurrentRandomQuestion(iPrevTime, m_iCurrentTime))
      		{
      			ExecuteJumpToCurrentRandomQuestion();
      			return;
      		}
      }
      
      int iQuestionTimeLeft = -2;
      int iQuestionTriesLeft = -2;
      int iQuestionNumber = 0;
      int iQuestionTotal = 0;
      if (m_TestEngine != null)
      {
         // determine values for changeable texts
         
         Question q = m_TestEngine.FindQuestionAt(time);
         
         if (q != null)
         {
            // TODO this is rather much processing (and rather often)
            // for rather stable and rather few information.
            
            // remaining time can be positive and rather small
            // or arbitrarily large negative
         
            long lQuestionTimeLeft = m_TestEngine.UpdateQuestionTimerAt(time);
            if (lQuestionTimeLeft >= 0)
               iQuestionTimeLeft = (int)lQuestionTimeLeft;
            else
               iQuestionTimeLeft = -1;
         
            if (q.GetMaximumAttempts() > 0)
               iQuestionTriesLeft = q.GetMaximumAttempts() - q.GetAnswerAttempts();
  
            Questionnaire qq = q.GetQuestionnaire();
            if (qq != null)
            {
               Question[] qs = qq.GetQuestions();
               iQuestionTotal = qs.length;
               for (int i=0; i<iQuestionTotal; ++i)
               {
                  if (qs[i] == q)
                  {
                     iQuestionNumber = i;
                     break;
                  }
               }
            }
         }
      }
      
      Rectangle rcText = null, rcMoveOrButton = null, rcInteractions = null, rcDynamics = null;
      rcText = m_ObjectQueue.UpdateChangeableTextsAt(time, iQuestionTriesLeft, iQuestionTimeLeft,
                                                     iQuestionNumber, iQuestionTotal);
      if (!m_Board.IsFeedbackActive())
      {
         rcMoveOrButton = m_ObjectQueue.ShowHideInteractives(time);
         rcInteractions = m_ObjectQueue.ActivateInteractions(time);
         rcDynamics = m_ObjectQueue.ShowHideActivateDynamics(time);
      }
           
		double dScale = m_Board.CalcScaleFactor();
      // PZI: disable painting of whiteboard behind structured clips to avoid parts of wb to become visible
		// do not calculate clipping for structured clips to avoid painting of whiteboard 
      Rectangle rcObjects = new Rectangle(0,0,0,0);
      if(!m_structuredClips)
          rcObjects = m_EventQueue.getClip(m_iCurrentTime, 1.0);
      // "1.0" instead of "dScale": scaling is done in RepaintArea().
      
      // TODO what about scaling?
      // TODO what about offsets?
      
      if (rcText != null)
         rcObjects.add(rcText);
      
      if (rcMoveOrButton != null)
         rcObjects.add(rcMoveOrButton);
      
      if (rcInteractions != null)
         rcObjects.add(rcInteractions);
      
      if (rcDynamics != null)
         rcObjects.add(rcDynamics);
      
      if (rcObjects.width > 0)
         m_Board.RepaintArea(rcObjects);
      
      // TODO if a ButtonArea becomes visible or hidden it might
      // have not the right activation state.
      // Example: mouse over button, replay continues button disappears,
      // mouse is moved (out of the buttons area), replay time jumps back,
      // button is again visible (and looks as if the mouse is over it).
      
   }
	
	
	public void start(EventInfo info) 
   { 
      m_bStarted = true;
   }
	
   public void pause(EventInfo info) 
   {
      m_bStarted = false;
   }
	
   public void stop(EventInfo info) 
   {
      m_bStarted = false;
   }
	
	
	public int getMediaTime()
   {
      return m_iCurrentTime;
   }
   
	public int getDuration()
   {
      return m_EventQueue.getDuration();
   }

	public void setDuration(int millis)
   {
      m_iLectureLength = millis;
	}

	public void setInfo(String key, String value) { }
	
   
   public void close(EventInfo info) 
   {
      if (m_Timer != null)
         m_Timer.stop();
      m_Timer = null;
      
      m_Coordinator = null;
   }
	
	private ImageIcon createIcon(String location)
   {
		URL loc = getClass().getResource(location);
		return new ImageIcon(loc);
	}
		
	private JButton createButton(String location)
   {
		ImageIcon icon = createIcon(location);
		if (m_ButtonBorder == null)
         m_ButtonBorder = BorderFactory.createEmptyBorder(2,2,2,2);
		
		JButton but = new JButton(icon);
		but.setBorder(m_ButtonBorder);
		but.setOpaque(false);
		
		return but;
	}
   
   public void FullScreenMouseMoved(MouseEvent evt)
   {
      if (m_ObjectQueue != null && m_ObjectQueue.ContainsInteraction())
         m_Board.mouseMoved(evt);
   }
   
   public void FullScreenMouseDragged(MouseEvent evt)
   {
      if (m_ObjectQueue != null && m_ObjectQueue.ContainsInteraction())
         m_Board.mouseDragged(evt);
   }
   
   public void FullScreenMousePressed(MouseEvent evt)
   {
      if (m_ObjectQueue != null && m_ObjectQueue.ContainsInteraction())
         m_Board.mousePressed(evt);
   }
   
   public void FullScreenMouseReleased(MouseEvent evt)
   {
      if (m_ObjectQueue != null && m_ObjectQueue.ContainsInteraction())
         m_Board.mouseReleased(evt);
   }
   
   public void FullScreenMouseClicked(MouseEvent evt)
   {
      if (m_ObjectQueue != null && m_ObjectQueue.ContainsInteraction())
         m_Board.mouseClicked(evt);
   }
   
   public void DeactivateVisibleQuestionDynamics(int iTimeMs)
   {
      Rectangle rcRepaint = m_ObjectQueue.DeactivateVisibleDynamics(iTimeMs, true);
      if (rcRepaint != null)
         m_Board.RepaintArea(rcRepaint);
   }
   
   /** Is blocking. Call from a thread. */
   public void ShowFeedback(Feedback feedback)
   {
      m_Board.ShowFeedback(feedback);
      
      //while(m_Board.IsFeedbackActive())
      //   try { Thread.sleep(50); } catch (InterruptedException exc) { }
   }
   
   public boolean IsFeedbackActive()
   {
      return m_Board.IsFeedbackActive();
   }
   
   public void RemoveAllFeedback()
   {
      m_Board.RemoveAllFeedback();
   }
  
   public void ExecuteAction(Questionnaire qq, boolean bSuccess)
   {
      if (qq != null)
      {
         String strAction = qq.GetActionString(bSuccess);
         //Manager.showError(strAction, Manager.WARNING, null);
         ExecuteAction(strAction, InteractionArea.ACTION_MOUSE_CLICK);
      }
   }
   
   public void ExecuteAction(Question q, boolean bSuccess)
   {
      if (q != null)
      {
         String strAction = q.GetActionString(bSuccess);
         //System.out.println(strAction);
         ExecuteAction(strAction, InteractionArea.ACTION_MOUSE_CLICK);
      }
   }
      
   private boolean IsJumpToCurrentRandomQuestion(int iPrevTime, int iActualTime)
   {
   		int iTimestamp = m_TestEngine.GetBeforeFirstQuestionTimestamp();
   		if (iPrevTime <= iTimestamp && iActualTime >= iTimestamp && iActualTime < iPrevTime + 250)
   		{
   			//m_TestEngine.ResetBeforeFirstQuestionTimestamp();
   			return true;
   		}
   		
   		return false;
   }
   
   private void ExecuteJumpToCurrentRandomQuestion()
   {
   		String strAction = "Rjump page " + m_iJumpRandomTimestamp + ";start";// m_TestEngine.GetFirstRandomQuestionMs();
   		ExecuteAction(strAction, InteractionArea.ACTION_MOUSE_CLICK);
   }
   
   private void ExecuteAction(InteractionArea area, int iActionType)
   {
      String strAction = area.GetActionString(iActionType);
      ExecuteAction(strAction, iActionType);
   }
   
   private void ExecuteAction(String strAction, int iActionType)
   {
      while (strAction != null && strAction.length() > 0)
      {
         boolean bSomethingFound = false;
         
         boolean bIsRandomJump = false;
         if (strAction.startsWith("Rjump"))
         {
         	bIsRandomJump = true;
         	strAction = strAction.substring(1);
         }
         
         if(strAction.startsWith("Rprev"))
         {
         	String sParam = strAction.substring(5);
         	int iQMs =  0;
         	int iTimestamp = 0;
         	try
         	{
         		iQMs = Integer.parseInt(sParam);
         		iTimestamp = GetPreviousPageStartTimestamp(iQMs);
         	}
         	catch (NumberFormatException ex)
         	{
         	}
         	strAction = "jump page " + iTimestamp;
         }
         if (strAction.startsWith("jump"))
         {
            String strParam = ExtractParameter(strAction);
            if (strParam != null)
            {
               if (strParam.startsWith("next"))
               {
                  m_Coordinator.requestNextSlide(this);
                  bSomethingFound = true;
               }
               else if (strParam.startsWith("prev"))
               {
                  m_Coordinator.requestPreviousSlide(true, this);
                  bSomethingFound = true;
               }
               else if (strParam.startsWith("question"))
               {
            	   if(m_TestEngine != null && m_TestEngine.AreAnyQuestionLeft())
            		   m_Coordinator.requestTime(m_iJumpRandomTimestamp, this);
            	   bSomethingFound = true;
               }
               else
               {
                  // jump over first parameter (like "mark" or "page")
                  strParam = ExtractParameter(strParam); 
                  
                  int iJumpTime = -1;
                  try
                  {
                     iJumpTime = Integer.parseInt(strParam);
                     m_Coordinator.requestTime(iJumpTime, this);
                     if (bIsRandomJump)
                     	m_iJumpRandomTimestamp = iJumpTime;
                     bSomethingFound = true;
                  }
                  catch (NumberFormatException exc)
                  {
                  }
               }
            }
         }
         else if (strAction.startsWith("open")) // open-file, open-url
         {
            String strParam = ExtractParameter(strAction);
            if (strParam != null)
            {
               boolean bResult = false;
               try
               {
                  bResult = m_Resources.StartFileOrUrl(strParam);
               }
               catch (IOException exc)
               {
                  System.err.println("Exception during open action: "+exc);
               }
               
               if (!bResult)
               {
                  System.err.println("Could not start: "+strParam);
                  
                  Manager.showError(SwingUtilities.windowForComponent(m_Board), 
                     Manager.getLocalized("executeFailed")+":\n"+strParam,
                     Manager.WARNING, null);
               }
               
               bSomethingFound = true;
            }
         }
         else if (strAction.startsWith("start"))
         {
            m_Coordinator.requestStart(this);
            bSomethingFound = true;
         }
         else if (strAction.startsWith("stop"))
         {
            m_Coordinator.requestStop(this);
            bSomethingFound = true;
         }
         else if (strAction.startsWith("audio"))
         {
            String strParam = ExtractParameter(strAction);
            if (strParam != null)
            {
               if (strParam.equals("on"))
               {
                  m_Coordinator.requestMute(false, this);
                  bSomethingFound = true;
               }
               else if (strParam.equals("off"))
               {
                  m_Coordinator.requestMute(true, this);
                  bSomethingFound = true;
               }
            }
         }
         else if (strAction.startsWith("close"))
         {
            m_Coordinator.requestClose(this);
            bSomethingFound = true;
         }
         else if (strAction.startsWith("check"))
         {
            int iTimeMs = m_iCurrentTime;
            
            boolean bSpecial = true;
            m_Coordinator.requestStop(bSpecial, this);
            
            boolean bSuccess = m_ObjectQueue.CheckVisibleDynamics(iTimeMs);
            
            // also displays feedback
            if (m_TestEngine != null)
               m_TestEngine.QuestionAnsweredAt(iTimeMs, bSuccess);
            
            // TODO update button state: is probably still mouse-over or pressed
            
            bSomethingFound = true;
         }
         else if (strAction.startsWith("reset"))
         {
            int iTimeMs = m_iCurrentTime;
            Rectangle rcReset = m_ObjectQueue.ResetVisibleDynamics(iTimeMs);
            
            if (rcReset != null && rcReset.width > 0)
               m_Board.RepaintArea(rcReset);
            
            bSomethingFound = true;
         }
         
         if (!bSomethingFound)
            System.err.println("Action not recognized: "+strAction);
               
         // there might be more than one action
         int idxNext = strAction.indexOf(';');
         if (idxNext > -1)
            strAction = strAction.substring(idxNext+1, strAction.length());
         else
            strAction = null;
      }
     
   }
   
   private String ExtractParameter(String strAction)
   {
      int idx = strAction.indexOf(' ');
      int idxEnd = strAction.indexOf(';');
      if (idxEnd < idx)
         idxEnd = strAction.length();
      
      if (idx > 0)
         return strAction.substring(idx+1, idxEnd);
      else
         return null;
   }
   
   // PZI: added to force repaint if search was performed
   public void repaint() {
      m_Board.repaint();
   }
	
	
	// Informationszugriff mit Seiteneffekt
	class Board extends JPanel implements MouseMotionListener, MouseListener, ComponentListener
   {
		private Dimension m_dimOriginal = null;
      private RenderingHints m_RenderHints = null;
      private double m_dLastScale = 1.0;
      private Point m_ptLastOffsets = new Point(0,0);
      
      private InteractionArea m_LastInteractionMove = null;
      private InteractionArea m_LastInteractionDown = null;
      private VisualComponent m_MovingComp = null;
      private Point m_ptLastMouseDragPos = null;
      private Point m_ptLastSnapCorrection = null;
      private boolean m_bHandCursor = false;
      private boolean m_bMoveCursor = false;
      
      private ArrayList m_aFeedback = new ArrayList(5);
      private ArrayList m_aHiddenComponents = new ArrayList(20);
      private HashMap m_mapNatives = new HashMap();
	
		Board()
      { 
			this(612,792,1.0f);
      }
		
      Board(int width, int height, float zoom)
      {
         m_dimOriginal = new Dimension(width, height);
         setFont(new JLabel().getFont().deriveFont(Font.ITALIC));
         
         if (m_ObjectQueue != null && m_ObjectQueue.ContainsInteraction())
         {
            addMouseListener(this);
            addMouseMotionListener(this);
         }

         setOpaque(true);
			//setBackground(Color.white);
			
         Dimension d = null;
         if (zoom < 0.99f || zoom > 1.01f)
            d = new Dimension((int)(zoom*width), (int)(zoom*height));
         else
            d = new Dimension(width, height);
         setPreferredSize(d);
			setMinimumSize(new Dimension((int)(0.2*width),(int)(0.2*height)));
         
         addComponentListener(this);
         
			m_RenderHints = new RenderingHints(null);

         // Note: It seems that the annoying text artefacts comes from
         // the next line. If the fractional metrics are switched off,
         // text is not distorted e.g. if the online pointer is moved
         // over the text. /MD 07.10.2002
         // -->
			//m_RenderHints.put(RenderingHints.KEY_FRACTIONALMETRICS,
			//					 RenderingHints.VALUE_FRACTIONALMETRICS_ON);
         // <--
         //
         // Doesn't make that much of a difference to font rendering
         // but causes problem with some fonts (eg Frutiger)
			//m_RenderHints.put(RenderingHints.KEY_TEXT_ANTIALIASING,
         //                 RenderingHints.VALUE_TEXT_ANTIALIAS_ON);
			m_RenderHints.put(RenderingHints.KEY_ANTIALIASING,
                          RenderingHints.VALUE_ANTIALIAS_ON);
         //m_RenderHints.put(RenderingHints.KEY_RENDERING,
         //                 RenderingHints.VALUE_RENDER_QUALITY);
      }
		
      double CalcScaleFactor()
      {
         // TODO what about "zoom" in the constructor?

         Dimension ich = m_dimOriginal;
         Dimension er = getSize();
         
			if (er.width/(double)ich.width < er.height/(double)ich.height)
				return er.width/(double)ich.width;
			else
				return er.height/(double)ich.height;
		}

      void CalcDrawingOffsets(double dScale, Point ptTarget)
      {
         // slightly double code'ish (below)
         
         // TODO what about "zoom" in the constructor?
         
         Dimension preferred = m_dimOriginal;
         Dimension size = getSize();
         ptTarget.x = (size.width - (int)(dScale*preferred.width))/2;
         ptTarget.y = (size.height - (int)(dScale*preferred.height))/2;
      }
      
      Point GetObjectEventPoint(MouseEvent evt)
      {
         return new Point(
            (int)Math.round((evt.getX() - m_ptLastOffsets.x) / m_dLastScale), 
            (int)Math.round((evt.getY() - m_ptLastOffsets.y) / m_dLastScale));
      }
      
      void RepaintArea(Rectangle rcObjectBounds)
      {
         // -1, +2 for antialias effects
         rcObjectBounds.x = (int)Math.round(rcObjectBounds.x * m_dLastScale) - 1;
         rcObjectBounds.y = (int)Math.round(rcObjectBounds.y * m_dLastScale) - 1;
         rcObjectBounds.width = (int)Math.round(rcObjectBounds.width * m_dLastScale) + 2;
         rcObjectBounds.height = (int)Math.round(rcObjectBounds.height * m_dLastScale) + 2;
            
         rcObjectBounds.x += m_ptLastOffsets.x;
         rcObjectBounds.y += m_ptLastOffsets.y;
          
         repaint(rcObjectBounds);
         //System.out.println("r "+rcObjectBounds);
      }
      
      protected void paintComponent(Graphics g)
      {
         super.paintComponent(g);
    
         ((Graphics2D)g).setRenderingHints(m_RenderHints);
         
         double dScale = CalcScaleFactor();
         
         // if (dScale != m_dLastScale)
         // this is not enough: if the size changes only on one axis
         // the scaling factor maybe does not change but the offset should

         if(m_structuredClips) {
             // do not paint whiteboard behind structured clips
             // Note: setting m_dLastScale to avoid side effects - maybe this is not required 
             m_dLastScale = dScale;
             return;
         }

         CalcDrawingOffsets(dScale, m_ptLastOffsets);
         g.translate(m_ptLastOffsets.x, m_ptLastOffsets.y);

         // PZI: fix bug 4698
         // clip paint because nothing should be painted outside the whiteboard area
         Shape previousClippingArea = g.getClip();
         g.setClip(0,0, (int)(dScale*m_dimBoardSize.width), (int)(dScale*m_dimBoardSize.height));
         
         m_EventQueue.paint(g, m_iCurrentTime, dScale);
         
         // paint the question elements above the normal elements
         m_ObjectQueue.PaintInteractives(g, m_iCurrentTime, dScale);
         
         // paint the feedback on the very top
         if (IsFeedbackActive())
         {
            ArrayList aFeedbackComponents = ((Feedback)m_aFeedback.get(0)).GetComponents();
            for (int i=0; i<aFeedbackComponents.size(); ++i)
            {
               VisualComponent comp = (VisualComponent)aFeedbackComponents.get(i);
               comp.paint(g, -1, -1, dScale);
            }
         }
   
         //g.setColor(Color.magenta);
         //Rectangle rcClip = g.getClip().getBounds();
         //g.drawRect(rcClip.x, rcClip.y, rcClip.width-1, rcClip.height-1);
         
         // PZI: restore old clipping
         g.setClip(previousClippingArea);

         // restore origin point for embedded components
         g.translate(-m_ptLastOffsets.x, -m_ptLastOffsets.y);
         
         m_dLastScale = dScale;
      }
      
      
      public void ShowFeedback(Feedback feedback)
      {
         m_aFeedback.add(feedback);
         HideAllNatives();
         repaint();
      }
      
      public boolean IsFeedbackActive()
      {
         return !m_aFeedback.isEmpty();
      }
      
      public void RemoveAllFeedback()
      {
         if (!m_aFeedback.isEmpty())
         {
            m_aFeedback.clear();
            
            ShowAllHiddenNatives();
            repaint();
         }
      }
      
      public void mouseClicked(MouseEvent evt)
      {
         // is handled by mouseReleased() to get proper button behaviour:
         // mouse can move between mouse press and release
      }
 
      public void mousePressed(MouseEvent evt)
      {
         if ((evt.getModifiers() & evt.BUTTON1_MASK) == 0)
            return; // only handle left mouse button
        
         int iCheckTime = m_iCurrentTime;
         Point ptClick = GetObjectEventPoint(evt);
         
         VisualComponent movingComp = m_ObjectQueue.FindMovingComp(
            ptClick.x, ptClick.y, iCheckTime);
         
         if (movingComp == null)
         {
            // normal case: no d&d
            
            InteractionArea clickArea = null;
            if (!IsFeedbackActive())
               clickArea = m_ObjectQueue.FindInteractionArea(ptClick.x, ptClick.y,  iCheckTime);
            else
            {
               ArrayList aFeedbackComponents = ((Feedback)m_aFeedback.get(0)).GetComponents();
               for (int i=0; i<aFeedbackComponents.size(); ++i)
               {
                  VisualComponent comp = (VisualComponent)aFeedbackComponents.get(i);
                  if (comp instanceof ButtonArea && comp.HitTest(ptClick.x, ptClick.y))
                     clickArea = (InteractionArea)comp;
               }
            }
         
            if (clickArea != null)
            {
               m_LastInteractionDown = clickArea;
         
               ExecuteAction(clickArea, InteractionArea.ACTION_MOUSE_DOWN);
            
               if (clickArea instanceof ButtonArea)
               {
                  ((ButtonArea)clickArea).Visualize(true, true);
                  RepaintArea(clickArea.GetActivityArea());
               }
            }
         }
         else
         {
            m_MovingComp = movingComp;
            m_ptLastMouseDragPos = evt.getPoint();
            m_ObjectQueue.InformMovingComponent(m_MovingComp);
            m_ptLastSnapCorrection = null;
         }
      }
     
      public void mouseReleased(MouseEvent evt)
      {
         if ((evt.getModifiers() & evt.BUTTON1_MASK) == 0)
            return; // only handle left mouse button
         
         Point ptClick = GetObjectEventPoint(evt);
         
         if (IsFeedbackActive())
         {
            boolean bOkClicked = false;
            
            ArrayList aFeedbackComponents = ((Feedback)m_aFeedback.get(0)).GetComponents();
            for (int i=0; i<aFeedbackComponents.size(); ++i)
            {
               if (aFeedbackComponents.get(i) instanceof ButtonArea)
               {
                  // currently there is only one button in a feedback
                  
                  ButtonArea button = (ButtonArea)aFeedbackComponents.get(i);
               
                  if (button.HitTest(ptClick.x, ptClick.y))
                     bOkClicked = true; 
               }
            }
            
            if (bOkClicked)
            {
               m_aFeedback.remove(0);
            
               if (!IsFeedbackActive())
                  ShowAllHiddenNatives();
               repaint();
            }
            
            return; // no standard event handling during feedback
         }
         
         if (m_MovingComp == null)
         {
            // normal case: no d&d
            
            int iCheckTime = m_iCurrentTime;
            
            InteractionArea clickArea = m_ObjectQueue.FindInteractionArea(
               ptClick.x, ptClick.y,  iCheckTime);
         
            if (clickArea != null)
            {
               ExecuteAction(clickArea, InteractionArea.ACTION_MOUSE_UP);
         
               if (m_LastInteractionDown != null)
               {
                  if (clickArea == m_LastInteractionDown)
                     ExecuteAction(clickArea, InteractionArea.ACTION_MOUSE_CLICK);
               }
            
               if (clickArea instanceof ButtonArea)
               {
                  ((ButtonArea)clickArea).Visualize(true, false);
                  RepaintArea(clickArea.GetActivityArea());
               }
            }
         
            m_LastInteractionDown = null;
         }
         else
         {
            Rectangle rcMovingDefine = m_MovingComp.getClip(1.0);
            m_MovingComp = null;
            m_ObjectQueue.InformMovingComponent(m_MovingComp);
            m_ptLastSnapCorrection = null;
            RepaintArea(rcMovingDefine);
         }
      }

      public void mouseMoved(MouseEvent evt)
      {
         int iCheckTime = m_iCurrentTime;
         Point ptClick = GetObjectEventPoint(evt);
         
         if (!IsFeedbackActive())
         {
            VisualComponent movingComp = m_ObjectQueue.FindMovingComp(
               ptClick.x, ptClick.y, iCheckTime);
         
            // set or reset move cursor
            if (movingComp != null)
            {
               if (!m_bMoveCursor)
               {
                  this.setCursor(Cursor.getPredefinedCursor(Cursor.MOVE_CURSOR));
                  m_bMoveCursor = true;
               }
            }
            else
            {
               if (m_bMoveCursor)
               {
                  this.setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
                  m_bMoveCursor = false;
               }
            }
         }
        
         InteractionArea moveArea = null;
         if (!IsFeedbackActive())
            moveArea = m_ObjectQueue.FindInteractionArea(ptClick.x, ptClick.y, iCheckTime);
         else
         {
            ArrayList aFeedbackComponents = ((Feedback)m_aFeedback.get(0)).GetComponents();
            for (int i=0; i<aFeedbackComponents.size(); ++i)
            {
               VisualComponent comp = (VisualComponent)aFeedbackComponents.get(i);
               if (comp instanceof ButtonArea && comp.HitTest(ptClick.x, ptClick.y))
                  moveArea = (InteractionArea)comp;
            }
         }
         
         if (moveArea != m_LastInteractionMove)
         {
            boolean bHandCursor = false;
            
            boolean bMousePressed = (evt.getModifiers() & evt.MOUSE_PRESSED) != 0;
            
            if (m_LastInteractionMove != null)
            {
               if (!m_LastInteractionMove.GetActivityArea().contains(ptClick))
                  ExecuteAction(m_LastInteractionMove, InteractionArea.ACTION_MOUSE_LEAVE);
               // else do nothing if the object disappeared under the mouse
               
               if (m_LastInteractionMove instanceof ButtonArea)
               {
                  ((ButtonArea)m_LastInteractionMove).Visualize(false, bMousePressed);
                  RepaintArea(m_LastInteractionMove.GetActivityArea());
               }
            }
            
            if (moveArea != null)
            {
               if (moveArea.HasHandCursor())
                  bHandCursor = true;
               
               ExecuteAction(moveArea, InteractionArea.ACTION_MOUSE_ENTER);
               
               if (moveArea instanceof ButtonArea)
               {
                  ((ButtonArea)moveArea).Visualize(true, bMousePressed);
                  RepaintArea(moveArea.GetActivityArea());
               }
            }
               
            m_LastInteractionMove = moveArea;
            
            if (bHandCursor != m_bHandCursor)
            {
               Component compHand = this;
               
               Window w = SwingUtilities.windowForComponent(this);
               if (w != null && w instanceof JWindow && ((JWindow)w).getGlassPane() != null)
                  compHand = ((JWindow)w).getGlassPane();
               
               if (bHandCursor)
                  compHand.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
               else
                  compHand.setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
               
               if (compHand != this)
                  this.setCursor(compHand.getCursor());
               
               m_bHandCursor = bHandCursor;
            }
         }
      }
      
      public void mouseEntered(MouseEvent evt)
      {
         // this also generates automatically a mouseMoved() event
         // so no special handling for ACTION_MOUSE_ENTER needed here
      }
      
      public void mouseExited(MouseEvent evt)
      {
         mouseMoved(evt);
      }
      
      public void mouseDragged(MouseEvent evt)
      {
         if (IsFeedbackActive())
            return;
         
         if (m_MovingComp == null)
         {
            // normal case: no d&d
            mouseMoved(evt);
         }
         else
         {
            Point ptMouseNow = evt.getPoint();
            
            double dScale = CalcScaleFactor();
            Rectangle rcOld = m_MovingComp.getClip(dScale);
            
            // do move
            float fMoveX = (float)((1/dScale)*(ptMouseNow.getX() - m_ptLastMouseDragPos.getX()));
            float fMoveY = (float)((1/dScale)*(ptMouseNow.getY() - m_ptLastMouseDragPos.getY()));
            
            if (m_ptLastSnapCorrection != null)
            {
               fMoveX -= m_ptLastSnapCorrection.x;
               fMoveY -= m_ptLastSnapCorrection.y;
            }
                                   
            m_MovingComp.MovePosition(fMoveX, fMoveY);
            
            // do snap-in
            Rectangle rcSnapIn = m_MovingComp.getClip(1.0, false);
            int iOrigX = rcSnapIn.x;
            int iOrigY = rcSnapIn.y;
            boolean bFound = m_ObjectQueue.DoSnapIn(m_MovingComp, rcSnapIn, m_iCurrentTime);
            if (bFound)
            {
               // rcSnapIn contains the new (snapped) position
               m_MovingComp.MovePosition(rcSnapIn.x - iOrigX, rcSnapIn.y - iOrigY);
               
               if (m_ptLastSnapCorrection == null)
                  m_ptLastSnapCorrection = new Point();
               
               m_ptLastSnapCorrection.x = rcSnapIn.x - iOrigX;
               m_ptLastSnapCorrection.y = rcSnapIn.y - iOrigY;
            }
            else if (m_ptLastSnapCorrection != null)
               m_ptLastSnapCorrection = null;
            
            Rectangle rcNew = m_MovingComp.getClip(dScale);
            
            // do repaint
            rcNew.add(rcOld);
            rcNew.x += m_ptLastOffsets.x;
            rcNew.y += m_ptLastOffsets.y;
            repaint(rcNew);
            
            m_ptLastMouseDragPos = ptMouseNow;
         }
      }
      
      public void componentResized(ComponentEvent evt)
      {
         double dScale = CalcScaleFactor();
         Point ptOffsets = new Point();
         CalcDrawingOffsets(dScale, ptOffsets);
         
         Component[] comps = getComponents();
         for (int i=0; i<comps.length; ++i)
         {
            if (comps[i] instanceof JComponent)
            {
               JComponent jc = (JComponent)comps[i];
               Rectangle rcOrig = (Rectangle)m_mapNatives.get(comps[i]);
            
               if (rcOrig != null)
               {
                  jc.setSize((int)(dScale * rcOrig.width),
                             (int)(dScale * rcOrig.height));
                  jc.setLocation(ptOffsets.x + (int)(dScale * rcOrig.x),
                                 ptOffsets.y + (int)(dScale * rcOrig.y));
              }
               
               if (comps[i] instanceof JTextField)
               {
                  Font font = comps[i].getFont();
                  Font fontNew = font.deriveFont((float)(0.6 * rcOrig.height * dScale));
                  comps[i].setFont(fontNew);
               }
            }
         }
      }
      
      public void componentHidden(ComponentEvent evt)
      {
      }
      
      public void componentMoved(ComponentEvent evt)
      {
      }
      
      public void componentShown(ComponentEvent evt)
      {
      }
      
      void AddNative(JComponent jc, Rectangle rcOrig)
      {
         if (jc != null && rcOrig != null)
         {
            m_mapNatives.put(jc, rcOrig);
            add(jc);
         }
      }
      
      private void HideAllNatives()
      {
         Component[] comps = getComponents();
         for (int i=0; i<comps.length; ++i)
         {
            if (comps[i].isVisible())
            {
               m_aHiddenComponents.add(comps[i]);
               comps[i].setVisible(false);
            }
         }
      }
      
      private void ShowAllHiddenNatives()
      {
         for (int i=0; i<m_aHiddenComponents.size(); ++i)
         {
            Component comp = (Component)m_aHiddenComponents.get(i);
            comp.setVisible(true);
         }
         m_aHiddenComponents.clear();
      }
   } // Board

}