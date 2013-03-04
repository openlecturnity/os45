package
{
    public class PageEntry
    {
        public function PageEntry(strIndex:String, 
                                  strImgSource:String, 
                                  strChapter:String, 
                                  strTitle:String, 
                                  strTime:String, 
                                  nPageNumber:int, 
                                  bIsClip:Boolean, 
                                  bIsClickPage:Boolean) 
                                  //imgIcon:SmoothImage)
        {
            this.pageEntryStrings = new Array(5);
            this.pageEntryStrings[0] = strIndex;
            this.pageEntryStrings[1] = strImgSource;
            this.pageEntryStrings[2] = strChapter;
            this.pageEntryStrings[3] = strTitle;
            this.pageEntryStrings[4] = strTime;
            //this.pageIcon = imgIcon;
            this.nPageNumber = nPageNumber;
            this.bIsClip = bIsClip;
            this.bIsClickPage = bIsClickPage;
        }

        public var pageEntryStrings:Array;
        //public var pageIcon:SmoothImage;
        // Bugfix 5794: index != page number!
        // TODO: double code: nPageNumber, bIsClip, bIsClickPage 
        // also exist in PageInfo class. 
        public var nPageNumber:int;
        public var bIsClip:Boolean;
        public var bIsClickPage:Boolean;
    }
}