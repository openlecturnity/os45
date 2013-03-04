package imc.lecturnity.util;

import java.awt.Color;
import java.util.Calendar;
import java.util.GregorianCalendar;
import javax.swing.UIManager;
import javax.swing.plaf.ColorUIResource;

public class ColorManager
{
   public static ColorUIResource blackResource = new ColorUIResource(Color.black);
   public static ColorUIResource greyResource = new ColorUIResource(new Color(0xd5d5d5));
   public static ColorUIResource darkResource = new ColorUIResource(new Color(0xb5b5b4));
   public static ColorUIResource orangeResource = new ColorUIResource(new Color(0xf7c9a7));

   public static void setImcColors()
   {
      UIManager.put("ToolTip.foreground", blackResource);
      UIManager.put("CheckBox.foreground", blackResource);
      UIManager.put("Label.foreground", blackResource);
      UIManager.put("RadioButton.foreground", blackResource);
      UIManager.put("Button.foreground", blackResource);
      UIManager.put("ScrollBar.foreground", greyResource);
      UIManager.put("Table.background", greyResource);
      UIManager.put("Label.background", greyResource);
      UIManager.put("TableHeader.background", greyResource);
      UIManager.put("ToolTip.background", orangeResource);
      UIManager.put("Button.background", greyResource);
      UIManager.put("Panel.background", greyResource);
      UIManager.put("CheckBox.background", greyResource);
      UIManager.put("RadioButton.background", greyResource);
      UIManager.put("TabbedPane.background", greyResource);
      UIManager.put("OptionPane.background", greyResource);
      UIManager.put("ScrollPane.background", greyResource);
      UIManager.put("Menu.background", greyResource);
      UIManager.put("MenuBar.background", greyResource);
      UIManager.put("MenuItem.background", greyResource);
      UIManager.put("ScrollBar.background", darkResource); // funktioniert bei Metal
      UIManager.put("Slider.background", greyResource);
      UIManager.put("Viewport.background", greyResource);
      UIManager.put("ProgressBar.background", greyResource);
      GregorianCalendar cal = new GregorianCalendar();
      int hour = cal.get(Calendar.HOUR_OF_DAY);
      if (hour > 1 && hour < 5)
         UIManager.put("control", orangeResource);
      else
         UIManager.put("control", greyResource);
      if (System.getProperty("os.name").startsWith("Win"))
      {
         UIManager.put("ScrollBar.thumb", greyResource);
         UIManager.put("ScrollBar.track", darkResource); // funktioniert bei Win L&F
      }
   }
}