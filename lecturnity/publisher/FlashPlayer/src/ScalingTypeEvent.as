package
{
	import flash.events.Event;

	public class ScalingTypeEvent extends Event
	{
		public var selectedScalingType:int;
		
		public function ScalingTypeEvent(type:String, selected:int, bubbles:Boolean=false, cancelable:Boolean=false)
		{
			this.selectedScalingType = selected;
			super(type, bubbles, cancelable);
		}
		
	}
}