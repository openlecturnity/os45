package imc.lecturnity.converter;

import imc.epresenter.filesdk.FileResources;
import imc.epresenter.filesdk.ResourceNotFoundException;
import imc.epresenter.filesdk.VideoClipInfo;

import imc.epresenter.filesdk.util.Localizer;

import imc.lecturnity.util.NativeUtils;

import imc.lecturnity.converter.wizard.ConvertWizardData;
import imc.lecturnity.converter.wizard.PublisherWizardData;
import imc.epresenter.converter.ConversionDisplay;
import imc.epresenter.converter.DefaultConversionDisplay;

import imc.epresenter.player.whiteboard.EventQueue;
import imc.epresenter.player.whiteboard.ObjectQueue;
import imc.epresenter.player.whiteboard.ProgressListener;
import imc.epresenter.player.whiteboard.VisualComponent;
import imc.epresenter.player.whiteboard.ButtonArea;
import imc.epresenter.player.whiteboard.Picture;

import imc.lecturnity.converter.DocumentData;

import java.awt.*;
import java.awt.image.BufferedImage;
import java.awt.geom.AffineTransform;

import javax.media.jai.JAI;
import javax.media.jai.RenderedOp;
import javax.media.jai.LookupTableJAI;
import javax.media.jai.TiledImage;
import javax.media.jai.operator.ColorQuantizerDescriptor;
import javax.media.jai.iterator.RandomIter;
import javax.media.jai.iterator.RandomIterFactory;
import java.awt.image.SampleModel;
import java.awt.image.RenderedImage;
import java.awt.image.DataBuffer;
import java.awt.image.renderable.ParameterBlock;

import javax.swing.JOptionPane;

import java.io.*;

import java.util.HashMap;
import java.lang.ref.SoftReference;

import com.sun.media.jai.codec.ImageEncoder;
import com.sun.media.jai.codec.ImageCodec;
import com.sun.media.jai.codec.PNGEncodeParam;

import com.sun.image.codec.jpeg.*;

/**
 * This class makes use of the Player facilities to parse event and
 * object queue in order to create snapshots of the slides like they
 * would be displayed in the Player.
 *
 * @see imc.epresenter.player.whiteboard.ObjectQueue
 * @see imc.epresenter.player.whiteboard.EventQueue
 * @author Martin Danielsson
 */
public class PictureExtracter
{
   //private static final Color BORDERLINE_COLOR  = new Color(0x333333);
   private static final Color BORDERLINE_COLOR  = new Color(0x000000);
   private static final Color SHADOW_COLOR      = new Color(0x686868);
   private static final Color SHADE_COLOR       = new Color(0x3F3F3F);
   private static final Color HILITE_COLOR      = new Color(0xFFFFFF);

   private static String PARSING = "[!]";
   private static String EVAL_NOTE = "[!]";
   private static String PREP_THUMBS = "[!]";
   
   static
   {
      try
      {
         Localizer l = new Localizer
            ("/imc/lecturnity/converter/PictureExtracter_",
             "en");

         PARSING = l.getLocalized("PARSING");
         EVAL_NOTE = l.getLocalized("EVAL_NOTE");
         PREP_THUMBS = l.getLocalized("PREP_THUMBS");
      }
      catch (Exception e)
      {
         JOptionPane.showMessageDialog(null, "Error reading Locale database:\n" +
                                       e.toString() + "\n" +
                                       "PictureExtracter", "Error",
                                       JOptionPane.ERROR_MESSAGE);
         e.printStackTrace();
      }
   }

   private ConvertWizardData data_  = null;
   private PublisherWizardData pwData_ = null;

   private FileResources resources_ = null;
   private Dimension size_ = null;
   private double zoomFactor_ = 1.0d;
   private ConversionDisplay display_ = null;

   private EventQueue eventQueue_;
   private ObjectQueue objectQueue_;

   private Color backgroundColor_ = Color.white;
   private Color shadowBackgroundColor_ = Color.white;
   private Color borderLineColor_ = BORDERLINE_COLOR;

   private BufferedImage imageBuffer_ = null;
   private RenderingHints antialiasRenderingHints_;
   private RenderingHints noRenderingHints_;
   private BasicStroke onePixelStroke_;

   private AffineTransform rotateTransform_;
   private AffineTransform identityTransform_;
   private AlphaComposite alphaComposite_;

   private boolean drawFrame_ = false;
   private boolean drawBorderLine_ = false;
   private boolean drawShadow_ = false;
   private boolean drawEvalNote_ = false;

   private int shadowThickness_ = 0;

   private HashMap aviFrameMap_ = null;
   
   /**
    * Creates a new instance of the <tt>PictureExtracter</tt>
    * class. The default size is set to 100x100 pixels, and messages
    * are output to <tt>stdout</tt>.
    */
   public PictureExtracter(PublisherWizardData pwData, 
                           FileResources resources)
      throws Exception
   {
      this(pwData, resources, new Dimension(100, 100),
           new DefaultConversionDisplay());
   }

