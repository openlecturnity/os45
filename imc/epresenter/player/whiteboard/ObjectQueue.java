package imc.epresenter.player.whiteboard;

import java.awt.*;
import java.io.*;
import java.net.URL;
import java.util.*;
import javax.swing.JPanel;

import imc.epresenter.filesdk.FileResources;
import imc.epresenter.filesdk.Questionnaire;
import imc.epresenter.filesdk.StopMarkReader;
import imc.epresenter.player.Manager;
import imc.epresenter.player.util.*;
import imc.lecturnity.util.FileUtils;


/** (Reads and) stores the actual content objects for the Event objects in EventQueue. */
public class ObjectQueue
{
   private VisualComponent[] content;
   private FileResources _resources;
   private InputStream inputStream;
   private String m_strCodepage;
   private ImageVault vault;
   private HashMap idCollector;
   private boolean m_bConsoleOperation;
   private boolean m_bContainsInteraction = false;
   private int[] m_aiInteractionIndices = null;
   private VisualComponent m_MovedComponent = null;
   private ArrayList m_aToPaintTargets = new ArrayList(10);
   
   public ObjectQueue(InputStream in, String strCodepage, FileResources resources)
   {
      this(in, strCodepage, resources, false);
   }
   
   public ObjectQueue(InputStream in, String strCodepage, FileResources resources, boolean bConsoleOperation)
   {
      m_bConsoleOperation = bConsoleOperation;
      inputStream = in;
      _resources = resources;
      m_strCodepage = strCodepage;
      //if (!m_bConsoleOperation)
      vault = new ImageVault();
      idCollector = new HashMap();
      
      m_bContainsInteraction = false;
   }
   
   public void startParsing(ProgressListener listener) throws Exception
   {
      //System.gc();
      //long mem1 = Runtime.getRuntime().totalMemory()-Runtime.getRuntime().freeMemory();
      content = parse(listener);
      //System.gc();
      //long mem2 = Runtime.getRuntime().totalMemory()-Runtime.getRuntime().freeMemory();
      //System.out.println("OQ: ObjectQueue occupies "+((mem2-mem1)/(float)1000)+" KB (by now)");
   }
   
   public boolean IsFilledRectangle(int idx)
   {
      return (idx > -1 && idx < content.length && content[idx] instanceof FilledRectangle);
   }
   
   public boolean IsSameFilledRectangle(int idx1, int idx2)
   {
      if (idx1 > -1 && idx1 < content.length && idx2 > -1 && idx2 < content.length)
      {
         if (content[idx1] instanceof FilledRectangle && content[idx2] instanceof FilledRectangle)
         {
            FilledRectangle r1 = (FilledRectangle)content[idx1];
            FilledRectangle r2 = (FilledRectangle)content[idx2];
            
            return r1.HasSameValues(r2);
         }
         else
            return false; // no rectangles
      }
      else
         return false; // wrong parameters
   }
   
   //
   // package access methods for getting the helper 
   // and information objects for parsing
   //
   
   FileResources GetResources()
   {
      return _resources;
   }
   
   String GetCodepage()
   {
      return m_strCodepage;
   }
   
   boolean IsConsoleOperation()
   {
      return m_bConsoleOperation;
   }
   
   ImageVault GetImageVault()
   {
      return vault;
   }
                          
   HashMap GetImageIds()
   {
      return idCollector;
   }
   
