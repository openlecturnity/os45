package imc.lecturnity.converter.wizard;

// NOTE: Do not write "import java.awt.*"!!! 
// (java.util.List is already imported!).
import java.awt.Dimension;
import java.awt.Container;
import java.awt.Point;
import java.awt.Font;
import java.awt.Color;
import java.awt.Insets;

import java.awt.dnd.*;
import java.awt.datatransfer.*;

import java.awt.event.*;

import javax.swing.*;
import javax.swing.border.BevelBorder;
import javax.swing.filechooser.FileFilter;

import javax.swing.event.ListSelectionListener;
import javax.swing.event.ListSelectionEvent;

import java.io.IOException;
import java.io.File;
import java.io.InputStream;
import java.io.FileInputStream;
import java.io.DataInputStream;

import java.util.Vector;
import java.util.List;

import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioFileFormat;

import imc.lecturnity.converter.ConverterWizard;

import imc.lecturnity.util.wizards.*;
import imc.lecturnity.util.ui.CustomJList;
import imc.lecturnity.util.ui.IconListCellRenderer;
import imc.lecturnity.util.ColorManager;
import imc.lecturnity.util.NativeUtils;
import imc.lecturnity.util.FileUtils;

import imc.epresenter.filesdk.FileResources;
import imc.epresenter.filesdk.util.Localizer;
import imc.epresenter.filesdk.util.XmlParser;
import imc.epresenter.filesdk.util.XmlFormatException;

class CdProjectDocsWizardPanel extends AbstractCdProjectWizardPanel
{
   private static String HEADER = "[!]";
   private static String SUBHEADER = "[!]";
   private static String ADD = "[!]";
   private static String ADD_LABEL = "[!]";
   private static char   MNEM_ADD = '?';
   private static String REMOVE = "[!]";
   private static String REMOVE_LABEL = "[!]";
   private static char   MNEM_REMOVE = '?';
   private static String UP = "[!]";
   private static String UP_LABEL = "[!]";
   private static char   MNEM_UP = '?';
   private static String DOWN = "[!]";
   private static String DOWN_LABEL = "[!]";
   private static char   MNEM_DOWN = '?';

   private static String MENU_REFRESH = "[!]";
   private static String MENU_ADD = "[!]";
   private static String MENU_REMOVE = "[!]";
   private static String MENU_UP = "[!]";
   private static String MENU_DOWN = "[!]";