   /**
    * Creates a new <tt>PictureExtracter</tt> instance.
    * @param pwData the Publisher wizard data collected up to now
    * @param resources a <tt>FileResources</tt> instance for the
    * recording document
    * @param pictureSize the maximum picture size of the snapshots to
    * create
    * @param display the display to output messages to.
    * @see ConversionDisplay
    * @see PublisherWizardData
    * @see FileResources
    */
   public PictureExtracter(PublisherWizardData pwData,
                           FileResources resources,
                           Dimension pictureSize,
                           ConversionDisplay display)
      throws Exception
   {
      super();

      pwData_ = pwData;

      resources_ = resources;
      size_ = pictureSize;
      display_ = display;
   
      backgroundColor_ = Color.white;
         
      if (display_ == null)
         display_ = new DefaultConversionDisplay();
   
      createBufferImage();
      initRenderingHints();
      initQueues();
   }

   /**
    * Get the picture size
    */
   public Dimension getPictureSize()
   {
      return size_;
   }

   /**
    * Sets a new maximal picture size of the snapshots.
    */
   public void setPictureSize(Dimension size)
   {
      size_ = size;
      createBufferImage();
   }

   /**
    * Set this to <i>true</i> if you want the
    * <tt>PictureExtracter</tt> instance to draw a one pixel frame
    * around the snapshots (sunken border).
    */
   public void setDrawFrame(boolean b)
   {
      drawFrame_ = b;
   }

   /**
    * Set this to <i>true</i> if you want the
    * <tt>PictureExtracter</tt> instance to draw a one pixel border line
    * around the snapshots.
    *
    * @see #setBorderLineColor(java.awt.Color)
    */
   public void setDrawBorderLine(boolean b)
   {
      drawBorderLine_ = b;
   }

   /**
    * Use this method to set the color the border line is drawn with.
    * (The default is 0x000000, black - previously 0x333333, a grey tone.)
    *
    * @see #setDrawBorderLine(boolean)
    */
   public void setBorderLineColor(Color c)
   {
      borderLineColor_ = c;
   }

   /**
    * Set this to <i>true</i> if you want the
    * <tt>PictureExtracter</tt> instance to draw a shadow 
    * on the right side and the bottom side of the snapshots.
    */
   public void setDrawShadow(boolean b)
   {
      drawShadow_ = b;
   }

   /**
    * Set the thickness (in <i>pixels</i>) of the shadow the 
    * <tt>PictureExtracter</tt> instance has to draw 
    * on the right side and the bottom side of the snapshots.
    */
   public void setShadowThickness(int n)
   {
      shadowThickness_ = n;
      createBufferImage();
   }

   /**
    * Use this method in order to add a "Lecturnity Evaluation version" 
    * note across the extracted picture.
    */
   public void setDrawEvalNote(boolean b)
   {
      drawEvalNote_ = b;
   }

   /**
    * Set the color of the background of the pictures to create.
    */
   public void setBackground(Color background)
   {
      backgroundColor_ = background;
   }

   /**
    * Set the color of the background behind the shadow 
    * of the pictures to create.
    */
   public void setShadowBackground(Color background)
   {
      shadowBackgroundColor_ = background;
   }

   /**
    * Write a snapshot of timestamp <tt>timestamp</tt> into a file
    * with the given <tt>fileName</tt>.
    * @throws IOException if something went wrong during the writing process
    */
   public void writePngPicture(int timeStamp, String fileName)
      throws IOException
   {
      writePngPicture(timeStamp, fileName, false);
   }
   
   /**
    * Write a snapshot of timestamp <tt>timestamp</tt> into a file
    * with the given <tt>fileName</tt> including/excluding interactive parts.
    * @throws IOException if something went wrong during the writing process
    */
   public void writePngPicture(int timeStamp, String fileName, boolean bIncludeInteractives)
      throws IOException
   {
      writePngPicture(timeStamp, 
                      new BufferedOutputStream(new FileOutputStream(fileName)), 
                      bIncludeInteractives);
   }
   
   /**
    * Write a clipping region <tt>rect</tt> of timestamp <tt>timestamp</tt> 
    * into a file with the given <tt>fileName</tt>.
    * @throws IOException if something went wrong during the writing process
    */
   public void writePngPicture(int timeStamp, String fileName, Rectangle rect)
      throws IOException
   {
      writePngPicture(timeStamp, fileName, rect, false);
   }
   
   /**
    * Write a clipping region <tt>rect</tt> of timestamp <tt>timestamp</tt> 
    * into a file with the given <tt>fileName</tt> including/excluding interactive parts.
    * @throws IOException if something went wrong during the writing process
    */
   public void writePngPicture(int timeStamp, String fileName, Rectangle rect, boolean bIncludeInteractives)
      throws IOException
   {
      writePngPicture(timeStamp,
                      new BufferedOutputStream(new FileOutputStream(fileName)),
                      rect, bIncludeInteractives);
   }
   
   /**
    * Write a snapshot of timestamp <tt>timestamp</tt> into the given
    * <tt>OutputStream</tt> instance (<tt>out</tt>). <b>Note:</b> The
    * output stream is <b>not</b> additionally buffered. You will need
    * to do this by yourself if you want to (recommended).
    * @throws IOException if something went wrong during the writing process
    */
   public void writePngPicture(int timeStamp, OutputStream out, boolean bIncludeInteractives)
      throws IOException
   {
      fillImageBuffer(timeStamp, bIncludeInteractives);

      ImageEncoder pngWriter = 
         ImageCodec.createImageEncoder("PNG", out,
                                       PNGEncodeParam.getDefaultEncodeParam(imageBuffer_));
      pngWriter.encode(imageBuffer_);

      out.close();
   }
   
