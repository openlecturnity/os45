package
{
    public class SearchNote
    {
        public function SearchNote(nTimeMs:int, nPage:int, strTitle:String, strType:String)
        {
            this.nTimeMs = nTimeMs;
            this.nPage = nPage;
            this.strTime = UsefulHelper.formatTimeInSeconds(nTimeMs / 1000.0);
            this.strTitle = strTitle;
            this.strType = strType;
        }

        public var nTimeMs:int;
        public var nPage:int;
        public var strTime:String;
        public var strTitle:String;
        public var strType:String;
    }
}