package imc.lecturnity.converter.wizard;

import javax.swing.*;
import javax.swing.border.TitledBorder;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

import java.awt.Dimension;
import java.awt.Point;
import java.awt.Font;
import java.awt.Container;
import java.awt.BorderLayout;
import java.awt.FlowLayout;
import java.awt.Frame;
import java.awt.Insets;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;

import java.io.File;

import imc.epresenter.filesdk.util.Localizer;

import imc.lecturnity.util.wizards.ButtonPanel;
import imc.lecturnity.util.wizards.WizardButton;
import imc.lecturnity.util.wizards.WizardPanel;
import imc.lecturnity.util.wizards.Wizard;

import imc.lecturnity.util.ui.PopupHelp;
import imc.lecturnity.util.ui.DirectoryChooser;

class CdProjectCacheDialog extends JDialog
{
   public static final int ACTION_CANCEL = 0;
   public static final int ACTION_OK     = 1;

   private static String CAPTION = "[!]";

   private static String LOCATION = "[!]";
   private static char   MNEM_LOCATION = '?';
   private static String PREFS = "[!]";
   private static char   MNEM_PREFS = '?';

   private static String LOC_BOX_LABEL = "[!]";
   private static String LOC_TEMP = "[!]";
   private static char   MNEM_LOC_TEMP = '?';
   private static String LOC_HOME = "[!]";
   private static char   MNEM_LOC_HOME = '?';
   private static String LOC_REGISTRY = "[!]";
   private static char   MNEM_LOC_REGISTRY = '?';

   private static String WHEN_ALWAYS = "[!]";
   private static char   MNEM_WHEN_ALWAYS = '?';
   private static String WHEN_CD = "[!]";
   private static char   MNEM_WHEN_CD = '?';
   private static String WHEN_NEVER = "[!]";
   private static char   MNEM_WHEN_NEVER = '?';
   private static String PREF_WHEN_BOX = "[!]";

   private static String DIR_BOX_LABEL = "[!]";
   private static String DIR_TEMP = "[!]";
   private static char   MNEM_DIR_TEMP = '?';
   private static String DIR_CUSTOM = "[!]";
   private static char   MNEM_DIR_CUSTOM = '?';
   private static String FIND = "[!]";
   private static char   MNEM_FIND = '?';

   private static String SIZE_BOX_LABEL = "[!]";
   private static String MB = "[!]";

