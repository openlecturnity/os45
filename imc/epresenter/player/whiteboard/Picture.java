package imc.epresenter.player.whiteboard;

import java.awt.*;
import java.awt.geom.*;
import java.awt.image.*;
import java.io.*;
import java.lang.ref.SoftReference;
import java.net.URL;
import java.util.ArrayList;
import java.util.HashMap;

import imc.epresenter.filesdk.FileResources;
import imc.epresenter.filesdk.ResourceNotFoundException;
import imc.epresenter.player.util.*;
import imc.epresenter.player.Manager;
import imc.lecturnity.util.FileUtils;

public class Picture extends VisualComponent {
   private static ArrayList errorShown = new ArrayList();
   
   private ImageVault vault;
   private HashMap idCollector;
    
	private int imageId;
	private String fname;
   private boolean imageAvailable;
   private FileResources resources;
   private short m_iInitialX, m_iInitialY;
   private short m_sBelongsKey = -1;
   
   
   private float m_fRemainderX = 0, m_fRemainderY = 0;

	Picture(String line, BufferedReader in, FileResources r, ImageVault v, HashMap idC, String strInputCodepage) {
		vault = v;
      idCollector = idC;
      resources = r;

		SGMLParser parser = new SGMLParser(line);
		m_iInitialX = extractShort(parser, "x");
		clip.x = m_iInitialX;
      m_iInitialY = extractShort(parser, "y");
      clip.y = m_iInitialY;
		clip.width = extractShort(parser, "width");
		clip.height = extractShort(parser, "height");
		fname = extractPath(parser, "fname", strInputCodepage);

      Integer id = (Integer)idCollector.get(fname);

      if (true)
      {
         if (id != null) // image with this name previously encountered
         {
            imageId = id.intValue();
            imageAvailable = true;
         }
         else // default load image in byte array and start async loading
         {
            loadImage();
         }
      }
      else
         imageAvailable = false;
           
      String strMove = extractString(parser, "move");
      m_bMoveable = strMove != null && strMove.equals("true");
      
      // maybe this image is moveable (drag&drop)
      if (m_bMoveable)
      {
         super.ParseActivityVisibility(parser, true);
         
         m_sBelongsKey = extractShort(parser, "belongs");
      }
	}
   
   public Rectangle GetOrigImageArea()
   {
      return new Rectangle(m_iInitialX, m_iInitialY, clip.width, clip.height);
   }
   
   public short GetBelongsKey()
   {
      return m_sBelongsKey;
   }
   
  
   public void MovePosition(float fDiffX, float fDiffY)
   {
      float fNewX = clip.x + fDiffX + m_fRemainderX;
      float fNewY = clip.y + fDiffY + m_fRemainderY;
     
      clip.x = (int)fNewX;
      clip.y = (int)fNewY;
      
      // x, y are ints, so the decimal place of the change would be lost;
      // store that: the small amounts should not add up
      m_fRemainderX = fNewX - clip.x;
      m_fRemainderY = fNewY - clip.y;
   }
   
   public void ResetPosition()
   {
      clip.x = m_iInitialX;
      clip.y = m_iInitialY;
		m_fRemainderX = 0;
      m_fRemainderY = 0;
   }
   
   public boolean HitTest(int x, int y)
   {
      return x >= clip.x && x < clip.x + clip.width && y >= clip.y && y < clip.y + clip.height;
   }
   
   public String GetFileName()
   {
      return fname;
   }

	public void paint(Graphics g, int width, int height, double scale) {
		if (vault != null && imageAvailable) {
		

         Image image = null;
         image = vault.getImage(imageId);
         
         // check if the size specified in the object queue
         // is different from the normal size, if so paint scaled (below)
         double dPreScaleX = 1.0;
         if (clip.width != image.getWidth(null))
            dPreScaleX = clip.width/(double)image.getWidth(null);
         double dPreScaleY = 1.0;
         if (clip.height != image.getHeight(null))
            dPreScaleY = clip.height/(double)image.getHeight(null);

         long time1 = System.currentTimeMillis();
         if (scale != 1.0 || dPreScaleX != 1.0 || dPreScaleY != 1.0)
         {
            /* old scaling code           
            RenderingHints renderHints = ((Graphics2D)g).getRenderingHints();
            renderHints.put(RenderingHints.KEY_INTERPOLATION,
                            RenderingHints.VALUE_INTERPOLATION_BICUBIC);
            renderHints.put(RenderingHints.KEY_RENDERING,
                            RenderingHints.VALUE_RENDER_QUALITY);
            ((Graphics2D)g).setRenderingHints(renderHints);
            
            g.drawImage(image, (int)(scale*clip.x + 0.5), (int)(scale*clip.y + 0.5),
                        (int)(scale*clip.width + 0.5), (int)(scale*clip.height + 0.5), null);
            */
            // PZI: improved scaling quality
            int iTargetWidth = (int) (scale * clip.width + 0.5);
            int iTargetHeight = (int) (scale * clip.height + 0.5);
            if (iTargetWidth > 0 && iTargetHeight > 0) {
               Image scaledImage = getScaledInstance(image, iTargetWidth, iTargetHeight);
               g.drawImage(scaledImage, (int) (scale * clip.x + 0.5), (int) (scale * clip.y + 0.5), null);
            }
         }
			else
				g.drawImage(image, clip.x, clip.y, null);
         long time2 = System.currentTimeMillis();
         //if (time2-time1 > 50)
         //   System.out.print("P"+(time2-time1)+" )"+image.getWidth(null)+"x"+image.getHeight(null)+") ");
            


		} else {
			Rectangle clip = getClip(scale);
			
         Graphics2D g2 = (Graphics2D)g;

         // replacement
			g.setColor(Color.black);
         g.fillRect(clip.x, clip.y, clip.width, clip.height);
         g.setColor(Color.red);
         g.drawRect(clip.x, clip.y, clip.width, clip.height);
      }
	}


