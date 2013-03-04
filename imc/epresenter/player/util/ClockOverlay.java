package imc.epresenter.player.util;

import java.awt.*;
import java.awt.font.FontRenderContext;
import java.awt.font.GlyphVector;
import javax.swing.JLayeredPane;
import javax.swing.JPanel;

import imc.lecturnity.util.ui.LayoutUtils;

public class ClockOverlay extends JPanel
{
   private TimeFormat          formatter_ = null;
   private int                 showMillis_ = 0;
   private boolean             running_ = false;
   private Polygon             playSymbol_ = null;
   private Rectangle           stopSymbol_ = null;
   private Rectangle           stringRect_ = null;
   private int                 fontHeight_ = -1;
   private boolean             isJava3_ = false;
   
   public ClockOverlay()
   {
      formatter_ = new TimeFormat();
      setOpaque(false);
      
      String sJavaVersion = System.getProperty("java.vm.version");
      if (sJavaVersion != null && sJavaVersion.length() >= 3 && sJavaVersion.charAt(2) < '5')
         isJava3_ = true;
   }

   public void show(int millis, boolean started)
   {
      if (millis/1000 != showMillis_/1000 || started != running_)
      {
         running_ = started;
         showMillis_ = millis;
         
         if (isVisible())
         {
            Rectangle restriction = new Rectangle(0, 0, getWidth(), getHeight());
            if (playSymbol_ != null && stringRect_ != null)
            {
               Rectangle smallerRestriction = new Rectangle(stringRect_);
               if (running_)
                  smallerRestriction.add(playSymbol_.getBounds());
               else
                  smallerRestriction.add(stopSymbol_);
               
               smallerRestriction.x -= 2;
               smallerRestriction.y -= 2;
               smallerRestriction.width += 5;
               smallerRestriction.height += 5;

               restriction = smallerRestriction;
            }
            
            repaint(restriction);
            
            if (isJava3_) // the above does not work for Java 1.3
               LayoutUtils.invokeRepaintOnContentPane(this, restriction);
         }

      }
   }

   public void paintComponent(Graphics g)
   {
     
      Graphics2D g2d = (Graphics2D)g;
     
      Dimension d = getSize();

      g.setFont(g.getFont().deriveFont(Font.BOLD));
      FontMetrics fm = g.getFontMetrics();

      String showString = formatter_.shortFormat(showMillis_);
      
      int stringWidth = fm.stringWidth(showString);
      int border = 7;
         
      if (playSymbol_ == null || stopSymbol_ == null)
      {
         FontRenderContext frc = g2d.getFontRenderContext();
         GlyphVector gv = g.getFont().createGlyphVector(frc, showString);

         fontHeight_ = gv.getVisualBounds().getBounds().height;
         int add = fontHeight_%2 == 0 ? 1 : 0;
         //System.out.println("font height="+fontHeight_);

         Point upperLeft = new Point(d.width-border-fontHeight_,
                                     d.height-border-fontHeight_);

         stopSymbol_ = new Rectangle(upperLeft.x+1, upperLeft.y+1, 
                                     fontHeight_-2+add, fontHeight_-2+add);
         
         playSymbol_ = new Polygon();
         playSymbol_.addPoint(stopSymbol_.x, stopSymbol_.y);
         playSymbol_.addPoint(stopSymbol_.x, stopSymbol_.y+stopSymbol_.height-1);
         playSymbol_.addPoint(stopSymbol_.x+stopSymbol_.width-1, 
                              stopSymbol_.y+stopSymbol_.height/2);
         // stopSymbol_.height is uneven (due to add) and so a "-1" is
         // not necessary after "/2"
      }
        
          
      if (running_)
      {
         g.setColor(Color.green);

         g.fillPolygon(playSymbol_);
      }
      else
      {
         g.setColor(Color.red);
         g2d.fill(stopSymbol_);
      }


      if (stringRect_ == null)
         stringRect_ = new Rectangle();
      stringRect_.x = d.width-stringWidth-border-fontHeight_-2;
      stringRect_.y = d.height-border-fontHeight_;
      stringRect_.width = stringWidth;
      stringRect_.height = fontHeight_;
      
      g.setColor(Color.gray);
      //g.setXORMode(Color.black);
      g.drawString(showString, stringRect_.x, d.height-border);

       
      //System.out.println("drawString() invoked for "+showString_
      //                   +" on "+(d.width-fontWidth-border-fontHeight_-2)
      //                   +","+(d.height-border));
      //System.out.println("clip="+g.getClip().getBounds());
   }


}