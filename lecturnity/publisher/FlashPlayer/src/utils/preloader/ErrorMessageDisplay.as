package utils.preloader
{
    import flash.display.BitmapData;
    import flash.display.Graphics;
    import flash.display.Sprite;
    import flash.geom.Matrix;
    
    import mx.core.UITextField;
    import mx.graphics.ImageSnapshot;
    import mx.resources.ResourceManager;
    
    public class ErrorMessageDisplay extends Sprite {
        private var g:Graphics;
        
        public function ErrorMessageDisplay() {
        }
        
        public function draw(ls:LoadScreen):Boolean {
            var bOK:Boolean = true;
            // Display an error message
            // Draw a Frame with dark grey border and light blue background
            g = this.graphics;
            g.clear();
            g.beginFill(0xC9D8FC);
            g.lineStyle(2, 0x333333, 1, true);
            if (ls.isUrlANetworkPlace())
                g.drawRoundRect(20, 20, 320, 220, 2);
            else
                g.drawRoundRect(20, 20, 320, 180, 2);
            //Use a TextField to display the text
            var uit:UITextField = new UITextField();
            uit.styleName = "stdNormal";
            var strText:String = "";
            var strResource:String = "";
            strText += "<p><font face='Arial' size='12'>";
            strResource = ResourceManager.getInstance().getString("localization", "NoDirectHtmlPossible");
            if (strResource == null) {
                bOK = false;
                strResource = "";
            }
            strText += strResource;
            if (ls.isUrlANetworkPlace()) {
                strResource = ResourceManager.getInstance().getString("localization", "NoNetworkPlaceReplay");
                if (strResource == null) {
                    bOK = false;
                    strResource = "";
                }
                strText += strResource;
            } else {
                strResource = ResourceManager.getInstance().getString("localization", "AlternativeStartHeader");
                if (strResource == null) {
                    bOK = false;
                    strResource = "";
                }
                strText += strResource;
            }
            strResource = ResourceManager.getInstance().getString("localization", "AlternativeStart");
            if (strResource == null) {
                bOK = false;
                strResource = "";
            }
            strText += strResource;
            strText += "</font></p>";
            uit.htmlText = strText;
            uit.autoSize = "left";
            // Text can not be displayed directly using mx.graphics 
            // --> Make a "snapshot" of the TextField and draw the bitmap data
            var textBitmapData:BitmapData = ImageSnapshot.captureBitmapData(uit);
            var sizeMatrix:Matrix = new Matrix();
            var coef:Number = Math.min(uit.measuredWidth / textBitmapData.width, 
                                       uit.measuredHeight / textBitmapData.height);
            sizeMatrix.a = coef;
            sizeMatrix.d = coef;
            textBitmapData =ImageSnapshot.captureBitmapData(uit, sizeMatrix);
            g.lineStyle(0, 0, 0);
            var sm:Matrix = new Matrix();
            sm.tx = 40;
            sm.ty = 40;
            g.beginBitmapFill(textBitmapData, sm, false);
            if (ls.isUrlANetworkPlace())
                g.drawRect(0, 0, 320, 220);
            else
                g.drawRect(0, 0, 320, 180);
            g.endFill();
            
            return bOK;
        }
    }
    
}