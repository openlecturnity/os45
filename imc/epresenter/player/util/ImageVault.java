package imc.epresenter.player.util;

import java.awt.*;
import java.awt.image.*;
import java.util.*;

import java.lang.reflect.*;

/**
 * Class for image loading and management.
 * Up to a given limited pixel count (-Dpixel.limit) images
 * are decoded directly. If this limit is exceeded further image decoding
 * is delayed until they are requested (getImage()).
 * Further image management is then done with a LRU queue: everytime
 * getImage() is invoked the respective image is decoded (if needed) and
 * moves to a more prominent place in the queue. If necessary 
 * in order to stay close to the pixel count limit other images
 * are removed from the queue and their image references are set to null. 
 */
public class ImageVault {
   private final int PIXEL_LIMIT = 
      Integer.parseInt(System.getProperty("pixel.limit", "8000000"))/2;
   // "/2" -> there is most likely more than one document instance at some time

	private LinkedList queue;
   private HashMap imageMap;
	private Component dummy;
	private boolean running;
	private Toolkit toolkit;
   private int pixelCounter = 0;
   private int idCounter = 0;
   private MediaTracker tracker;
      

	public ImageVault() {
		queue = new LinkedList();
      imageMap = new HashMap();
      dummy = new Label();
		toolkit = Toolkit.getDefaultToolkit();
      tracker = new MediaTracker(dummy);
   }

	public int putImage(byte[] data, int len) {
		ImageInformation info = new ImageInformation(data, len);
      int id = putImage(info);
      return id;
	}

	public int putImage(String fileName) {
		ImageInformation info = new ImageInformation(fileName);
	   int id = putImage(info);
      return id;
	}

   private int putImage(ImageInformation info) {
      // generate a unique key for later image access
		int id = idCounter++;
		Integer key = new Integer(id);
      
      // add the respective ImageInformation object to the image map
      // some information are already set others are determined later on
      imageMap.put(key, info);
      
      // is there still some space in the image queue?
      if (pixelCounter < PIXEL_LIMIT)
      {
         loadImage(info);
         
         addToQueue(key, info);
      }
      
		return id;

	}

   public Image getImage(int id) {
		Integer key = new Integer(id);
		
      ImageInformation info = (ImageInformation)imageMap.get(key);
      
      // the image reference might have been set to null earlier
      // in order to stay below/close to the pixel limit
      if (info.m_Image == null)
      {
         // if necessary before (re-)loading the image first make
         // some room in the image queue
         while(pixelCounter >= PIXEL_LIMIT && !queue.isEmpty())
            removeFirstFromQueue();
         
         loadImage(info);
      }
      
      // in any case "boosts" the rank of this image in the queue
      addToQueue(key, info);
     		
      return info.m_Image;
   }


   private Image loadImage(ImageInformation info)
   {
      Image image = null;
      
      if (info.m_aData != null)
         image = toolkit.createImage(info.m_aData, 0, info.m_nLength);
      else
         image = toolkit.createImage(info.m_sFileName);
      
      tracker.addImage(image, 0);
      try { tracker.waitForID(0); } catch (InterruptedException e) {}
      tracker.removeImage(image, 0);
       
      if (info.m_nWidth < 0)
      {
         // needs only be done once
         // but it must be done before any calls to addToQueue() and removeFirstFromQueue()
         info.m_nWidth = image.getWidth(null);
         info.m_nHeight = image.getHeight(null);
      }
            
      if (info.m_bIsGif)
      {
         // there is some problem with gif images and transparency
         // this "conversion" helps
         BufferedImage image2 = new BufferedImage(
            info.m_nWidth, info.m_nHeight, BufferedImage.TYPE_INT_ARGB);
               
         image2.getGraphics().drawImage(image, 0,0, dummy);
				
         image = image2;
      }
      
      info.m_Image = image;
 
      return image;
   }
   
