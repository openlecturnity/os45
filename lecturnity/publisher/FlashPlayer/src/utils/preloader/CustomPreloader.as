package utils.preloader
{
    import flash.display.Sprite;
    import flash.events.Event;
    import flash.events.ProgressEvent;
    import flash.events.SecurityErrorEvent;
    import flash.events.TimerEvent;
    import flash.utils.Timer;
    
    import mx.controls.Alert;
    import mx.events.FlexEvent;
    import mx.preloaders.DownloadProgressBar;

    public final class CustomPreloader extends DownloadProgressBar {
        public  var loader : LoadScreen;
        private var m_timer : Timer;
        private var m_nWaitForCompletionIndex : int;
    
        public function CustomPreloader() {
            super();
        }
        
        override public function initialize() : void {
            super.initialize();
            
            this.loader = new LoadScreen();
            this.addChild(this.loader);
            
            this.m_nWaitForCompletionIndex = 0;
            
            this.m_timer = new Timer(100, 0);
            this.m_timer.addEventListener(TimerEvent.TIMER, handleTimerTick);
            this.m_timer.start();
        }
        
        override public function set preloader(preloader : Sprite):void {
            preloader.addEventListener(ProgressEvent.PROGRESS,  SWFDownLoadScreen);
            preloader.addEventListener(Event.COMPLETE,          SWFDownloadComplete);
            preloader.addEventListener(FlexEvent.INIT_PROGRESS, FlexInitProgress);
            preloader.addEventListener(FlexEvent.INIT_COMPLETE, FlexInitComplete);
            preloader.addEventListener(SecurityErrorEvent.SECURITY_ERROR, securityErrorHandler);
        }
    
        private function SWFDownLoadScreen(event : ProgressEvent) : void {
            var prog : Number = event.bytesLoaded / event.bytesTotal * 100;
            if (this.loader) {
                this.loader.progress = prog;
            }
        }
        
        private function handleTimerTick(event : TimerEvent) : void {
            this.loader.updateView(event);

            // Check: Progress 100% but InitComplete not reached? 
            // This can occur in cases of sandbox security problems. 
            if (this.loader.progress >= 100.0 && !this.loader.ready) {
                if (m_nWaitForCompletionIndex < 20) {
                    // Delay: 20*125 msec = 2.5 seconds
                    m_nWaitForCompletionIndex++;
                } else {
                    // Stop the timer
                    // (Hack: Sometimes two loader refreshs are necessary. 
                    // --> Stop the timer after the 2nd refresh is performed).
                    if (m_nWaitForCompletionIndex > 20)
                        this.m_timer.stop();
                    m_nWaitForCompletionIndex++;
                    // Abort the loader and make a refresh
                    this.loader.abort();
                    this.loader.updateView(event);
                } 
            }
        }
    
        private function securityErrorHandler(event : SecurityErrorEvent) : void {
            mx.controls.Alert.show("Security error detected: \n" + event);
        }

        private function SWFDownloadComplete(event : Event) : void {}
    
        private function FlexInitProgress(event : Event) : void {}
    
        private function FlexInitComplete(event : Event) : void {
            this.loader.ready = true;
            this.m_timer.stop();
            this.dispatchEvent(new Event(Event.COMPLETE));
        }
        
        override protected function showDisplayForInit(elapsedTime:int, count:int):Boolean {
            return true;
        }
        
        override protected function showDisplayForDownloading(elapsedTime:int,
                                                              event:ProgressEvent):Boolean {
            return true;
        }

    }
}