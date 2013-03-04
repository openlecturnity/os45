package imc.epresenter.player.util;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

public class LED extends JPanel implements ActionListener {
	private boolean active;

	// flashing things
	private boolean wasActive;
	private int flashTimeout;
	private long lastActionTime;
	private Timer flashTimer;


	public LED() {
		this(Color.green, Color.green.darker().darker());
	}
	public LED(Color c) {
		this(c, c.darker().darker());
	}
	public LED(Color fore, Color back) {
		setBorder(BorderFactory.createLineBorder(Color.black));
		setForeground(fore);
		setBackground(back);
		setOpaque(true);
		setMinimumSize(new Dimension(8,8));
		setPreferredSize(new Dimension(16, 16));
	}

	protected void paintComponent(Graphics g) {
		if (active) g.setColor(getForeground());
		else g.setColor(getBackground());
		g.fillRect(0,0,getWidth(),getHeight());
	}

	public void actionPerformed(ActionEvent e) {
		active = !active;
		repaint();
		if (flashTimeout > 0) {
			long currentTime = System.currentTimeMillis();
			flashTimeout -= (int)(currentTime-lastActionTime);
			lastActionTime = currentTime;
			if (flashTimeout <= 0) stopFlashing();
		}
	}

	public void setActive(boolean b) {
		active = b;
		repaint();
	}

	public void setColor(Color c) {
		setForeground(c);
		setBackground(c.darker().darker());
		repaint();
	}

	public void startFlashing() {
		startFlashing(500, 0);
	}

	public void startFlashing(int millis, int timeout) {
		flashTimeout = timeout;
		wasActive = active;
		active = !active;
		repaint();
		flashTimer = new Timer(millis, this);
		if (flashTimeout > 0) lastActionTime = System.currentTimeMillis();
		flashTimer.start();
	}

	public void stopFlashing() {
		flashTimer.stop();
		active = wasActive;
		repaint();
	}

	public boolean isFlashing() {
		return flashTimer != null && flashTimer.isRunning();
	}
}
		


