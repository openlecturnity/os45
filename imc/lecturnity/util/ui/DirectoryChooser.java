package imc.lecturnity.util.ui;

import java.awt.*;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

import javax.swing.*;
import javax.swing.tree.TreePath;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreeNode;
import javax.swing.event.TreeSelectionListener;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.border.BevelBorder;

import java.io.IOException;
import java.io.File;

import java.util.Vector;

import imc.epresenter.filesdk.util.Localizer;
import imc.lecturnity.converter.ConverterWizard;

public class DirectoryChooser extends JDialog
{
   public static final int OPTION_APPROVE = 1;
   public static final int OPTION_CANCEL  = 2;

   private static String CAPTION = "[!]";
   private static String NEW_DIR = "[!]";
   private static char   MNEM_NEW_DIR = '?';
   private static String OK = "[!]";
   private static char   MNEM_OK = '?';
   private static String CANCEL = "[!]";
   private static char   MNEM_CANCEL = '?';
   private static String INPUT = "[!]";
   private static String ENTER_NEW_DIR = "[!]";
   private static String ERROR = "[!]";
   private static String ERR_INVALID = "[!]";
   private static String ERR_CREATE = "[!]";

   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/util/ui/DirectoryChooser_",
             "en");

         CAPTION = l.getLocalized("CAPTION");
         NEW_DIR = l.getLocalized("NEW_DIR");
         MNEM_NEW_DIR = l.getLocalized("MNEM_NEW_DIR").charAt(0);
         OK = l.getLocalized("OK");
         MNEM_OK = l.getLocalized("MNEM_OK").charAt(0);
         CANCEL = l.getLocalized("CANCEL");
         MNEM_CANCEL = l.getLocalized("MNEM_CANCEL").charAt(0);
         INPUT = l.getLocalized("INPUT");
         ENTER_NEW_DIR = l.getLocalized("ENTER_NEW_DIR");
         ERROR = l.getLocalized("ERROR");
         ERR_INVALID = l.getLocalized("ERR_INVALID");
         ERR_CREATE = l.getLocalized("ERR_CREATE");
      }
      catch (IOException e)
      {
         JOptionPane.showMessageDialog(null, "Could not open Locale database!\n"+
                                       "imc.lecturnity.util.ui.DirectoryChooser",
                                       "Error", JOptionPane.ERROR_MESSAGE);
         System.exit(1);
      }
   }

   private Window parentFrame_;
   
   private DirJTree dirTree_;
   private DefaultTreeModel treeModel_;
   private JButton newDirButton_, okButton_;

   private int returnValue_ = OPTION_CANCEL;

   public DirectoryChooser(Dialog parent)
   {
      super(parent, CAPTION, true);
      init(parent);
   }

   public DirectoryChooser(Frame parent)
   {
      super(parent, CAPTION, true);
      init(parent);
   }

   private void init(Window parent)
   {
      parentFrame_ = parent;
      
      Container r = getContentPane();

      r.setLayout(new BorderLayout());
      
      JPanel buttonPanel = new JPanel() {
            public Insets getInsets()
            {
               return new Insets(10, 10, 10, 10);
            }
         };
      buttonPanel.setLayout(new BorderLayout());
      buttonPanel.setFont(imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12));
      JPanel leftButtons = new JPanel() {
            public Insets getInsets()
            {
               return new Insets(0, 0, 0, 0);
            }
            
            public Dimension getPreferredSize()
            {
               return new Dimension(120, 45);
            }
         };
      leftButtons.setLayout(null);
      leftButtons.setFont(imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12));

      ButtonListener buttonListener = new ButtonListener();

      newDirButton_ = new JButton(NEW_DIR);
      newDirButton_.setFont(imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12));
      newDirButton_.setSize(new Dimension(105, 30));
      newDirButton_.setLocation(new Point(10, 5));
      newDirButton_.addActionListener(buttonListener);
      newDirButton_.setEnabled(false);
      newDirButton_.setMnemonic(MNEM_NEW_DIR);
      leftButtons.add(newDirButton_);
      buttonPanel.add(leftButtons, BorderLayout.WEST);

      JPanel rightButtons = new JPanel() {
            public Insets getInsets()
            {
               return new Insets(0, 0, 0, 0);
            }
            
            public Dimension getPreferredSize()
            {
               return new Dimension(230, 40);
            }
         };
      rightButtons.setLayout(null);
      rightButtons.setFont(imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12));

      okButton_ = new JButton(OK);
      okButton_.setFont(imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12));
      okButton_.setSize(new Dimension(105, 30));
      okButton_.setLocation(5, 5);
      okButton_.addActionListener(buttonListener);
      okButton_.setEnabled(false);
      okButton_.setMnemonic(MNEM_OK);
      rightButtons.add(okButton_);
      JButton cancelButton = new JButton(CANCEL);
      cancelButton.setFont(imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12));
      cancelButton.setSize(new Dimension(105, 30));
      cancelButton.setLocation(115, 5);
      cancelButton.addActionListener(buttonListener);
      cancelButton.setMnemonic(MNEM_CANCEL);
      rightButtons.add(cancelButton);

      buttonPanel.add(rightButtons, BorderLayout.EAST);

      r.add(buttonPanel, BorderLayout.SOUTH);

      JPanel contentPanel = new JPanel() {
            public Insets getInsets()
            {
               return new Insets(20, 20, 0, 20);
            }
         };
      contentPanel.setLayout(new BorderLayout());
      contentPanel.setFont(imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12));
      
      dirTree_ = DirJTree.createDirJTree();
      dirTree_.addTreeSelectionListener(new SelectListener());
      treeModel_ = (DefaultTreeModel) dirTree_.getModel();
      JScrollPane treeScroller = new JScrollPane(dirTree_) {
            public Dimension getPreferredSize()
            {
               return new Dimension(300, 300);
            }
         };
      treeScroller.setBorder(new BevelBorder(BevelBorder.LOWERED));

      contentPanel.add(treeScroller, BorderLayout.CENTER);
      
      r.add(contentPanel, BorderLayout.CENTER);
      
      pack();
   }

   public int showDialog()
   {
      if (parentFrame_ != null)
      {
         Point parentLoc = parentFrame_.getLocation();
         Dimension parentSize = parentFrame_.getSize();
         Dimension size = getSize();
         int xDiff = parentSize.width - size.width;
         xDiff = (xDiff > 0) ? xDiff : 0;
         int yDiff = parentSize.height - size.height;
         yDiff = (yDiff > 0) ? yDiff : 0;
         
         setLocation(new Point(parentLoc.x + xDiff/2, parentLoc.y + yDiff/2));
      }

      super.show();
      return returnValue_;
   }

   public File getSelectedDir()
   {
      TreePath path = dirTree_.getSelectionPath();
      File file = null;
      if (path != null)
      {
         FileNode node = (FileNode) path.getLastPathComponent();
         if (node != null)
            file = node.getFile();
      }

      return file;
   }

   public void setSelectedDir(File dir)
   {
      if (dir == null)
         return;

      Vector path = new Vector();
      File dirFile = null;
      try
      {
         dirFile = dir.getCanonicalFile();
      }
      catch (IOException e)
      {
         dirFile = dir;
      }

      while (dirFile != null)
      {
         path.addElement(dirFile);
         dirFile = dirFile.getParentFile();
      }

      if (path.size() == 0)
         return;

      FileNode node = (FileNode) treeModel_.getRoot();
      node = node.getChild((File) (path.elementAt(path.size()-1)));
      FileNode lastNode = node;
      
      int count = path.size() - 2;
      while (node != null && count >= 0)
      {
         node.deInitChildren();
         treeModel_.reload(node);
         node = node.getChild((File) (path.elementAt(count--)));
         if (node != null)
            lastNode = node;
      }

      if (lastNode != null)
      {
         TreeNode[] nodes = treeModel_.getPathToRoot(lastNode);
         TreePath actPath = new TreePath(nodes);
         dirTree_.expandPath(actPath);
         dirTree_.setSelectionPath(actPath);
         dirTree_.scrollPathToVisible(actPath);
      }
   }

   private void checkSelection()
   {
      TreePath path = dirTree_.getSelectionPath();
      File file = null;
      if (path != null)
      {
         FileNode node = (FileNode) path.getLastPathComponent();
         if (node != null)
         {
            if (node.getFile() != null)
            {
               okButton_.setEnabled(true);
               newDirButton_.setEnabled(true);
               return;
            }
         }
      }

      okButton_.setEnabled(false);
      newDirButton_.setEnabled(false);
   }

   private void createNewDir()
   {
      File currentDir = getSelectedDir();

      String newDir = JOptionPane.showInputDialog
         (this, ENTER_NEW_DIR + currentDir, INPUT,
          JOptionPane.QUESTION_MESSAGE);

      if (newDir == null)
         return;

      newDir = currentDir.toString() + File.separatorChar + newDir;

      try
      {
         File newDirFile = new File(newDir);
         boolean success = newDirFile.mkdirs();
         setSelectedDir(newDirFile);
      }
      catch (SecurityException e)
      {
         e.printStackTrace();
         JOptionPane.showMessageDialog(this, ERR_CREATE + e.getMessage(),
                                       ERROR, JOptionPane.ERROR_MESSAGE);
      }
   }

   private class ButtonListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         String cmd = e.getActionCommand();
         if (cmd.equals(NEW_DIR))
         {
            createNewDir();
            return;
         }
         else if (cmd.equals(OK))
            returnValue_ = OPTION_APPROVE;
         else
            returnValue_ = OPTION_CANCEL;

         DirectoryChooser.this.dispose();
      }
   }

   private class SelectListener implements TreeSelectionListener
   {
      public void valueChanged(TreeSelectionEvent e)
      {
         checkSelection();
      }
   }
}
