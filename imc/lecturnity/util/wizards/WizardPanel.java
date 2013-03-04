package imc.lecturnity.util.wizards;

/*
 * File:             WizardPanel.java
 * Code conventions: 18.09.2001
 * Author:           Martin Danielsson
 *
 * $Id: WizardPanel.java,v 1.46 2009-08-07 11:40:44 kuhn Exp $
 */

import javax.swing.*;

import java.awt.*;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

import java.io.IOException;

import imc.epresenter.filesdk.util.Localizer;
import imc.lecturnity.util.FileUtils;
import imc.lecturnity.util.ImagePanel;
import imc.lecturnity.converter.ConverterWizard;

public abstract class WizardPanel extends JPanel
{
   public static final int    BUTTON_SIZE_X  = 80;
   public static final int    BUTTON_SIZE_Y  = 24;
   public static final int    BUTTON_SPACE_X = 10;
   public static final Insets BUTTONPANEL_INSETS = new Insets(17, 15, 15, 15);

   public static final int BUTTON_NEXT   = 1;
   public static final int BUTTON_BACK   = 2;
   public static final int BUTTON_FINISH = 4;
   public static final int BUTTON_CANCEL = 8;
   public static final int BUTTON_QUIT   = 16;
   public static final int BUTTON_HELP   = 32;
   public static final int BUTTON_CUSTOM = 64;

   public static final int NEXT_BACK_CANCEL = (BUTTON_NEXT |
                                               BUTTON_BACK |
                                               BUTTON_CANCEL);
   public static final int BACK_FINISH_CANCEL = (BUTTON_BACK |
                                                 BUTTON_FINISH |
                                                 BUTTON_CANCEL);
   public static final int BACK_QUIT_CANCEL = (BUTTON_BACK |
                                               BUTTON_QUIT |
                                               BUTTON_CANCEL);

   private static Localizer localizer_;

   public static String NEXT = "Next >";
   public static String BACK = "< Back";
   public static String FINISH = "Finish";
   public static String CANCEL = "Cancel";
   public static String QUIT = "Quit";
   public static String HELP = "Help";
   public static String OK = "OK";
   public static String PROFILE_WRITE = "[!]";

   // keyboard mnemonics
   public static char MNEM_NEXT   = 'n';
   public static char MNEM_BACK   = 'b';
   public static char MNEM_FINISH = 'f';
   public static char MNEM_CANCEL = 'c';
   public static char MNEM_QUIT   = 'q';
   public static char MNEM_HELP   = 'h';
   public static char MNEM_OK = '?';

