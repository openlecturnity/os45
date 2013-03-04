package imc.lecturnity.util.ui;

import javax.swing.JList;
import javax.swing.JLabel;
import javax.swing.ListCellRenderer;
import javax.swing.Icon;
import javax.swing.ImageIcon;

import java.awt.Component;
import java.awt.Color;

import java.util.HashMap;

import imc.lecturnity.util.ColorManager;

/**
 * This class can be used in conjunction with a <tt>JList</tt> in
 * order to make the JList display icons in front of each list
 * entry. Instanciate a new <tt>IconListCellRenderer</tt> instance
 * either with a resource string or with an existing <tt>Icon</tt>
 * instance, then set the <tt>ListCellRenderer</tt> of the
 * <tt>JList</tt> to the new <tt>IconListCellRenderer</tt> instance:
 * <code>
 * JList jList = ...;
 * IconListCellRenderer ilcr = new IconListCellRenderer("/.../my_icon.png");
 * jList.setCellRenderer(ilcr);
 * </code>
 *
 * <p>If you want your list to display different icons for different
 * entries, make your objects in the <tt>JList</tt> implement the
 * {@link IconListObject IconListObject} interface and give the
 * corresponding resource string for each object. These icons will be
 * stored in a hash map.</p>
 *
 * @see IconListObject
 */
public class IconListCellRenderer extends JLabel implements ListCellRenderer
{
   private static boolean USE_CUSTOM_COLORS = false;
   
   private Icon defaultIcon_;
   private HashMap iconMap_;
   
   public IconListCellRenderer(String defaultIconResource)
   {
      this(new ImageIcon("".getClass().getResource(defaultIconResource)));
   }
   
   public IconListCellRenderer(Icon defaultIcon)
   {
      super(defaultIcon, LEFT);
      defaultIcon_ = defaultIcon;
      iconMap_ = new HashMap();
      setOpaque(true);
   }

   public Component getListCellRendererComponent(JList list, Object value,
                                                 int index, boolean isSelected,
                                                 boolean cellHasFocus)
   {
      if (value instanceof IconListObject)
      {
         IconListObject ilo = (IconListObject) value;
         String resourceString = ilo.getIconResourceString();
         Icon icon = (Icon) iconMap_.get(resourceString);
         if (icon == null)
         {
            try
            {
               icon = new ImageIcon(getClass().getResource(resourceString));
            }
            catch (Exception e)
            {
               icon = defaultIcon_;
               System.err.println("unknown Icon resource: " + resourceString);
               e.printStackTrace();
            }
            
            iconMap_.put(resourceString, icon);
         }
         
         setIcon(icon);
      }
      setText(value.toString());
//       setToolTipText(value.toString());
      if (list.isEnabled())
      {
         //if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
         if (USE_CUSTOM_COLORS)
         {
            setBackground(isSelected ? ColorManager.orangeResource : Color.white);
            setForeground(isSelected ? Color.black : Color.black);
         }
         else
         {
            setBackground(isSelected ? javax.swing.UIManager.getColor("List.selectionBackground") :
                                       javax.swing.UIManager.getColor("List.background"));
            setForeground(isSelected ? javax.swing.UIManager.getColor("List.selectionForeground") :
                                       javax.swing.UIManager.getColor("List.foreground"));
         }
      }
      else
      {
         //if (imc.lecturnity.converter.ConverterWizard.USE_CUSTOM_COLORS)
         if (USE_CUSTOM_COLORS)
         {
            setBackground(ColorManager.greyResource);
            setForeground(ColorManager.darkResource);
         }
         else
         {
            setBackground(javax.swing.UIManager.getColor("control"));
            setForeground(javax.swing.UIManager.getColor("controlDkShadow"));
         }
      }
      return this;
   }
}
