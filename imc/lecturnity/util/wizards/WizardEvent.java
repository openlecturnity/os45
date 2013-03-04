package imc.lecturnity.util.wizards;

public class WizardEvent
{
   /**
    * The {@link #getEvent() getEvent()} method returns
    * <tt>CANCELLED</tt> if the originating {@link Wizard Wizard} was
    * exited using the "Cancel" button.
    * @see #FINISHED
    * @see #getEvent()
    */
   public static final int CANCELLED = -1;

   /**
    * The {@link #getEvent() getEvent()} method returns
    * <tt>FINISHED</tt> if the originating {@link Wizard Wizard} was
    * exited using the "Quit" button (success).
    * @see #CANCELLED
    * @see #getEvent()
    */
   public static final int FINISHED  = 0;

   private Wizard wizard_;
   private int event_;
   private Object listenerData_;

   public WizardEvent(Wizard wizard, int event, Object listenerData)
   {
      super();

      wizard_ = wizard;
      event_ = event;
      listenerData_ = listenerData;
   }
   
   /**
    * Returns the {@link Wizard Wizard} object this event originates
    * from.
    * @return the <tt>Wizard</tt> object this event originates
    * from.
    */
   public Wizard getWizard()
   {
      return wizard_;
   }

   /**
    * Returns the listener data supplied at the instanciation of the
    * originating {@link Wizard Wizard} object.
    * @return the listener data supplied at the instanciation of the
    * originating {@link Wizard Wizard} object.
    */
   public Object getListenerData()
   {
      return listenerData_;
   }
   
   /**
    * Returns the type of the <tt>WizardEvent</tt>.
    * @return the type of the <tt>WizardEvent</tt>.
    * @see #CANCELLED
    * @see #FINISHED
    */
   public int getEvent()
   {
      return event_;
   }
}