   /**
    * Write a clipping region <tt>rect</tt> of timestamp <tt>timestamp</tt> 
    * into the given <tt>OutputStream</tt> instance (<tt>out</tt>). 
    * <b>Note:</b> The output stream is <b>not</b> additionally buffered. 
    * You will need to do this by yourself if you want to (recommended).
    * @throws IOException if something went wrong during the writing process
    */
   public void writePngPicture(int timeStamp, OutputStream out, Rectangle rect, boolean bIncludeInteractives)
      throws IOException
   {
      BufferedImage bi = new BufferedImage(rect.width, rect.height, BufferedImage.TYPE_INT_ARGB);
      fillImageBuffer(timeStamp, bIncludeInteractives);
      
      Graphics g = (Graphics) bi.getGraphics();
      g.drawImage(imageBuffer_, -rect.x, -rect.y, null);

      ImageEncoder pngWriter = 
         ImageCodec.createImageEncoder("PNG", out,
                                       PNGEncodeParam.getDefaultEncodeParam(bi));
      pngWriter.encode(bi);

      out.close();
   }

   /**
    * Write the active region <tt>rect</tt> of a ButtonArea <tt>buttonArea</tt> 
    * into a file with the given <tt>fileName</tt>. 
    * The button state (normal, over, pressed, inactive) has to be set before 
    * calling this method.
    * @throws IOException if something went wrong during the writing process
    */
   public void writePngPicture(ButtonArea buttonArea, String fileName, Rectangle rect, 
                               boolean bUsePaletteColors)
      throws IOException
   {
      writePngPicture(buttonArea,
                      new BufferedOutputStream(new FileOutputStream(fileName)), 
                      rect, bUsePaletteColors);
      
   }
   
   /**
    * Write athe active region <tt>rect</tt> of a ButtonArea <tt>buttonArea</tt> 
    * into the given <tt>OutputStream</tt> instance (<tt>out</tt>). 
    * The button state (normal, over, pressed, inactive) has to be set before 
    * calling this method.
    * <b>Note:</b> The output stream is <b>not</b> additionally buffered. 
    * You will need to do this by yourself if you want to (recommended).
    * @throws IOException if something went wrong during the writing process
    */
   public void writePngPicture(ButtonArea buttonArea, OutputStream out, Rectangle rect, 
                               boolean bUsePaletteColors)
      throws IOException
   {
      if (!bUsePaletteColors)
         createBufferImage(true); // create a BufferedImage with alpha channel

      BufferedImage bi = new BufferedImage(rect.width, rect.height, BufferedImage.TYPE_INT_ARGB);
      fillImageBuffer(buttonArea);

      Graphics g = (Graphics) bi.getGraphics();
      g.drawImage(imageBuffer_, -rect.x, -rect.y, null);

      if (bUsePaletteColors)
      {
         TiledImage ti = imageReduceColorsToPalette(bi);
      
         PNGEncodeParam.Palette encPngPal = new PNGEncodeParam.Palette();
         // use index 255 as transparent color (all others are opaque)
         byte[] alpha = new byte[256];
         for (int i = 0 ; i < 256; ++i)
            alpha[i] = (byte)0xFF;
         alpha[255] = (byte)0x00;
         encPngPal.setPaletteTransparency(alpha);

         ImageEncoder pngWriter = 
         ImageCodec.createImageEncoder("PNG", out,
                                       encPngPal);

         pngWriter.encode(ti);
      }
      else
      {
         ImageEncoder pngWriter = 
         ImageCodec.createImageEncoder("PNG", out,
                                       PNGEncodeParam.getDefaultEncodeParam(bi));

         pngWriter.encode(bi);
      }

      out.close();
   }

   /**
    * Write the active region <tt>rect</tt> of a Picture <tt>picture</tt> 
    * into a file with the given <tt>fileName</tt>. 
    * @throws IOException if something went wrong during the writing process
    */
   public void writePngPicture(Picture picture, String fileName, Rectangle rect, 
                               boolean bUsePaletteColors)
      throws IOException
   {
      writePngPicture(picture,
                      new BufferedOutputStream(new FileOutputStream(fileName)), 
                      rect, bUsePaletteColors);
      
   }
   
