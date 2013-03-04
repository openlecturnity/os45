package com.adobe.ac
{
	public class Observe extends Observer
	{
		private var _handler : Function;
		private var _source : Object;
 		
		override public function get handler() : Function
		{
			return _handler;
		}
 	
		public function set handler( value : Function ) : void
		{
			_handler = value;
			if( value != null )
			{
				isHandlerInitialized = true;
				if( isHandlerInitialized && isSourceInitialized )
				{
					callHandler();
				}
			}
		}
  
		override public function get source() : Object
		{
			return _source;
		}
 	
		public function set source( value : Object ) : void
		{	
			_source = value; 
			isSourceInitialized = true;    	
			if( isHandlerInitialized && isSourceInitialized )
			{
				callHandler();
			}
		}
	}
}