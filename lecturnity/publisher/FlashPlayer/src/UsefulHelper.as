package
{
    //import de.slidestar.app.model.SlidestarML;
    
    import flash.display.InteractiveObject;
    import flash.events.MouseEvent;
    
    import mx.controls.dataGridClasses.DataGridColumn;
    import mx.core.Container;
    import mx.core.UIComponent;
    import mx.formatters.DateFormatter;
    
    import org.osflash.thunderbolt.Logger;
    
    
    public class UsefulHelper {
        
        public static function dispatchMouseDownEvent(iobject:InteractiveObject):void {
            if(iobject) iobject.dispatchEvent(new MouseEvent(MouseEvent.MOUSE_DOWN));
        }
        
        public static function dispatchMouseOffEvents(iobject:InteractiveObject):void {
            if(iobject) {
                iobject.dispatchEvent(new MouseEvent(MouseEvent.MOUSE_UP));
                iobject.dispatchEvent(new MouseEvent(MouseEvent.ROLL_OUT));
            }
        }
        
        public static function dateLabelFunction(item:Object, column:DataGridColumn):String {
            return dateFormatFunction(new Date(item[column.dataField]));
        }
        
        public static function dateFormatFunction(date:*, format:String=null):String {
            if (date is String) {
                if (date.charAt(5) == date.charAt(7) == '-') {
                    date = new Date(date.substr(0,4), Number(date.substr(5,2))-1, date.substr(8,2));
                } else {
                    date = new Date();
                }
            }
            var dFormat:DateFormatter = new DateFormatter();
            //dFormat.formatString = SlidestarML.getInstance().locals.files.gui.formats.dateformat;
            dFormat.formatString = "day: DD, month: MM, year: YYYY";
            if (format) {
                dFormat.formatString = format;
            }
            return dFormat.format(date);
        }
        
        public static function getAllChildren(root:Container):Array {
            var uiComponent:UIComponent;
            var children:Array = root.getChildren();
            var result:Array = new Array();
            for each(uiComponent in children) {
                result.push(uiComponent);
                if(uiComponent is Container) {
                    var allChildren:Array = getAllChildren(uiComponent as Container);
                    for each(uiComponent in allChildren) {
                        result.push(uiComponent);
                    }
                }
            } 
            return result;
        }
        
        public static function getAllChildrenByNames(root:Container):Object {
            var uiComponent:UIComponent;
            var children:Array = root.getChildren();
            var result:Object = new Object();
            for each(uiComponent in children) {
                result[uiComponent.id] = uiComponent;
                if(uiComponent is Container) {
                    var allChildren:Array = getAllChildren(uiComponent as Container);
                    for each(uiComponent in allChildren) {
                        result[uiComponent.id] = uiComponent;
                    }
                }
            }            
            return result;
        }
        
        public static function implodeStringArray(array:Array,delimiter:String):String {
            var ret:String = "";
            for (var i:int=0;i<array.length-1;i++) {
                if (array[i] is String)
                    ret += array[i] + delimiter;
            }
            if (array[array.length-1] is String)
                ret += array[array.length-1];
            
            return ret;
        }
        
        /**
            Formats in integer into a String into the form h:mm:ss where hours are left aside if not needed
         **/
        public static function formatTimeInSeconds(timeInSeconds:int):String {
            var result:String;
            var seconds:String;
            var minutes:String;
            var hours:String;
            
            seconds = (timeInSeconds % 60).toString();
            minutes = (Math.floor(timeInSeconds / 60)%60).toString();
            hours = (Math.floor(timeInSeconds / 3600)).toString();
            
            if(seconds.length == 1) seconds = "0" + seconds;
            if(hours!="0" && minutes.length == 1) minutes = "0" + minutes;
            
            result = minutes+":"+seconds;
            if(hours!="0") result = hours+":"+result;
            
            return result;
        }
        
        public static function randomString():String {
            var strLength:Number = 24;
            var chrArr:Array = new Array("a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z");
            var rndStr:String = "";

            for (var i:Number=0; i < strLength; i++) {
                  var getChar:Number = int(Math.random() * 24);
                  rndStr += chrArr[getChar];
            }
            return rndStr;
        }
        
        static public function round( value : Number, digits : int = 0 ):Number {
            var rounder : Number = Math.pow( 10, digits );
            return Math.round(value * rounder) / rounder;
        }
        
        public static function highlightText(_in:String, _seek:String, _delimeter:String=" "):String {
            if (_seek.length<=0) {
                return _in;
            }
            _seek = _seek.replace(new RegExp("[+-<>)(~*\"']", "gi"), '');
            var replaceStrings:Array = [_seek];
            if(_delimeter) {
                replaceStrings = _seek.split(_delimeter);
            }
            
            Logger.debug("highlightText", replaceStrings);
            
            var reg:RegExp = new RegExp("("+replaceStrings.join("|")+")", "gi");
            _in = _in.replace(reg, "<font color='#CC0000'><i>$&</i></font>");
            
            return _in;
        }
        
        public static function stripHtmlTags(html:String, tags:String = null):String {
            var tagsToBeKept:Array = new Array();
            if (tags.length > 0)
                tagsToBeKept = tags.split(new RegExp("\\s*,\\s*"));
         
            var tagsToKeep:Array = new Array();
            for (var i:int = 0; i < tagsToBeKept.length; i++)
            {
                if (tagsToBeKept[i] != null && tagsToBeKept[i] != "")
                    tagsToKeep.push(tagsToBeKept[i]);
            }
         
            var toBeRemoved:Array = new Array();
            var tagRegExp:RegExp = new RegExp("<([^>\\s]+)(\\s[^>]+)*>", "g");
         
            var foundedStrings:Array = html.match(tagRegExp);
            for (i = 0; i < foundedStrings.length; i++) 
            {
                var tagFlag:Boolean = false;
                if (tagsToKeep != null) 
                {
                    for (var j:int = 0; j < tagsToKeep.length; j++)
                    {
                        var tmpRegExp:RegExp = new RegExp("<\/?" + tagsToKeep[j] + "[^<>]*?>", "i");
                        var tmpStr:String = foundedStrings[i] as String;
                        if (tmpStr.search(tmpRegExp) != -1) 
                            tagFlag = true;
                    }
                }
                if (!tagFlag)
                    toBeRemoved.push(foundedStrings[i]);
            }
            for (i = 0; i < toBeRemoved.length; i++) 
            {
                var tmpRE:RegExp = new RegExp("([\+\*\$\/])","g");
                var tmpRemRE:RegExp = new RegExp((toBeRemoved[i] as String).replace(tmpRE, "\\$1"),"g");
                html = html.replace(tmpRemRE, "");
            } 
            return html;
        }
        
        public static function clipEmail(email:String):String {
            return String(email.split("@")[0]).replace("."," ");
        }
            
    }
}