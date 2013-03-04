package imc.epresenter.filesdk;

/**
 * An instance of this class contains data on a video clip used in a
 * presentation. These clips are usually screen grabbing clips, but
 * may be of any kind.
 */
public class VideoClipInfo
{
   public VideoClipInfo(String clipFileName, long startTimeMs)
   {
      this.clipFileName = clipFileName;
      this.startTimeMs  = startTimeMs;
   }

   /**
    * The file name of the video file.
    */
   public String clipFileName = null;

   /**
    * The start time of this clip in the presentation, in <i>ms</i>.
    */
   public long startTimeMs;

   /**
    * The width of the video clip, or <tt>-1</tt> if unknown.
    */
   public int videoWidth = -1;

   /**
    * The height of the video clip, or <tt>-1</tt> if unknown.
    */
   public int videoHeight = -1;

   /**
    * The duration of the video clip in ms, or <tt>-1</tt> if unknown.
    */
   public int videoDurationMs = -1;
   
   /**
    * The number of frames per second * 10.
    */
   public int videoFramerateTenths = -10;
   
   /**
    * The FCC of the video stream.
    */
   public int fccCodec = 0;
      
   /**
    * PZI: Clip Structuring Information
    */
   public boolean structureAvailable = false;
   public boolean imagesAvailable = false;
   public boolean fulltextAvailable = false;   
}
