package imc.epresenter.player;

public interface Coordinator
{
   public abstract void requestStart(SRStreamPlayer origin);
   public abstract void requestScrollStart(SRStreamPlayer origin);
   public abstract void requestScrollStop(SRStreamPlayer origin);
   public abstract void requestStop(SRStreamPlayer origin);
   public abstract void requestStop(boolean bSpecial, SRStreamPlayer origin);
   public abstract void requestTime(int time, SRStreamPlayer origin);
   public abstract void requestNextSlide(SRStreamPlayer origin);
   public abstract void requestPreviousSlide(boolean bRealPrevious, SRStreamPlayer origin);
   public abstract void requestMute(boolean bDoMute, SRStreamPlayer origin);
   public abstract void informEndOfMediaReached(SRStreamPlayer origin);
   public abstract void requestClose(SRStreamPlayer origin);
   public abstract void requestRepaintOfWhiteboard();
}