   private static String LPD_DESC = "[!]";
   private static String ERR_NO_LPD = "[!]";
   private static String LPD_EXISTS = "[!]";
   private static String INFORMATION = "[!]";
   private static String LPD_NAME_EXISTS = "[!]";
   private static String LPD_EVAL_TOO_LONG = "[!]";
   private static String LPD_EVAL_CHECK_FAIL = "[!]";

   
   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/wizard/CdProjectDocsWizardPanel_",
             "en");

         HEADER = l.getLocalized("HEADER");
         SUBHEADER = l.getLocalized("SUBHEADER");
         ADD = l.getLocalized("ADD");
         ADD_LABEL = l.getLocalized("ADD_LABEL");
         MNEM_ADD = l.getLocalized("MNEM_ADD").charAt(0);
         REMOVE = l.getLocalized("REMOVE");
         REMOVE_LABEL = l.getLocalized("REMOVE_LABEL");
         MNEM_REMOVE = l.getLocalized("MNEM_REMOVE").charAt(0);
         UP = l.getLocalized("UP");
         UP_LABEL = l.getLocalized("UP_LABEL");
         MNEM_UP = l.getLocalized("MNEM_UP").charAt(0);
         DOWN = l.getLocalized("DOWN");
         DOWN_LABEL = l.getLocalized("DOWN_LABEL");
         MNEM_DOWN = l.getLocalized("MNEM_DOWN").charAt(0);
   
         MENU_REFRESH = l.getLocalized("MENU_REFRESH");
         MENU_ADD = l.getLocalized("MENU_ADD");
         MENU_REMOVE = l.getLocalized("MENU_REMOVE");
         MENU_UP = l.getLocalized("MENU_UP");
         MENU_DOWN = l.getLocalized("MENU_DOWN");

         LPD_DESC = l.getLocalized("LPD_DESC");
         ERR_NO_LPD = l.getLocalized("ERR_NO_LPD");
         LPD_EXISTS = l.getLocalized("LPD_EXISTS");
         INFORMATION = l.getLocalized("INFORMATION");
         LPD_NAME_EXISTS = l.getLocalized("LPD_NAME_EXISTS");
         LPD_EVAL_TOO_LONG = l.getLocalized("LPD_EVAL_TOO_LONG");
         LPD_EVAL_CHECK_FAIL = l.getLocalized("LPD_EVAL_CHECK_FAIL");
      }
      catch (IOException e)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database!",
                                       "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }

   private WizardPanel nextWizard_ = null;

   private DefaultListModel docsListModel_;
   private JList docsList_;

   private JButton addButton_, removeButton_, upButton_, downButton_;

   private JLabel totalSizeLabel_;

   private JPopupMenu popupMenu_ = null;
   private JMenuItem refreshItem_, addItem_, removeItem_, upItem_, downItem_;

   private File lastDirectory_;

   public CdProjectDocsWizardPanel(WizardData data)
   {
      super(data);

//       System.out.println("constructor CdProjectDocsWizardPanel() -->");

      setPreferredSize(new Dimension(500, 400));

//       addButtons(BUTTON_BACK | BUTTON_FINISH | BUTTON_QUIT | BUTTON_CANCEL);
      addButtons(BUTTON_BACK | BUTTON_QUIT | BUTTON_NEXT | BUTTON_CANCEL);

      setButtonToolTip(BUTTON_QUIT, TT_QUIT);
      setButtonToolTip(BUTTON_NEXT, TT_NEXT);
      setButtonToolTip(BUTTON_CANCEL, TT_CANCEL);

      useHeaderPanel(HEADER, SUBHEADER,
                     "/imc/lecturnity/converter/images/lecturnity_logo.gif");

      Container r = getContentPanel();
      r.setLayout(null);

      Font headerFont = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.BOLD, 12);
      Font descFont   = imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12);

      docsListModel_ = new DefaultListModel();
      for (int i=0; i<cdData.projectDocs.size(); i++)
      {
//          System.out.println("adding " + cdData.projectDocs.elementAt(i));
         docsListModel_.addElement(cdData.projectDocs.elementAt(i));
      }

      docsList_ = new CustomJList(docsListModel_);
      docsList_.setCellRenderer(new IconListCellRenderer
         ("/imc/lecturnity/converter/images/icon_dok.gif"));
      docsList_.addListSelectionListener(new DocsSelectionListener());
      docsList_.addMouseListener(new ListMouseListener());
      docsList_.addKeyListener(new ListKeyListener());
      new DropTarget(docsList_, new DnDListener());

      JScrollPane listScroller = new JScrollPane(docsList_);
      listScroller.setBorder(new BevelBorder(BevelBorder.LOWERED));
      listScroller.setSize(440, 190);
      listScroller.setLocation(30, 30);
      
      r.add(listScroller);

      ButtonListener buttonListener = new ButtonListener();

      int bHeight = 24;
      int bWidth  = 90;
      int bSpace  = 5;
      int bOffset = 30;

      addButton_ = new JButton(ADD_LABEL) { public Insets getInsets() { return new Insets(1, 1, 1, 1); } };
      addButton_.setSize(bWidth, bHeight);
      addButton_.setLocation(bOffset + 0, 225);
      addButton_.setMnemonic(MNEM_ADD);
      addButton_.setToolTipText(ADD);
      addButton_.setEnabled(true);
      addButton_.setActionCommand(MENU_ADD);
      addButton_.addActionListener(buttonListener);
      r.add(addButton_);

      removeButton_ = new JButton(REMOVE_LABEL) { public Insets getInsets() { return new Insets(1, 1, 1, 1); } };
      removeButton_.setSize(bWidth, bHeight);
      removeButton_.setLocation(bOffset + (bWidth + bSpace), 225);
      removeButton_.setMnemonic(MNEM_REMOVE);
      removeButton_.setToolTipText(REMOVE);
      removeButton_.setEnabled(false);
      removeButton_.setActionCommand(MENU_REMOVE);
      removeButton_.addActionListener(buttonListener);
      r.add(removeButton_);

      upButton_ = new JButton(UP_LABEL) { public Insets getInsets() { return new Insets(1, 1, 1, 1); } };
      upButton_.setSize(bWidth, bHeight);
      upButton_.setLocation(bOffset + 2*(bWidth + bSpace), 225);
      upButton_.setMnemonic(MNEM_UP);
      upButton_.setToolTipText(UP);
      upButton_.setEnabled(false);
      upButton_.setActionCommand(MENU_UP);
      upButton_.addActionListener(buttonListener);
      r.add(upButton_);

      downButton_ = new JButton(DOWN_LABEL) { public Insets getInsets() { return new Insets(1, 1, 1, 1); } };
      downButton_.setSize(bWidth, bHeight);
      downButton_.setLocation(bOffset + 3*(bWidth + bSpace), 225);
      downButton_.setMnemonic(MNEM_DOWN);
      downButton_.setToolTipText(DOWN);
      downButton_.setEnabled(false);
      downButton_.setActionCommand(MENU_DOWN);
      downButton_.addActionListener(buttonListener);
      r.add(downButton_);

