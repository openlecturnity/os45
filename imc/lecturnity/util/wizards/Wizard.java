package imc.lecturnity.util.wizards;

/*
 * File:             Wizard.java
 * Code conventions: 18.09.2001
 * Author:           Martin Danielsson
 *
 * $Id: Wizard.java,v 1.23 2010-03-02 10:09:56 kuhn Exp $
 */

import java.awt.*;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.Vector;
import javax.swing.JFrame;
import javax.swing.JButton;
import javax.swing.JOptionPane;

import imc.lecturnity.util.NativeUtils;
import imc.lecturnity.util.ui.PopupHelp;

public class Wizard extends JFrame implements ComponentListener, Runnable
{
   public static Color BG_COLOR     = javax.swing.UIManager.getColor("control");
   public static Color SHADE_COLOR  = javax.swing.UIManager.getColor("controlShadow");
   public static Color HILITE_COLOR = javax.swing.UIManager.getColor("controlLtHighlight");

   private WizardPanel nextWizard_;
   private WizardListener wizardListener_;
   private Object listenerData_;
   private Container root_;
   private boolean m_bPositionSavingActive = false;
   private long m_lLastPositionTime = 0;

   public Wizard(WizardPanel firstWizard, WizardListener wizardListener, 
                 Object listenerData, Image frameIcon)
   {
      this(firstWizard, wizardListener, 
            listenerData, frameIcon, false);
   }
   
   public Wizard(WizardPanel firstWizard, WizardListener wizardListener, 
         Object listenerData, Image frameIcon, boolean bIsPowerTrainerMode)
   {
      super(firstWizard.getDescription());

      setIconImage(frameIcon);

      wizardListener_ = wizardListener;
      listenerData_ = listenerData;

      setResizable(false);
      setLocation(100, 100);
      
      String strPosition = NativeUtils.getPublisherProperty("WindowPosition");
      if (strPosition != null && strPosition.length() > 0)
      {
         int idx = strPosition.indexOf(",");
         if (idx > -1)
         {
            try
            {
               int x = Integer.parseInt(strPosition.substring(0, idx));
               int y = Integer.parseInt(strPosition.substring(idx + 1));
               
               setLocation(x, y);
            }
            catch (NumberFormatException exc)
            {
               System.err.println("Wrong position in registry: "+strPosition);
               // do nothing
            }
         }
      }

      root_ = getContentPane();

      root_.setLayout(new BorderLayout());

      nextWizard_ = firstWizard;

      addWindowListener(new WizardWindowListener());
      addComponentListener(this);

      root_.add(nextWizard_, BorderLayout.CENTER);
      pack();
      nextWizard_.refreshDataDisplay();
      // Note: If this is called before pack() the focus might not be right;
      // at least the one in SelectActionWizardPanel.
      
      if(!bIsPowerTrainerMode)
      {
         setVisible(true);
         toFront();
      }
      
     
      Thread thread = new WizardThread();
      thread.start();
   }
   
   public void finish()
   {
      setVisible(false);
      
      nextWizard_ = null;
      wizardListener_ = null;
      listenerData_ = null;
      root_ = null;
      
      dispose();      
   }
   
   public void componentHidden(ComponentEvent e)
   {
   }
   
   public void componentMoved(ComponentEvent e)
   {
      m_lLastPositionTime = System.currentTimeMillis();
      if (!m_bPositionSavingActive)
      {
         m_bPositionSavingActive = true;
         new Thread(this).start();
         
         // Do not save the position for every event 
         // but only the "last" one in a row.
      }
   }
   
   public void componentResized(ComponentEvent e)
   {
   }
   
   public void componentShown(ComponentEvent e)
   {
   }
   
   // Delayed position saving:
   public void run()
   {
      while (System.currentTimeMillis() - m_lLastPositionTime < 1000)
      {
         try { Thread.sleep(200); } catch (InterruptedException exc) {}
      }
      
      NativeUtils.setPublisherProperty("WindowPosition", getX()+","+getY());
      m_bPositionSavingActive = false;
   }

   private class WizardThread extends Thread
   {
      public void run()
      {
         try
         {
            Vector wizardTrace = new Vector();
            
            int result = WizardEvent.CANCELLED;
            
            while (nextWizard_ != null)
            {
               JButton defaultButton = nextWizard_.getDefaultButton();
               if (defaultButton != null)
                  getRootPane().setDefaultButton(defaultButton);
               
               int action = nextWizard_.displayWizard();

               WizardData data = null;
               if (action != WizardPanel.BUTTON_CANCEL)
                  data = nextWizard_.getWizardData();

               WizardPanel lastWizard = nextWizard_;
               
               switch (action)
               {
               case WizardPanel.BUTTON_NEXT:
                  wizardTrace.addElement(lastWizard);
                  data = nextWizard_.getWizardData();
                  nextWizard_ = data.nextWizardPanel;
                  break;
                  
               case WizardPanel.BUTTON_BACK:
                  nextWizard_ = (WizardPanel) (wizardTrace.lastElement());
                  wizardTrace.removeElementAt(wizardTrace.size() - 1);
                  break;
                  
               case WizardPanel.BUTTON_FINISH:
                  wizardTrace.addElement(lastWizard);
                  data = nextWizard_.getWizardData();
                  nextWizard_ = data.finishWizardPanel;
                  break;
                  
               case WizardPanel.BUTTON_CANCEL:
                  nextWizard_ = null;
                  result = WizardEvent.CANCELLED;
                  break;
                  
               case WizardPanel.BUTTON_QUIT:
                  data = nextWizard_.getWizardData();
                  nextWizard_ = null;
                  result = WizardEvent.FINISHED;
                  break;
               }
               
               if (nextWizard_ != null)
               {
                  // close any open help popup windows:
                  PopupHelp.closeHelpWindows();

                  synchronized(Wizard.this)
                  {
                     root_.removeAll();
                     root_.add(nextWizard_, BorderLayout.CENTER);
                  }
                  pack();
                  nextWizard_.refreshDataDisplay();
                  root_.repaint();
                  nextWizard_.repaint();
               }
               else if (action != WizardPanel.BUTTON_CANCEL &&
                        action != WizardPanel.BUTTON_QUIT)
               {
                  System.err.println("!!! No next wizard panel although it is not quit or cancel.");
               }
            }

            if (wizardListener_ != null)
               wizardListener_.wizardFinished(new WizardEvent(Wizard.this,
                                                              result,
                                                              listenerData_));
            dispose();


         }
         catch (Error error)
         {
            // ooooops.
            StringWriter sw = new StringWriter();
            PrintWriter errorWriter = new PrintWriter(sw);
            error.printStackTrace(errorWriter);
            error.printStackTrace();
            String errorMsg = sw.toString();
            JOptionPane.showMessageDialog(null, errorMsg, "FATAL ERROR",
                                          JOptionPane.ERROR_MESSAGE);
            System.exit(-1);
         }
         catch (Exception error)
         {
            // ooooops.
            StringWriter sw = new StringWriter();
            PrintWriter errorWriter = new PrintWriter(sw);
            error.printStackTrace(errorWriter);
            error.printStackTrace();
            String errorMsg = sw.toString();
            JOptionPane.showMessageDialog(null, errorMsg, "FATAL EXCEPTION",
                                          JOptionPane.ERROR_MESSAGE);
            System.exit(-1);
         }
      }
   }

   private class WizardWindowListener extends WindowAdapter 
   {
      public void windowClosing(WindowEvent e)
      {
         nextWizard_.cancelWizardPanel();
      }
   }
}