   private static String HELP_CACHE_WHERE = "[!]";
   private static String HELP_CACHE_WHEN = "[!]";
   private static String HELP_CACHE_SIZE = "[!]";
   private static String HELP_CACHE_DIR = "[!]";
   private static String HELP_NOTE = "[!]";
   private static String HELP_NOTES = "[!]";

   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/wizard/CdProjectCacheDialog_",
             "en");

         CAPTION = l.getLocalized("CAPTION");
         LOCATION = l.getLocalized("LOCATION");
         MNEM_LOCATION = l.getLocalized("MNEM_LOCATION").charAt(0);
         PREFS = l.getLocalized("PREFS");
         MNEM_PREFS = l.getLocalized("MNEM_PREFS").charAt(0);

         LOC_BOX_LABEL = l.getLocalized("LOC_BOX_LABEL");
         LOC_TEMP = l.getLocalized("LOC_TEMP");
         MNEM_LOC_TEMP = l.getLocalized("MNEM_LOC_TEMP").charAt(0);
         LOC_HOME = l.getLocalized("LOC_HOME");
         MNEM_LOC_HOME = l.getLocalized("MNEM_LOC_HOME").charAt(0);
         LOC_REGISTRY = l.getLocalized("LOC_REGISTRY");
         MNEM_LOC_REGISTRY = l.getLocalized("MNEM_LOC_REGISTRY").charAt(0);

         PREF_WHEN_BOX = l.getLocalized("PREF_WHEN_BOX");
         WHEN_ALWAYS = l.getLocalized("WHEN_ALWAYS");
         MNEM_WHEN_ALWAYS    = l.getLocalized("MNEM_WHEN_ALWAYS").charAt(0);
         WHEN_CD = l.getLocalized("WHEN_CD");
         MNEM_WHEN_CD = l.getLocalized("MNEM_WHEN_CD").charAt(0);
         WHEN_NEVER = l.getLocalized("WHEN_NEVER");
         MNEM_WHEN_NEVER = l.getLocalized("MNEM_WHEN_NEVER").charAt(0);

         DIR_BOX_LABEL = l.getLocalized("DIR_BOX_LABEL");
         DIR_TEMP = l.getLocalized("DIR_TEMP");
         MNEM_DIR_TEMP = l.getLocalized("MNEM_DIR_TEMP").charAt(0);
         DIR_CUSTOM = l.getLocalized("DIR_CUSTOM");
         MNEM_DIR_CUSTOM = l.getLocalized("MNEM_DIR_CUSTOM").charAt(0);
         FIND = l.getLocalized("FIND");
         MNEM_FIND = l.getLocalized("MNEM_FIND").charAt(0);

         SIZE_BOX_LABEL = l.getLocalized("SIZE_BOX_LABEL");
         MB = l.getLocalized("MB");

         HELP_CACHE_WHERE = l.getLocalized("HELP_CACHE_WHERE");
         HELP_CACHE_WHEN = l.getLocalized("HELP_CACHE_WHEN");
         HELP_CACHE_SIZE = l.getLocalized("HELP_CACHE_SIZE");
         HELP_CACHE_DIR = l.getLocalized("HELP_CACHE_DIR");
         HELP_NOTE = l.getLocalized("HELP_NOTE");
         HELP_NOTES = l.getLocalized("HELP_NOTES");
      }
      catch (Exception e)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database!\n" +
                                       "CdProjectCacheDialog_\n" + 
                                       e.getMessage(),
                                       "Error", JOptionPane.ERROR_MESSAGE);
         e.printStackTrace();
         System.exit(1);
      }
   }

   private CdProjectWizardData data_ = null;
   private int actionTaken_          = ACTION_CANCEL;

   private JTabbedPane  tabbedPane_    = null;

   private JRadioButton tempRadio_     = null;
   private JRadioButton homeRadio_     = null;
   private JRadioButton registryRadio_ = null;

   private JRadioButton cacheAlwaysRadio_ = null;
   private JRadioButton cacheCdRadio_     = null;
   private JRadioButton cacheNeverRadio_  = null;

   private JRadioButton dirTempRadio_   = null;
   private JRadioButton dirCustomRadio_ = null;
   private JTextField   dirField_       = null;
   private JButton      findButton_     = null;
   
   private PopupHelp    helpPopup_      = null;

   private JSlider    cacheSizeSlider_ = null;
   private JTextField cacheSizeField_  = null;

   private String     helpTextSave_    = "";
   private String     helpTextDefault_ = "";

   public CdProjectCacheDialog(Frame parent, CdProjectWizardData data)
   {
      // this is a modal dialog, so we'll pass "true" as third parameter
      super(parent, CAPTION, true);
      data_ = data;

      setDefaultCloseOperation(DISPOSE_ON_CLOSE);

      initGui();
      fillGui();
      checkDirEnable();
      centerDialog(parent);
   }
   
   private void initGui()
   {
      Container r = getContentPane();
      r.setLayout(new BorderLayout());

      // Init some help texts:
      helpTextSave_    = "<html>" + HELP_CACHE_WHERE + "<br>" + HELP_NOTE + "</html>";
      helpTextDefault_ = "<html>" + HELP_CACHE_WHEN + "</font><br>" + 
         HELP_CACHE_DIR + "</font><br>" +
         HELP_CACHE_SIZE + "<br>" + HELP_NOTES + "</html>";

      String helpCacheWhere = helpTextSave_;
      String helpCacheWhen  = "<html>" + HELP_CACHE_WHEN + "<br>" + HELP_NOTE + "</html>";
      String helpCacheDir   = "<html>" + HELP_CACHE_DIR + "<br>" + HELP_NOTE + "</html>";
      String helpCacheSize  = "<html>" + HELP_CACHE_SIZE + "<br>" + HELP_NOTE + "</html>";

      //       JButton b = new JButton("hallo");
      //       b.addActionListener(new ActionListener() { public void actionPerformed(ActionEvent e) { dispose(); } });
      //       r.add(b, BorderLayout.CENTER);

      // The buttons:

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

      JButton helpButton = new WizardButton(WizardPanel.HELP);
      helpButton.setMnemonic(WizardPanel.MNEM_HELP);
      helpButton.addActionListener(buttonListener);
      bp.addLeftButton(helpButton);
      helpPopup_ = new PopupHelp(helpButton, helpTextSave_);

      r.add(bp, BorderLayout.SOUTH);

      // The center panel with the tabbed pane

      JPanel center = new JPanel() { 
            public Dimension getPreferredSize() 
            { 
               return new Dimension(420, 360); 
            } 
            
            public Insets getInsets()
            {
               return new Insets(7, 7, 0, 7);
            }
         };
      center.setLayout(new BorderLayout());
      r.add(center, BorderLayout.CENTER);

      tabbedPane_ = new JTabbedPane(JTabbedPane.TOP);

      // Save location panel

      JPanel locationPanel = new JPanel() 
         {
            public Insets getInsets() { return new Insets(10, 10, 10, 10); }
         };
      locationPanel.setLayout(new BorderLayout());
      Box prefsPanel    = new Box(BoxLayout.Y_AXIS)
         {
            public Insets getInsets() { return new Insets(10, 10, 10, 10); }
         };

      tabbedPane_.addTab(LOCATION, locationPanel);
      tabbedPane_.addTab(PREFS, prefsPanel);
      tabbedPane_.addChangeListener(new PaneListener());

      JPanel saveBox = new JPanel();
      saveBox.setLayout(null);
      saveBox.setPreferredSize(new Dimension(380, 130));
      saveBox.setBorder(new TitledBorder(" " + LOC_BOX_LABEL + " "));
      new PopupHelp(saveBox, helpCacheWhere);

      ButtonGroup bg = new ButtonGroup();
      tempRadio_ = new JRadioButton(LOC_TEMP);
      tempRadio_.setMnemonic(MNEM_LOC_TEMP);
      tempRadio_.setSize(340, 20);
      tempRadio_.setLocation(20, 30);
      new PopupHelp(tempRadio_, helpCacheWhere);
      bg.add(tempRadio_);
      saveBox.add(tempRadio_);

      homeRadio_ = new JRadioButton(LOC_HOME);
      homeRadio_.setMnemonic(MNEM_LOC_HOME);
      homeRadio_.setSize(340, 20);
      homeRadio_.setLocation(20, 60);
      new PopupHelp(homeRadio_, helpCacheWhere);
      bg.add(homeRadio_);
      saveBox.add(homeRadio_);
      
      registryRadio_ = new JRadioButton(LOC_REGISTRY);
      registryRadio_.setMnemonic(MNEM_LOC_REGISTRY);
      registryRadio_.setSize(340, 20);
      registryRadio_.setLocation(20, 90);
      new PopupHelp(registryRadio_, helpCacheWhere);
      bg.add(registryRadio_);
      saveBox.add(registryRadio_);

      locationPanel.add(saveBox, BorderLayout.NORTH);

      // Preferences panel

      JPanel whenPanel = new JPanel();
      whenPanel.setLayout(null);
      whenPanel.setPreferredSize(new Dimension(380, 130));
      whenPanel.setBorder(new TitledBorder(" " + PREF_WHEN_BOX + " "));
      new PopupHelp(whenPanel, helpCacheWhen);

      bg = new ButtonGroup();
      cacheAlwaysRadio_ = new JRadioButton(WHEN_ALWAYS, false);
      cacheAlwaysRadio_.setMnemonic(MNEM_WHEN_ALWAYS);
      cacheAlwaysRadio_.setSize(340, 20);
      cacheAlwaysRadio_.setLocation(20, 30);
      new PopupHelp(cacheAlwaysRadio_, helpCacheWhen);
      bg.add(cacheAlwaysRadio_);
      whenPanel.add(cacheAlwaysRadio_);

      cacheCdRadio_ = new JRadioButton(WHEN_CD, false);
      cacheCdRadio_.setMnemonic(MNEM_WHEN_CD);
      cacheCdRadio_.setSize(340, 20);
      cacheCdRadio_.setLocation(20, 60);
      new PopupHelp(cacheCdRadio_, helpCacheWhen);
      bg.add(cacheCdRadio_);
      whenPanel.add(cacheCdRadio_);

      cacheNeverRadio_ = new JRadioButton(WHEN_NEVER, false);
      cacheNeverRadio_.setMnemonic(MNEM_WHEN_NEVER);
      cacheNeverRadio_.setSize(340, 20);
      cacheNeverRadio_.setLocation(20, 90);
      new PopupHelp(cacheNeverRadio_, helpCacheWhen);
      bg.add(cacheNeverRadio_);
      whenPanel.add(cacheNeverRadio_);

      prefsPanel.add(whenPanel);

      // Directory
      JPanel dirPanel = new JPanel();
      dirPanel.setLayout(null);
      dirPanel.setPreferredSize(new Dimension(380, 100));
      dirPanel.setBorder(new TitledBorder(" " + DIR_BOX_LABEL + " "));
      new PopupHelp(dirPanel, helpCacheDir);

      RadioListener radioListener = new RadioListener();

      bg = new ButtonGroup();
      dirTempRadio_ = new JRadioButton(DIR_TEMP);
      dirTempRadio_.setMnemonic(MNEM_DIR_TEMP);
      dirTempRadio_.setSize(340, 20);
      dirTempRadio_.setLocation(20, 30);
      dirTempRadio_.addActionListener(radioListener);
      new PopupHelp(dirTempRadio_, helpCacheDir);
      bg.add(dirTempRadio_);
      dirPanel.add(dirTempRadio_);

      dirCustomRadio_ = new JRadioButton(DIR_CUSTOM);
      dirCustomRadio_.setMnemonic(MNEM_DIR_CUSTOM);
      dirCustomRadio_.setSize(70, 20);
      dirCustomRadio_.setLocation(20, 60);
      dirCustomRadio_.addActionListener(radioListener);
      new PopupHelp(dirCustomRadio_, helpCacheDir);
      bg.add(dirCustomRadio_);
      dirPanel.add(dirCustomRadio_);

      dirField_ = new JTextField();
      dirField_.setSize(173, 22);
      dirField_.setLocation(90, 59);
      new PopupHelp(dirField_, helpCacheDir);
      dirPanel.add(dirField_);

      findButton_ = new JButton(FIND) { 
            public Insets getInsets() { return new Insets(1, 1, 1, 1); }
         };
      findButton_.setMnemonic(MNEM_FIND);
      findButton_.setSize(95, WizardPanel.BUTTON_SIZE_Y);
      findButton_.setLocation(270, 60 - (WizardPanel.BUTTON_SIZE_Y - 20)/2);
      findButton_.addActionListener(buttonListener);
      dirPanel.add(findButton_);

      prefsPanel.add(dirPanel);

      // Size panel

      JPanel sizePanel = new JPanel();
      sizePanel.setLayout(null);
      sizePanel.setPreferredSize(new Dimension(380, 70));
      sizePanel.setBorder(new TitledBorder(" " + SIZE_BOX_LABEL + " "));
      new PopupHelp(sizePanel, helpCacheSize);
      prefsPanel.add(sizePanel);

      cacheSizeSlider_ = new JSlider(JSlider.HORIZONTAL, 0, 2048, 250);
      cacheSizeSlider_.setSize(200, 40);
      cacheSizeSlider_.setLocation(60, 25);
      //cacheSizeSlider_.setLabelTable(cacheSizeSlider_.createStandardLabels(1000, 0));
      //cacheSizeSlider_.setPaintLabels(true);
      cacheSizeSlider_.setMajorTickSpacing(1000);
      cacheSizeSlider_.setMinorTickSpacing(100);
      cacheSizeSlider_.setPaintTicks(true);
      cacheSizeSlider_.addChangeListener(new SliderListener());
      new PopupHelp(cacheSizeSlider_, helpCacheSize);
      sizePanel.add(cacheSizeSlider_);

      cacheSizeField_ = new JTextField();
      cacheSizeField_.setSize(50, 22);
      cacheSizeField_.setLocation(267, 34);
      cacheSizeField_.addKeyListener(new SizeFieldListener());
      new PopupHelp(cacheSizeField_, helpCacheSize);
      sizePanel.add(cacheSizeField_);
      
      JLabel mbLabel = new JLabel(MB);
      mbLabel.setSize(40, 20);
      mbLabel.setLocation(324, 35);
      sizePanel.add(mbLabel);
      
      JPanel glue = new JPanel();
      glue.setLayout(new BorderLayout());
      prefsPanel.add(glue);

      center.add(tabbedPane_, BorderLayout.CENTER);

      setResizable(false);
      pack();
   }

   private void fillGui()
   {
      switch (data_.cacheSettings)
      {
      case CdProjectWizardData.CACHE_SETTINGS_HOME:
         tempRadio_.setSelected(false);
         homeRadio_.setSelected(true);
         registryRadio_.setSelected(false);
         break;

      case CdProjectWizardData.CACHE_SETTINGS_REGISTRY:
         tempRadio_.setSelected(false);
         homeRadio_.setSelected(false);
         registryRadio_.setSelected(true);
         break;

      case CdProjectWizardData.CACHE_SETTINGS_TEMP:
      default:
         tempRadio_.setSelected(true);
         homeRadio_.setSelected(false);
         registryRadio_.setSelected(false);
         break;
      }

      switch (data_.cacheWhen)
      {
      case CdProjectWizardData.CACHE_WHEN_CD:
         cacheAlwaysRadio_.setSelected(false);
         cacheCdRadio_.setSelected(true);
         cacheNeverRadio_.setSelected(false);
         break;

      case CdProjectWizardData.CACHE_WHEN_NEVER:
         cacheAlwaysRadio_.setSelected(false);
         cacheCdRadio_.setSelected(false);
         cacheNeverRadio_.setSelected(true);
         break;

      case CdProjectWizardData.CACHE_WHEN_ALWAYS:
      default:
         cacheAlwaysRadio_.setSelected(true);
         cacheCdRadio_.setSelected(false);
         cacheNeverRadio_.setSelected(false);
         break;
      }

      switch (data_.cacheWhere)
      {
      case CdProjectWizardData.CACHE_WHERE_CUSTOM:
         dirTempRadio_.setSelected(false);
         dirCustomRadio_.setSelected(true);
         break;
         
      case CdProjectWizardData.CACHE_WHERE_TEMP:
      default:
         dirTempRadio_.setSelected(true);
         dirCustomRadio_.setSelected(false);
         break;
      }

      dirField_.setText(data_.cacheDirectory);
      cacheSizeField_.setText("" + data_.cacheSize);
      cacheSizeSlider_.setValue(data_.cacheSize);
   }

   public int getActionTaken()
   {
      return actionTaken_;
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

   private void findDirectory()
   {
      DirectoryChooser dirChooser = new DirectoryChooser(this);

      if (!dirField_.getText().equals(""))
         dirChooser.setSelectedDir(new File(dirField_.getText()));
      
      int result = dirChooser.showDialog();
      if (result == DirectoryChooser.OPTION_APPROVE)
      {
         dirField_.setText(dirChooser.getSelectedDir().toString());
      }
   }

   private void tryOk()
   {
      if (homeRadio_.isSelected())
         data_.cacheSettings = CdProjectWizardData.CACHE_SETTINGS_HOME;
      else if (registryRadio_.isSelected())
         data_.cacheSettings = CdProjectWizardData.CACHE_SETTINGS_REGISTRY;
      else
         data_.cacheSettings = CdProjectWizardData.CACHE_SETTINGS_TEMP;

      if (cacheCdRadio_.isSelected())
         data_.cacheWhen = CdProjectWizardData.CACHE_WHEN_CD;
      else if (cacheNeverRadio_.isSelected())
         data_.cacheWhen = CdProjectWizardData.CACHE_WHEN_NEVER;
      else
         data_.cacheWhen = CdProjectWizardData.CACHE_WHEN_ALWAYS;

      if (dirCustomRadio_.isSelected())
         data_.cacheWhere = CdProjectWizardData.CACHE_WHERE_CUSTOM;
      else
         data_.cacheWhere = CdProjectWizardData.CACHE_WHERE_TEMP;

      data_.cacheDirectory = dirField_.getText();
      data_.cacheSize = cacheSizeSlider_.getValue();
      
      actionTaken_ = ACTION_OK;
      PopupHelp.closeHelpWindows();
      dispose();
   }

   private void checkDirEnable()
   {
      boolean enableDir = (dirCustomRadio_.isSelected());
      dirField_.setEnabled(enableDir);
      findButton_.setEnabled(enableDir);
   }

   private class ButtonListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         String cmd = e.getActionCommand();

         if (cmd.equals(WizardPanel.OK))
         {
            tryOk();
//             actionTaken_ = ACTION_OK;
//             dispose();
         }
         else if (cmd.equals(WizardPanel.CANCEL))
         {
            actionTaken_ = ACTION_CANCEL;
            PopupHelp.closeHelpWindows();
            dispose();
         }
         else if (cmd.equals(FIND))
         {
            findDirectory();
         }
         else if (cmd.equals(WizardPanel.HELP))
         {
            if (!PopupHelp.isHelpVisible())
               helpPopup_.popupHelp();
            else
               helpPopup_.closeHelpWindows();
         }
      }
   }

   private class RadioListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         checkDirEnable();
      }
   }

   private class SliderListener implements ChangeListener
   {
      public void stateChanged(ChangeEvent e)
      {
         try
         {
            int v = Integer.parseInt(cacheSizeField_.getText());
            if (cacheSizeSlider_.getValue() != v)
               cacheSizeField_.setText("" + cacheSizeSlider_.getValue());
         }
         catch (Exception ex)
         {
            // ignore for now
         }
      }
   }

   private class SizeFieldListener extends KeyAdapter
   {
      public void keyReleased(KeyEvent e)
      {
         try
         {
            int v = Integer.parseInt(cacheSizeField_.getText());
            if (v < 0) v = 0;
            if (v > 2048) v = 2048;
            cacheSizeSlider_.setValue(v);
         }
         catch (Exception ex)
         {
            // do nothing.
         }
      }
   }

   private class PaneListener implements ChangeListener
   {
      public void stateChanged(ChangeEvent e)
      {
         String title = tabbedPane_.getTitleAt(tabbedPane_.getSelectedIndex());
         if (title.equals(LOCATION))
            helpPopup_.setText(helpTextSave_);
         else
            helpPopup_.setText(helpTextDefault_);
      }
   }
}
