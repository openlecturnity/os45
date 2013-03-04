package imc.lecturnity.util.ui;

import java.awt.Component;

import javax.swing.JTree;
import javax.swing.Icon;
import javax.swing.ImageIcon;
import javax.swing.tree.*;
import javax.swing.event.TreeExpansionListener;
import javax.swing.event.TreeExpansionEvent;

import java.io.File;
import java.io.FileFilter;
import javax.swing.filechooser.FileSystemView;

import java.util.HashMap;
import java.util.Enumeration;

import imc.lecturnity.util.ColorManager;

public class DirJTree extends JTree
{
   public static DirJTree createDirJTree()
   {
      DefaultTreeModel dtm = new DefaultTreeModel(new RootNode());
      return new DirJTree(dtm);
   }

   private DirJTree(DefaultTreeModel treeModel)
   {
      super(treeModel);

      setCellRenderer(new FileTreeCellRenderer());
      DefaultTreeSelectionModel dtsm = new DefaultTreeSelectionModel();
      dtsm.setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);
      setSelectionModel(dtsm);
      addTreeExpansionListener(new FileTreeExpansionListener(treeModel));
   }

   public File getSelectedDir()
   {
      TreePath treePath = getSelectionPath();
      FileNode node = (FileNode) treePath.getLastPathComponent();

      return node.getFile();
   }
}

class FileTreeCellRenderer extends DefaultTreeCellRenderer
{
   private static boolean USE_CUSTOM_COLORS = false;
   
   private HashMap iconMap_ = new HashMap();

   Icon defaultOpenIcon_;
   Icon defaultClosedIcon_;

   public FileTreeCellRenderer()
   {
      super();

      defaultOpenIcon_ = getDefaultOpenIcon();
      defaultClosedIcon_ = getDefaultClosedIcon();

      //if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
      if (USE_CUSTOM_COLORS)
      {
         setBackgroundSelectionColor(ColorManager.orangeResource);
         setTextSelectionColor(ColorManager.blackResource);
      }
      else
      {
         setBackgroundSelectionColor(javax.swing.UIManager.getColor("Tree.selectionBackground"));
         setTextSelectionColor(javax.swing.UIManager.getColor("Tree.selectionForeground"));
      }
//       setOpaque(true);
   }

   public Component getTreeCellRendererComponent
      (JTree tree, Object value, boolean sel, boolean expanded, boolean leaf,
       int row, boolean hasFocus)
   {
      this.hasFocus = hasFocus;
      this.selected = sel;
      
      FileNode node = (FileNode) value;

      String stringValue = 
         tree.convertValueToText(value, sel, expanded, leaf, row, hasFocus);

      setText(stringValue);

      if(sel)
      {
         setForeground(getTextSelectionColor());
         //if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
         if (USE_CUSTOM_COLORS)
            setBackgroundSelectionColor(ColorManager.orangeResource);
         else
            setBackgroundSelectionColor(javax.swing.UIManager.getColor("Tree.selectionBackground"));
      }
      else
         setForeground(getTextNonSelectionColor());
      
      String iconResource = node.getIconResource();
      if (iconResource != null)
      {
         Icon icon = (Icon) iconMap_.get(iconResource);
         if (icon == null)
         {
            try
            {
               icon = new ImageIcon(getClass().getResource(iconResource));
//                System.out.println("new ImageIcon(" + iconResource +")");
               iconMap_.put(iconResource, icon);
            }
            catch (Exception e)
            {
            }
         }
         
         if (icon != null)
         {
            setIcon(icon);
         }
         else
         {
            if (expanded)
               setIcon(defaultOpenIcon_);
            else
               setIcon(defaultClosedIcon_);
         }
      }
      else
      {
         if (expanded)
            setIcon(defaultOpenIcon_);
         else
            setIcon(defaultClosedIcon_);
      }

      setComponentOrientation(tree.getComponentOrientation());

      return this;
   }
                                                 
}


class RootNode extends FileNode
{
   public RootNode()
   {
      super(null, null);
      parent_ = null;
   }

   public String toString()
   {
      return "Arbeitsplatz";
   }
   
   public String getIconResource()
   {
      return "/imc/lecturnity/util/ui/images/my_computer.png";
   }

   protected void initChildren()
   {
      File[] roots = File.listRoots();
      children_ = new FileNode[roots.length];
      for (int i=0; i<roots.length; i++)
         children_[i] = new FileNode(this, roots[i]);
   }
}

class FileTreeExpansionListener implements TreeExpansionListener
{
   private DefaultTreeModel treeModel_;

   public FileTreeExpansionListener(DefaultTreeModel treeModel)
   {
      treeModel_ = treeModel;
   }

   public void treeCollapsed(TreeExpansionEvent e)
   {
      TreePath path = e.getPath();
      FileNode node = (FileNode) path.getLastPathComponent();
      node.deInitChildren();

      treeModel_.reload(node);
   }

   public void treeExpanded(TreeExpansionEvent e)
   {
      // not used
   }
}