   private void addToQueue(Integer key, ImageInformation info)
   {
      if (queue.contains(key))
      {
         // will be re-added below in order for it to have a higher rank
         queue.remove(key);
      }
      else
      {
         // not yet in queue: increase pixelCounter
         pixelCounter += info.m_nWidth*info.m_nHeight;
         //System.out.println("+"+(info.m_nWidth*info.m_nHeight));
      
      }
      
		queue.add(key);
	}
 
   private ImageInformation removeFirstFromQueue()
   {
      ImageInformation info = null;
      
      Integer key = (Integer)queue.removeFirst();
      
      info = (ImageInformation)imageMap.get(key);
      
      info.m_Image = null;
      pixelCounter -= info.m_nWidth*info.m_nHeight;
      //System.out.println("-"+(info.m_nWidth*info.m_nHeight));
      
      return info;
   }
   
   
   /**
    * Contains all necessary information for the image handling and loading/
    * decoding in this class.
    * An image can either come from a file or a byte array.
    */
   private static class ImageInformation
   {
      byte[]  m_aData = null;
      int     m_nLength = -1;
      String  m_sFileName = null;
      boolean m_bIsGif = false;
      
      Image   m_Image = null;
      
      int     m_nWidth = -1;
      int     m_nHeight = -1;
      
      
      ImageInformation(byte[] data, int length)
      {
         m_aData = data;
         m_nLength = length;
         
         if (length >= 3)
         {
            if ((char)data[0] == 'G' && (char)data[1] == 'I' && (char)data[2] == 'F')
               m_bIsGif = true;
         }
		
      }
      
      ImageInformation(String fileName)
      {
         m_sFileName = fileName;
         
         if (fileName.toLowerCase().endsWith(".gif"))
            m_bIsGif = true;
      }
   }
}

            // unworking code to determine if an image has transparency

            /*
            if (queue.size() == 1)
            {
               System.out.println(width+"x"+height);
               Class c = image.getSource().getClass();
               System.out.println(c);
               Method[] ms = c.getMethods();
               for (int i=0; i<ms.length; i++)
               {
                  System.out.println(" "+ms[i].getReturnType()+" "+ms[i].getName());
               }


            }
            */
            
            //System.out.println(image+" "+width+" "+height);

            /*
            int[] pixels = new int[width*height];
				PixelGrabber pg = new PixelGrabber(image, 0, 0, width, height, pixels, 0, width);

            try { pg.grabPixels(); } catch (InterruptedException e) {}
            
            boolean hasTransparency = false;
            for (int p=0; p<pixels.length; p++)
            {
               if ((pixels[p] & 0xff000000) != 0xff000000)
               {
                  hasTransparency = true;
               }
            }
            */
            
            /*
            
            int size = 101*101+2;
            int size2 = size/2;
            IntHash hash = new IntHash(size);
            for (int p=0; p<pixels.length; p++)
            {
               if (hash.size() > size2)
                  break;
               //if (!hash.containsKey(pixels[p] & 0xffffff))
               //   hash.put(pixels[p] & 0xffffff, 0);
            }

            System.out.println(image+" "+hash.size()+" colors");
				*/

            /*
            if (!hasTransparency) {
            //if(System.getProperty("app.imagesInTrueColor") == null && width*height > 65536) { // try to save memory 
					/*
               byte[] red = new byte[512];
               byte[] green = new byte[512];
               byte[] blue = new byte[512];

               for (int r=0; r<4; r++)
               {
                  for (int g=0; g<4; g++)
                  {
                     for (int b=0; b<4; b++)
                     {
                        int idx = r*16+g*4+b;
                        red[idx] = (byte)(r/3.0f*255);
                        green[idx] = (byte)(g/3.0f*255);
                        blue[idx] = (byte)(b/3.0f*255);
                     }
                  }
               }
               
               IndexColorModel icm = new IndexColorModel(6, 64, red, green, blue, 63);
               */
  


