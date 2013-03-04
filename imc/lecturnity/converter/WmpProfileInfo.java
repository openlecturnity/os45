package imc.lecturnity.converter;

public class WmpProfileInfo
{
   /**
    * Some static variables for the WMT version
    */
   // WMT_UNKNOWN is NOT WMT standard
   public static final int WMT_UNKNOWN	= 0x00000;
   // WMT_VER_UCP (user custom profile) is NOT WMT standard
   public static final int WMT_VER_UCP	= 0x20000;
   // WMT_VER_4_0 is WMT standard for WM 6.4
   public static final int WMT_VER_4_0	= 0x40000;
   // WMT_VER_7_0 is WMT standard for WM 7.0 and 7.1
   public static final int WMT_VER_7_0	= 0x70000;
   // WMT_VER_8_0 is WMT standard for WM 8.0
   public static final int WMT_VER_8_0	= 0x80000;
   // WMT_VER_9_0 is WMT standard for WM 9.0 (but no system profiles exist)
   public static final int WMT_VER_9_0	= 0x90000;

   /**
    * Members
    */
   public String  name;
   public boolean isVideoProfile;
   public int     videoWidth;
   public int     videoHeight;
   public int     wmtVersion;
   public int     wmtIndex;
   
   /**
    * Empty Constructor
    */
   public WmpProfileInfo()
   {
      name           = "";
      isVideoProfile = false;
      videoWidth     = 0;
      videoHeight    = 0;
      wmtVersion     = WMT_UNKNOWN;
      wmtIndex       = 0;
   }

   /**
    * Constructor
    */
   public WmpProfileInfo(String  name, 
                         boolean isVideoProfile, 
                         int     videoWidth, 
                         int     videoHeight,
                         int     wmtVersion,
                         int     wmtNumber)
   {
      this.name            = new String(name);
      this.isVideoProfile  = isVideoProfile;
      this.videoWidth      = videoWidth;
      this.videoHeight     = videoHeight;
      this.wmtVersion      = wmtVersion;
      this.wmtIndex        = wmtIndex;
   }
   
   /**
    * Copy Constructor
    */
   public WmpProfileInfo(WmpProfileInfo wpi)
   {
      this.name            = new String(wpi.name);
      this.isVideoProfile  = wpi.isVideoProfile;
      this.videoWidth      = wpi.videoWidth;
      this.videoHeight     = wpi.videoHeight;
      this.wmtVersion      = wpi.wmtVersion;
      this.wmtIndex        = wpi.wmtIndex;
   }
}
