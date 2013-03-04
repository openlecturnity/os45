import java.awt.*;
import java.awt.image.BufferedImage;
import java.awt.event.*;
import javax.swing.*;

public class GridDragon
{
	public static void main(String[] args)
	{
		JFrame f = new JFrame("Messbecher");
		
		JMenuBar bar = new JMenuBar();
		JMenu menu1 = new JMenu("File");
		menu1.add(new JMenuItem("Mecha"));
		bar.add(menu1);

		f.setJMenuBar(bar);

		JLabel filler = new JLabel("<html>Humpa. Humpa.<br>Trara!</html>");
		//filler.setBackground(Color.red);
		filler.setOpaque(true);
		filler.setBorder(BorderFactory.createEtchedBorder());
		//f.setContentPane(filler);

		JPanel panel = new JPanel();
		JToolBar tools = new JToolBar("THE tools");
		tools.add(filler);
		//panel.add("Center", new JLabel("Mug drug stuck."));
		//panel.add("South", tools);
		GridBagLayout layout = new GridBagLayout();
		panel.setLayout(layout);
		GridBagConstraints constraints = new GridBagConstraints();
		JLabel label0 = new JLabel("label0");
		panel.add(label0);
		constraints.gridwidth = GridBagConstraints.REMAINDER;
		JLabel label1 = new JLabel("label1");
		layout.setConstraints(label1, constraints);
		panel.add(label1);
		JLabel label2 = new JLabel("label2");
		constraints.fill = GridBagConstraints.HORIZONTAL;
		label2.setOpaque(true);
		label2.setBackground(Color.blue);
		layout.setConstraints(label2, constraints);
		panel.add(label2);


		JSlider slider = new JSlider();
		slider.setPaintTicks(true);
		slider.setMajorTickSpacing(10);
		slider.setPaintTrack(true);
		slider.setPreferredSize(new Dimension(
			300, slider.getPreferredSize().height));
		
		layout.setConstraints(slider, constraints);
		panel.add(slider);



		BufferedImage alphaImage = new BufferedImage(
			50, 50, BufferedImage.TYPE_INT_ARGB);
		//Color alphaBlack = new Color(0x80000000, true);
		Color alphaBlack = Color.black;
		alphaImage.getGraphics().setColor(alphaBlack);
		alphaImage.getGraphics().fillRect(0,0,50,50);
		JLabel obskur = new JLabel("text", new ImageIcon(alphaImage), JLabel.CENTER);
		obskur.setLocation(10, 10);
		obskur.setSize(obskur.getPreferredSize());

		f.getLayeredPane().setLayer(obskur, JLayeredPane.PALETTE_LAYER.intValue());



		f.getContentPane().add("Center", panel);


		f.pack();

		f.show();

		/*
		try {
			Thread.sleep(4000);
		} catch (Exception e) {}

		panel.remove(label2);
		
		constraints.fill = GridBagConstraints.NONE;
		layout.setConstraints(label2, constraints);

		panel.add(label2);

		label2.invalidate();
		
		panel.doLayout();
		*/
	
	}
}