//       addButton_ = new FlatButton
//          (new ImageIcon
//             (getClass().getResource
//              ("/imc/lecturnity/converter/images/add_button.png")),
//           MENU_ADD);
//       addButton_.setSize(40, 30);
//       int xOff = 162;
//       addButton_.setLocation(xOff + 0, 225);
//       addButton_.setToolTipText(ADD);
//       addButton_.setDisabledIcon(new ImageIcon
//          (getClass().getResource
//           ("/imc/lecturnity/converter/images/add_button_disabled.png")));
//       addButton_.setEnabled(true);
//       addButton_.addActionListener(buttonListener);
//       r.add(addButton_);

//       removeButton_ = new FlatButton
//          (new ImageIcon
//             (getClass().getResource
//              ("/imc/lecturnity/converter/images/remove_button.png")),
//           MENU_REMOVE);
//       removeButton_.setSize(40, 30);
//       removeButton_.setLocation(xOff + 45, 225);
//       removeButton_.setToolTipText(REMOVE);
//       removeButton_.setDisabledIcon(new ImageIcon
//          (getClass().getResource
//           ("/imc/lecturnity/converter/images/remove_button_disabled.png")));
//       removeButton_.setEnabled(false);
//       removeButton_.addActionListener(buttonListener);
//       r.add(removeButton_);

//       upButton_ = new FlatButton
//          (new ImageIcon
//             (getClass().getResource
//              ("/imc/lecturnity/converter/images/up_button.png")),
//           MENU_UP);
//       upButton_.setSize(40, 30);
//       upButton_.setLocation(xOff + 90, 225);
//       upButton_.setToolTipText(UP);
//       upButton_.setDisabledIcon(new ImageIcon
//          (getClass().getResource
//           ("/imc/lecturnity/converter/images/up_button_disabled.png")));
//       upButton_.setEnabled(false);
//       upButton_.addActionListener(buttonListener);
//       r.add(upButton_);