   /**
    * Write athe active region <tt>rect</tt> of a Picture <tt>picture</tt> 
    * into the given <tt>OutputStream</tt> instance (<tt>out</tt>). 
    * <b>Note:</b> The output stream is <b>not</b> additionally buffered. 
    * You will need to do this by yourself if you want to (recommended).
    * @throws IOException if something went wrong during the writing process
    */
   public void writePngPicture(Picture picture, OutputStream out, Rectangle rect, 
                               boolean bUsePaletteColors)
      throws IOException
   {
      if (!bUsePaletteColors)
         createBufferImage(true); // create a BufferedImage with alpha channel

      BufferedImage bi = new BufferedImage(rect.width, rect.height, BufferedImage.TYPE_INT_ARGB);
      fillImageBuffer(picture);

      Graphics g = (Graphics) bi.getGraphics();
      g.drawImage(imageBuffer_, -rect.x, -rect.y, null);

      if (bUsePaletteColors)
      {
         TiledImage ti = imageReduceColorsToPalette(bi);
      
         PNGEncodeParam.Palette encPngPal = new PNGEncodeParam.Palette();
         // use index 255 as transparent color (all others are opaque)
         byte[] alpha = new byte[256];
         for (int i = 0 ; i < 256; ++i)
            alpha[i] = (byte)0xFF;
         alpha[255] = (byte)0x00;
         encPngPal.setPaletteTransparency(alpha);

         ImageEncoder pngWriter = 
         ImageCodec.createImageEncoder("PNG", out,
                                       encPngPal);

         pngWriter.encode(ti);
      }
      else
      {
         ImageEncoder pngWriter = 
         ImageCodec.createImageEncoder("PNG", out,
                                       PNGEncodeParam.getDefaultEncodeParam(bi));

         pngWriter.encode(bi);
      }

      out.close();
   }

   public void writeAviFramePngPicture(String aviFileName, String fileName)
      throws Exception
   {
      writeAviFramePngPicture(aviFileName, 
                              new BufferedOutputStream(new FileOutputStream(fileName)));
   }

   public void writeAviFramePngPicture(String aviFileName, OutputStream out)
      throws Exception
   {
      fillAviImageBuffer(aviFileName);

      ImageEncoder pngWriter = 
         ImageCodec.createImageEncoder("PNG", out,
                                       PNGEncodeParam.getDefaultEncodeParam(imageBuffer_));
      pngWriter.encode(imageBuffer_);

      out.close();
   }

   /**
    * Write a snapshot of timestamp <tt>timestamp</tt> into a file
    * with the given <tt>fileName</tt>.
    * @throws IOException if something went wrong during the writing process
    */
   public void writeJpegPicture(int timeStamp, String fileName)
      throws IOException
   {
      writeJpegPicture(timeStamp, fileName, false);
   }

   /**
    * Write a snapshot of timestamp <tt>timestamp</tt> into a file
    * with the given <tt>fileName</tt>.
    * @throws IOException if something went wrong during the writing process
    */
   public void writeJpegPicture(int timeStamp, String fileName, boolean bIncludeInteractives)
      throws IOException
   {
      writeJpegPicture(timeStamp, 
                       new BufferedOutputStream(new FileOutputStream(fileName)), 
                       bIncludeInteractives);
   }

   /**
    * Write a clipping region <tt>rect</tt> of timestamp <tt>timestamp</tt> 
    * into a file with the given <tt>fileName</tt>.
    * @throws IOException if something went wrong during the writing process
    */
   public void writeJpegPicture(int timeStamp, String fileName, Rectangle rect)
      throws IOException
   {
      writeJpegPicture(timeStamp, fileName, rect, false);
   }
   
   /**
    * Write a clipping region <tt>rect</tt> of timestamp <tt>timestamp</tt> 
    * into a file with the given <tt>fileName</tt> including/excluding interactive parts.
    * @throws IOException if something went wrong during the writing process
    */
   public void writeJpegPicture(int timeStamp, String fileName, Rectangle rect, boolean bIncludeInteractives)
      throws IOException
   {
      writeJpegPicture(timeStamp,
                       new BufferedOutputStream(new FileOutputStream(fileName)),
                       rect, bIncludeInteractives);
   }
   
   /**
    * Write a snapshot of timestamp <tt>timestamp</tt> into the given
    * <tt>OutputStream</tt> instance (<tt>out</tt>). <b>Note:</b> The
    * output stream is <b>not</b> additionally buffered. You will need
    * to do this by yourself if you want to (recommended).
    * @throws IOException if something went wrong during the writing process
    */
   public void writeJpegPicture(int timeStamp, OutputStream out, boolean bIncludeInteractives)
      throws IOException
   {
      fillImageBuffer(timeStamp, bIncludeInteractives);

//       ImageEncoder jpegWriter = 
//          ImageCodec.createImageEncoder("JPEG", out,
//                                        new JPEGEncodeParam());
//       jpegWriter.encode(imageBuffer_);

      JPEGEncodeParam param = JPEGCodec.getDefaultJPEGEncodeParam(imageBuffer_);
      JPEGImageEncoder encoder = JPEGCodec.createJPEGEncoder(out, param);
      encoder.encode(imageBuffer_);

      out.flush();
      out.close();
   }

   /**
    * Write a clipping region <tt>rect</tt> of timestamp <tt>timestamp</tt> 
    * into the given <tt>OutputStream</tt> instance (<tt>out</tt>). 
    * <b>Note:</b> The output stream is <b>not</b> additionally buffered. 
    * You will need to do this by yourself if you want to (recommended).
    * @throws IOException if something went wrong during the writing process
    */
   public void writeJpegPicture(int timeStamp, OutputStream out, Rectangle rect, boolean bIncludeInteractives)
      throws IOException
   {
      BufferedImage bi = new BufferedImage(rect.width, rect.height, BufferedImage.TYPE_INT_ARGB);
      fillImageBuffer(timeStamp, bIncludeInteractives);
      
      Graphics g = (Graphics) bi.getGraphics();
      g.drawImage(imageBuffer_, -rect.x, -rect.y, null);

      JPEGEncodeParam param = JPEGCodec.getDefaultJPEGEncodeParam(imageBuffer_);
      JPEGImageEncoder encoder = JPEGCodec.createJPEGEncoder(out, param);
      encoder.encode(imageBuffer_);

      out.flush();
      out.close();   }
   
