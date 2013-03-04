package imc.lecturnity.util;

import java.awt.Component;
import java.awt.Dimension;
import java.awt.Insets;
import java.awt.Rectangle;
import java.util.Arrays;
import javax.swing.*;
import javax.swing.table.*;
import javax.swing.event.TableModelListener;
import javax.swing.event.TableModelEvent;

public class SpecialTable extends JTable
{
   private boolean[] shortColumn_;
   private ResizeTrigger resizeTrigger_;


   public SpecialTable()
   {
      this(null, null);
   }

   public SpecialTable(TableModel tm)
   {
      this(tm, filledTrueleanArray(tm.getColumnCount()));
   }


   private static boolean[] filledTrueleanArray(int length)
   {
      boolean[] array = new boolean[length];
      Arrays.fill(array, true);
      
      return array;
   }


   public SpecialTable(TableModel tm, boolean[] shortColumn)
   {
      setDefaultRenderer(Component.class, new ComponentCellRenderer());
      setDefaultEditor(Component.class, new ComponentCellEditor());
      setIntercellSpacing(new Dimension(5, 1));
      
      if (tm != null)
         setModel(tm, shortColumn);
   }

   public void setModel(TableModel tm, boolean[] shortColumn)
   {
      if (resizeTrigger_ != null && getModel() != null)
         getModel().removeTableModelListener(resizeTrigger_);

      super.setModel(tm);
      shortColumn_ = shortColumn;

      if (resizeTrigger_ == null)
         resizeTrigger_ = new ResizeTrigger();
      getModel().addTableModelListener(resizeTrigger_);

      adjustColumnWidths();
   }

   private void adjustColumnWidths()
   {
      //
      // column width thing
      //
      TableModel tm = getModel();
      
      //if (getColumnModel().getColumnCount() == 0)
      //   return;

      //System.out.println("adjust");

      JTableHeader header = getTableHeader();
      TableColumnModel cm = getColumnModel();
      int rows = tm.getRowCount();
      Dimension spacing = getIntercellSpacing();
      for (int c=0; c<cm.getColumnCount(); c++)
      {
         if (shortColumn_[c])
         {
            // check for header
            TableCellRenderer headerRenderer = cm.getColumn(c).getHeaderRenderer();
            if (headerRenderer == null)
               headerRenderer = header.getDefaultRenderer();

            Component comp1 = headerRenderer.getTableCellRendererComponent(
               this, tm.getColumnName(c), true, true, -1, c);

            int additionalWidth = spacing.width+4;
            if (comp1 instanceof JComponent)
            {
               Insets in = ((JComponent)comp1).getBorder().getBorderInsets(comp1);
               additionalWidth = in.left+in.right+spacing.width;
            }

            int width = comp1.getPreferredSize().width+additionalWidth;

            // check for every row
            for (int r=0; r<rows; r++)
            {
               Component comp = getCellRenderer(r, c).getTableCellRendererComponent(
                  this, tm.getValueAt(r, c), true, true, r, c);

               Dimension pref = comp.getPreferredSize();

               if (pref.width+spacing.width > width)
                  width = pref.width+spacing.width;
            }

            cm.getColumn(c).setMinWidth(width);
            cm.getColumn(c).setMaxWidth(width);
         }
      }

   }

   public void triggerResizeAndRepaint()
   {
      resizeAndRepaint();
   }


   private class ResizeTrigger implements TableModelListener
   {

      public void tableChanged(TableModelEvent evt)
      {
         //System.out.println("RT: "+evt);
         //if (evt.getType() == TableModelEvent.INSERT)
         //   System.out.println("INSERT");
         adjustColumnWidths();
      }

   }

   
   private class ComponentCellRenderer implements TableCellRenderer
   {
      public Component getTableCellRendererComponent(JTable table, Object value,
                                                     boolean s, boolean f,
                                                     int row, int column)
      {
         return (Component)value;
      }
   }
   
   private class ComponentCellEditor extends AbstractCellEditor implements TableCellEditor
   {
      private Object currentValue_;

      public Object getCellEditorValue()
      {
         return currentValue_;
      }

      public Component getTableCellEditorComponent(JTable table, Object value,
                                                   boolean s, int row, int column)
      {
         currentValue_ = value;
         return (Component)value;
      }

   }

      
   public static void main(String[] args)
   {
      /* 
      JComponent[][] tableData2 = new JComponent[][] { 
         { new JLabel("i"), new JLabel("B") },
         { new JLabel("Ai"), new JLabel("B") },
         { new JLabel("r"), new JLabel("B") }
      };
      String[] tableHeader2 = new String[] { "HiiisttlL", "H2" };
     
      JTable weez = new SpecialTable(new MyTableModel(tableData2, tableHeader2),
                                     new boolean[] { true, false });
      

      JScrollPane pane = new JScrollPane(weez);
      pane.setPreferredSize(new Dimension(pane.getPreferredSize().width, 100));

      JOptionPane.showConfirmDialog(null, pane);

      System.exit(0);
      */
   }


 
}