package
{
    import flash.display.DisplayObject;
    import mx.controls.VideoDisplay;
    import mx.controls.videoClasses.VideoPlayer;

    public class SmoothVideoDisplay extends VideoDisplay
    {
        public var smoothing:Boolean=false;
        public var deblocking:int=0;

        public function SmoothVideoDisplay()
        {
            super();
        }

        override public function addChild(child:DisplayObject):DisplayObject
        {
            var video:VideoPlayer = VideoPlayer(child);
            video.smoothing=smoothing;
            video.deblocking=deblocking;

            return super.addChild(child);
        }
    }
}
