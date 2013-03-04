package imc.epresenter.player.master;


import java.awt.*;
import java.awt.event.*;
import java.awt.font.GlyphVector;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;
import javax.swing.*;
import javax.swing.border.Border;
import javax.swing.border.BevelBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

public class TimeSlider extends JPanel implements MouseListener, MouseMotionListener
{
	public static void main(String[] args)
	{
		JFrame f = new JFrame("Heldenplatz");
		
		JPanel p = new JPanel();
		//p.setBorder(BorderFactory.createEmptyBorder(3,3,3,3));
      TimeSlider t = new TimeSlider(20*60*1000);
		p.add("Center", t);
		p.setBackground(Color.red.darker());
		//f.getContentPane().setLayout(new GridLayout(1,1));
		f.getContentPane().add("Center", p);
		
		JPanel buttons = new JPanel(new FlowLayout(FlowLayout.CENTER, 3, 0));
		buttons.add(createButton("../icons/go_to_beginning.gif"));
		buttons.add(createButton("../icons/previous_slide.gif"));
		buttons.add(createButton("../icons/play.gif"));
		buttons.add(createButton("../icons/next_slide.gif"));
		buttons.add(createButton("../icons/go_to_end.gif"));
		buttons.setBorder(BorderFactory.createLineBorder(Color.black, 1));
		buttons.setBackground(Color.blue);

		JPanel label = new JPanel(new GridLayout(1,1));
		JLabel l = new JLabel("00:00:00");
		label.add(l);
		label.setBorder(BorderFactory.createLineBorder(Color.black, 1));
		
		JPanel p2 = new JPanel();
		p2.add("West", buttons);
		p2.add("East", label);
		
		
		f.getContentPane().add("West", p2);
		
		
		
		f.pack();

		f.show();



      try { Thread.sleep(4000); } catch (Exception e) {}
      t.setValue(10*60*1000);
      
      try { Thread.sleep(2000); } catch (Exception e) {}
      t.setValue(15*60*1000);
      
      try { Thread.sleep(2000); } catch (Exception e) {}
      t.setValue(20*60*1000);

	}

   private static JButton createButton(String fileName)
	{
		JButton button = new JButton(new ImageIcon(fileName));
		button.setBorder(BorderFactory.createEmptyBorder());
		return button;
	}



	private int _millis;
   private int _value;
   private boolean _valueAdjusting;
	private BufferedImage _knobImage;
   private Rectangle _knobPosition;
   private Rectangle2D[] _labelBounds;
   private ChangeListener _listener;
   private int _innerWidth;


	public TimeSlider(int millis)
	{
      _millis = millis;

      setForeground(Color.black);
		setBackground(new Color(0xfbdfb2));
		setOpaque(true);
		
		Border bevelBorder = BorderFactory.createBevelBorder(
			BevelBorder.LOWERED);//, Color.white, Color.black);
		setBorder(bevelBorder);

      setFont(new JLabel().getFont().deriveFont(Font.PLAIN));
		// setFont(new Font("SansSerif", Font.PLAIN, 14));
		// actual font size (height) for numbers is less because
		// they have no part below the base line
	   
		Insets insets = getBorder().getBorderInsets(this);
		
		setMinimumSize(new Dimension(120, 24+insets.top+insets.bottom));
		setPreferredSize(new Dimension(400, 26+insets.top+insets.bottom));
		setMaximumSize(new Dimension(1600, 30+insets.top+insets.bottom));

      addMouseListener(this);
      addMouseMotionListener(this);
	}

   public void setChangeListener(ChangeListener l)
   {
      _listener = l;
   }

   public void setExternalValue(int v)
   {
      setValue(v);
   }

   public void setValue(int v)
   {
      if (v < 0 || v > _millis)
         throw new IllegalArgumentException(
            "Value ("+v+") out of range (0-"+_millis+").");

      int oldValue = _value;
      _value = v;

      if (_knobImage != null)
      {
         int x = (int)((Math.min(_value, oldValue)/(float)_millis)*_innerWidth);
         int y = (int)((Math.max(_value, oldValue)/(float)_millis)*_innerWidth);

         int offset = (getWidth()-_innerWidth)/2;

         repaint(offset+x-_knobImage.getWidth(),0,
                 offset+y-x+_knobImage.getWidth()*2,getHeight());
      }
      else
         repaint();
   }

