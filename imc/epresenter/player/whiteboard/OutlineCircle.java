package imc.epresenter.player.whiteboard;

import java.io.*;

class OutlineCircle extends OutlineRectangle 
{
	OutlineCircle(String line, BufferedReader in) 
   {
      super(line, in);
      
      m_bIsActuallyACircle = true;
	}
}