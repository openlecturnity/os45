package imc.lecturnity.util.wizards;

import java.awt.Point;
import java.awt.Dimension;
import java.awt.Font;
import javax.swing.JTextArea;

public class WizardTextArea extends JTextArea
{
   private static boolean USE_CUSTOM_COLORS = false;
   
   public WizardTextArea(String content, Point location, 
                         Dimension size)
   {
      this(content, location, size, null);
   }
   
   public WizardTextArea(String content, Point location, 
                         Dimension size, Font font)
   {
      super(content);
      //if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
      if (USE_CUSTOM_COLORS)
         setBackground(Wizard.BG_COLOR);
      else
         setOpaque(false);
      setLocation(location);
      setSize(size);
      setLineWrap(true);
      setWrapStyleWord(true);
      if (font != null)
         setFont(font);
      setEditable(false);
   }
}
