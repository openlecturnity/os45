package  imc.epresenter.media.codec.video.vcm;

import java.awt.Dimension;
import java.util.Vector;
import javax.media.Format;
import javax.media.format.RGBFormat;
import javax.media.format.VideoFormat;

import com.sun.media.vfw.BitMapInfo;
import com.sun.media.vfw.VCM;
import com.sun.media.codec.video.vcm.NativeDecoder;


/*
 * This is nearly the original NativeDecoder. The difference is that
 * padding is taken into account if the input-encoding is "twlc".
 *
 * Reason:
 * If videos have uneven line widths Windows (vfw) pads these lines, meaning
 * some bytes are added in order to have a line size that divides evenly with 4.
 * If now a codec (twlc) provides correctly padded output Java will crash
 * (natively) upon freeing the data buffers.
 */


public class NativeDecoderPadded extends NativeDecoder
{
   public Format [] getSupportedOutputFormats(Format in) 
   {
      if (in == null)
         return outputFormats;

      if (matches(in, inputFormats) == null)
         return new Format[0];

      VideoFormat video = (VideoFormat) in;
      Dimension size = video.getSize();
      if (size == null)
         size = new Dimension(320, 240);

      int padding = 0;
      if (in.getEncoding().equalsIgnoreCase("twlc"))
      {
         // this is a rather dodgy workaround so that 
         // twlc-videos can have uneven widths
         if (size.width*3 % 4 != 0)
            padding = 4 - size.width*3 % 4;
      }

      RGBFormat [] tryFormats = new RGBFormat[] {
         new RGBFormat(size, size.width * size.height * 3 + size.height * padding,
                       Format.byteArray,
                       video.getFrameRate(), // frame rate
                       24,
                       3, 2, 1,
                       3,
                       size.width * 3 + padding,
                       Format.TRUE,
                       Format.NOT_SPECIFIED), // endian

            new RGBFormat(size, size.width * size.height * 3 + size.height * padding,
                          Format.byteArray,
                          video.getFrameRate(), // frame rate
                          24,
                          3, 2, 1,
                          3,
                          size.width * 3 + padding,
                          Format.FALSE, // flipped
                          Format.NOT_SPECIFIED), // endian

            new RGBFormat(size, size.width * size.height,
                          Format.shortArray,
                          video.getFrameRate(), // frame rate
                          16,
                          0x07C00, 0x003e0, 0x0001F,
                          1, size.width,
                          Format.FALSE, // flipped
                          Format.NOT_SPECIFIED), // endian

            new RGBFormat(size, size.width * size.height,
                          Format.shortArray,
                          video.getFrameRate(), // frame rate
                          16,
                          0x07C00, 0x003e0, 0x0001F,
                          1, size.width,
                          Format.TRUE,
                          Format.NOT_SPECIFIED) // endian


      };
      Vector supportedOuts = new Vector();
      int handle = VCM.icLocate("vidc", "RGB",
                                new BitMapInfo((VideoFormat) video),
                                null,
                                VCM.ICMODE_DECOMPRESS);
      if (handle != 0) {
         for (int i=0; i<tryFormats.length; i++) {
            if ( VCM.icDecompressBegin(handle,
                                       new BitMapInfo((VideoFormat) video),
                                       new BitMapInfo(tryFormats[i])) ) {
               supportedOuts.addElement(tryFormats[i]);
               VCM.icDecompressEnd(handle);
            } else {
            }
         }
         VCM.icClose(handle);
      }

      Format [] outs = new Format[supportedOuts.size()];
      for (int i = 0; i < outs.length; i++) {
         outs[i] = (Format) supportedOuts.elementAt(i);
      }

      return outs;
   }
}