   private VisualComponent[] parse(ProgressListener listener) throws Exception
   {
      CounterInputStream cis = new CounterInputStream(inputStream);
      
      BufferedReader in = FileUtils.createBufferedReader(cis, m_strCodepage, true, null);
      
      ArrayList alComponentCollector = new ArrayList(800);
      String line;
      boolean groupCountWarningGiven = false;
      
      // find beginning of whiteboard content
      do 
      {
         line = in.readLine();
      }
      while (!(line.startsWith("<WHITEBOARD>") || line.startsWith("<WB")));
      
      m_bContainsInteraction = false;
      ArrayList alInteractionCollector = new ArrayList(100);
      
      // parse until end of whiteboard content is reached
      while (true)
      {
         line = in.readLine();
         if (listener != null)
            listener.progressAchieved(cis.readBytes());
         
         if (line == null || line.startsWith("</WH") || line.startsWith("</WB"))
            break;
         else
         {
            try
            {
               // find appropriate subclass of VisualComponent for this line
               VisualComponent vc = null;
               
               vc = VisualComponent.CreateComponent(line, in, this, alComponentCollector);
               
               if (vc == null)
               {
                  // special cases for old or invisible objects
                  
                  if (line.startsWith("<MARK"))
                  {
                     StopMarkReader.parseMark(line);
                     // as vc remains null:
                     // will also generate a (visual) dummy component (see below)
                  }
                  else if (line.startsWith("<QUESTIONNAIRE"))
                  {
                     Questionnaire.ParseQuestionnaire(line, in, this);
                     // as vc remains null:
                     // will also generate a (visual) dummy component (see below)
                  }
                  else if (line.startsWith("<APP"))
                  {
                     // handeled by ApplicationPlayer (in extra files)
                     continue;
                  }
                  else if (line.startsWith("<GROUP") && !groupCountWarningGiven)
                  {
                     int idx1 = line.indexOf("count=");
                     int idx2 = line.indexOf(" ", idx1);
                     if (idx2 == -1)
                        idx2 = line.indexOf(">", idx1);
                     if (idx1 != -1 && idx2 != -1)
                     {
                        int count = Integer.parseInt(line.substring(idx1+6, idx2));
                        if (count > 1)
                        {
                           Manager.showError(Manager.getLocalized("groupProblem"),
                                             Manager.WARNING, null);
                           groupCountWarningGiven = true;
                        }
                     }
                  }
               }
               
              

               // dummy element to take a number:
               if (vc == null && !(line.startsWith("<GROUP") || line.startsWith("</GROUP")))
                  vc = new VisualComponent();
            
               if (vc != null)
               {
                  if (vc.GetVisibilityTimes() != null)
                  {
                     m_bContainsInteraction = true;
                     alInteractionCollector.add(new Integer(alComponentCollector.size()));
                  }
                                                
                  alComponentCollector.add(vc);
               }
               
            }
            catch (Exception e) 
            {
               System.err.println("A parse error occured before byte "+cis.readBytes()+":");
               System.err.println(line);
               e.printStackTrace();
               throw e;
            }
         }
      }
      
      VisualComponent[] comps = new VisualComponent[alComponentCollector.size()];
      alComponentCollector.toArray(comps);
      
      //System.out.println("ObjectQueue: "+comps.length+" objects.");
      
      
      if (alInteractionCollector.size() > 0)
      {
         m_aiInteractionIndices = new int[alInteractionCollector.size()];
         
         for (int i=0; i<m_aiInteractionIndices.length; ++i)
            m_aiInteractionIndices[i] = ((Integer)alInteractionCollector.get(i)).intValue();
      }
      
      return comps;
   }
  
   public boolean ContainsInteraction()
   {
      return m_bContainsInteraction;
   }
   
   public boolean IsInteraction(int iObjectIndex)
   {
      if (m_bContainsInteraction)
      {
         if (iObjectIndex >= 0 && iObjectIndex < content.length)
            return content[iObjectIndex] instanceof InteractionArea;
      }
      
      return false;
   }
     
   public InteractionArea GetInteractionArea(int idxObject)
   {
      if (idxObject >= 0 && idxObject < content.length)
      {
         if (content[idxObject] instanceof InteractionArea)
         {
            return (InteractionArea)content[idxObject];
         }
      }
      
      return null;
   }
   
   // TODO following methods more or less double code'ish.
   
   public InteractionArea FindInteractionArea(int x, int y, int iTime)
   {
      if (m_bContainsInteraction)
      {
         // iterate in reverse order: check areas on top first
         for (int i=m_aiInteractionIndices.length-1; i>=0; --i)
         {
            if (content[m_aiInteractionIndices[i]] instanceof InteractionArea)
            {
               InteractionArea area = (InteractionArea)content[m_aiInteractionIndices[i]];
               
               // TODO distinguish check between components that are sensitive to mouse
               // move events and simple mouse press events (more rare)
               if (area.HitTest(x, y) && area.IsActiveAt(iTime))
                  return area;
            }
         }
      }
      
      return null;
   }
   
   Rectangle ShowHideInteractives(int iTimeMs)
   {
      Rectangle rcRepaintArea = null;
         
      if (m_bContainsInteraction)
      {
         for (int i=0; i<m_aiInteractionIndices.length; ++i)
         {
            VisualComponent vc = content[m_aiInteractionIndices[i]];
            
            if (vc.IsMoveable() || vc instanceof ButtonArea || vc instanceof TargetPoint || vc instanceof Text)
            {
               boolean bChange = vc.SetVisible(vc.IsVisibleAt(iTimeMs));
               if (bChange)
               {
                  if (rcRepaintArea == null)
                     rcRepaintArea = vc.getClip(1.0);
                  else
                     rcRepaintArea.add(vc.getClip(1.0));
               }
            }
         }
      }
       
      return rcRepaintArea;
   }
   
