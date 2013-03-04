package imc.epresenter.player.util;


import java.awt.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.*; // for test in main

public class ComponentPlacer {
	private Component[] _components;
	//private Point _upperLeft;
	
	// components must already be visible (sizes are right, upper left known)
	// there only needs something to be done if they have all the same location
	// otherwise it is likely that they are already placed by a window manager
	public ComponentPlacer(Component[] toDo) {
		_components = toDo;
		//for (int i=0; i<_components.length; i++) System.out.println(_components[i].getSize());
	}

	public Dimension doThePlacement(Dimension maximum) {
		if (_components != null && _components.length > 0) {
			Dimension usedSpace = _components[0].getSize();
			ArrayList emptyRects = new ArrayList(_components.length);

			for (int i=1; i<_components.length; i++) {
				Component c = _components[i];
				if (c == null) continue;
				Dimension d = c.getSize();
				boolean placed = false;


				Iterator iter = emptyRects.iterator();
				while(iter.hasNext()) {
					Rectangle free = (Rectangle)iter.next();
					// is it placeable in this open spot?
					if (free.width >= d.width && free.height >= d.height) {
						iter.remove();
						placeIt(c, free.x, free.y, maximum);
						// is there still some space empty?
						if (d.width < free.width || d.height < free.height) {
							// where is more free space: right or below?
							if (free.width - d.width > free.height - d.height)
								emptyRects.add(new Rectangle(
									free.x+d.width, free.y, free.width-d.width, free.height));
							else
								emptyRects.add(new Rectangle(
									free.x, free.y+d.height, free.width, free.height-d.height));
						}
						placed = true;
						break;
					}
				}

				if (!placed) {
					// better place it right or below placed components?
					// -> which ratio is nearer to 1?
					float ratioR = (usedSpace.width+d.width)/(float)Math.max(usedSpace.height, d.height);
					float ratioB = (usedSpace.height+d.height)/(float)Math.max(usedSpace.width, d.width);
					//System.out.println("used="+usedSpace+" current="+d+" rs "+ratioR+" "+ratioB);
					if (Math.abs(ratioR-1) < Math.abs(1/ratioB-1)) { 
						// place it to the right
						placeIt(c, usedSpace.width, 0, maximum);
						if (usedSpace.height > d.height) {
							// empty space is to the lower right
							emptyRects.add(new Rectangle(
								usedSpace.width, d.height, d.width, usedSpace.height-d.height));
						} else if (usedSpace.height < d.height) {
							// empty space is to the lower left
							emptyRects.add(new Rectangle(
								0, usedSpace.height, usedSpace.width, d.height-usedSpace.height));
						}
						usedSpace = new Dimension(usedSpace.width+d.width, Math.max(usedSpace.height, d.height));
					} else {
						// place it below
						placeIt(c, 0, usedSpace.height, maximum);
						if (usedSpace.width > d.width) {
							// empty space is to the lower right
							emptyRects.add(new Rectangle(
								d.width, usedSpace.height, usedSpace.width-d.width, d.height));
						} else if (usedSpace.width < d.width) {
							// empty space is to the upper right
							emptyRects.add(new Rectangle(
								usedSpace.width, 0, d.width-usedSpace.width, usedSpace.height));
						}
						usedSpace = new Dimension(Math.max(usedSpace.width, d.width), usedSpace.height+d.height);
					}
				}
			}


			return usedSpace;
		} else {
			return new Dimension(0,0);
		}
	}

	// with respect to the screen size
	private void placeIt(Component comp, int x, int y, Dimension max) {
		Dimension d = comp.getSize();
		if (x+d.width > max.width) x = max.width-d.width;
		if (y+d.height > max.height) y = max.height-d.height;
		comp.setLocation(x, y);
	}



    public static void main(String[] args) {
        System.out.println(System.getProperty("os.name"));

        JPanel panel = new JPanel();
        panel.setPreferredSize(new Dimension(400,300));
        JPanel panel2 = new JPanel();
        panel2.setPreferredSize(new Dimension(200,300));
        final JFrame f1 = new JFrame("F1");
        f1.setContentPane(panel);
        f1.pack();
        final JFrame f2 = new JFrame("F2");
        f2.setContentPane(panel);
        f2.pack();
		final JFrame f3 = new JFrame("F3");
        f3.setContentPane(panel2);
        f3.pack();
		final JFrame f4 = new JFrame("F4");
        f4.setContentPane(panel2);
        f4.pack();
		f1.setVisible(true);
        f2.setVisible(true);
		f3.setVisible(true);
        f4.setVisible(true);
		
		ComponentPlacer cp = new ComponentPlacer(new Component[] { f1, f3, f4, f2 });
		try { Thread.sleep(3000); } catch (InterruptedException e) {}
		System.out.println(cp.doThePlacement(null));

		f1.setLocation(0,0);
		/*
        f1.setLocation(100,100);
        f2.setLocation(100,100+f1.getHeight());
        f1.addComponentListener(new ComponentAdapter() {
            public void componentMoved(ComponentEvent e) {
                int X = f1.getLocation().x;
                int Y = f1.getLocation().y+f1.getHeight();
                f2.setLocation(X,Y);
            }
            public void componentResized(ComponentEvent e) {
            }
        });
        f2.addComponentListener(new ComponentAdapter() {
            public void componentMoved(ComponentEvent e) {
            }
            public void componentResized(ComponentEvent e) {
            }
        });
        f1.setVisible(true);
        f2.setVisible(true);
		*/
    }
}

