package imc.lecturnity.util.wizards;

import java.awt.Insets;
import java.awt.BorderLayout;
import java.awt.FlowLayout;
import java.awt.Component;
import java.awt.Graphics;
import java.awt.Font;

import javax.swing.JPanel;

import imc.lecturnity.converter.ConverterWizard;

public class ButtonPanel extends JPanel
{
   private JPanel rightButtons_;
   private JPanel leftButtons_;
   private boolean drawBreak_ = true;

   public ButtonPanel()
   {
      this(true);
   }

   public ButtonPanel(boolean drawBreak)
   {
      super();
      drawBreak_ = drawBreak;
      setLayout(new BorderLayout());
      rightButtons_ = new JPanel(new FlowLayout(FlowLayout.RIGHT, 0, 0));
      rightButtons_.setFont(imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12));
      add(rightButtons_, BorderLayout.CENTER);
      leftButtons_ = new JPanel(new FlowLayout(FlowLayout.LEFT, 0, 0));
      leftButtons_.setFont(imc.lecturnity.converter.ConverterWizard.createDefaultFont(Font.PLAIN, 12));
      add(leftButtons_, BorderLayout.WEST);
   }

   public Insets getInsets()
   {
      return WizardPanel.BUTTONPANEL_INSETS; // new Insets(17, 30, 15, 15);
   }

   public void showBreakLine(boolean drawBreak)
   {
      drawBreak_ = drawBreak;
      this.repaint();
   }
   
   public void paint(Graphics g)
   {
      super.paint(g);
      if (drawBreak_)
      {
         int width = getSize().width;
         g.setColor(Wizard.SHADE_COLOR);
         g.drawLine(0, 0, width, 0);
         g.setColor(Wizard.HILITE_COLOR);
         g.drawLine(0, 1, width, 1);
      }
   }

   public void addRightButton(Component component)
   {
      rightButtons_.add(component);
   }

   public void addLeftButton(Component component)
   {
      leftButtons_.add(component);
   }
}
