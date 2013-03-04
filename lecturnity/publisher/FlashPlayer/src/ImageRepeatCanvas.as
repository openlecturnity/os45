/*
    The ImageRepeatCanvas provides a container in which
    embeded images can be repeated
    as designers do in html tables.
*/
 
package
{
    import flash.display.Bitmap;
    import flash.display.Graphics;
    import mx.containers.Canvas;
 
    /**
     *  The ImageRepeatCanvas container gives a way of creating
     * one image that repeat
     *  across either directions.
     *
     *  @mxml
     *
     *
<pre>
     *  <namespace:ImageRepeatCanvas
     *       repeatImage="{EmbededImage Class Reference}"
     *       repeatDirection="horizantal|horizantal"
     *    />
     *</pre>
*/
 
    public class ImageRepeatCanvas extends Canvas
    {
        //-------------------------------------------------------------
        //  Variables
        //-------------------------------------------------------------
        private var bgImg:Bitmap = new Bitmap();
        private var direction:String;
        public var repeatImage:Class;
 
        //--------------------------------------------------------------
        //  Constants
        //--------------------------------------------------------------
        public static var REPEAT_HORIZANAL:String = "horizantal";
        public static var REPEAT_VERTICAL:String = "vertical";
 
        //---------------------------------------------------------------
        //  Constructor
        //---------------------------------------------------------------
        public function ImageRepeatCanvas()
        {
            super();
 
        }
 
        /**
         *  A setter method to set the direction for repeation of image
         */
        [Inspectable(category="General", enumeration="horizantal,vertical", defaultValue="horizantal")]
        public function set repeatDirection(val:String):void{
            direction = val;
        }
 
        /**
         *  @private
         */
        override protected function updateDisplayList(w:Number, h:Number):void{
            super.updateDisplayList(w,h);
            bgImg.bitmapData = new repeatImage().bitmapData;
            if(bgImg){
                switch(direction){
                    case ImageRepeatCanvas.REPEAT_HORIZANAL:
                                    h = bgImg.height;
                                    break;
                    case ImageRepeatCanvas.REPEAT_VERTICAL:
                                    w = bgImg.width;
                                    break;
                }
                var Grpx:Graphics = graphics;
                Grpx.clear();
 
                Grpx.beginBitmapFill(new repeatImage().bitmapData);
                drawRoundRect(0,0,w,h);
                Grpx.endFill();
            }
        }
    }
}