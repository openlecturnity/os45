package imc.lecturnity.converter;

public class TelepointerEntry
{
   public static final int TYPE_POINTER   = 0;
   public static final int TYPE_CURSOR    = 1;

   public long       lTimestamp;
   public int        nX;
   public int        nY;
   public int        nType;
   public boolean    bIsVisible;
   
   /**
    * Empty Constructor
    */
   public TelepointerEntry()
   {
      lTimestamp        = 0;
      nX                = 0;
      nY                = 0;
      nType             = TYPE_POINTER; // default
      bIsVisible        = true;
   }

   /**
    * Constructor
    */
   public TelepointerEntry(long     timestamp, 
                           int      x, 
                           int      y, 
                           int      nType, 
                           boolean  isVisible)
   {
      this.lTimestamp      = timestamp;
      this.nX              = x;
      this.nY              = y;
      this.nType           = nType;
      this.bIsVisible      = isVisible;
   }

   public TelepointerEntry(long     timestamp,
                           int      x,
                           int      y,
                           boolean  isVisible)
   {
       this.lTimestamp      = timestamp;
       this.nX              = x;
       this.nY              = y;
       this.nType           = TYPE_POINTER; // default
       this.bIsVisible      = isVisible;
   }

   /**
    * Copy Constructor
    */
   public TelepointerEntry(TelepointerEntry te)
   {
      this.lTimestamp      = te.lTimestamp ;
      this.nX              = te.nX;
      this.nY              = te.nY;
      this.nType           = te.nType;
      this.bIsVisible      = te.bIsVisible;
   }
}
