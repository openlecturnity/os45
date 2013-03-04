package imc.lecturnity.converter.wizard;

import javax.swing.*;

import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

import java.awt.event.*;

import java.awt.Frame;
import java.awt.Insets;
import java.awt.Container;
import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.Point;

import imc.epresenter.filesdk.util.Localizer;

import imc.lecturnity.util.wizards.ButtonPanel;
import imc.lecturnity.util.wizards.WizardButton;
import imc.lecturnity.util.wizards.WizardPanel;
import imc.lecturnity.converter.StreamingMediaConverter;

class ExtendedVideoDialog extends JFrame
{
   public static final int ACTION_CANCEL = 0;
   public static final int ACTION_OK     = 1;

   private static String CAPTION = "[!]";
   private static String VIDEOSELECT_TAB = "[!]";
   private static String VIDEOSYNC_TAB = "[!]";
   private static char   MNEM_VIDEOSELECT = '?';
   private static char   MNEM_VIDEOSYNC = '?';

   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/wizard/ExtendedVideoDialog_",
             "en");
         CAPTION = l.getLocalized("CAPTION");
         VIDEOSELECT_TAB = l.getLocalized("VIDEOSELECT_TAB");
         VIDEOSYNC_TAB = l.getLocalized("VIDEOSYNC_TAB");
         MNEM_VIDEOSELECT = l.getLocalized("MNEM_VIDEOSELECT").charAt(0);
         MNEM_VIDEOSYNC = l.getLocalized("MNEM_VIDEOSYNC").charAt(0);
      }
      catch (Exception e)
      {
         JOptionPane.showMessageDialog(null, "Could not open ExtendedVideoDialog locale database!\n"+
                                       e.toString(),
                                       "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }

   private PublisherWizardData pwData_;
   
   private JTabbedPane tabbedPane_;

   private VideoSelectWizardPanel  panelVideoSelect_;
   private VideoSyncWizardPanel    panelVideoSync_;

   private int actionTaken_ = ACTION_CANCEL;

   private Frame       parent_      = null;
   private WizardPanel parentPanel_ = null;

   private int         enabledParentButtons_ = 0;

   public ExtendedVideoDialog(Frame parent, 
                              WizardPanel parentPanel, 
                              PublisherWizardData pwData) 
   {
//       super(parent, CAPTION, true);
      super(CAPTION);

      parent_      = parent;
      parentPanel_ = parentPanel;
      pwData_      = pwData;

//       setDefaultCloseOperation(DISPOSE_ON_CLOSE);

      initGui();

      centerDialog(parent);
   }

   public void setTabIndex(int tabIndex)
   {
      tabbedPane_.setSelectedIndex(tabIndex);
   }
   
   public void checkEnabled()
   {
      boolean enableExtendedVideoSettings = true;

      enableExtendedVideoSettings = pwData_.HasNormalVideo();

      setVideoTabEnabled(enableExtendedVideoSettings);
   }
   
   public void setVideoTabEnabled(boolean isEnabled)
   {
      int idxVideo = tabbedPane_.indexOfTab(VIDEOSYNC_TAB);
      if (idxVideo >= 0)
         tabbedPane_.setEnabledAt(idxVideo, isEnabled);
   }
   
   private void initGui()
   {
      addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent e)
            {
               doCancel();
            }
         });

      if (parentPanel_ != null)
      {
         setIconImage(parentPanel_.getFrameIcon());
      
         enabledParentButtons_ = parentPanel_.getEnabledButtons();
         parentPanel_.setEnableButton(enabledParentButtons_, false);
      }
      
      Container r = getContentPane();
      r.setLayout(new BorderLayout());

      ButtonPanel bp = new ButtonPanel(false);
      ButtonListener buttonListener = new ButtonListener();
      WizardButton okButton = new WizardButton(WizardPanel.OK);
      okButton.setMnemonic(WizardPanel.MNEM_OK);
      okButton.setDefaultCapable(true);
      okButton.addActionListener(buttonListener);
      bp.addRightButton(okButton);
      bp.addRightButton(Box.createHorizontalStrut(WizardPanel.BUTTON_SPACE_X));
      WizardButton cancelButton = new WizardButton(WizardPanel.CANCEL);
      cancelButton.setMnemonic(WizardPanel.MNEM_CANCEL);
      cancelButton.addActionListener(buttonListener);
      bp.addRightButton(cancelButton);

      r.add(bp, BorderLayout.SOUTH);

      // The center panel with the tabbed pane
      JPanel center = new JPanel() { 
            public Dimension getPreferredSize() 
            { 
               return new Dimension(480, 360); 
            } 
            
            public Insets getInsets()
            {
               return new Insets(7, 7, 0, 7);
            }
         };
      center.setLayout(new BorderLayout());
      r.add(center, BorderLayout.CENTER);

      // Create a tabbed pane to hold the other panels
      tabbedPane_ = new JTabbedPane(JTabbedPane.TOP);
      
      // Add a ChangeListener to the tabbed pane
      MyChangeListener mcl = new MyChangeListener();
      tabbedPane_.addChangeListener(mcl);

      // Add the panel with the video selection
      panelVideoSelect_ = new VideoSelectWizardPanel(this, pwData_);
      tabbedPane_.addTab(VIDEOSELECT_TAB, panelVideoSelect_);
      //tabbedPane_.setMnemonicAt(0, MNEM_VIDEOSELECT);
      // Mnemonics are only supported with JDK 1.4.0 and higher
      
      // Add the panel with the video synchronisation
      panelVideoSync_ = new VideoSyncWizardPanel(this, pwData_);
      tabbedPane_.addTab(VIDEOSYNC_TAB, panelVideoSync_);
      //tabbedPane_.setMnemonicAt(1, MNEM_VIDEOSYNC);
      
      // Select the actual panel
      tabbedPane_.setSelectedIndex(pwData_.m_iVideoPanelType);

      center.add(tabbedPane_, BorderLayout.CENTER);


      // Enable/Disable the panel with the video size
      checkEnabled();
      
      setResizable(false);
      pack();
   }
   
   private void centerDialog(Frame parent)
   {
      if (parent == null)
         return;

      Point pPos = parent.getLocation();
      Dimension pDim = parent.getSize();

      Dimension myDim = getSize();
      int myX = pPos.x + (pDim.width - myDim.width)/2;
      if (myX < 0) 
         myX = 0;
      int myY = pPos.y + (pDim.height - myDim.height)/2;
      if (myY < 0)
         myY = 0;

      setLocation(new Point(myX, myY));
   }

   private void doOk()
   {
      actionTaken_ = ACTION_OK;
      
      if (panelVideoSelect_.verifyNext())
         pwData_ = panelVideoSelect_.getWizardData();
      else
         return;
         
      if (panelVideoSync_.verifyNext())
         pwData_ = panelVideoSync_.getWizardData();
      else
         return;

      pwData_.m_iVideoPanelType = tabbedPane_.getSelectedIndex();
      
      dispose();
   }

   private void doCancel()
   {
      actionTaken_ = ACTION_CANCEL;
      dispose();
   }

   public void dispose()
   {
      if (parentPanel_ != null)
      {
         parentPanel_.setEnableButton(enabledParentButtons_, true);
         parentPanel_.refreshDataDisplay();
      }
      super.dispose();
   }

   private class ButtonListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         String cmd = e.getActionCommand();
         if (cmd.equals(WizardPanel.OK))
         {
            doOk();
         }
         else if (cmd.equals(WizardPanel.CANCEL))
         {
            doCancel();
         }
//          else if (cmd.equals(WizardPanel.HELP))
//          {
//             doHelp();
//          }
      }
   }
   private class MyChangeListener implements ChangeListener
   {
      public void stateChanged(ChangeEvent e)
      {
         //System.out.println("Tabbed Pane: " + tabbedPane_.getSelectedIndex());

         // We have to check if we can leave the "select presentation" pane without errors
         // ==> call 'verifyExtensions()'
         if ( (tabbedPane_.getSelectedIndex() == 1) && (panelVideoSelect_ != null) )
         {
            if (!panelVideoSelect_.verifyExtensions())
               tabbedPane_.setSelectedIndex(0);
         }
      }
   }
}