   public static Font HEADER_FONT = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.BOLD, 12);
   public static Font NORMAL_FONT = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12);

   protected static ImageIcon DIALOG_ICON = null;
   private static Image rightHeaderLogo_ = null;
   
   private static boolean USE_CUSTOM_COLORS = false;

   private Frame parentFrame_ = null;

   static
   {
      try
      {
         localizer_ = new Localizer("/imc/lecturnity/util/wizards/WizardPanel_", "en");

         NEXT = localizer_.getLocalized("BUTTON_NEXT");
         BACK = localizer_.getLocalized("BUTTON_BACK");
         FINISH = localizer_.getLocalized("BUTTON_FINISH");
         CANCEL = localizer_.getLocalized("BUTTON_CANCEL");
         QUIT = localizer_.getLocalized("BUTTON_QUIT");
         HELP = localizer_.getLocalized("BUTTON_HELP");
         OK = localizer_.getLocalized("BUTTON_OK");
         PROFILE_WRITE = localizer_.getLocalized("PROFILE_WRITE");

         MNEM_NEXT   = localizer_.getLocalized("MNEM_NEXT").charAt(0);
         MNEM_BACK   = localizer_.getLocalized("MNEM_BACK").charAt(0);
         MNEM_FINISH = localizer_.getLocalized("MNEM_FINISH").charAt(0);
         MNEM_CANCEL = localizer_.getLocalized("MNEM_CANCEL").charAt(0);
         MNEM_QUIT   = localizer_.getLocalized("MNEM_QUIT").charAt(0);
         MNEM_HELP   = localizer_.getLocalized("MNEM_HELP").charAt(0);
         MNEM_OK = localizer_.getLocalized("MNEM_OK").charAt(0);
      }
      catch (IOException e)
      {
         System.err.println("WizardPanel: Could not instanciate Localizer instance.");
         localizer_ = null;
      }
   }

   protected ButtonPanel buttonPanel_;
   protected ContentPanel rootPanel_;
   protected HeaderPanel headerPanel_;

   private WizardButton nextButton_ = null;
   private WizardButton backButton_ = null;
   private WizardButton finishButton_ = null;
   private WizardButton cancelButton_ = null;
   private WizardButton quitButton_ = null;
   private WizardButton helpButton_ = null;
   private WizardButton customButton_ = null;

   private WizardLabel headerLabel_;
   private WizardLabel subHeaderLabel_;
   private ImagePanel headerImagePanel_;

   private ActionListener buttonListener_;

   private Boolean syncObj_ = new Boolean(false);
   private int returnValue_ = BUTTON_CANCEL;
   private boolean m_bDoQuitWithoutWait = false;

   private Dimension preferredSize_ = new Dimension(400, 400);

   public WizardPanel()
   {
      super();
      
      if (DIALOG_ICON == null)
         DIALOG_ICON = new ImageIcon(getClass().getResource
                                     ("/imc/lecturnity/converter/images/LECTURNITY_Publisher.png"));

      if (rightHeaderLogo_ == null)
         rightHeaderLogo_ = (new ImageIcon(getClass().getResource
                                          ("/imc/lecturnity/converter/images/wizard_header_right.gif"))).getImage();

      setLayout(new BorderLayout());
      rootPanel_ = new ContentPanel();
      if (USE_CUSTOM_COLORS)
         rootPanel_.setBackground(Wizard.BG_COLOR);
      buttonPanel_ = new ButtonPanel();
      if (USE_CUSTOM_COLORS)
         buttonPanel_.setBackground(Wizard.BG_COLOR);

      add(rootPanel_, BorderLayout.CENTER);
      add(buttonPanel_, BorderLayout.SOUTH);

      setPreferredSize(new Dimension(400, 400));
   }
   
   public static WizardPanel createInstance(WizardData data)
      throws InstantiationException
   {
      throw new InstantiationException("WizardPanel is an abstract class: cannot be instanciated.");
   }
   
   public void refreshDataDisplay()
   {
   }

   public Image getFrameIcon()
   {
      return DIALOG_ICON.getImage();
   }

   public abstract String getDescription();
   
   public abstract WizardData getWizardData();

   public int displayWizard()
   {
      synchronized(syncObj_)
      {
         try
         {
            if (!m_bDoQuitWithoutWait)
                syncObj_.wait();
         }
         catch (InterruptedException e)
         {
            return BUTTON_CANCEL;
         }
      }

      return returnValue_; 
   }

   protected JPanel getContentPanel()
   {
      return rootPanel_;
   }

   protected void addButtons(int buttonMask)
   {
      addButtons(buttonMask, null, (char) 0, false);
   }

   protected void addButtons(int buttonMask, String strCustom)
   {
      addButtons(buttonMask | BUTTON_CUSTOM, strCustom, (char) 0, false);
   }
   
   protected void addButtons(int buttonMask, boolean bIsProfileQuit)
   {
      if (bIsProfileQuit)
         addButtons(buttonMask, null, (char) 0, true);
      else
         addButtons(buttonMask);
   }

   protected void addButtons(int buttonMask, String strCustom, char mnemonic)
   {
      addButtons(buttonMask, strCustom, mnemonic, false);
   }
   
   protected void addButtons(int buttonMask, String strCustom, char mnemonic, boolean bIsProfileQuit)
   {
      boolean addCancel = (BUTTON_CANCEL & buttonMask) > 0;
      boolean addNext = (BUTTON_NEXT & buttonMask) > 0;
      boolean addBack = (BUTTON_BACK & buttonMask) > 0;
      boolean addFinish = (BUTTON_FINISH & buttonMask) > 0;
      boolean addQuit = (BUTTON_QUIT & buttonMask) > 0;
      boolean addHelp = (BUTTON_HELP & buttonMask) > 0;
      boolean addCustom = (BUTTON_CUSTOM & buttonMask) > 0;

      buttonListener_ = new ButtonListener();

      if (addBack)
         addButton(BUTTON_BACK);
      if (addQuit)
      {
         addButton(BUTTON_QUIT, bIsProfileQuit);
      }
      if (addFinish)
         addButton(BUTTON_FINISH);
      if (addNext)
         addButton(BUTTON_NEXT);
      if (addCancel)
      {
         buttonPanel_.addRightButton(Box.createHorizontalStrut(BUTTON_SPACE_X));
         addButton(BUTTON_CANCEL);
      }

      if (addHelp)
         addButton(BUTTON_HELP);
      if (addCustom)
         addButton(BUTTON_CUSTOM, strCustom, mnemonic, null);

//       buttonPanel_.validate();
   }

   protected WizardButton getDefaultButton()
   {
      if (nextButton_ != null)
         return nextButton_;
      if (finishButton_ != null)
         return finishButton_;
      if (quitButton_ != null)
         return quitButton_;
      return null;
   }

   private void addButton(int buttonId)
   {
      addButton(buttonId, null, (char) 0, null);
   }
   
   private void addButton(int buttonId, boolean bIsProfileQuit)
   {
      String strQuit = null;
      if (bIsProfileQuit)
         strQuit = PROFILE_WRITE;
      
      addButton(buttonId, null, (char) 0, strQuit);
   }

   private void addButton(int buttonId, String strCustom, char mnemonic, String strQuit)
   {
      WizardButton button = null;

      switch(buttonId)
      {
      case BUTTON_NEXT:
         button = new WizardButton(NEXT);
         button.setFont(NORMAL_FONT);
         button.setDefaultCapable(true);
         nextButton_ = button;
         nextButton_.setMnemonic(MNEM_NEXT);
         break;

      case BUTTON_BACK:
         button = new WizardButton(BACK);
         button.setFont(NORMAL_FONT);
         backButton_ = button;
         backButton_.setMnemonic(MNEM_BACK);
         break;

      case BUTTON_FINISH:
         button = new WizardButton(FINISH);
         button.setFont(NORMAL_FONT);
         button.setDefaultCapable(true);
         finishButton_ = button;
         finishButton_.setMnemonic(MNEM_FINISH);
         break;

      case BUTTON_QUIT:
         String strToUse = QUIT;
         boolean bDifferentSize = false;
         if (strQuit != null)
         {
            strToUse = strQuit;
            bDifferentSize = true;
         }
         button = new WizardButton(strToUse);
         if (bDifferentSize)
         {
            Dimension dimText = new JButton(strToUse).getPreferredSize();
            button.setPreferredSize(new Dimension(dimText.width, button.getPreferredSize().height));
         }
         button.setFont(NORMAL_FONT);
         button.setDefaultCapable(true);
         quitButton_ = button;
         quitButton_.setMnemonic(MNEM_QUIT);
         break;

      case BUTTON_CANCEL:
         button = new WizardButton(CANCEL);
         button.setFont(NORMAL_FONT);
         cancelButton_ = button;
         cancelButton_.setMnemonic(MNEM_CANCEL);
         break;

      case BUTTON_HELP:
         button = new WizardButton(HELP);
         button.setFont(NORMAL_FONT);
         helpButton_ = button;
         helpButton_.setMnemonic(MNEM_HELP);
         break;

      case BUTTON_CUSTOM:
         button = new WizardButton(strCustom);
         button.setFont(NORMAL_FONT);
         customButton_ = button;
         if (mnemonic != 0)
            customButton_.setMnemonic(mnemonic);
         break;
      }

      if (USE_CUSTOM_COLORS)
         button.setBackground(Wizard.BG_COLOR);
      button.addActionListener(buttonListener_);
      if (buttonId != BUTTON_HELP && buttonId != BUTTON_CUSTOM)
         buttonPanel_.addRightButton(button);
      else 
         buttonPanel_.addLeftButton(button);
   }

   protected void useHeaderPanel(String header,
                                 String subHeader,
                                 String iconResource)
   {
      headerPanel_ = new HeaderPanel(header, subHeader, iconResource);
      add(headerPanel_, BorderLayout.NORTH);
   }

   public void setEnableButton(int buttonId, boolean enable)
   {
      if ((buttonId & BUTTON_NEXT) > 0)
         nextButton_.setEnabled(enable);
      if ((buttonId & BUTTON_BACK) > 0)
         backButton_.setEnabled(enable);
      if ((buttonId & BUTTON_FINISH) > 0)
         finishButton_.setEnabled(enable);
      if ((buttonId & BUTTON_CANCEL) > 0)
         cancelButton_.setEnabled(enable);
      if ((buttonId & BUTTON_QUIT) > 0)
         quitButton_.setEnabled(enable);
      if ((buttonId & BUTTON_CUSTOM) > 0)
         customButton_.setEnabled(enable);
   }

   public void setButtonVisible(int buttonId, boolean visible)
   {
      if ((buttonId & BUTTON_NEXT) > 0)
         nextButton_.setVisible(visible);
      if ((buttonId & BUTTON_BACK) > 0)
         backButton_.setVisible(visible);
      if ((buttonId & BUTTON_FINISH) > 0)
         finishButton_.setVisible(visible);
      if ((buttonId & BUTTON_CANCEL) > 0)
         cancelButton_.setVisible(visible);
      if ((buttonId & BUTTON_QUIT) > 0)
         quitButton_.setVisible(visible);
      if ((buttonId & BUTTON_CUSTOM) > 0)
         customButton_.setVisible(visible);
   }

   public void setButtonFocused(int buttonId)
   {
      if ((buttonId & BUTTON_NEXT) > 0)
         nextButton_.requestFocus();
      if ((buttonId & BUTTON_BACK) > 0)
         backButton_.requestFocus();
      if ((buttonId & BUTTON_FINISH) > 0)
         finishButton_.requestFocus();
      if ((buttonId & BUTTON_CANCEL) > 0)
         cancelButton_.requestFocus();
      if ((buttonId & BUTTON_QUIT) > 0)
         quitButton_.requestFocus();
      if ((buttonId & BUTTON_CUSTOM) > 0)
         customButton_.requestFocus();
   }

   /**
    * @return The bitmask for the currently enabled buttons.
    */
   public int getEnabledButtons()
   {
      int mask = 0;
      if (nextButton_ != null && nextButton_.isEnabled())
         mask |= BUTTON_NEXT;
      if (backButton_ != null && backButton_.isEnabled())
         mask |= BUTTON_BACK;
      if (finishButton_ != null && finishButton_.isEnabled())
         mask |= BUTTON_FINISH;
      if (cancelButton_ != null && cancelButton_.isEnabled())
         mask |= BUTTON_CANCEL;
      if (quitButton_ != null && quitButton_.isEnabled())
         mask |= BUTTON_QUIT;
      if (customButton_ != null && customButton_.isEnabled())
         mask |= BUTTON_CUSTOM;

      return mask;
   }

   /**
    * @return The bitmask for all available buttons.
    */
   public int getButtons()
   {
      int mask = 0;
      if (nextButton_ != null)
         mask |= BUTTON_NEXT;
      if (backButton_ != null)
         mask |= BUTTON_BACK;
      if (finishButton_ != null)
         mask |= BUTTON_FINISH;
      if (cancelButton_ != null)
         mask |= BUTTON_CANCEL;
      if (quitButton_ != null)
         mask |= BUTTON_QUIT;
      if (customButton_ != null)
         mask |= BUTTON_CUSTOM;

      return mask;
   }

   public void setButtonToolTip(int buttonId, String toolTip)
   {
      if ((buttonId & BUTTON_NEXT) > 0)
         nextButton_.setToolTipText(toolTip);
      if ((buttonId & BUTTON_BACK) > 0)
         backButton_.setToolTipText(toolTip);
      if ((buttonId & BUTTON_FINISH) > 0)
         finishButton_.setToolTipText(toolTip);
      if ((buttonId & BUTTON_CANCEL) > 0)
         cancelButton_.setToolTipText(toolTip);
      if ((buttonId & BUTTON_QUIT) > 0)
         quitButton_.setToolTipText(toolTip);
      if ((buttonId & BUTTON_CUSTOM) > 0)
         customButton_.setToolTipText(toolTip);
   }