   private void loadImage()
   {

      //System.out.print("Q"+fname+" ");
      
      int dataLength = 0;
      int nArraySize = 8192;
      // try to get the array size right from the start:
      int iImageFileSize = (int)resources.getFileSize(fname);
      if (iImageFileSize > 0)
         nArraySize = iImageFileSize;
      byte[] data = new byte[nArraySize];
      try { // fetch the data
         
         BufferedInputStream bis = new BufferedInputStream(resources.createInputStream(fname));

         int readed = 0;
         int counter = 0;
         while ((readed = bis.read(data,counter,data.length-counter)) > 0) {
            counter += readed;
            if (counter == data.length && iImageFileSize <= 0) {
               byte[] newdata = new byte[data.length*2];
               System.arraycopy(data,0,newdata,0,data.length);
               data = newdata;
            }
         }

         dataLength = counter;
         imageAvailable = true;

         bis.close();
      } catch (ResourceNotFoundException e) {
         if (!errorShown.contains(fname)) {
            errorShown.add(fname);
            Manager.showError(Manager.getLocalized("imageNotFound")+" "+fname, Manager.WARNING, e);
         }
         imageAvailable = false;
      } catch (Exception e) {
         System.err.println("Could not load image "+fname+" ("+e+")!");
         imageAvailable = false;
      }


      if (imageAvailable)
      {
         // the byte array is stored in the vault 
         // for initial and later image decoding
         if (vault != null)
            imageId = vault.putImage(data, dataLength);
         // else probably console operation: no graphics display present
         // and thus no images needed (actually this prevents image loading at all)
         idCollector.put(fname, new Integer(imageId));
      }
   }
   
   /**
    * Convenience method that returns a scaled instance of the provided {@code BufferedImage}.
    * 
    * based on code from 
    * http://today.java.net/pub/a/today/2007/04/03/perils-of-image-getscaledinstance.html
    * 
    * @param image
    *           the original image to be scaled
    * @param iTargetWidth
    *           the desired width of the scaled instance, in pixels
    * @param iTargetHeight
    *           the desired height of the scaled instance, in pixels
    * @return a scaled version of the original {@code BufferedImage}
    */
   static synchronized BufferedImage getScaledInstance(Image image, int iTargetWidth,
            int iTargetHeight) {
        // convert to BufferedImage
        BufferedImage bufferedImage = toBufferedImage(image);

        // scaledBufferedImage is used as temporary buffer during scaling
        BufferedImage scaledBufferedImage = bufferedImage;

        int iType = (bufferedImage.getTransparency() == Transparency.OPAQUE) ? 
                BufferedImage.TYPE_INT_RGB : BufferedImage.TYPE_INT_ARGB;

        if (iTargetWidth <= 0)
            iTargetWidth = 1;
        if (iTargetHeight <= 0)
            iTargetHeight = 1;

        // Use multi-step technique: start with original size, then scale down
        // in multiple passes with drawImage() until the target size is reached
        int iWdith = bufferedImage.getWidth();
        int iHeight = bufferedImage.getHeight();

        do {
            if (iWdith > iTargetWidth)
                iWdith /= 2;
            if (iWdith < iTargetWidth)
                iWdith = iTargetWidth;

            if (iHeight > iTargetHeight)
                iHeight /= 2;
            if (iHeight < iTargetHeight)
                iHeight = iTargetHeight;

            BufferedImage tempBufferedImage = new BufferedImage(iWdith, iHeight, iType);
            Graphics2D g2 = tempBufferedImage.createGraphics();
            g2.setRenderingHint(RenderingHints.KEY_INTERPOLATION,
                    RenderingHints.VALUE_INTERPOLATION_BILINEAR);
            g2.drawImage(scaledBufferedImage, 0, 0, iWdith, iHeight, null);
            g2.dispose();

            scaledBufferedImage = tempBufferedImage;

        } while (iWdith != iTargetWidth || iHeight != iTargetHeight);

        return scaledBufferedImage;
    }

    // This method returns a buffered image with the contents of an image
    static BufferedImage toBufferedImage(Image image) {
        if (image instanceof BufferedImage)
            return (BufferedImage) image;

        // Create a buffered image using the default color model
        BufferedImage bufferedImage = new BufferedImage(image.getWidth(null),
                image.getHeight(null), BufferedImage.TYPE_INT_ARGB);

        // Copy image to buffered image
        Graphics graphics = bufferedImage.createGraphics();

        // Paint the image onto the buffered image
        graphics.drawImage(image, 0, 0, null);
        graphics.dispose();

        return bufferedImage;
    }
}