   public int getValue()
   {
      return _value;
   }

   public boolean isValueAdjusting()
   {
      return _valueAdjusting;
   }


	protected void paintComponent(Graphics g)
	{
      super.paintComponent(g); // background color
		Graphics2D g2D = (Graphics2D)g;


		Insets insets = getBorder().getBorderInsets(this);
		g.translate(insets.left, insets.top);

		int height = getHeight()-insets.top-insets.bottom;
		int width = getWidth()-insets.left-insets.right;
      
		if (_knobImage == null || _knobImage.getHeight() != height)
			_knobImage = createKnobImage(height);
		
		int emptyBorderHeight = 3;

		g.translate(_knobImage.getWidth()/2, emptyBorderHeight);
		height -= 2*emptyBorderHeight;
		width -= _knobImage.getWidth();

		g.setColor(getForeground());
		g.drawRect(0,0,width-1,height-1);

		g.translate(1,1);  
		height -= 2;
		width -= 2;
		
		_innerWidth = width;
		
      /*
		int fontHeight = Math.min(height-4, 18);
		if (getFont().getSize() != fontHeight)
			setFont(getFont().deriveFont((float)fontHeight));
         */
		g.setFont(getFont());
		
      /*
		g2D.setRenderingHints(new RenderingHints(
			RenderingHints.KEY_TEXT_ANTIALIASING,
			RenderingHints.VALUE_TEXT_ANTIALIAS_ON));
		   */

		Rectangle2D maxLabelSize = getFont().createGlyphVector(
			g2D.getFontRenderContext(), "888").getVisualBounds();

		int numLabels = (int)(width / (maxLabelSize.getWidth()*2.2));
		// 3 = at least 2 label-widths spacing between 2 labels
		int millisPerLabel = 0; // dummy for later code

		if (numLabels > 0)
		{
			millisPerLabel = _millis / numLabels;
			if (millisPerLabel > 60*60*1000) millisPerLabel = 2*60*60*1000;
			else if (millisPerLabel > 30*60*1000) millisPerLabel = 60*60*1000;
			else if (millisPerLabel > 20*60*1000) millisPerLabel = 30*60*1000;
			else if (millisPerLabel > 15*60*1000) millisPerLabel = 20*60*1000;
			else if (millisPerLabel > 10*60*1000) millisPerLabel = 15*60*1000;
			else if (millisPerLabel > 5*60*1000) millisPerLabel = 10*60*1000;
			else if (millisPerLabel > 2*60*1000) millisPerLabel = 5*60*1000;
			else if (millisPerLabel > 60*1000) millisPerLabel = 2*60*1000;
			else if (millisPerLabel > 30*1000) millisPerLabel = 60*1000;
         else if (millisPerLabel > 20*1000) millisPerLabel = 30*1000;
         else if (millisPerLabel > 10*1000) millisPerLabel = 20*1000;
         else if (millisPerLabel > 5*1000) millisPerLabel = 10*1000;
         else millisPerLabel = 5*1000;

			numLabels = _millis / millisPerLabel;
		}

		if (_labelBounds == null || numLabels != _labelBounds.length)
		{
			_labelBounds = new Rectangle2D[numLabels];
		
         // double code (below)
			for (int i=0; i<numLabels; i++)
			{
				int labelMillis = i*millisPerLabel;
				String labelText = null;
            if (millisPerLabel >= 60*1000)
               labelText = ((labelMillis)/(60*1000))+"";
            else
               labelText = (labelMillis/1000)+"s";

				_labelBounds[i] = getFont().createGlyphVector(
					g2D.getFontRenderContext(), labelText).getVisualBounds();
			}
		}

      // double code (above)
		for (int i=0; i<numLabels; i++)
		{
			Rectangle2D labelSize = _labelBounds[i];
			int labelMillis = (i+1)*millisPerLabel;
			String labelText = null;
         if (millisPerLabel >= 60*1000)
            labelText = ((labelMillis)/(60*1000))+"";
         else
            labelText = (labelMillis/1000)+"s";
         int position = (int)((labelMillis/(float)_millis)*width);
			float x = position-(float)labelSize.getWidth()/2;
			float y = (height-(float)labelSize.getHeight())/2+(float)labelSize.getHeight()-1;
         if (x+labelSize.getWidth() < width)
            g2D.drawString(labelText, x, y);
      }

      if (millisPerLabel > 0)
      {
         int strokeMillis = (int)(0.5*millisPerLabel);
         while(strokeMillis < _millis)
         {
            int x = (int)((strokeMillis/(float)_millis)*width)-2;
            if (x+4 < width)
               g.drawLine(x,height/2,x+4,height/2);
            strokeMillis += millisPerLabel;
         }
      }
		

		g.translate(-1,-1);
		
		g.translate(-_knobImage.getWidth()/2, -emptyBorderHeight);  
		

		
      int x = (int)((_value/(float)_millis)*(width+1));
      g.drawImage(_knobImage,x,0,this);
      _knobPosition = new Rectangle(x+insets.left,insets.top,
                                    _knobImage.getWidth(),_knobImage.getHeight());


		g.translate(-insets.left, -insets.top);
		// now the borders are painted

	}


