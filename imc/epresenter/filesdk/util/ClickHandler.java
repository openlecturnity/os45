package imc.epresenter.filesdk.util;

import java.awt.event.MouseListener;
import java.awt.event.MouseEvent;

/**
 * Use this class in conjunction with lightweight components
 * (e.g. <tt>JLabel</tt> instances) from which you need to receive
 * "click" events. Normally, java only dispatches
 * <tt>mouseClicked()</tt> events if the mouse has not moved between
 * the <tt>mousePressed()</tt> and the <tt>mouseReleased()</tt>
 * event. This handler implements a "button-like" behaviour: The
 * <tt>mouseClicked()</tt> event is dispatched if the mouse is pressed
 * and released inside the same component. You will need a
 * <tt>ClickHandler</tt> instance for each component.
 *
 * <p>Usage:
 * <pre>
 * myLabel.addMouseListener(new ClickHandler(new MyMouseListener()));
 * [ ... ]
 *
 * private class MyMouseListener extends MouseAdapter
 * {
 *    public void mouseClicked(MouseEvent e)
 *    {
 *        <i>// handle click...</i>
 *    }
 * }
 * </pre>
 *
 * @author <a href="mailto:martin.danielsson@im-c.de">Martin Danielsson</a>
 */
public class ClickHandler implements MouseListener
{
   private MouseListener mouseListener_ = null;
   private boolean isInComponent_ = false;
   private boolean isMousePressed_ = false;
   private int clickCount_ = 0;
   private long lastClickTime_ = -1;

   public ClickHandler(MouseListener mouseListener)
   {
      mouseListener_ = mouseListener;
   }
   
   public void mouseEntered(MouseEvent e)
   {
      isInComponent_ = true;
      mouseListener_.mouseEntered(e);
   }

   public void mouseExited(MouseEvent e)
   {
      isInComponent_ = false;
      mouseListener_.mouseExited(e);
   }

   public void mousePressed(MouseEvent e)
   {
      isMousePressed_ = true;
      mouseListener_.mousePressed(e);
   }

   public void mouseReleased(MouseEvent e)
   {
      isMousePressed_ = false;
      mouseListener_.mouseReleased(e);
      if (isInComponent_)
         mouseListener_.mouseClicked(e);
   }

   public void mouseClicked(MouseEvent e)
   {
      // these events will not be used here; they are managed over the
      // mousePressed and mouseReleased events in order to simulate a
      // correct behaviour (windows like) of the mouse events.
   }
}