//       downButton_ = new FlatButton
//          (new ImageIcon
//             (getClass().getResource
//              ("/imc/lecturnity/converter/images/down_button.png")),
//           MENU_DOWN);
//       downButton_.setSize(40, 30);
//       downButton_.setLocation(xOff + 135, 225);
//       downButton_.setToolTipText(DOWN);
//       downButton_.setDisabledIcon(new ImageIcon
//          (getClass().getResource
//           ("/imc/lecturnity/converter/images/down_button_disabled.png")));
//       downButton_.setEnabled(false);
//       downButton_.addActionListener(buttonListener);
//       r.add(downButton_);

      totalSizeLabel_ = new JLabel("0 MB", JLabel.RIGHT);
      totalSizeLabel_.setFont(descFont);
      totalSizeLabel_.setSize(100, 22);
      totalSizeLabel_.setLocation(370, 230);
      r.add(totalSizeLabel_);

      // init the popup menu
      popupMenu_ = new JPopupMenu();
      refreshItem_ = popupMenu_.add(MENU_REFRESH);
      refreshItem_.addActionListener(buttonListener);
      popupMenu_.addSeparator();
      addItem_ = popupMenu_.add(MENU_ADD);
      addItem_.addActionListener(buttonListener);
      removeItem_ = popupMenu_.add(MENU_REMOVE);
      removeItem_.addActionListener(buttonListener);
      upItem_ = popupMenu_.add(MENU_UP);
      upItem_.addActionListener(buttonListener);
      downItem_ = popupMenu_.add(MENU_DOWN);
      downItem_.addActionListener(buttonListener);

