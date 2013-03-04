package imc.epresenter.player;

import java.awt.event.ActionListener;

public interface DocumentController
{
   public void closePerformed();
   public void loadDocument(String location);
   public void loadDocument(String location, int time);
   public void loadDocument(String location, int time, 
                            boolean startup, ActionListener finishListener);
   public void closeDocument(Document d);
}
