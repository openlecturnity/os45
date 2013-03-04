package
{
    import mx.collections.ArrayCollection;
    
    public class PageInfo
    {
        public function PageInfo(nPageNumber:int, nBeginMs:int, nEndMs:int, 
                                 strChapter:String, strTitle:String, strKeywords:String, 
                                 strThumbnail:String, bIsClip:Boolean, 
                                 bIsClickPage:Boolean) {
            this.nPageNumber = nPageNumber;
            this.nBeginMs = nBeginMs;
            this.nEndMs = nEndMs;
            this.strChapter = strChapter;
            this.strTitle = strTitle;
            this.strKeywords = strKeywords;
            this.strThumbnail = strThumbnail;
            this.bIsClip = bIsClip;
            this.bIsClickPage = bIsClickPage;
            aFullTexts = new ArrayCollection();
        }

        public function AddFullText(fullText:FullText):void {
            aFullTexts.addItem(fullText);
        }

        public function GetFullTextArray():ArrayCollection {
            return aFullTexts;
        }

        public var nPageNumber:int;
        public var nBeginMs:int;
        public var nEndMs:int;
        public var strChapter:String;
        public var strTitle:String;
        public var strKeywords:String;
        public var strThumbnail:String;
        public var bIsClip:Boolean;
        public var bIsClickPage:Boolean;
        private var aFullTexts:ArrayCollection;
    }
}