   Rectangle ActivateInteractions(int iTime)
   {
      Rectangle rcRepaintArea = null;
         
      if (m_bContainsInteraction)
      {
         for (int i=0; i<m_aiInteractionIndices.length; ++i)
         {
            if (content[m_aiInteractionIndices[i]] instanceof InteractionArea)
            {
               InteractionArea area = (InteractionArea)content[m_aiInteractionIndices[i]];
           
               boolean bChange = area.SetActive(area.IsActiveAt(iTime));
               if (bChange)
               {
                  if (rcRepaintArea == null)
                     rcRepaintArea = area.GetActivityArea();
                  else
                     rcRepaintArea.add(area.GetActivityArea());
               }
            }
         }
      }
      
      return rcRepaintArea;
    }
   
   VisualComponent FindMovingComp(int x, int y, int iTime)
   {
      if (m_bContainsInteraction)
      {
         for (int i=0; i<m_aiInteractionIndices.length; ++i)
         {
            VisualComponent vc = content[m_aiInteractionIndices[i]];
            
            if (vc.IsMoveable() && vc.HitTest(x, y) && vc.IsActiveAt(iTime) && !vc.IsMovingDisabled())
            {
               return vc;
            }
         }
      }
      
      return null;
   }
		
   void GetDynamics(ArrayList alDynamics)
   {
      if (m_bContainsInteraction && alDynamics != null)
      {
         for (int i=0; i<m_aiInteractionIndices.length; ++i)
         {
            if (m_aiInteractionIndices[i] >= 0 && m_aiInteractionIndices[i] < content.length)
            {
               if (content[m_aiInteractionIndices[i]] instanceof DynamicComponent)
               {
                  alDynamics.add(content[m_aiInteractionIndices[i]]);
               }
            }
         }
      }
   }
   
   Rectangle ShowHideActivateDynamics(int iTimeMs)
   {
      Rectangle rcRepaintArea = null;
      if (m_bContainsInteraction)
      {
         for (int i=0; i<m_aiInteractionIndices.length; ++i)
         {
            if (content[m_aiInteractionIndices[i]] instanceof DynamicComponent)
            {
               DynamicComponent dc = (DynamicComponent)content[m_aiInteractionIndices[i]];
               boolean bChangeShow = dc.ShowHide(iTimeMs);
               boolean bChangeActive = dc.SetActive(dc.IsActiveAt(iTimeMs));
               if (bChangeShow || bChangeActive)
               {
                  if (rcRepaintArea == null)
                     rcRepaintArea = dc.GetActivityArea();
                  else
                     rcRepaintArea.add(dc.GetActivityArea());
               }
            }
         }
      }
      
      return rcRepaintArea;      
   }
   
   boolean CheckVisibleDynamics(int iTimeMs)
   {
      // TODO what if they went invisible after initiating this check?
      // Is that possible at all?
      
      boolean bAtLeastOneFound = false;
      
      if (m_bContainsInteraction)
      {
         for (int i=0; i<m_aiInteractionIndices.length; ++i)
         {
            // TODO make them inherit the same interface
            
            if (content[m_aiInteractionIndices[i]] instanceof DynamicComponent)
            {
               DynamicComponent dc = (DynamicComponent)content[m_aiInteractionIndices[i]];
                  
               if (dc.IsVisible())
               {
                  bAtLeastOneFound = true;
                 
                  if (!dc.CheckAnswer())
                     return false;
               }
            }
            else if (content[m_aiInteractionIndices[i]] instanceof TargetPoint)
            {
               TargetPoint tp = (TargetPoint)content[m_aiInteractionIndices[i]];
                  
               if (tp.IsActiveAt(iTimeMs))
               {
                  bAtLeastOneFound = true;
                     
                  if (!tp.CheckAnswer())
                     return false;
               }
            }
         
         }
      }
      
      return bAtLeastOneFound; // there was at least one? and all answers were correct
   }
   
