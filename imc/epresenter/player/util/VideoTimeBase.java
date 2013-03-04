package imc.epresenter.player.util;

import javax.media.*;

public class VideoTimeBase implements TimeBase {
    private long startMillis; // time of construction or last getRate()
    private long timeOfSetRate;
    private float rate;

    public VideoTimeBase() {
        startMillis = TimeSource.currentTimeMillis();
        timeOfSetRate = 0; // = getMilliseconds()
        rate = 1.0f;
    }
    
    public synchronized void setRate(float r) {
        rate = r;
        timeOfSetRate = getMilliseconds(); // perserve the already verstrichene time
        startMillis = TimeSource.currentTimeMillis();
    }
    
    private long getMilliseconds() {
        return timeOfSetRate+(long)(rate*(TimeSource.currentTimeMillis()-startMillis));
    }
    
    public synchronized long getNanoseconds() {
        //System.out.print("t"+getMilliseconds()+" ");
        return getMilliseconds()*1000000L;
    }

    public synchronized Time getTime() {
        return new Time(getNanoseconds());
    }
}