//    protected void focusProceedButton()
//    {
//       if (nextButton_ != null)
//          nextButton_.requestFocus();
//       else if (finishButton_ != null)
//          finishButton_.requestFocus();
//    }

   public void setPreferredSize(Dimension preferredSize)
   {
      preferredSize_ = preferredSize;
   }

   public Dimension getPreferredSize()
   {
      return preferredSize_;
   }

   /**
    * This method is called if a user has clicked the "next" or
    * "finish" button. You can override this method in order to make a
    * check of the legality of the content of your wizard. If the
    * content is OK, this method should return <i>true</i>, and the
    * next wizard is displayed. If this method returns <i>false</i>,
    * you should have displayed a warning/error message which tells
    * the user why something is wrong. In that case, the next wizard
    * panel will <i>not</i> be displayed. As a default implementation
    * in <tt>WizardPanel</tt>, this method always returns <i>true</i>
    * (the next wizard panel is always displayed).
    */
   public boolean verifyNext()
   {
      return true;
   }

   /**
    * This method is called if a user has clicked the "quit"
    * button. You can override this method in order to make a
    * check of the legality of the content of your wizard. If the
    * content is OK, this method should return <i>true</i>, and the
    * user is allowed to quit from the wizard. If this method returns <i>false</i>,
    * you should have displayed a warning/error message which tells
    * the user why something is wrong. </p>
    * <p>As a default implementation
    * in <tt>WizardPanel</tt>, this method always returns <i>true</i>
    * (the user is always allowed to quit).
    */
   public boolean verifyQuit()
   {
      return true;
   }

   /**
    * Calling this method is like clicking the "next" button. This
    * method will call the {@link #verifyNext() verifyNext()} method
    * in order to check if the settings of the wizard panel are legal.
    */
   public void doNext()
   {
      if (verifyNext())
      {
         returnValue_ = BUTTON_NEXT;
         synchronized (syncObj_)
         {
            syncObj_.notify();
         }
      }
   }

   /**
    * Calling this method is like clicking the "finish" button. This
    * method will call the {@link #verifyNext() verifyNext()} method
    * in order to check if the settings of the wizard panel are legal.
    */
   public void doFinish()
   {
      if (verifyNext())
      {
         returnValue_ = BUTTON_FINISH;
         synchronized (syncObj_)
         {
            syncObj_.notify();
         }
      }
   }

   /**
    * Calling this method is like clicking the "Quit" button. This
    * method will call the {@link #verifyQuit() verifyQuit()} method
    * in order to check if the settings of the wizard panel are legal.
    */
   public void doQuit()
   {
      if (verifyQuit())
      {
         returnValue_ = BUTTON_QUIT;
         synchronized (syncObj_)
         {
            syncObj_.notify();
         }
      }
   }
   
   /**
    * Special case for Powertrainer mode: as there is no gui: Quit will never be clicked.
    * Used in displayWizard() above.
    */
   public void doQuitOnDoQuit() {
       m_bDoQuitWithoutWait = true;
   }

   protected void setContentPanelInsets(Insets insets)
   {
      rootPanel_.setInsets(insets);
   }

   protected void cancelWizardPanel()
   {
      returnValue_ = BUTTON_CANCEL;
      synchronized (syncObj_)
      {
         syncObj_.notify();
      }
   }

   protected Frame getFrame()
   {
      if (parentFrame_ == null)
      {
         Container c = this;
         while (!(c instanceof Frame) && (c != null))
         {
            c = c.getParent();
         }
         
         if (c != null)
            if (c instanceof Frame)
               parentFrame_ = (Frame) c;
      }

      return parentFrame_;
   }

   /**
    * This method is called shortly before a <tt>WizardPanel</tt>
    * instance is left, disregarding the way the panel is left
    * (cancel, next, quit,...). You may override it to perform any
    * tasks necessary before leaving the panel, e.g. transferring data
    * from and to a <tt>WizardData</tt> object. This method is called
    * each time a button is pressed, even if any of the
    * <tt>verifyNext()</tt> or <tt>verifyQuit()</tt> methods return
    * <I>false</i>, so you have to pay attention to what you do here.
    */
   protected void notifyLeavePanel()
   {
   }

   /**
    * This method is called if the <tt>WizardPanel</tt> has a help
    * button and if that button was pressed. It is up to you to do
    * something useful with this event.
    */
   protected void helpCommand()
   {
   }

   /**
    * This method is called if you have added a custom button to a
    * wizard panel and that button is pressed. You have to override
    * this method in order to receive those events.
    *
    * @param command the String contained in the custom button. Can
    * usually be ignored.
    */
   protected void customCommand(String command)
   {
   }

   /**
    * @return the help button (or <tt>null</tt> if not available) of
    * this wizard panel.
    */
   protected JButton getHelpButton()
   {
      return helpButton_;
   }
   
   /**
    * Look if one String is contained in an array of Strings.
    * All Strings are assumed to be Integers or Doubles in sorted order.
    *
    * @returns the index if found (a positive value) or the insert position + 1
    * as negative value; can return Integer.MIN_VALUE in case a parse error occured
    */
   protected int findArrayIndex(String[] aStrings, String toSearch)
   {
      boolean bErrorOccured = false;
      boolean bExactMatchFound = false;
      int idx = Integer.MIN_VALUE;
      
      double dToLookFor = 0;
      try { dToLookFor = Double.parseDouble(toSearch); } catch (Exception exc) { bErrorOccured = true; }
      for (int i=0; i<aStrings.length && !bErrorOccured; ++i)
      {
         double dContent = 0;
         try { dContent = Double.parseDouble(aStrings[i]); } catch (Exception exc) { bErrorOccured = true; }
         
         if (dToLookFor == dContent)
         {
            bExactMatchFound = true;
            idx = i;
            break;
         }
         else if (dToLookFor > dContent)
         {
            // nothing; continue with loop
         }
         else
         {
            idx = (i+1)*(-1);
            break;
         }
         
      }
      
      if (idx == Integer.MIN_VALUE && !bErrorOccured)
         idx = (aStrings.length+1)*(-1);
      
      if (bErrorOccured)
         return Integer.MIN_VALUE;
      else
         return idx;
   }
   

   protected boolean filenameIsOk(String fileName)
   {
      boolean allCharsOk = FileUtils.checkUrlFileName(fileName);

      if (!allCharsOk)
      {
         JOptionPane.showMessageDialog(this, localizer_.getLocalized("FILENAME_BAD_CHARS"),
                                       localizer_.getLocalized("ERROR"), JOptionPane.ERROR_MESSAGE);
      }

      return allCharsOk;
   }
   
   

   private class ContentPanel extends JPanel
   {
      private Insets insets_ = new Insets(10, 10, 10, 10);

      public Insets getInsets()
      {
         return insets_;
      }

      public void setInsets(Insets insets)
      {
         insets_ = insets;
      }
   }

   private class HeaderPanel extends JPanel
   {
      private Image image_ = null;
      private int imgWidth_   = -1;
      private int imgHeight_  = -1;
      private int logoWidth_  = -1;
      private int logoHeight_ = -1;
      private Dimension preferredSize_;
      private String header_;
      private String subHeader_;
      
      public HeaderPanel(String header, String subHeader,
                         String imageResource)
      {
         super();

         header_ = header;
         subHeader_ = subHeader;

         Dimension wizSize = WizardPanel.this.getPreferredSize();
         preferredSize_ = new Dimension(wizSize.width, 55);
         
         setBackground(Color.white);
         setLayout(null);

         if (imageResource != null)
         {
            ImageIcon iconImage = new ImageIcon
               (getClass().getResource(imageResource));
         
            image_ = iconImage.getImage();

            imgHeight_ = image_.getHeight(this);
            imgWidth_  = image_.getWidth(this);
         }

         logoWidth_ = rightHeaderLogo_.getWidth(this);
         logoHeight_ = rightHeaderLogo_.getHeight(this);

/*
         WizardLabel headerLabel = new WizardLabel(header);
         headerLabel.setFont(HEADER_FONT);
         headerLabel.setSize(wizSize.width - imgWidth_ - 10 - logoWidth_, 15);
         headerLabel.setLocation(imgWidth_ + 10, 10);
         add(headerLabel);
         
         WizardLabel subHeaderLabel = new WizardLabel(subHeader);
         subHeaderLabel.setFont(NORMAL_FONT);
         subHeaderLabel.setSize(wizSize.width - imgWidth_ + 30 - logoWidth_, 15);
         subHeaderLabel.setLocation(imgWidth_ + 10, 27);
         add(subHeaderLabel);
*/
      }

      public Dimension getPreferredSize()
      {
         return preferredSize_;
      }

      public void paint(Graphics g)
      {
         super.paint(g);

         Dimension size = getSize();
         
         g.setColor(Wizard.SHADE_COLOR);
         g.drawLine(0, size.height-2, size.width, size.height-2);
         g.setColor(Wizard.HILITE_COLOR);
         g.drawLine(0, size.height-1, size.width, size.height-1);

         int logoYSpace = (size.height - logoHeight_)/2 - 1;
         g.drawImage(rightHeaderLogo_, size.width - logoWidth_, logoYSpace, this);
         
         g.setColor(Color.black);
         g.setFont(HEADER_FONT);
         g.drawString(header_, imgWidth_ + 10, 22);
         g.setFont(NORMAL_FONT);
         g.drawString(subHeader_, imgWidth_ + 10, 39);

         if (image_ != null)
         {
            int remainder = (getPreferredSize().height - imgHeight_ - 2)/2;
            g.drawImage(image_, remainder, remainder, this);
         }
      }
   }

   private class WizardLabel extends JLabel
   {
      public WizardLabel(String label)
      {
         super(label);
      }
   }

   private class ButtonListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         // String cmd = e.getActionCommand();
         WizardButton src = (WizardButton) e.getSource();

         // if it was the help button which was pressed, then do
         // nothing except calling the "help" button
         if (src == helpButton_)
         {
            helpCommand();
            return;
         }
         else if (src == customButton_)
         {
            customCommand(customButton_.getText());
            return;
         }

         notifyLeavePanel();

         boolean displayNext = true;

         if (src == nextButton_)
         {
            displayNext = verifyNext();
            returnValue_ = BUTTON_NEXT;
         }
         else if (src == backButton_)
            returnValue_ = BUTTON_BACK;
         else if (src == cancelButton_)
            returnValue_ = BUTTON_CANCEL;
         else if (src == finishButton_)
         {
            displayNext = verifyNext();
            returnValue_ = BUTTON_FINISH;
         }
         else if (src == quitButton_)
         {
            displayNext = verifyQuit();
            returnValue_ = BUTTON_QUIT;
         }
         else
            System.err.println("WizardPanel::ButtonListener::actionPerformed() Unknown event!");

         if (displayNext)
         {
            synchronized(syncObj_)
            {
               syncObj_.notify();
            }
         }
         // else do nothing, wait for next click
      }
   }
}
