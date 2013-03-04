package imc.lecturnity.util.ui;

import java.awt.Color;

import javax.swing.JList;
import javax.swing.ListModel;

import javax.swing.event.ListSelectionListener;
import javax.swing.event.ListSelectionEvent;

import imc.lecturnity.util.ColorManager;

/**
 * This class extends the <tt>JList</tt> class in the following ways:
 * <ul>
 *   <li>If the list is disabled, it displays a grey background</li>
 *   <li>The list displays a tool tip for the (first) selected
 * element</li>
 * </ul>
 *
 * @author Martin Danielsson
 */
public class CustomJList extends JList
{
   private boolean displayToolTips_ = true;

   /**
    * Create a <tt>CusomtJList</tt> instance for the given <tt>ListModel</tt>.
    */
   public CustomJList(ListModel listModel)
   {
      super(listModel);
      addListSelectionListener(new ToolTipUpdater());
   }

   /**
    * Enable/disable the <tt>CustomJList</tt> instance
    */
   public void setEnabled(boolean b)
   {
      super.setEnabled(b);
//       setBackground
//          (b ? Color.white : ColorManager.greyResource);
      setBackground(b ? Color.white : javax.swing.UIManager.getColor("control"));
   }

   /**
    * Use this method to determine whether tool tips should be
    * displayed or not.
    */
   public void setDisplayToolTips(boolean b)
   {
      displayToolTips_ = b;
   }

   private class ToolTipUpdater implements ListSelectionListener
   {
      public void valueChanged(ListSelectionEvent e)
      {
         if (!displayToolTips_)
            return;

         int index = getSelectedIndex();
         if (index != -1)
            setToolTipText(getModel().getElementAt(index).toString());
         else
            setToolTipText(null);
      }
   }
}
