package imc.epresenter.player;

import javax.swing.JPanel;

public abstract class VideoPanel extends JPanel
{

   //
   // three methods for hiding the video during user scaling
   //
   public abstract void hideVideo();

   public abstract boolean isVideoVisible();

   public abstract void showVideo();


}