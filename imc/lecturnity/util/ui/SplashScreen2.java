package imc.lecturnity.util.ui;

import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.*;

/** Currently not used; the old splash screen was adapted to the new look
    of 4.0 and the file copied here "SplashScreen.java".
    
    That is now also used as about screen in Player and as splash screen in Publisher.
    
    One reason not-using: Dependency on lsutl32 which is bad for cd exports (and Player installer).
    */
public class SplashScreen2 {
   private static boolean s_bIsLoaded;

   static {
      try {
         System.loadLibrary("lsutl32");
         s_bIsLoaded = true;
      } catch (UnsatisfiedLinkError e) {
         s_bIsLoaded = false;
      }
   }
      
   public native boolean Show();
   public native void ShowAsAbout(Window wndParent);
   public native void Hide();
   public native Rectangle GetScreenBounds();

}