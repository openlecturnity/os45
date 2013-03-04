package utils.preloader
{
    import flash.display.Loader;
    import flash.events.TimerEvent;
    import flash.utils.ByteArray;
    
    public class LoadScreen extends Loader
    {
        [ Embed(source="../../assets/LecInfiniteProgress.swf", mimeType="application/octet-stream") ]
        public var lecProgressSwf:Class;
        public var progress:Number;
        private var isReady:Boolean = false;
        private var isAborted:Boolean = false;
        private var emd:ErrorMessageDisplay;
        
        public function LoadScreen()
        {
            super();
            emd = new ErrorMessageDisplay();
            this.visible = false;

            this.loadBytes( new lecProgressSwf() as ByteArray );
        }
        
        public function abort() : void {
            isAborted = true;
        }
        
        public function updateView( event:TimerEvent ):void
        {
            if (this.isAborted) {
                //this.stage.removeChild(this);
                var bDrawOk:Boolean = emd.draw(this);
                if (bDrawOk) {
                    this.stage.addChild(emd);
                    emd.x = this.stage.stageWidth/2 - emd.width/2;
                    emd.y = this.stage.stageHeight/2 - emd.height/2;
                } else {
                    this.stage.addChild(this);
                    this.x = this.stage.stageWidth/2 - this.width/2;
                    this.y = this.stage.stageHeight/2 - this.height/2;
                }
                this.visible=true;
            } else {
                this.stage.addChild(this);
                this.x = this.stage.stageWidth/2 - this.width/2;
                this.y = this.stage.stageHeight/2 - this.height/2;
                this.visible=true;
                if( this.isReady && this.progress >= 100 )
                    closeScreen();    
            }
            
        }
        
        public function closeScreen():void
        {
            this.parent.removeChild(this);
        }
        
        public function isUrlANetworkPlace() : Boolean {
            // Bugfix 5358
            var bUrlIsNetworkPlace:Boolean = false;
            var strUrl:String = this.loaderInfo.loaderURL;
            var idx:int = strUrl.indexOf("file:")
            if (strUrl.indexOf("file:") >= 0) {
                strUrl = strUrl.substring(idx+5);
                if (strUrl.indexOf(":") < 0)
                    bUrlIsNetworkPlace = true;
            }

            return bUrlIsNetworkPlace;
        }
        
        public function set ready(value : Boolean) : void {
            this.isReady = value;
            this.visible = !this.isReady;
        }
        
        public function get ready() : Boolean { 
            return this.isReady;
        }
    }
    
}