   private void drawShadow(Graphics2D g)
   {
      if (shadowThickness_ > 0)
      {
         g.setColor(shadowBackgroundColor_);
         g.fillRect(size_.width, 0, 
                    (size_.width+shadowThickness_), (size_.height+shadowThickness_));
         g.fillRect(0, size_.height, 
                    (size_.width+shadowThickness_), (size_.height+shadowThickness_));
      }
   }

   private void drawBorderExtras(Graphics2D g)
   {
      if (drawFrame_)
      {
         g.setRenderingHints(noRenderingHints_);
         g.setStroke(onePixelStroke_);
         g.setColor(SHADE_COLOR);
         g.drawLine(0, 0, size_.width-1, 0);
         g.drawLine(0, 1, 0, size_.height-1);
         g.setColor(HILITE_COLOR);
         g.drawLine(1, size_.height-1, size_.width-1, size_.height-1);
         g.drawLine(size_.width-1, 0, size_.width-1, size_.height-1);
      }

      if (drawBorderLine_)
      {
         g.setRenderingHints(noRenderingHints_);
         g.setStroke(onePixelStroke_);
         g.setColor(borderLineColor_);
         g.drawLine(0, 0, size_.width-1, 0);
         g.drawLine(0, 1, 0, size_.height-1);
         g.drawLine(1, size_.height-1, size_.width-1, size_.height-1);
         g.drawLine(size_.width-1, 0, size_.width-1, size_.height-1);
      }
      
      if (shadowThickness_ > 0)
      {
         g.setRenderingHints(noRenderingHints_);
         g.setStroke(onePixelStroke_);
         g.setColor(SHADOW_COLOR);
         for (int i = 0; i < shadowThickness_; ++i)
         {
            g.drawLine(shadowThickness_, (size_.height+i), (size_.width+i), (size_.height+i));
            g.drawLine((size_.width+i), shadowThickness_, (size_.width+i), (size_.height+i));
         }
      }

      if (drawEvalNote_)
      {
         g.setRenderingHints(antialiasRenderingHints_);
         g.setComposite(alphaComposite_);
         double fontHeight = .08*(Math.sqrt(size_.width*size_.width + size_.height*size_.height));
         Font font = ConverterWizard.createDefaultFont(Font.PLAIN, (int) fontHeight);
         g.setFont(font);
         FontMetrics fontMetrics = g.getFontMetrics();
         String evalNote = EVAL_NOTE;
         int stringWidth = fontMetrics.stringWidth(evalNote);
         g.setTransform(rotateTransform_);
         g.setColor(Color.red);
         g.drawString(evalNote, size_.width/2 - stringWidth/2, size_.height/2 + ((int) fontHeight/2));
         g.setTransform(identityTransform_);
         g.setComposite(AlphaComposite.Clear);
      }
   }

   private void fillImageBuffer(int timeStamp)
   {
      fillImageBuffer(timeStamp, false);
   }
   
   private void fillImageBuffer(int timeStamp, boolean bIncludeInteractives)
   {
      Graphics2D g = imageBuffer_.createGraphics();
      
      g.setRenderingHints(antialiasRenderingHints_);
      g.setColor(backgroundColor_);
      g.fillRect(0, 0, size_.width, size_.height);

      drawShadow(g);
      if (bIncludeInteractives)
         eventQueue_.paint(g, timeStamp, zoomFactor_, true, true);
      else
         eventQueue_.paint(g, timeStamp, zoomFactor_, true, false);
      drawBorderExtras(g);
   }

   private void fillImageBuffer(ButtonArea buttonArea)
   {
      Graphics2D g = imageBuffer_.createGraphics();

      Composite origComposite = g.getComposite();
      
      g.setComposite(AlphaComposite.getInstance(AlphaComposite.CLEAR, 1.0f));
      g.setRenderingHint(RenderingHints.KEY_ALPHA_INTERPOLATION, 
         RenderingHints.VALUE_ALPHA_INTERPOLATION_QUALITY);
      g.fillRect(0, 0, size_.width, size_.height);
      g.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC, 1.0f));
            
      //g.setRenderingHints(noRenderingHints_);
      g.setRenderingHints(antialiasRenderingHints_);
      //g.setColor(backgroundColor_);
      //g.fillRect(0, 0, size_.width, size_.height);
      
      //drawShadow(g);
      buttonArea.paint(g, size_.width, size_.height, zoomFactor_);
      //drawBorderExtras(g);

      g.setComposite(origComposite);
   }

   private void fillImageBuffer(Picture picture)
   {
      Graphics2D g = imageBuffer_.createGraphics();

      Composite origComposite = g.getComposite();
      
      g.setComposite(AlphaComposite.getInstance(AlphaComposite.CLEAR, 1.0f));
      g.setRenderingHint(RenderingHints.KEY_ALPHA_INTERPOLATION, 
         RenderingHints.VALUE_ALPHA_INTERPOLATION_QUALITY);
      g.fillRect(0, 0, size_.width, size_.height);
      g.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC, 1.0f));
      
      //g.setRenderingHints(noRenderingHints_);
       antialiasRenderingHints_.put(RenderingHints.KEY_INTERPOLATION,
                                    RenderingHints.VALUE_INTERPOLATION_BICUBIC);
      g.setRenderingHints(antialiasRenderingHints_);
      //g.setColor(backgroundColor_);