//       System.out.println("<-- constructor CdProjectDocsWizardPanel()");
   }

   public String getDescription()
   {
      return "";
   }

   public int displayWizard()
   {
      if (cdData.filePending)
      {
         boolean found = false;
         for (int i=0; i<cdData.projectDocs.size(); i++)
            found = found || 
               cdData.projectDocs.elementAt(i).toString().equals(cdData.addThisFile);
         if (!found)
            addLpdFile(new File(cdData.addThisFile));
      }

      return super.displayWizard();
   }

   public WizardData getWizardData()
   {
      if (nextWizard_ == null)
         nextWizard_ = new CdProjectTemplateWizardPanel(cdData);

      cdData.nextWizardPanel = nextWizard_;

      return cdData;
   }

   public synchronized void refreshDataDisplay()
   {
//       System.out.println("refreshDataDisplay() -->");

      docsListModel_.removeAllElements();
      for (int i=0; i<cdData.projectDocs.size(); i++)
      {
         // System.out.println("adding " + cdData.projectDocs.elementAt(i));
         docsListModel_.addElement(cdData.projectDocs.elementAt(i));
         // addLpdFile(new File(cdData.projectDocs.elementAt(i).toString()));
      }

      calcTotalSize();
//       System.out.println("<-- refreshDataDisplay()");
   }

   public boolean verifyNext()
   {
      // verify that all docs are correct:
      boolean allOk = true;

      for (int i=0; i<cdData.projectDocs.size() && allOk; i++)
      {
         String fileName = cdData.projectDocs.elementAt(i).toString();
         File file = new File(fileName);
         if (file.exists())
         {
            if (!isLpdDoc(file))
            {
               JOptionPane.showMessageDialog(this, ERR_NO_LPD + fileName,
                                             ERROR, JOptionPane.ERROR_MESSAGE);
               allOk = false;
            }

            if (ConverterWizard.getVersionType() == NativeUtils.EVALUATION_VERSION ||
                ConverterWizard.getVersionType() == NativeUtils.UNIVERSITY_EVALUATION_VERSION)
            {
               if (!verifyEvalDocument(file))
               {
                  JOptionPane.showMessageDialog
                     (this, LPD_EVAL_TOO_LONG + file.toString(),
                      INFORMATION, JOptionPane.INFORMATION_MESSAGE);
                  allOk = false;
               }
            }
         }
         else
         {
            JOptionPane.showMessageDialog(this, FILE_NOT_FOUND + '\n' + fileName,
                                          ERROR, JOptionPane.ERROR_MESSAGE);
            allOk = false;
         }
      }

      if (!allOk)
         return false;

      // write settings and project
      writeSettings();
      return writeProject();
   }

   public boolean verifyQuit()
   {
      // write settings and project
      writeSettings();
      return writeProject();
   }

   protected void notifyLeavePanel()
   {
      // apply changes to doc list:
      applyDocsChanges();

      super.notifyLeavePanel();
   }

   private void applyDocsChanges()
   {
      cdData.projectDocs.removeAllElements();
      for (int i=0; i<docsListModel_.getSize(); i++)
         cdData.projectDocs.addElement(docsListModel_.getElementAt(i));
   }

   private boolean isLpdDoc(File lpdFile)
   {
      try
      {
         DataInputStream in = new DataInputStream(new FileInputStream(lpdFile));
         byte[] buf = new byte[4];
         
         in.readFully(buf);
         in.close();
         
         return (buf[0] == 'E' &&
                 buf[1] == 'P' &&
                 buf[2] == 'F');
      }
      catch (IOException e)
      {
         return false;
      }
   }

   private void calcTotalSize()
   {
      long totalSize = 0;
      for (int i=0; i<docsListModel_.getSize(); i++)
         totalSize += ((FileListObject) docsListModel_.get(i)).getFileSize();

      int totalMb = 0;
      if (totalSize > 0)
         totalMb = (int) (totalSize / (1024*1024)) + 1;
      
      if (totalMb > 600)
         totalSizeLabel_.setForeground(Color.red);
      else
         totalSizeLabel_.setForeground(Color.black);

      totalSizeLabel_.setText("" + totalMb + " MB");

      if (totalMb > 0)
         setEnableButton(BUTTON_NEXT, true);
      else
         setEnableButton(BUTTON_NEXT, false);
   }

   private void performAdd()
   {
      JFileChooser fileChooser = new JFileChooser();
      if (lastDirectory_ != null)
         fileChooser.setCurrentDirectory(lastDirectory_);
      else
         fileChooser.setCurrentDirectory(new File(NativeUtils.getRecordingsPath()));

      fileChooser.setMultiSelectionEnabled(true);

      FileFilter fileFilter = FileUtils.createFileFilter(LPD_DESC, ".lpd");
      fileChooser.addChoosableFileFilter(fileFilter);
      fileChooser.addChoosableFileFilter(fileChooser.getAcceptAllFileFilter());
      fileChooser.setFileFilter(fileFilter);
      
      Dimension d = fileChooser.getPreferredSize();
      fileChooser.setPreferredSize(new Dimension(d.width + 100, d.height + 50));

      int action = fileChooser.showOpenDialog(this);

      lastDirectory_ = fileChooser.getCurrentDirectory();

      if (action == fileChooser.APPROVE_OPTION)
      {
         File[] selectedFiles = fileChooser.getSelectedFiles();
         for (int i=0; i<selectedFiles.length; i++)
            addLpdFile(selectedFiles[i]);
      }
   }

   private boolean addLpdFile(File lpdFile)
   {
      if (lpdFile.exists())
      {
         // check for exactly the same file:
         boolean found = false;
         String lpdFileName = lpdFile.toString();
         for (int i=0; i<docsListModel_.size(); i++)
         {
            String otherFileName = docsListModel_.elementAt(i).toString();
            found = found || otherFileName.equals(lpdFileName);
         }

         if (found)
         {
            // file already inserted to cd project!
            JOptionPane.showMessageDialog
               (this, LPD_EXISTS + lpdFileName, INFORMATION,
                JOptionPane.INFORMATION_MESSAGE);
            return false;
         }

         // check for files with equal names
         lpdFileName = lpdFile.getName();
         found = false;
         for (int i=0; i<docsListModel_.size(); i++)
         {
            String stripName = 
               (new File(docsListModel_.elementAt(i).toString())).getName();
            found = found ||
               stripName.equals(lpdFileName);
         }
         
         if (found)
         {
            JOptionPane.showMessageDialog
               (this, LPD_NAME_EXISTS + (lpdFile.toString()), INFORMATION,
                JOptionPane.INFORMATION_MESSAGE);
            return false;
         }

         // ok, let's check if it's really an LPD file
         if (!isLpdDoc(lpdFile))
         {
            JOptionPane.showMessageDialog
               (this, ERR_NO_LPD + lpdFile,
                ERROR, JOptionPane.ERROR_MESSAGE);
            return false;
         }
         else
         {
            if (ConverterWizard.getVersionType() == NativeUtils.EVALUATION_VERSION ||
                ConverterWizard.getVersionType() == NativeUtils.UNIVERSITY_EVALUATION_VERSION)
            {
               if (!verifyEvalDocument(lpdFile))
               {
                  JOptionPane.showMessageDialog
                     (this, LPD_EVAL_TOO_LONG + lpdFile.toString(),
                      INFORMATION, JOptionPane.INFORMATION_MESSAGE);
                  return false;
               }
            }

            docsListModel_.addElement
               (new FileListObject("" + lpdFile));
            calcTotalSize();
            return true;
         }
      }
      else
      {
         JOptionPane.showMessageDialog
            (this, FILE_NOT_FOUND + " " + lpdFile,
             ERROR, JOptionPane.ERROR_MESSAGE);
         return false;
      }
   }

   private boolean verifyEvalDocument(File lpdFile)
   {
      try
      {
         FileResources fileResources = 
            FileResources.createFileResources(lpdFile.toString());

         String audioResource = extractAudioResource(fileResources);

         InputStream audioStream = 
            fileResources.createBufferedInputStream(audioResource, 1024);

         AudioFileFormat aff = AudioSystem.getAudioFileFormat(audioStream);
         int numFrames = aff.getFrameLength();
         float frameRate = aff.getFormat().getFrameRate();

         float docLength = (1f*numFrames)/frameRate; // in seconds

         // ok, if shorter than 3 minutes
         return (docLength < 181f);
      }
      catch (Exception e)
      {
         e.printStackTrace();
         JOptionPane.showMessageDialog
            (this, LPD_EVAL_CHECK_FAIL + lpdFile.toString() + "\n\n" +
             e.toString(), ERROR, JOptionPane.ERROR_MESSAGE);
         return false;
      }
   }

   private String extractAudioResource(FileResources fileResources)
      throws Exception
   {
      XmlParser parser = 
         new XmlParser(fileResources.createConfigFileInputStream());

      int ttype = XmlParser.TAG_START;
      boolean inDocument = false;
      boolean inFiles = false;
      boolean inAudio = false;
      String audioResourceName = null;
      while (ttype != XmlParser.EOF)
      {
         ttype = parser.getNextTag();
         
         switch (ttype)
         {
         case XmlParser.TAG_START: {
            String tag = parser.getToken();
            if (inDocument)
            {
               if (inFiles)
               {
                  if (tag.equalsIgnoreCase("AUDIO"))
                     inAudio = true;
                  // else ignore
               }
               else if (tag.equalsIgnoreCase("FILES"))
                  inFiles = true;
               // else ignore
            }
            else if (tag.equalsIgnoreCase("DOCUMENT"))
               inDocument = true;
               // else ignore
            break;
         }

         case XmlParser.TAG_END: {
            String tag = parser.getToken();
            if (inDocument)
            {
               if (tag.equalsIgnoreCase("DOCUMENT"))
                  inDocument = false;
               else if (inFiles)
               {
                  if (tag.equalsIgnoreCase("FILES"))
                     inFiles = false;
                  else if (inAudio)
                  {
                     if (tag.equalsIgnoreCase("AUDIO"))
                        inAudio = false;
                  }
               }
            }
            break;
         }

         case XmlParser.CONTENT: {
            String content = parser.getToken();
            if (inDocument)
            {
               if (inFiles)
               {
                  if (inAudio)
                     audioResourceName = content;
               }
            }
         }
         }
      }

      return audioResourceName;
   }

   private void performRemove()
   {
      // after we have removed the marked documents, no documents
      // are marked anymore, so we'll disable the buttons.
      removeButton_.setEnabled(false);
      upButton_.setEnabled(false);
      downButton_.setEnabled(false);

      int[] indices = docsList_.getSelectedIndices();
      java.util.Arrays.sort(indices);

      // remove backwards to avoid difficulties:
      for (int i=indices.length-1; i>=0; i--)
         docsListModel_.remove(indices[i]);

      calcTotalSize();
   }

   private void performUp()
   {
      int[] indices = docsList_.getSelectedIndices();
      // we'll rely on the indices to be correct to allow an "up"

      try
      {
         for (int i=0; i<indices.length; i++)
         {
            Object a = docsListModel_.getElementAt(indices[i]);
            Object b = docsListModel_.getElementAt(indices[i] - 1);
            docsListModel_.setElementAt(a, indices[i] - 1);
            docsListModel_.setElementAt(b, indices[i]);
         }
         
         for (int i=0; i<indices.length; i++)
            indices[i]--;
         
         docsList_.setSelectedIndices(indices);
      }
      catch (Exception e)
      {
         e.printStackTrace();
      }
   }

   private void performDown()
   {
      int[] indices = docsList_.getSelectedIndices();
      try
      {
         // we'll rely on the indices to be correct to allow a "down"
         for (int i=indices.length-1; i>=0; i--)
         {
            Object a = docsListModel_.getElementAt(indices[i]);
            Object b = docsListModel_.getElementAt(indices[i] + 1);
            docsListModel_.setElementAt(a, indices[i] + 1);
            docsListModel_.setElementAt(b, indices[i]);
         }
         
         for (int i=0; i<indices.length; i++)
            indices[i]++;
         docsList_.setSelectedIndices(indices);
      }
      catch (Exception e)
      {
         e.printStackTrace();
      }
   }

   private void performRefresh()
   {
      int[] indices = docsList_.getSelectedIndices();
      for (int i=0; i<indices.length; i++)
      {
         FileListObject flo = (FileListObject) docsListModel_.getElementAt(i);
         flo.refresh();
      }

      calcTotalSize();
      docsList_.repaint();
   }

   private void handleMouseEvent(MouseEvent e)
   {
      if (!e.isPopupTrigger())
         return;

      int index = docsList_.locationToIndex(e.getPoint());
      if (index == -1)
         return;

      if (!docsList_.isSelectedIndex(index))
         docsList_.setSelectedIndex(index);
      
      if (popupMenu_ == null)
      {
      }

      removeItem_.setEnabled(removeButton_.isEnabled());
      upItem_.setEnabled(upButton_.isEnabled());
      downItem_.setEnabled(downButton_.isEnabled());

      popupMenu_.show(docsList_, e.getX(), e.getY());
   }

   private class FlatButton extends JButton
   {
      public FlatButton(Icon icon, String actionCommand)
      {
         super(icon);
         setBorderPainted(false);
         setActionCommand(actionCommand);
      }
   }

   private class ButtonListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         String cmd = e.getActionCommand();

         if (cmd.equals(MENU_ADD))
            performAdd();
         else if (cmd.equals(MENU_REMOVE))
            performRemove();
         else if (cmd.equals(MENU_UP))
            performUp();
         else if (cmd.equals(MENU_DOWN))
            performDown();
         else if (cmd.equals(MENU_REFRESH))
            performRefresh();
         else
            System.out.println("unknown: " + cmd);
      }
   }

   private class DocsSelectionListener implements ListSelectionListener
   {
      public void valueChanged(ListSelectionEvent e)
      {
         int index = docsList_.getSelectedIndex();
         if (index == -1)
         {
            // no selection done
            removeButton_.setEnabled(false);
            upButton_.setEnabled(false);
            downButton_.setEnabled(false);
         }
         else
         {
            removeButton_.setEnabled(true);

            int[] indices = docsList_.getSelectedIndices();
            int firstIndex = indices[0];
            int lastIndex = indices[indices.length - 1];

            int numIndices = docsListModel_.getSize();

            if (firstIndex > 0)
               upButton_.setEnabled(true);
            else
               upButton_.setEnabled(false);
            
            if (lastIndex < numIndices - 1)
               downButton_.setEnabled(true);
            else
               downButton_.setEnabled(false);
         }
      }
   }

   private class ListMouseListener extends MouseAdapter
   {
      public void mouseClicked(MouseEvent e)
      {
         handleMouseEvent(e);
      }

      public void mousePressed(MouseEvent e)
      {
         handleMouseEvent(e);
      }

      public void mouseReleased(MouseEvent e)
      {
         handleMouseEvent(e);
      }
   }

   private class ListKeyListener extends KeyAdapter
   {
      public void keyReleased(KeyEvent e)
      {
         switch (e.getKeyCode())
         {
         case KeyEvent.VK_DELETE:
            performRemove();
            break;

         case KeyEvent.VK_INSERT:
            performAdd();
            break;
         }
      }
   }

   private class DnDListener implements DropTargetListener
   {
      public void dragEnter(DropTargetDragEvent dtde)
      {
//          System.out.println("dragEnter()");
//          DataFlavor[] df = dtde.getCurrentDataFlavors();
//          for (int i=0; i<df.length; i++)
//             System.out.println("DataFlavor #" + i + ": " + df[i].toString());

         if (!dtde.isDataFlavorSupported(DataFlavor.javaFileListFlavor))
         {
//             System.out.println("data flavor not supported: javaFileListFlavor");
            dtde.rejectDrag();
         }
         else
         {
            dtde.acceptDrag(DnDConstants.ACTION_COPY);
//             System.out.println("data flavor supported: javaFileListFlavor");
         }
      }

      public void dragExit(DropTargetEvent dte)
      {
//          System.out.println("dragExit()");
      }

      public void dragOver(DropTargetDragEvent dtde)
      {
//          System.out.println("dragOver()");
         if (!dtde.isDataFlavorSupported(DataFlavor.javaFileListFlavor))
         {
            dtde.rejectDrag();
         }
      }

      public void drop(DropTargetDropEvent dtde)
      {
//          System.out.println("drop()");
         if (!dtde.isDataFlavorSupported(DataFlavor.javaFileListFlavor))
         {
//             System.out.println("reject drop!");
            dtde.rejectDrop();
         }
         else
         {
            Transferable t = dtde.getTransferable();
            try
            {
//                DataFlavor[] flavors = t.getTransferDataFlavors();
//                for (int i=0; i<flavors.length; i++)
//                   System.out.println("TransferDataFlavor[" + i + "] == " + flavors[i].toString());

               if (t.isDataFlavorSupported(DataFlavor.javaFileListFlavor))
               {
                  dtde.acceptDrop(DnDConstants.ACTION_COPY);
                  List fileList = (List) t.getTransferData(DataFlavor.javaFileListFlavor);
                  for (int i=0; i<fileList.size(); i++)
                  {
                     File file = (File) fileList.get(i);
                     addLpdFile(file);
//                      System.out.println("file #" + i + ": " + file.getName());
                  }
                  dtde.getDropTargetContext().dropComplete(true);
               }
//                else
//                   System.out.println("Tranferable::DataFlavor not supported!!?");
            }
            catch (UnsupportedFlavorException e)
            {
               e.printStackTrace();
               dtde.rejectDrop();
            }
            catch (IOException e)
            {
               e.printStackTrace();
               dtde.rejectDrop();
            }
         }
      }

      public void dropActionChanged(DropTargetDragEvent dtde)
      {
//          System.out.println("dragActionChanged()");
      }
   }
}
