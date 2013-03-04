package imc.epresenter.player.whiteboard;

import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.*;

import imc.epresenter.player.util.SGMLParser;

public class Pointer extends VisualComponent {
    private static final int IDX_ONLINE_POINTER = 0;
    private static final int IDX_MOUSE_POINTER = 1;
    
    // These variables are static as every pointer (object) looks the same
    // and has the same size. (Purpose: save heap space)
	private static Image[] s_aImgPointer;
    private static double s_dLastScale = 1.0;
	private static final int s_iInitialSize = 30;
	
    private short m_x, m_y, m_type;
	
	Pointer(String line, BufferedReader in) {
        boolean bConsoleOperation = System.getProperty("mode.consoleoperation", "false").equals("true");
      
		SGMLParser parser = new SGMLParser(line);
        m_x = extractShort(parser, "x");
        m_y = extractShort(parser, "y");
		clip.x = (short)(m_x);
		clip.y = (short)(m_y);
		clip.width = s_iInitialSize;
		clip.height = s_iInitialSize;
        m_type = IDX_ONLINE_POINTER;
        if (extractString(parser, "type").equals("mouse"))
            m_type = IDX_MOUSE_POINTER;
        else
            clip.x -= s_iInitialSize;
		
		if (s_aImgPointer == null && !bConsoleOperation)
            s_aImgPointer = createPointerImage(1.0);
	}
	
	void paint(Graphics g, int width, int height, double dScale) {
		if (dScale != s_dLastScale) {
			s_aImgPointer = createPointerImage(dScale);
			s_dLastScale = dScale;
		}
		
        int iX = m_x;
        if (m_type == IDX_ONLINE_POINTER)
            iX -= s_iInitialSize;
        // This is more or less a "pointing direction"
        
		g.drawImage(s_aImgPointer[m_type], (int)(dScale*(iX)), (int)(dScale*m_y), null);

		//g.setColor(Color.blue);
		//g.drawRect((int)(dScale*clip.x), (int)(dScale*clip.y),
		//		   (int)(dScale*clip.width)-1,(int)(dScale*clip.height)-1);
		
	}
	
	public boolean IsMouseCursor() {
	    return (m_type == IDX_MOUSE_POINTER);
	}

	private Color fadeColors(Color c1, Color c2, double fade) {
		if (fade > 1) fade = 1;
		if (fade < 0) fade = 0;
		double d1 = 1-fade;
		double d2 = fade;
		int red = (int)(c1.getRed()*d1+c2.getRed()*d2);
		int green = (int)(c1.getGreen()*d1+c2.getGreen()*d2);
		int blue = (int)(c1.getBlue()*d1+c2.getBlue()*d2);
		return new Color(red, green, blue);
	}

	private Image[] createPointerImage(double dScale) {
        int isize = (int)(dScale*s_iInitialSize);
        
        // The (old) online pointer variant
        //
		Image pImageOnline = new BufferedImage(isize, isize, BufferedImage.TYPE_INT_ARGB);

		Point[] ps = new Point[4];
		ps[0] = new Point(isize-1,0);
		ps[1] = new Point(0,(int)(0.5*isize));
		ps[2] = new Point((int)(0.35*isize),(int)(0.65*isize));
		ps[3] = new Point((int)(0.5*isize),isize-1);
        
        // Two polygons (triangles) with different colors
		Polygon pointer1 = new Polygon();
		Polygon pointer2 = new Polygon();

		pointer1.addPoint(ps[0].x, ps[0].y);
		pointer2.addPoint(ps[0].x, ps[0].y);
		pointer1.addPoint(ps[1].x, ps[1].y);
		pointer1.addPoint(ps[2].x, ps[2].y);
		pointer2.addPoint(ps[2].x, ps[2].y);
		pointer2.addPoint(ps[3].x, ps[3].y);

		Graphics g = pImageOnline.getGraphics();
		g.setColor(fadeColors(Color.red, Color.black, 0.1));
		g.fillPolygon(pointer2);
		g.setColor(fadeColors(Color.red, Color.white, 0.3));
		g.fillPolygon(pointer1);
        
        // Draw black border
		g.setColor(Color.black);
		g.drawLine(ps[0].x, ps[0].y, ps[1].x, ps[1].y);
		g.drawLine(ps[1].x, ps[1].y, ps[2].x, ps[2].y);
		g.drawLine(ps[2].x, ps[2].y, ps[3].x, ps[3].y);
		g.drawLine(ps[3].x, ps[3].y, ps[0].x, ps[0].y);
        
      
        // Now the newer (fixed appearance) mouse pointer variant;
        // it gets only 12x21 in size (out of the possible 30 on scale 1.0)
        //
        Image pImageMouse = new BufferedImage(isize, isize, BufferedImage.TYPE_INT_ARGB);
        Point[] ps2 = new Point[10];
        ps2[0] = new Point(0, 0);
        ps2[1] = new Point(0, (int)(16 * dScale));
        ps2[2] = new Point((int)(4 * dScale), (int)(12 * dScale));
        ps2[3] = new Point((int)(4 * dScale), (int)(13 * dScale));
        ps2[4] = new Point((int)(8 * dScale), (int)(20 * dScale));
        ps2[5] = new Point((int)(9 * dScale), (int)(20 * dScale));
        ps2[6] = new Point((int)(10 * dScale), (int)(19 * dScale));
        ps2[7] = new Point((int)(7 * dScale), (int)(12 * dScale));
        ps2[8] = new Point((int)(7 * dScale), (int)(11 * dScale));
        ps2[9] = new Point((int)(11 * dScale), (int)(11 * dScale));
        
        Graphics gMouse = pImageMouse.getGraphics();
        Polygon mouse = new Polygon();
        for (int i=0; i<ps2.length; ++i)
            mouse.addPoint(ps2[i].x, ps2[i].y);
        
        gMouse.setColor(Color.white);
        gMouse.fillPolygon(mouse);
        gMouse.setColor(Color.black);
        gMouse.drawPolygon(mouse);

		return new Image[] { pImageOnline, pImageMouse };
	}
}