   Rectangle ResetVisibleDynamics(int iTimeMs)
   {
      Rectangle rcRepaintArea = null;
      
      // TODO what if they went invisible after initiating this check?
      // Is that possible at all?
      
      if (m_bContainsInteraction)
      {
         for (int i=0; i<m_aiInteractionIndices.length; ++i)
         {
            VisualComponent vc = content[m_aiInteractionIndices[i]];
            Rectangle rcOldPos = vc.getClip(1.0);
            
            boolean bAddRegion = false;
            
            if (vc.IsMoveable())
            {
               if (vc.IsVisibleAt(iTimeMs))
               {
                  vc.ResetPosition();
                  bAddRegion = true;
               }
            }
            else if (vc instanceof DynamicComponent)
            {
               DynamicComponent dc = (DynamicComponent)vc;
               
               if (dc.IsVisible())
               {
                  dc.Reset();
                  bAddRegion = true;
               }
            }
            else if (vc instanceof TargetPoint)
            {
               TargetPoint tp = (TargetPoint)vc;
                  
               if (tp.IsActiveAt(iTimeMs))
               {
                  tp.SnapOut(null); // no snapped-in component anymore
               }
            }
         
            if (bAddRegion)
            {
               // Note: these regions are always added as repaint areas
               // regardless of any change (this is different from the other
               // methods here).
               
               if (rcRepaintArea == null)
                  rcRepaintArea = vc.getClip(1.0);
               else
                  rcRepaintArea.add(vc.getClip(1.0));
               
               rcRepaintArea.add(rcOldPos);
            }
         }
      }
      
      return rcRepaintArea;
   }
   
   Rectangle DeactivateVisibleDynamics(int iTimeMs)
   {
      return DeactivateVisibleDynamics(iTimeMs, true);
   }
   
   Rectangle DeactivateVisibleDynamics(int iTimeMs, boolean bQuestionOnly)
   {
      Rectangle rcRepaintArea = null;
      
      if (m_bContainsInteraction)
      {
         for (int i=0; i<m_aiInteractionIndices.length; ++i)
         {
            VisualComponent vc = content[m_aiInteractionIndices[i]];
            
            if (vc.IsVisibleAt(iTimeMs))
            {
               boolean bChange = false;
               
               if (vc instanceof InteractionArea)
               {
                  InteractionArea ivc = (InteractionArea)vc;
                  
                  if (bQuestionOnly)
                  {
                     if (ivc.IsQuestionObject())
                        bChange = ivc.DisableForever();
                  }
                  else
                     bChange = ivc.DisableForever();
               }
               else if (vc.IsMoveable())
                  bChange = vc.DisableMoving();
               else if (vc instanceof TargetPoint)
                  bChange = false;// do nothing; moveable already disabled
               // else ignore (eg changeable text)
               
               if (bChange)
               {
                  if (rcRepaintArea == null)
                     rcRepaintArea = vc.getClip(1.0);
                  else
                     rcRepaintArea.add(vc.getClip(1.0));
               }
                  
            }
           
         }
      }
       
      return rcRepaintArea;
   }
   
   void paint(Graphics g, RangeGroup ranges, double scale) 
   {
      paint(g, ranges, scale, false);
   }
   
   void paint(Graphics g, RangeGroup ranges, double scale, boolean ignorePointer) 
   {
      // paint (old and inactive) objects specified in the event queue
      
      Shape s = g.getClip();
      Rectangle toDo;
      if (s != null)
         toDo = g.getClip().getBounds();
      else
         toDo = new Rectangle(0, 0, 2000, 2000);

      synchronized(ranges)
      {
         int width = -1;
         int height = -1;
         ranges.initIterator();
         while(ranges.hasNextValue()) 
         {
            int value = ranges.nextValue();
            if (value >= 0 && value < content.length)
            {
               if (content[value].getClip(scale).intersects(toDo)) 
               {
                  if (!(ignorePointer && content[value] instanceof Pointer))
                     content[value].paint(g,width,height,scale);
               }
            }
         }
      }
   }
   
   /**
    * @param moveComp can also be null to reset it.
    */ 
   void InformMovingComponent(VisualComponent moveComp)
   {
      m_MovedComponent = moveComp;
   }
   
   void PaintInteractives(Graphics g, int iTimeMs, double dScale) 
   {
      // paint (new) interactive objects specified only in the object queue
      
      if (m_bContainsInteraction)
      {
         Shape s = g.getClip();
         Rectangle rcToDo;
         if (s != null)
            rcToDo = g.getClip().getBounds();
         else
            rcToDo = new Rectangle(0, 0, 2000, 2000);

         
         m_aToPaintTargets.clear();
         for (int i=0; i<m_aiInteractionIndices.length; ++i)
         {
            VisualComponent vc = content[m_aiInteractionIndices[i]];
            boolean bIsVisible = vc.IsVisibleAt(iTimeMs);
            if (vc.IsVisibleAt(iTimeMs) && vc.getClip(dScale).intersects(rcToDo))
            {
               if (vc instanceof TargetPoint)
                  m_aToPaintTargets.add(vc); // paint later
               else if (vc != m_MovedComponent)
                  vc.paint(g, -1, -1, dScale);
               // else will be painted on top (below)
            }
         }
         
         if (m_aToPaintTargets.size() > 0)
         {
            for (int i=0; i<m_aToPaintTargets.size(); ++i)
            {
               VisualComponent vc = (VisualComponent)m_aToPaintTargets.get(i);
               vc.paint(g, -1, -1, dScale);
            }
         }
         
         if (m_MovedComponent != null)
            m_MovedComponent.paint(g, -1, -1, dScale);
      }
   }

