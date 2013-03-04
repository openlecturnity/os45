package
{
    import flash.events.*;
    import flash.media.Video;
    import flash.net.NetConnection;
    import flash.net.NetStream;
    import flash.utils.*;
    
    import mx.controls.Alert;
    import mx.core.UIComponent;

    public class LecturnityVideoDisplay extends UIComponent
    {
        // Members
        private var m_strBaseUrl:String = "";
        private var m_strStreamUrl:String = "";
        private var m_strFlvSource:String = "";
        private var m_nVideoWidth:int = 0;
        private var m_nVideoHeight:int = 0;
        private var m_nLengthMs:int = 0;
        private var m_NetConnection:NetConnection = null;
        private var m_NetStream:NetStream = null;
        private var m_Video:Video = null;
        private var m_bIsConnectingToServer:Boolean = false;
        private var m_bIsPlaying:Boolean = false;
        private var m_bIsTrueStreaming:Boolean = false;
        private var m_bNetConnectionSuccessful:Boolean = false;
        private var m_bVideoReady:Boolean = false;
        private var m_bStreamStripFlvSuffix:Boolean = true;
        private var m_nStartBufferLength:int = 1;
        private var m_nExpandedBufferLength:int = 10;
        private var m_nLastSeekTimeSec:Number = 0.0;
        private var m_fBufferingSum:Number = 0;
        private var m_nBufferingIndex:int = 0;
        private var m_nBufferIntervalID:uint = 0;
        private var m_nSeekIntervalID:uint = 0;

        // Constructor
        public function LecturnityVideoDisplay(strBaseUrl:String, 
                                               strStreamUrl:String, 
                                               strFlvSource:String, 
                                               nVideoWidth:int, 
                                               nVideoHeight:int, 
                                               nLengthMs:int, 
                                               bStreamStripFlvSuffix:Boolean)
        {
            super();

            m_strBaseUrl = strBaseUrl;
            m_strStreamUrl = strStreamUrl;
            m_strFlvSource = strFlvSource;
            m_nVideoWidth = nVideoWidth;
            m_nVideoHeight = nVideoHeight;
            m_nLengthMs = nLengthMs;
            m_bStreamStripFlvSuffix = bStreamStripFlvSuffix;

            SetNetConnection();
        }


        // Public methods
        public function IsReady():Boolean {
            return m_bVideoReady;
        }

        public function IsConnectingToServer():Boolean {
            return m_bIsConnectingToServer;
        }

        public function IsPlaying():Boolean {
            return m_bIsPlaying;
        }

        public function IsTrueStreaming():Boolean {
            return m_bIsTrueStreaming;
        }

        public function GetSmoothing():Boolean {
            return m_Video.smoothing;
        }

        public function SetSmoothing(bSmooth:Boolean):void {
            m_Video.smoothing = bSmooth;
        }

        public function IsBuffering():Boolean {
            var bIsBuffering:Boolean = false;
            var pctBuffer:int;
            if (m_bIsTrueStreaming) {
                pctBuffer = GetBufferPercentage();
                if (pctBuffer < 100) {
                    var fVideoStreamLengthSec:Number = m_nLengthMs / 1000.0;
                    if (m_NetStream.time < (fVideoStreamLengthSec - m_NetStream.bufferTime))
                        bIsBuffering = true;
                }
            } else {
                // Bugfix 5490 for local replay
                pctBuffer = GetLocalBufferPercentage();
                if (pctBuffer < 100) {
                    bIsBuffering = true;
                }
            }
            return bIsBuffering;
        }

        public function GetBufferPercentage():int {
            if (!m_bVideoReady) {
                return 0;
            }

            var pctBuffer:int = 0;
            if (m_bIsTrueStreaming) {
                if (m_NetStream.bufferTime > 0.0)
                    pctBuffer = Math.min(Math.round((m_NetStream.bufferLength / m_NetStream.bufferTime) * 100), 100);
            } else {
                if (m_NetStream.bytesTotal > 0)
                    pctBuffer = Math.min(Math.round((m_NetStream.bytesLoaded / m_NetStream.bytesTotal) * 100), 100);
            }

            return pctBuffer;
        }

        // For bugfix 5490
        public function GetLocalBufferPercentage():int {
            if (!m_bVideoReady) {
                return 0;
            }

            var pctBuffer:int = 0;
            if (m_NetStream.bufferTime > 0.0)
                pctBuffer = Math.min(Math.round((m_NetStream.bufferLength / m_NetStream.bufferTime) * 100), 100);
            return pctBuffer;
        }

        public function GetLength():Number {
            return m_nLengthMs / 1000.0;
        }

        public function GetLengthMs():int {
            return m_nLengthMs;
        }

        public function GetTime():Number {
            if (!m_bVideoReady)
                return 0.0;
            return m_NetStream.time;
        }

        public function GetTimeMs():int {
            return Math.round(GetTime() * 1000);
        }

        public function Play():void {
            if (m_bVideoReady) {
                m_NetStream.resume();
                m_bIsPlaying = true;
            }
        }

        public function Pause():void {
            if (m_bVideoReady) {
                m_NetStream.pause();
                m_bIsPlaying = false;
            }
        }

        public function Stop():void {
            if (m_bVideoReady) {
                m_NetStream.pause();
                m_bIsPlaying = false;
            }
        }

        public function Seek(fTimeSec:Number):void {
            if (m_bVideoReady) {
                // Bugfix 5157: Multiple seeking to the end must be suppressed 
                // (otherwise no replay is possible any more)
                if ((fTimeSec >= (m_nLengthMs / 1000.0)) && (fTimeSec == m_nLastSeekTimeSec))
                    return;

                // Correct Seeking is only possible during replay
                // Paused video: start temporary replay
                if (!m_bIsPlaying)
                    m_NetStream.resume();

                // Bugfix 5143: 
                if (isNaN(fTimeSec))
                    fTimeSec = 0.0;
                // Seeking should only be possible for correct timestamps
                if (fTimeSec < 0.0)
                    fTimeSec = 0.0;
                if (fTimeSec > (m_nLengthMs / 1000.0))
                    fTimeSec = (m_nLengthMs / 1000.0);

                if (m_bIsTrueStreaming)
                {
                    // Streaming case:
                    // Make a delayed seek
                    if (m_nSeekIntervalID == 0) {
                        // Different seeking behaviour depending on seeking forward or backwards. 
                        var currentTimeSec:Number = m_NetStream.time;
                        if (fTimeSec > currentTimeSec) {
                            // Seeking forward: first make a delay and then seek
                            m_nSeekIntervalID = setInterval(DelayedSeek, 50, fTimeSec, true)
                        } else {
                            // Seeking backwards: first seek and then make a delay
                            //m_NetStream.seek(fTimeSec);
                            //m_nSeekIntervalID = setInterval(DelayedSeek, 50, fTimeSec, false)
                            // Bugfix 5125: 
                            // No difference is necessary for seeking forward or backwards
                            // (Not clear, why this different handling worked better during the implementation time)
                            // Seeking backwards: first make a delay and 
                            m_nSeekIntervalID = setInterval(DelayedSeek, 50, fTimeSec, true)
                        }
                    }
                } else {
                    // Non-streaming case:
                    // Problems may occur when seeking to the end of the video
                    // (especially with Screengrabbing clips)
                    // "Hack": Jump 2 seconds before the end and after a short delay to the end
                    if (Math.round(fTimeSec * 1000.0) > (m_nLengthMs - 2000)) {
                        var fTempSec:Number = fTimeSec - 2.0;
                        if (fTempSec < 0.0)
                            fTempSec = 0.0;
                        m_NetStream.seek(fTempSec);
                        m_nSeekIntervalID = setInterval(DelayedSeek, 250, fTimeSec, true)
                    } else {
                        // Bugfix 5121: Check if the whole video is available 
                        // (Progressive Download)
                        var nPercentDownloaded:int = GetBufferPercentage();
                        // Determine the time according to the downlaod percentage
                        var relativeTimeSec:Number = (nPercentDownloaded / 100.0) * (m_nLengthMs / 1000.0);
                        if ((nPercentDownloaded < 100) && (fTimeSec > relativeTimeSec)) {
                            // Seek to the already loaded timestamp
                            m_NetStream.seek(relativeTimeSec);
                        } else {
                            // Direct seek
                            m_NetStream.seek(fTimeSec);
                        }
                    }
                }

                m_nLastSeekTimeSec = fTimeSec;

                // Paused video: stop temporary replay
                if (!m_bIsPlaying)
                    m_NetStream.pause();
            }
        }

        public function SeekMs(nTimeMs:int):void {
            Seek(nTimeMs / 1000.0);
        }

        public function SetPosition(x:Number, y:Number):void {
            // The "Video" object must be used for re-positioning
            // (Re-positioning the LecturnityVideoDisplay object has no effect)
            if (m_bVideoReady) {
                m_Video.x = x;
                m_Video.y = y;
            }
        }

        public function SetSize(width:Number, height:Number):void {
            // The "Video" object must be used for resizing
            // (Resizing the LecturnityVideoDisplay object has no effect)
            if (m_bVideoReady) {
                m_Video.width = width;
                m_Video.height = height;
            }
        }


        // Private methods
        private function SetNetConnection():void {
            // Streaming requires AMF0 (not AMF3)
            NetConnection.defaultObjectEncoding = flash.net.ObjectEncoding.AMF0;
            // Create a new Net Connection
            m_NetConnection = new NetConnection();
            m_NetConnection.addEventListener(NetStatusEvent.NET_STATUS, NetStatusHandler);
            m_NetConnection.addEventListener(SecurityErrorEvent.SECURITY_ERROR, SecurityErrorHandler);
            m_NetConnection.addEventListener(IOErrorEvent.IO_ERROR, IoErrorHandler);
            m_NetConnection.addEventListener(AsyncErrorEvent.ASYNC_ERROR, AsyncErrorHandler);
            if (m_strStreamUrl.length > 0) {
                if (m_strStreamUrl != "null") {
                    m_bIsConnectingToServer = true;
                    m_NetConnection.connect(m_strStreamUrl);
                    m_bIsTrueStreaming = true;
                } else {
                    m_NetConnection.connect(null);
                    m_bIsTrueStreaming = false;
                }
            } else {
                m_NetConnection.connect(null);
                m_bIsTrueStreaming = false;
            }
        }

        private function ConnectStream():void {
            // Create a metadata event handling object for the NetStream
            var objTempClient:Object = new Object();
            objTempClient.onMetaData = MetaDataHandler;
            // Create the NetStream
            m_NetStream = new NetStream(m_NetConnection);
            m_NetStream.addEventListener(NetStatusEvent.NET_STATUS, NetStatusHandler);
            m_NetStream.addEventListener(AsyncErrorEvent.ASYNC_ERROR, AsyncErrorHandler);
            m_NetStream.client = objTempClient;
            m_NetStream.bufferTime = m_nStartBufferLength;
            if (m_strStreamUrl.length > 0)
                m_NetStream.play(m_strFlvSource); // Streaming case: no baseUrl required 
            else
                m_NetStream.play(m_strBaseUrl + m_strFlvSource);
            m_NetStream.seek(0);
            m_NetStream.pause();
            // Create a "Video" object
            m_Video = new Video();
            m_Video.width = m_nVideoWidth;
            m_Video.height = m_nVideoHeight;
            m_Video.smoothing = true;
            m_Video.attachNetStream(m_NetStream);
            this.width = m_nVideoWidth;
            this.height = m_nVideoHeight;
            this.addChild(m_Video);
            m_bVideoReady = true;
        }

        private function DelayedSeek(fTimeSec:Number, bDoSeek:Boolean):void {
            clearInterval(m_nSeekIntervalID);
            m_nSeekIntervalID = 0;
            if (bDoSeek)
                m_NetStream.seek(fTimeSec);

            // Paused video: stop temporary replay
            if (!m_bIsPlaying)
                m_NetStream.pause();
        }

        private function DelayedCheckNetStreamBuffer():void {
            m_nBufferingIndex++;
            // Create the sum of the buffer over a period of 40x125 msec (= 5 sec)
            if (m_nBufferingIndex < 40) {
                m_fBufferingSum += m_NetStream.bufferLength;
            } else {
                if (m_bNetConnectionSuccessful && m_bIsPlaying && m_fBufferingSum <= 0) {
                    // A/V stream should play, but nothing is buffered 
                    // --> A/V stream not found
                    Stop();
                    trace("A/V stream not found: " + m_strFlvSource);
                    //mx.controls.Alert.show("A/V stream not found: " + m_strFlvSource);
                    mx.controls.Alert.show(resourceManager.getString('localization', 'FlvNotFoundError') + m_strFlvSource);
                }
                m_nBufferingIndex = 0;
                m_fBufferingSum = 0.0;
                clearInterval(m_nBufferIntervalID);
                m_nBufferIntervalID = 0;
            }
        }

        // Error Handlers
        private function NetStatusHandler(event:NetStatusEvent):void {
            //mx.controls.Alert.show("NetStatusHandler Event: " + event.info.code);
            switch (event.info.code) {
                case "NetConnection.Connect.Failed":
                    trace("Failed to connect server: " + m_strStreamUrl);
                    m_bIsConnectingToServer = false;
                    if (!Globals.g_bNetConnectionFailed) {
                        mx.controls.Alert.show(resourceManager.getString('localization', 'NetConnectionFailedError') + m_strStreamUrl);
                        Globals.g_bNetConnectionFailed = true;
                    }
                    break;
                case "NetConnection.Connect.Rejected":
                    trace("Server connection rejected: " + m_strStreamUrl);
                    m_bIsConnectingToServer = false;
                    if (!Globals.g_bNetConnectionFailed) {
                        mx.controls.Alert.show(resourceManager.getString('localization', 'NetConnectionFailedError') + m_strStreamUrl);
                        Globals.g_bNetConnectionFailed = true;
                    }
                    break;
                case "NetConnection.Connect.Success":
                    trace("Successfully connected to server: " + m_strStreamUrl);
                    m_bIsConnectingToServer = false;
                    //mx.controls.Alert.show("Successfully connected to server: " + m_strStreamUrl);
                    if (!m_bNetConnectionSuccessful)
                        ConnectStream();
                    m_bNetConnectionSuccessful = true;
                    break;
                case "NetStream.Play.StreamNotFound":
                    trace("Unable to locate audio/video: " + m_strFlvSource);
                    mx.controls.Alert.show(resourceManager.getString('localization', 'FlvNotFoundError') + m_strFlvSource);
                    break;
                ///*
                case "NetStream.Play.Start":
                    trace("Replay started");
                    // "NetStream.Play.StreamNotFound" seems not to work
                    // --> use this hack: Check the Buffer after a delay
                    m_nBufferingIndex = 0;
                    m_nBufferIntervalID = setInterval(DelayedCheckNetStreamBuffer, 125);
                    break;
                case "NetStream.Play.Stop":
                    trace("Replay stopped");
                    break;
                case "NetStream.Play.Complete":
                    trace("Replay complete");
                    //mx.controls.Alert.show("Replay complete");
                    break;
                //*/
                // "Dual-Threshold Buffering": Change the buffer time dynamically
                case "NetStream.Buffer.Empty":
                    trace("Buffer empty");
                    m_NetStream.bufferTime = m_nStartBufferLength;
                    break;
                case "NetStream.Buffer.Full":
                    trace("Buffer full");
                    m_NetStream.bufferTime = m_nExpandedBufferLength;
                    break;
                // NetStatusEvents which can occur but do not need special handling
                case "NetStream.Buffer.Flush":
                    trace("NetStatusHandler Event: " + event.info.code);
                    break;
                case "NetStream.Seek.Notify":
                    trace("NetStatusHandler Event: " + event.info.code);
                    break;
                case "NetStream.Play.Reset":
                    trace("NetStatusHandler Event: " + event.info.code);
                    break;
                case "NetStream.Pause.Notify":
                    trace("NetStatusHandler Event: " + event.info.code);
                    break;
                case "NetStream.Unpause.Notify":
                    trace("NetStatusHandler Event: " + event.info.code);
                    break;
                default:
                    trace("Unexpected NetStatusHandler Event: " + event.info.code);
                    //mx.controls.Alert.show("Unexpected NetStatusHandler Event: " + event.info.code);
            }
        }

        private function SecurityErrorHandler(event:SecurityErrorEvent):void {
            trace("SecurityErrorHandler: " + event);
            mx.controls.Alert.show(resourceManager.getString('localization', 'FlvSecurityError') + event);
        }

        private function IoErrorHandler(event:AsyncErrorEvent):void {
            // Not handled so far
            trace("IoErrorHandler: " + event);
        }

        private function AsyncErrorHandler(event:AsyncErrorEvent):void {
            // Not handled so far
            trace("AsyncErrorHandler: " + event);
        }

        // Metadata Handler
        private function MetaDataHandler(obj:Object):void {
            // Check Metadata for corrupt FLV with duration 0 (zero) and file size > 0
            // (Criteria "duration 0" is not enough - there are audio FLV with 
            // "duration 0" and "filesize 0" which can be replayed perfectly)
            var nDuration:int = -1;
            var nFileSize:int = -1;
            for (var x:String in obj) {
                trace("METADATA "+ x + " is " + obj[x]);
                //mx.controls.Alert.show("METADATA "+ x + " is " + obj[x] + "   (" + m_strFlvSource + ")");

                if (x == "duration") {
                    nDuration = int(obj[x]);
                }
                if (x == "filesize") {
                    nFileSize = int(obj[x]);
                }
            }
            // File size of corrupt FLVs seem to be always 307 bytes
            // (but we use a range of [0-500] for the check)
            if ((nDuration == 0) && (nFileSize > 0) && (nFileSize <= 500)) {
                trace("Empty audio/video found: " + m_strFlvSource);
                mx.controls.Alert.show(resourceManager.getString('localization', 'FlvEmptyError') + m_strFlvSource);
            }
        }

        // Misc. functions
        private function CheckFlvSuffix(flvName:String):String {
            var newFlvName:String = new String (flvName);
            if (m_bStreamStripFlvSuffix != "false") {
                var nIdx:int = newFlvName.indexOf(".flv");
                if (!m_bIsTrueStreaming) {
                    if (nIdx == -1) {
                        newFlvName = newFlvName + ".flv";
                        return (newFlvName);
                    }
                } else if (-1 < nIdx) {
                    return (newFlvName.substring(0, nIdx));
                }
            } else {
                return (newFlvName);
            }
            return (newFlvName);
        }

    }
}