//      g.fillRect(0, 0, size_.width, size_.height);
      
      //drawShadow(g);
      picture.paint(g, size_.width, size_.height, zoomFactor_);
      //drawBorderExtras(g);

      g.setComposite(origComposite);
   }

   private void fillAviImageBuffer(String aviFileName)
      throws Exception
   {
      System.out.println("aviFileName == \"" + aviFileName + "\"");

      BufferedImage bufferedImage = null;

      if (aviFrameMap_ == null)
         aviFrameMap_ = new HashMap();

      SoftReference ref = (SoftReference) aviFrameMap_.get(aviFileName);
      if (ref != null)
      {
         System.out.println("Reference is not null");
         bufferedImage = (BufferedImage) ref.get();
      }

      if (bufferedImage == null)
      {
         System.out.println("bufferedImage == null");

         byte[] pixelData = NativeUtils.retrieveFirstVideoFrame(aviFileName);

         VideoClipInfo vci = new VideoClipInfo(aviFileName, 0);
         boolean bSuccess = NativeUtils.retrieveVideoInfo("", vci);
         
         if (bSuccess && pixelData != null)
         {
            bufferedImage = new BufferedImage(vci.videoWidth, vci.videoHeight,
                                              BufferedImage.TYPE_INT_ARGB);
            int [] intPixels = new int[vci.videoWidth*vci.videoHeight];
            int c = 0;
            int p = 0;
            int padding = 4 - (vci.videoWidth * 3) % 4;
            if (padding == 4)
               padding = 0;

            for (int y=0; y<vci.videoHeight; ++y)
            {
               p = (vci.videoHeight - y - 1) * vci.videoWidth;

               for (int x=0; x<vci.videoWidth; ++x)
               {
                  intPixels[p] = (0xff000000 |
                                  ((pixelData[c+2] & 0xff) << 16) | 
                                  ((pixelData[c+1] & 0xff) << 8) | 
                                  (pixelData[c] & 0xff));
                  
                  ++p;
                  c += 3;
               }
               
               c += padding;
            }

            bufferedImage.setRGB(0, 0, vci.videoWidth, vci.videoHeight,
                                 intPixels, 0, vci.videoWidth);

            aviFrameMap_.put(aviFileName, new SoftReference(bufferedImage));
         }
      }

      if (bufferedImage != null)
      {
         double xFactor = (1.0 * size_.width) / (1.0 * (bufferedImage.getWidth() - 2)/*vci.videoWidth*/);
         double yFactor = (1.0 * size_.height) / (1.0 * (bufferedImage.getHeight() - 2)/*vci.videoHeight*/);
         double factor  = (xFactor < yFactor) ? xFactor : yFactor;

         int imgWidth   = (int) (factor * (bufferedImage.getWidth() - 2)/*vci.videoWidth*/);
         int imgHeight  = (int) (factor * (bufferedImage.getHeight() - 2)/*vci.videoHeight*/);
         
         // security checks:
         if (imgWidth <= 0)
            imgWidth = 1;
         if (imgHeight <= 0)
            imgHeight = 1;
         if (imgWidth > bufferedImage.getWidth() - 2)
            imgWidth = bufferedImage.getWidth() - 2;
         if (imgHeight > bufferedImage.getHeight() - 2)
            imgHeight = bufferedImage.getHeight() - 2;
         
         int offX       = (size_.width - imgWidth) / 2;
         int offY       = (size_.height - imgHeight) / 2;

//          System.out.println("w: " + imgWidth + ", h: " + imgHeight + ", offX: " + offX + ", offY: " + offY +
//                             ", orig: w: " + vci.videoWidth + ", h: " + vci.videoHeight +
//                             ", sw: " + size_.width + ", sh: " + size_.height);

         Graphics2D g = imageBuffer_.createGraphics();
         g.setRenderingHints(antialiasRenderingHints_);

         g.setColor(Color.black);
         g.fillRect(0, 0, size_.width, size_.height);

         drawShadow(g);
         g.setRenderingHints(noRenderingHints_);
         g.drawImage(bufferedImage.getScaledInstance(imgWidth, imgHeight, Image.SCALE_SMOOTH), offX, offY, null);
         drawBorderExtras(g);
      }
      else
         throw new IOException("Could not extract video image data!");
   }

   /**
    * Returns the {@link ObjectQueue ObjectQueue} instance used by
    * this <tt>PictureExtracter</tt> instance.
    */
   public ObjectQueue getObjectQueue()
   {
      return objectQueue_;
   }

   /**
    * Returns the {@link EventQueue EventQueue} instance used by
    * this <tt>PictureExtracter</tt> instance.
    */
   public EventQueue getEventQueue()
   {
      return eventQueue_;
   }

   private void createBufferImage()
   {
      createBufferImage(false); // default: do not use alpha channel
   }
   
   private void createBufferImage(boolean bUseAlpha)
   {
      DocumentData dd = pwData_.GetDocumentData();

      double wishWidth = (double)size_.width;
      double wishHeight = (double)size_.height;
      double isWidth = 0.0;
      double isHeight = 0.0;
      isWidth = (double)dd.GetWhiteboardDimension().width;
      isHeight = (double)dd.GetWhiteboardDimension().height;
      // Special case: "Denver" documents have no whiteboard
      if ( (isWidth == 0.0) && (isHeight == 0.0) ) {
          isWidth = wishWidth;
          isHeight = wishHeight;
      }

      double horizZoom = wishWidth/isWidth;
      double vertZoom  = wishHeight/isHeight;

      zoomFactor_ = Math.min(horizZoom, vertZoom);

      size_ = new Dimension((int) (zoomFactor_*isWidth+0.5), (int) (zoomFactor_*isHeight+0.5));

      //imageBuffer_ = new BufferedImage(size_.width, size_.height,
      //                                 BufferedImage.TYPE_3BYTE_BGR);
      
      // Consider the place for the shadow (if any)
//      imageBuffer_ = new BufferedImage(size_.width + shadowThickness_, 
//                                       size_.height + shadowThickness_,
//                                       BufferedImage.TYPE_3BYTE_BGR);
      if (bUseAlpha)
         imageBuffer_ = new BufferedImage(size_.width + shadowThickness_, 
                                          size_.height + shadowThickness_,
                                          BufferedImage.TYPE_INT_ARGB);
      else
         imageBuffer_ = new BufferedImage(size_.width + shadowThickness_, 
                                          size_.height + shadowThickness_,
                                          BufferedImage.TYPE_3BYTE_BGR);

//      rotateTransform_ = AffineTransform.getRotateInstance(1.80*Math.PI, .5*size_.width, .5*size_.height);
      double dAngleRad = Math.atan((double)size_.height / (double)size_.width);
      //double dAngleDeg = dAngleRad * 180.0 / Math.PI;
      rotateTransform_ = AffineTransform.getRotateInstance(-dAngleRad, .5*size_.width, .5*size_.height);
   }

   private void initRenderingHints()
   {
      antialiasRenderingHints_ = new RenderingHints(null);
      antialiasRenderingHints_.put(RenderingHints.KEY_FRACTIONALMETRICS,
                                   RenderingHints.VALUE_FRACTIONALMETRICS_ON);
      antialiasRenderingHints_.put(RenderingHints.KEY_TEXT_ANTIALIASING,
                                   RenderingHints.VALUE_TEXT_ANTIALIAS_ON);
      antialiasRenderingHints_.put(RenderingHints.KEY_ANTIALIASING,
                                   RenderingHints.VALUE_ANTIALIAS_ON);
//       antialiasRenderingHints_.put(RenderingHints.KEY_INTERPOLATION,
//                                    RenderingHints.VALUE_INTERPOLATION_BICUBIC);

      noRenderingHints_ = new RenderingHints(null);

      onePixelStroke_ = new BasicStroke(1f);
      
      alphaComposite_ = AlphaComposite.getInstance(AlphaComposite.SRC_OVER, .6f);
      identityTransform_ = new AffineTransform();
   }

   private void initQueues() throws Exception
   {
      ProgressListener progressListener = new QueueProgressListener();

      DocumentData dd = pwData_.GetDocumentData();

      String strCodePage = dd.GetCodepage();
      String strObjFileName = dd.GetObjFileName();
      String strEvqFileName = dd.GetEvqFileName();
      // Special case: "Denver" documents have no OBJ/EVQ
      if ( (strObjFileName.length() == 0) && (strEvqFileName.length() == 0) )
          return;

      //
      // CREATE OBJECT QUEUE
      //
      long streamLength = resources_.getFileSize(strObjFileName);
      if (streamLength == -1)
         throw new ResourceNotFoundException("Resource " + strObjFileName + 
                                             " not found!");
      InputStream in = new BufferedInputStream(resources_.createInputStream(strObjFileName));

      display_.displayCurrentFile(strObjFileName, (int) streamLength, PARSING);
      display_.displayCurrentProgressStep("prepareObjForThumbs", PREP_THUMBS);
      display_.displayCurrentProgress(0.0f);

      objectQueue_ = new ObjectQueue(in, strCodePage, resources_);
      objectQueue_.startParsing(progressListener);

      //
      // CREATE EVENT QUEUE
      //
      streamLength = resources_.getFileSize(strEvqFileName);
      if (streamLength == -1)
         throw new ResourceNotFoundException("Resource " + strEvqFileName + 
                                             " not found!");
      in = new BufferedInputStream(resources_.createInputStream(strEvqFileName));

      display_.displayCurrentFile(strEvqFileName, (int) streamLength, PARSING);
      display_.displayCurrentProgressStep("prepareEvqForThumbs", PREP_THUMBS);
      display_.displayCurrentProgress(0.0f);

      eventQueue_ = new EventQueue(in, objectQueue_);
      eventQueue_.startParsing(progressListener, true);

      // FIND BACKGROUND COLOR
      backgroundColor_ = eventQueue_.getFirstBackgroundColor();
   }

   private class QueueProgressListener implements ProgressListener
   {
      public void progressAchieved(long t)
      {
         display_.displayCurrentFileProgress((int) t);
      }
   }

   /**
    * Produces 8bit RGBA PNG palette image from the source 32bit RGBA image.
    * Note that the transparency is only preserved for the fully transparent
    * source pixels.
    * (adapted from this example: http://archives.java.sun.com/cgi-bin/wa?A2=ind0504&L=jai-interest&D=0&P=620)
    */
   private TiledImage imageReduceColorsToPalette(BufferedImage image)
   {
      // the expected image should have 4 bands
      SampleModel sampleModel = image.getSampleModel();
      int numBands = sampleModel.getNumBands();
      if (numBands != 4)
      {
         throw new RuntimeException("Error in color reduction: source image is not of type ARGB!");
      }

      // ColorQuantizer only accepts 3-band RGB images
      int[] band = null;
      band = new int[] {0, 1, 2}; //dropping alpha
      ParameterBlock pb = new ParameterBlock();
      pb.addSource(image);
      pb.add(band);
      RenderedImage rgb = (RenderedImage) JAI.create("bandSelect", pb);

      // extract the alpha channel for use below
      band = new int[] {3};
      pb = new ParameterBlock();
      pb.addSource(image);
      pb.add(band);
      RenderedImage alpha = (RenderedImage) JAI.create("bandSelect", pb);

      // transform semi-transparent pixels to opaque pixels using alpha value
      RandomIter iterA = RandomIterFactory.create(alpha, null);
      for (int y = 0; y < image.getHeight(); y++)
      {
         for (int x = 0; x < image.getWidth(); x++)
         {
            int origRgb = image.getRGB(x, y);//rgb value
            int transparency = iterA.getSample(x, y, 0);//alpha channel
            if (transparency < 255)
            {
               Color clrOrigRgb = new Color(origRgb);
               int r = clrOrigRgb.getRed() + (255-transparency);
               int g = clrOrigRgb.getGreen() + (255-transparency);
               int b = clrOrigRgb.getBlue() + (255-transparency);
               if (r > 255)
                  r = 255;
               if (g > 255)
                  g = 255;
               if (b > 255)
                  b = 255;
               Color clrNewRgb = new Color(r, g, b);
               // set the new color value to the image
               image.setRGB(x, y, clrNewRgb.getRGB());
            }
         }
      }

      // format - to avoid the possible problem of compatibility between
      // the destination image's ColorModel and SampleModel
      pb = new ParameterBlock();
      pb.addSource(rgb);
      pb.add(DataBuffer.TYPE_BYTE);
      rgb = (RenderedImage) JAI.create("Format",pb);

      // get optimized color map
      pb = new ParameterBlock();
      pb.addSource(rgb);
      pb.add(ColorQuantizerDescriptor.MEDIANCUT); // possible: MEDIANCUT, NEUQUANT, OCTTREE
      pb.add(255); //reserving the 255 index for the fully transparent pixels
      RenderedOp paletteIm = JAI.create("ColorQuantizer", pb, null);
      LookupTableJAI lut = (LookupTableJAI) paletteIm.getProperty("LUT");

      // convert 3-band RGB image to color-indexed image using dithering
      pb = new ParameterBlock();
      pb.addSource(rgb);
      pb.add(lut);
      RenderedImage im = (RenderedImage) JAI.create("ErrorDiffusion", pb);

      // create the final converted image
      TiledImage convertedImage = new TiledImage(im.getMinX(),
                                                 im.getMinY(),
                                                 im.getWidth(),
                                                 im.getHeight(),
                                                 im.getMinX(),
                                                 im.getMinY(),
                                                 im.getSampleModel(),
                                                 im.getColorModel());

      int height = im.getHeight();
      int width = im.getWidth();

      // use the source alpha channel (prepared above) to replace the indexes in our
      // converted image to be set to 255
      RandomIter iter = RandomIterFactory.create(im, null);
///      RandomIter iterA = RandomIterFactory.create(alpha, null); // (already defined above)

      for (int y = 0; y < height; y++)
      {
         for (int x = 0; x < width; x++)
         {
            int index = iter.getSample(x, y, 0);//indexed image
            int transparency = iterA.getSample(x, y, 0);//alpha channel
            int origRgb = image.getRGB(x, y);//rgb value
            Color clrRgb = new Color(origRgb);

            //if (transparency == 0)
            // semi transparent pixels:
            // use a treshold: "light" semi transparent colors are set to full transparency 
            // ("dark" semi transparent colors are transformed in the step above
            if ((transparency < 255) && !((clrRgb.getRed() < 128) || (clrRgb.getGreen() < 128) || (clrRgb.getBlue() < 128)))
            {
               index = 255;
            }

            convertedImage.setSample(x, y, 0, index);
         }
      }
      
      return convertedImage;
   }
}
