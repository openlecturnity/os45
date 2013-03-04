package
{
    public class ClipInfo
    {
        public function ClipInfo(strClipFlvFile:String, width:int, height: int, nBeginMs:int, nEndMs:int)
        {
            this.strClipFlvFile = strClipFlvFile;
            this.strClipUrl = strClipFlvFile; // full URL path is set later on
            this.nWidth = width;
            this.nHeight = height;
            this.nBeginMs = nBeginMs;
            this.nEndMs = nEndMs;
        }

        public var strClipFlvFile:String;
        public var strClipUrl:String;
        public var nWidth:int;
        public var nHeight:int;
        public var nBeginMs:int;
        public var nEndMs:int;
    }
}