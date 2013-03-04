package imc.lecturnity.converter;

public class SlideEntry
{
   public int        nImageNumber;
   public long       lTimestamp;
   public String     strImageName;
   public boolean    bIsNewPage;
   public int        nLayerIndex;
   public int        nX;
   public int        nY;
   public int        nWidth;
   public int        nHeight;
   
   /**
    * Empty Constructor
    */
   public SlideEntry()
   {
      nImageNumber      = 0;
      lTimestamp        = 0;
      strImageName      = "";
      bIsNewPage        = false;
      nLayerIndex       = 0;
      nX                = 0;
      nY                = 0;
      nWidth            = 0;
      nHeight           = 0;
   }

   /**
    * Constructor
    */
   public SlideEntry(int      imageNumber, 
                     long     timestamp, 
                     String   imageName, 
                     boolean  isNewPage, 
                     int      layerIndex,
                     int      x,
                     int      y,
                     int      width,
                     int      height)
   {
      this.nImageNumber       = imageNumber;
      this.lTimestamp         = timestamp;
      this.strImageName       = new String(imageName);
      this.bIsNewPage         = isNewPage;
      this.nLayerIndex        = layerIndex;
      this.nX                 = x;
      this.nY                 = y;
      this.nWidth             = width;
      this.nHeight            = height;
   }

   /**
    * Copy Constructor
    */
   public SlideEntry(SlideEntry se)
   {
      this.nImageNumber       = se.nImageNumber;
      this.lTimestamp         = se.lTimestamp;
      this.strImageName       = new String(se.strImageName);
      this.bIsNewPage         = se.bIsNewPage;
      this.nLayerIndex        = se.nLayerIndex;
      this.nX                 = se.nX;
      this.nY                 = se.nY;
      this.nWidth             = se.nWidth;
      this.nHeight            = se.nHeight;
   }
}