   public void mousePressed(MouseEvent e)
   {
      if (_knobPosition.contains(new Point(e.getX(), e.getY())))
      {
         _valueAdjusting = true;
      }
   }

   public void mouseReleased(MouseEvent e)
   {
      int newValue = calculateNewValue(e.getX());
      setValue(newValue);

      _valueAdjusting = false;
      
      if (_listener != null) _listener.stateChanged(new ChangeEvent(this));
   }

   public void mouseDragged(MouseEvent e)
   {
      if (_valueAdjusting)
      {
         int newValue = calculateNewValue(e.getX());
         setValue(newValue);

         if (_listener != null) _listener.stateChanged(new ChangeEvent(this));
      }
   }

   public void mouseClicked(MouseEvent e) {}
   public void mouseEntered(MouseEvent e) {}
   public void mouseExited(MouseEvent e) {}
   public void mouseMoved(MouseEvent e) {}
   

   private int calculateNewValue(int x)
   {
      int innerOffset = (getWidth()-_innerWidth)/2;
      if (x < innerOffset) return 0;
      if (x > innerOffset+_innerWidth) return _millis;

      x -= innerOffset;
      if (x > _innerWidth) x = _innerWidth;
      return (int)((x/(float)_innerWidth)*_millis);
   }


	private BufferedImage createKnobImage(int height)
	{
		
		BufferedImage knob = new BufferedImage(
			(int)(1.7*height), height, BufferedImage.TYPE_INT_ARGB);
		Graphics2D knobGraphics = knob.createGraphics();
		
		knobGraphics.setRenderingHints(new RenderingHints(
			RenderingHints.KEY_ANTIALIASING,
			RenderingHints.VALUE_ANTIALIAS_ON));


		int arcSize = 6; 
		knobGraphics.setColor(new Color(255, 255, 255, 128));
		knobGraphics.fillRoundRect(
			1,1, knob.getWidth()-3, knob.getHeight()-3, arcSize,arcSize);
		knobGraphics.setColor(Color.black);
		knobGraphics.drawRoundRect(
			1,1,knob.getWidth()-3, knob.getHeight()-3, arcSize,arcSize);
		knobGraphics.setColor(Color.white);
		knobGraphics.drawLine(arcSize/2+2,2,knob.getWidth()-4-arcSize/2,2);
		knobGraphics.drawLine(2,arcSize/2+2,2,knob.getHeight()-4-arcSize/2);
		knobGraphics.setColor(Color.red);
		knobGraphics.drawLine(knob.getWidth()/2, 3, knob.getWidth()/2, knob.getHeight()-4);
		
		return knob;
	}




}