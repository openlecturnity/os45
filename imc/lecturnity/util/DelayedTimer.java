package imc.lecturnity.util;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;


/**
 * Waits after activate() for events (invocations of delayFurther()) and if
 * after the specified amount of time no event has occured it 
 * informs the specified ActionListener and deactivates.
 */
public class DelayedTimer implements Runnable
{
   private ActionListener listener_;
   private int delay_;
   private ActionEvent event_ = new ActionEvent(this, 0, "grace up");
   private boolean isActivated_;
   private long lastEventMillis_;

   public DelayedTimer(ActionListener toBeInformed, int delay)
   {
      this(toBeInformed, delay, false);
   }

   public DelayedTimer(ActionListener toBeInformed, int delay, boolean startImmediatly)
   {
      listener_ = toBeInformed;
      delay_ = delay;

      if (startImmediatly)
         activate();
   }


   public void activate()
   {
      isActivated_ = true;
      new Thread(this).start();
   }

   public void delayFurther()
   {
      if (!isActivated_)
         activate();
      lastEventMillis_ = System.currentTimeMillis();
   }

   public void deactivate()
   {
      isActivated_ = false;
   }

   public void run()
   {
      lastEventMillis_ = System.currentTimeMillis();

      while (isActivated_)
      {
         long millis = System.currentTimeMillis();

         if (millis - lastEventMillis_ > delay_)
         {
            listener_.actionPerformed(event_);
            isActivated_ = false;

         }
         else
         {
            try { Thread.sleep(delay_ - (millis-lastEventMillis_)); } catch (InterruptedException e) {}
         }
      }
   }

   public boolean isAlive()
   {
      return isActivated_;
   }
}