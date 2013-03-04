package
{
	import flash.events.Event;

	public class LanguageEvent extends Event
	{
		public var selectedLanguage:int;
		
		public function LanguageEvent(type:String, selected:int, bubbles:Boolean=false, cancelable:Boolean=false)
		{
			this.selectedLanguage = selected;
			super(type, bubbles, cancelable);
		}
		
	}
}