package imc.lecturnity.util.wizards;

import javax.swing.JButton;

import java.awt.Dimension;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.util.EventListener;

public class WizardButton extends JButton
{
   private static boolean workAroundEnterBug_ = true;
   
   private Dimension m_dimOverrideSize = null;

   public WizardButton(String caption)
   {
      super(caption);
   }

   public Dimension getPreferredSize()
   {
      if (m_dimOverrideSize != null)
         return m_dimOverrideSize;
      
      return new Dimension(WizardPanel.BUTTON_SIZE_X, WizardPanel.BUTTON_SIZE_Y);
   }
   
   public void setPreferredSize(Dimension dimOverride)
   {
      m_dimOverrideSize = dimOverride;
   }
      
   public Insets getInsets()
   {
      return new Insets(1, 1, 1, 1);
   }


   protected void processMouseEvent(MouseEvent evt)
   {
      super.processMouseEvent(evt);

      /*

      // the problem seems to be this):
      // if you change the component (button) beneath the mouse cursor
      // programmatically, the new component is not aware that the mouse is
      // positioned over it, therefore upon a mouse click the native side generates
      // a "mouse enter" event instead of the "mouse pressed" event
      // the "mouse released" event is ignored if it is still the same component
      // otherwise there will be another "mouse enter" event for the new
      // component (thus the static workAroundEnterBug_ is needed )
      //
      // so without this work around: upon a mouse click 
      // (without prior movement) you'll get
      // a single "mouse entered" event 
      // with this work around: you'll get another "mouse entered" event

      if (evt.getID() == MouseEvent.MOUSE_ENTERED && evt.getClickCount() > 0)
      {
         if(workAroundEnterBug_)
         {
            EventListener[] listeners = getListeners(ActionListener.class);
            if (listeners != null && listeners.length > 0)
            {
               for (int i=0; i<listeners.length; ++i)
                  ((ActionListener)listeners[i]).actionPerformed(
                     new ActionEvent(evt.getSource(), 0, getText()));
            }
         }
         workAroundEnterBug_ = !workAroundEnterBug_;
      }
      */
   }

      
}