   boolean DoSnapIn(VisualComponent comp, Rectangle rcSource, int iTimeMs)
   {
      if (m_bContainsInteraction)
      {
         int iSnapDistance = (rcSource.width + rcSource.height) / 5;
         
         for (int i=0; i<m_aiInteractionIndices.length; ++i)
         {
            if (content[m_aiInteractionIndices[i]] instanceof TargetPoint)
            {
               TargetPoint tp = (TargetPoint)content[m_aiInteractionIndices[i]];
            
               if (tp.IsActiveAt(iTimeMs))
               {
                  if (tp.IsAttractive(rcSource, comp))
                  {
                     tp.SnapIn(comp, rcSource);
                     return true;
                  }
                  else
                  {
                     // if this component was registered unregister it
                     tp.SnapOut(comp);
                  }
               }
            }
         }
      }
      
      return false;
   }
   
   Rectangle UpdateChangeableTextsAt(int iTimeMs, int iQuestionTriesLeft, int iQuestionTimeLeft,
                                     int iQuestionNumber, int iQuestionTotal)
   {
      Rectangle rcRepaintArea = null;
      
      if (m_bContainsInteraction)
      {
         int iHours = iQuestionTimeLeft/(60*60*1000);
         iQuestionTimeLeft -= iHours * 60*60*1000;
         int iMinutes = iQuestionTimeLeft/(60*1000);
         iQuestionTimeLeft -= iMinutes * 60*1000;
         int iSeconds = iQuestionTimeLeft/1000;
         
         String strNewTime = "";
         if (iMinutes < 10)
            strNewTime += "0";
         strNewTime += iMinutes;
         strNewTime += ":";
         if (iSeconds < 10)
            strNewTime += "0";
         strNewTime += iSeconds;
         
         if (iQuestionTriesLeft < 0)
            iQuestionTriesLeft = 0;
         
         iQuestionNumber++;
          
         for (int i=0; i<m_aiInteractionIndices.length; ++i)
         {
            if (content[m_aiInteractionIndices[i]] instanceof Text)
            {
               Text text = (Text)content[m_aiInteractionIndices[i]];
               
               if (text.IsVisibleAt(iTimeMs))
               {
                  boolean bChange = false;
                  if (text.GetTextType() == Text.TEXT_TYPE_CHANGE_TIME)
                     bChange = text.ChangeText(strNewTime);
                  else if (text.GetTextType() == Text.TEXT_TYPE_CHANGE_TRIES)
                     bChange = text.ChangeText(iQuestionTriesLeft+"");
                  else if (text.GetTextType() == Text.TEXT_TYPE_CHANGE_NUMBER)
                     bChange = text.ChangeText(iQuestionNumber+"/"+iQuestionTotal);
                  
                  if (bChange)
                  {
                     Rectangle rcText = text.getClip(1.0);
                     if (rcRepaintArea == null)
                        rcRepaintArea = rcText;
                     else
                        rcRepaintArea.add(rcText);
                  }
               }
            }
         }
      }
      
      return rcRepaintArea;
   }
   
   Rectangle getClip(RangeGroup ranges, double scale)
   {
      Rectangle total = null;
      synchronized(ranges)
      {
         ranges.initIterator();
         while(ranges.hasNextValue()) {
            int idx = ranges.nextValue();
            if (idx >= 0 && idx < content.length)
            {
               Rectangle comp = content[idx].getClip(scale);
               if (total == null) total = new Rectangle(comp);
               else total.add(comp);
            }
         }
      }
      return total != null ? total : new Rectangle(0,0,-1,-1);
   }
   
   public VisualComponent[] getVisualComponents()
   {
      return content;
   }
   
   //   
   // only for easy interface for Publisher
   //
   public int[] getStopTimes()
   {
      return getStopTimes(true);
   }
   
   public int[] getStopTimes(boolean bClear)
   {
      return StopMarkReader.getStopTimes(bClear);
   }
   
   public Questionnaire[] GetQuestionnaires(boolean bClear)
   {
      return Questionnaire.GetAllQuestionnaires(bClear);
   }
   
   public int[] GetAllInteractionIndices()
   {
      return m_aiInteractionIndices;
   }
}