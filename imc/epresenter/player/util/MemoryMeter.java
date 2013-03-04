package imc.epresenter.player.util;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

public class MemoryMeter extends JPanel implements ActionListener, Timeable {
   private TimeSource m_time;
   private Runtime m_runtime;
   private boolean m_bDoGc = false;

	public MemoryMeter()
   {
      this(false);
	}
   
	public MemoryMeter(boolean bUseFrame)
   {
      JFrame frame = null;
      m_runtime = Runtime.getRuntime();
      
		if (bUseFrame)
      {
         frame = new JFrame("Memory");
         frame.setContentPane(this);
         frame.pack();
      }
      
      setForeground(Color.red);
      setBackground(Color.gray);
      setOpaque(true);
      
      m_time = new TimeSource(this, 1000);
      m_time.start();
      
      if (frame != null)
         frame.show();
	}

	protected void paintComponent(Graphics g)
   {
      Dimension dimSize = getSize();
      
      if (isOpaque())
         g.clearRect(0,0,dimSize.width,dimSize.height);
      
      long lTotal = m_runtime.totalMemory();
      long lUsed = lTotal - m_runtime.freeMemory();
      double dPercentage = lUsed/(double)lTotal;
      
      g.setColor(getForeground());
      g.fillRect(0,0,(int)(dPercentage*dimSize.width),dimSize.height);
      
      g.setColor(Color.black);
      g.drawString((lUsed/(1024*1024))+"M", 5, dimSize.height-5);
      
      String strTotal = (lTotal/(1024*1024))+"M";
      g.drawString(strTotal, dimSize.width-(strTotal.length() > 3 ? 35 : 25), dimSize.height-5);
	}

   public Dimension getPreferredSize()
   {
      return new Dimension(120, 25);
   }
   
	public void actionPerformed(ActionEvent e)
   {
	}
   
   public void setDoingGc(boolean bDoIt)
   {
      m_bDoGc = bDoIt;
      
      m_time.stop();
      
      m_time = new TimeSource(this, m_bDoGc ? 5000 : 1000);
      m_time.start();
   }

   public void tickTime()
   {
      if (m_bDoGc)
         System.gc();
      repaint();
   }
}
		


