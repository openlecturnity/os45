package imc.lecturnity.util.ui;

import javax.swing.tree.*;

import java.io.File;
import java.io.FileFilter;
import javax.swing.filechooser.FileSystemView;

import java.util.Enumeration;

class FileNode implements TreeNode
{
   protected static DirFileFilter dirFilter_ = new DirFileFilter();
   protected static FileSystemView systemView_ = FileSystemView.getFileSystemView();

   protected File file_;
   protected FileNode[] children_ = null;
   protected FileNode parent_ = null;

   public FileNode(FileNode parent, File file)
   {
      file_ = file;
      parent_ = parent;
   }

   public File getFile()
   {
      return file_;
   }

   public Enumeration children()
   {
//       System.out.println("children()");
      return new ChildEnumeration();
   }

   public String toString()
   {
//       System.out.println("toString()");
      String tmp = file_.getName();
      if (tmp.equals(""))
         return file_.toString();
      else
         return tmp;
   }

   public boolean getAllowsChildren()
   {
      return true;
   }

   public TreeNode getChildAt(int index)
   {
      if (children_ == null)
         initChildren();

      return children_[index];
   }

   public FileNode getChild(File file)
   {
      if (children_ == null)
         initChildren();

      FileNode node = null;
      for (int i=0; i<children_.length; i++)
      {
         if (children_[i].getFile().equals(file))
         {
            node = children_[i];
            break;
         }
      }

      return node;
   }

   public int getChildCount()
   {
      if (children_ == null)
         initChildren();

      return children_.length;
   }

   public int getIndex(TreeNode node)
   {
      if (children_ == null)
         initChildren();

      int index = -1;
      for (int i=0; i<children_.length; i++)
      {
         if (node == children_[i])
         {
            index = i;
            break;
         }
      }

      return index;
   }

   public TreeNode getParent()
   {
      return parent_;
   }

   public boolean isLeaf()
   {
      return false;
   }

   public String getIconResource()
   {
      if (systemView_.isRoot(file_))
         return "/imc/lecturnity/util/ui/images/drive.png";
      else
         return null;
   }
   
   protected synchronized void initChildren()
   {
      File[] subDirs = file_.listFiles(dirFilter_);
      if (subDirs == null)
      {
         children_ = new FileNode[0];
         return;
      }

      java.util.Arrays.sort(subDirs);

      children_ = new FileNode[subDirs.length];
      for (int i=0; i<subDirs.length; i++)
         children_[i] = new FileNode(this, subDirs[i]);
   }

   protected synchronized void deInitChildren()
   {
      children_ = null;
   }

   private class ChildEnumeration implements Enumeration
   {
      private int pos_;
      
      public ChildEnumeration()
      {
         pos_ = 0;
      }

      public boolean hasMoreElements()
      {
         return (pos_ < children_.length);
      }

      public Object nextElement()
      {
         return children_[pos_++];
      }

   }

   private static class DirFileFilter implements java.io.FileFilter
   {
      public boolean accept(File file)
      {
         return file.isDirectory();
      }
   }
}
