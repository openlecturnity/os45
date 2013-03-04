package imc.epresenter.player;

import java.awt.Component;
import java.util.ArrayList;
import javax.swing.table.AbstractTableModel;

public class MyTableModel extends AbstractTableModel
{
   private Object[][] tableData_;
   private String[] tableHeader_;

   public MyTableModel(Object[][] data, String[] header)
   {
      for (int i=0; i<data.length; i++)
         if (data[i].length != header.length)
            throw new IllegalArgumentException("Header length does not match data length.");

      tableData_ = data;
      tableHeader_ = header;
   }


   public int getColumnCount()
   {
      return tableHeader_.length;
   }

   public int getRowCount()
   {
      return tableData_.length;
   }

   public Object getValueAt(int row, int column)
   {
      if (row < getRowCount())
         return tableData_[row][column];
      else
         return null;
   }

   public void setValueAt(Object value, int row, int column)
   {
      if (value != null)
      {
         tableData_[row][column] = value;
         fireTableCellUpdated(row, column);
      }
      else
         System.out.println("Aua!");
   }




   public String getColumnName(int column)
   {
      return tableHeader_[column];
   }

   public boolean isCellEditable(int row, int column)
   {
      return getValueAt(row, column) instanceof Component || getValueAt(row, column) instanceof Boolean;
   }

   public Class getColumnClass(int column)
   {
      int row = 0;
      while(row < getRowCount() && getValueAt(row, column) == null)
         row++;
      if (getValueAt(row, column) != null)
         return getValueAt(row, column).getClass();
      else
         return Object.class;
   }

   public void exchangeData(Object[][] newData)
   {
      if (newData.length == 0 || newData[0].length == getColumnCount())
      {
         tableData_ = newData;
         
         fireTableDataChanged();
      }
   
   }

   public void removeRows(int[] rows)
   {
      ArrayList deleteHelper = new ArrayList(tableData_.length);
      for (int i=0; i<tableData_.length; i++)
         deleteHelper.add(tableData_[i]);
      for (int i=rows.length-1; i>=0; i--)
         deleteHelper.remove(i);
      Object[][] newData = new Object[tableData_.length-rows.length][tableData_[0].length];
      deleteHelper.toArray(newData);

      tableData_ = newData;

      fireTableDataChanged();
   }
}