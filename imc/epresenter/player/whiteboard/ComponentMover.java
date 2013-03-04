package imc.epresenter.player.whiteboard;

import java.awt.*;
import java.awt.event.*;

public class ComponentMover implements MouseListener, MouseMotionListener {
	private Component moveComp;
	private Component listenComp;
	private Component rangeComp;
	private Point startPoint;
	
	public ComponentMover(Component move, Component range) {
		this(move,move,range);
	}
	
	public ComponentMover(Component move, Component listen, Component range) {
		moveComp = move;
		listenComp = listen;
		rangeComp = range;
		listenComp.addMouseListener(this);
		listenComp.addMouseMotionListener(this);
	}
	
	public void mouseClicked(MouseEvent e) {}
	public void mouseReleased(MouseEvent e) {}
	public void mouseEntered(MouseEvent e) {}
	public void mouseExited(MouseEvent e) {}
	public void mouseMoved(MouseEvent e) {}
	
	public void mousePressed(MouseEvent e) {
		startPoint = e.getPoint();
	}
	
	public void mouseDragged(MouseEvent e) {
		Point actionPoint = e.getPoint();
		int diffX = actionPoint.x - startPoint.x;
		int diffY = actionPoint.y - startPoint.y;
		Point locationPoint = moveComp.getLocation();
		locationPoint = new Point(locationPoint.x+diffX, locationPoint.y+diffY);
		correctRangeViolation(locationPoint);
		moveComp.setLocation(locationPoint);
	}
	
	private void correctRangeViolation(Point location) {
		int toomuchX = location.x+moveComp.getSize().width-rangeComp.getSize().width;
		int toomuchY = location.y+moveComp.getSize().height-rangeComp.getSize().height;
		if (toomuchX > 0) location.x -= toomuchX;
		if (toomuchY > 0) location.y -= toomuchY;
		if (location.x < 0) location.x = 0;
		if (location.y < 0) location.y = 0;
	}
}
