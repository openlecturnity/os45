package com.fxcomponents.controls
{
import com.fxcomponents.controls.fxslider.FXSliderThumb;
import com.fxcomponents.skins.SliderThumbSkin;
import com.fxcomponents.skins.SliderTrackSkin;

import flash.display.DisplayObject;
import flash.display.Graphics;
import flash.display.Sprite;
import flash.events.MouseEvent;
import flash.geom.Point;

import mx.core.IFlexDisplayObject;
import mx.core.UIComponent;
import mx.core.mx_internal;
import mx.effects.Move;
import mx.events.SliderEvent;
import mx.events.TweenEvent;
import mx.styles.ISimpleStyleClient;
import mx.styles.StyleProxy;

import org.osflash.thunderbolt.Logger;

use namespace mx_internal;

[Event(name="change", type="mx.events.SliderEvent")]
[Event(name="thumbDrag", type="mx.events.SliderEvent")]
[Event(name="thumbPress", type="mx.events.SliderEvent")]
[Event(name="thumbRelease", type="mx.events.SliderEvent")]

[Style(name="thumbColor", type="uint", format="Color", inherit="no")]
[Style(name="thumbOutlineColor", type="uint", format="Color", inherit="no")]
[Style(name="thumbSkin", type="Class", inherit="no", states="up, over, down, disabled")]
[Style(name="thumbUpSkin", type="Class", inherit="no")]
[Style(name="thumbOverSkin", type="Class", inherit="no")]
[Style(name="thumbDownSkin", type="Class", inherit="no")]
[Style(name="thumbDisabledSkin", type="Class", inherit="no")]
[Style(name="trackSkin", type="Class", inherit="no")]
[Style(name="trackHighlightSkin", type="Class", inherit="no")]
[Style(name="trackColor", type="uint", format="Color", inherit="no")]

public class FXSlider extends UIComponent
{
	public function FXSlider()
	{
		super();
	}
	
	
	private var g:Graphics;
	private var e:SliderEvent;
	private var skinClass:Class;
	private var xOffset:Number;
	private var thumbIsPressed:Boolean = false;
	
	private var moveTween:Move;
	
	/** display objects */
	
	protected var track:IFlexDisplayObject;
	protected var highlight:IFlexDisplayObject;
	protected var bound:Sprite;
	protected var thumb:FXSliderThumb;
	
	/** style */
	
	private var thumbSkin:Class;
	private var thumbOverSkin:Class;
	private var thumbDownSkin:Class;
	private var trackSkin:Class;
	private var trackHighlightSkin:Class;
	private var trackColor:uint;
	
	/** properties */
	
	private var _maximum:Number = 100;
	
    public function get maximum():Number
    {
        return _maximum;
    }
	
	public var sliderThumbWidth:Number = NaN;
	public var sliderThumbHeight:Number = NaN;
	public var sliderThumbOverTrack:Boolean = false;
	public var sliderThumbUseCenterAsReferencePoint:Boolean = false;
	private var _sliderThumbReferenceY:Number = 0;

    public function set maximum(value:Number):void
    {
        _maximum = value;
        
        invalidateProperties();
        invalidateDisplayList();
    }
	
	private var _value:Number = 0;
	
	private var valueChanged:Boolean = false;
	
	public function set value(value:Number):void
	{
		_value = value;
		
		valueChanged = true;
		
		invalidateProperties();
		invalidateDisplayList();
	}
	
	public function get value():Number
	{
		// Edit by Lars
		var val:Number = 0;

		// If SliderThumb is within the track, value for left edge and right edge is calculated accordingly
		if(!sliderThumbOverTrack) {
			val = (thumb.x - thumb.width/2)/(unscaledWidth - thumb.width)*maximum;
		// If SliderThumb is over the track, but reference point (which means the skins middle) is subtracted from position
		} else if (sliderThumbUseCenterAsReferencePoint) {
			val = (thumb.x+(thumb.width/2))/unscaledWidth * maximum;
		// If SliderThumb is just over the track, calculation is very simple
		} else {
			val = (thumb.x/unscaledWidth) * maximum;
		}
		
		
		Logger.debug("value: "+val);
		return val;
	}
	
	/** */
	
	private function get boundMin():Number
	{
		//Edit by Lars
		var referenceX:Number = 0;
		if(sliderThumbUseCenterAsReferencePoint) referenceX = thumb.width/2;
		
		if(!sliderThumbOverTrack) return thumb.width/2-referenceX;
		else return 0-referenceX;
	}
	
	private function get boundMax():Number
	{
		var referenceX:Number = 0;
		if(sliderThumbUseCenterAsReferencePoint) referenceX = thumb.width/2;

		//Edit by Lars 
		if(!sliderThumbOverTrack) return Math.max(thumb.width/2-referenceX, bound.width - thumb.width/2 - referenceX) ;
		else return bound.width-referenceX;
	}
	
	/**
	 * Creates any child components of the component. For example, the
	 * ComboBox control contains a TextInput control and a Button control
	 * as child components.
	 */
	
	override protected function createChildren():void
	{
		super.createChildren();
		
		thumbSkin = getStyle("thumbSkin");
        if(!thumbSkin)
        	thumbSkin = com.fxcomponents.skins.SliderThumbSkin;
		
		thumbOverSkin = getStyle("thumbOverSkin");
		thumbDownSkin = getStyle("thumbDownSkin");
		
		trackSkin = getStyle("trackSkin");
        if(!trackSkin)
        	trackSkin = com.fxcomponents.skins.SliderTrackSkin;
		
		trackHighlightSkin = getStyle("trackHighlightSkin");
        if(!trackHighlightSkin)
        	trackHighlightSkin = com.fxcomponents.skins.SliderHighlightSkin;
		
		if(!getStyle("thumbSkin"))
			setStyle("thumbSkin", SliderThumbSkin);
		
		if(!getStyle("trackColor"))
			setStyle("trackColor", 0xaaaaaa);
			
		if(!getStyle("thumbColor"))
			setStyle("thumbColor", 0xcccccc);
			
		if(!getStyle("thumbOutlineColor"))
			setStyle("thumbOutlineColor", 0x555555);
		
		systemManager.addEventListener(MouseEvent.MOUSE_UP, onMouseUp);
		
		if (!track)
        {
            skinClass = trackSkin;
			
            track = new skinClass();

            if (track is ISimpleStyleClient)
                ISimpleStyleClient(track).styleName = this;

            addChild(DisplayObject(track)); 
        }
        
        if (!highlight)
        {
            skinClass = trackHighlightSkin;
			
            highlight = new skinClass();

            if (track is ISimpleStyleClient)
                ISimpleStyleClient(highlight).styleName = this;

            addChild(DisplayObject(highlight)); 
        }
		
		bound = new Sprite();
		addChild(bound);
		
		bound.addEventListener(MouseEvent.MOUSE_DOWN, bound_onMouseDown);
		
		thumb = new FXSliderThumb();
		//Edit by Lars 
		if(sliderThumbUseCenterAsReferencePoint) {
			if(!isNaN(sliderThumbHeight) && !isNaN(sliderThumbWidth)) {
				thumb.x = 0 - (sliderThumbWidth/2);
			} else {
				thumb.x = 0 - (thumb.width/2);
			}
		}

		
		addChild(thumb);
		
		thumb.addEventListener(MouseEvent.MOUSE_DOWN, onMouseDown);
		
		moveTween = new Move(thumb);
		
		moveTween.addEventListener(TweenEvent.TWEEN_UPDATE, onTweenUpdate);
		moveTween.addEventListener(TweenEvent.TWEEN_END, onTweenEnd);
		
		moveTween.duration = 300;
	}
	
	/**
	 * Commits any changes to component properties, either to make the 
	 * changes occur at the same time, or to ensure that properties are set in 
	 * a specific order.
	 */
	
	override protected function commitProperties():void
	{
		super.commitProperties();
		
		thumb.styleName = new StyleProxy(this, null);
        thumb.skinName = "thumbSkin";
        thumb.overSkinName = "thumbOverSkin";
        thumb.downSkinName = "thumbDownSkin";
	}
	
	// Created by Lars
	protected function calculateSliderThumbX():Number {
    	// Edit by Lars
		var referenceX:Number = 0;
		if(sliderThumbUseCenterAsReferencePoint) referenceX = thumb.width/2;
    	
    	// TODO: not sliderThumbTrack but with reference point
    	if(!sliderThumbOverTrack) return thumb.width/2 + Math.round((_value/_maximum)*(unscaledWidth - thumb.width));
    	else return Math.round((_value/_maximum*unscaledWidth)-referenceX);
	}
	
	/**
	 * Sizes and positions the children of the component on the screen based on 
	 * all previous property and style settings, and draws any skins or graphic 
	 * elements used by the component. The parent container for the component 
	 * determines the size of the component itself.
	 */
	
	override protected function updateDisplayList(unscaledWidth:Number, unscaledHeight:Number):void
    {
        super.updateDisplayList(unscaledWidth, unscaledHeight);
		
		if(valueChanged)
        {
        	thumb.x = calculateSliderThumbX();
        	valueChanged = false;
        }
		
		// draw
		
		var x:int;
        var y:int;
        var w:int;
        var h:int;
		
		// Edit by Lars
		if(!isNaN(sliderThumbHeight) && !isNaN(sliderThumbWidth)) {
			thumb.setActualSize(sliderThumbWidth, sliderThumbHeight);		
			thumb.y = (unscaledHeight - sliderThumbHeight) / 2
		} else {
	        w = Math.round(unscaledHeight/2)*2;
	        h = unscaledHeight;
	        thumb.setActualSize(w, h);
		}
		
        // position
        g = bound.graphics;
        
        g.clear();
        g.beginFill(0xffaaaa, 0);
        g.drawRect(0, 0, unscaledWidth, unscaledHeight);
        
        
        // size
        
        track.setActualSize(unscaledWidth, unscaledHeight);
		highlight.setActualSize(thumb.x+thumb.width/2, unscaledHeight);
		
		Logger.debug("Thumb.x: "+thumb.x+"; Thumb.width: "+thumb.width+";");
    }
    
    override protected function measure():void
	{
		super.measure();
		
		measuredWidth = 200;
		measuredHeight = 13;
	}
	
	/** event handlers */
	
	private function onMouseDown(event:MouseEvent):void
	{
		systemManager.addEventListener(MouseEvent.MOUSE_MOVE, onMouseMove, true);
		
		xOffset = event.localX;
		
		thumbIsPressed = true;
		
		e = new SliderEvent(SliderEvent.THUMB_PRESS);
        e.value = value;
        dispatchEvent(e);
	}
	
	private function onMouseUp(event:MouseEvent):void
	{
		systemManager.removeEventListener(MouseEvent.MOUSE_MOVE, onMouseMove, true);
		
		if(thumbIsPressed)
		{
			e = new SliderEvent(SliderEvent.THUMB_RELEASE);
	        e.value = value;
	        dispatchEvent(e);
	        
	        e = new SliderEvent(SliderEvent.CHANGE);
	        e.value = value;
	        dispatchEvent(e);
	        
	        thumbIsPressed = false;
		}
	}
	
	private function onMouseMove(event:MouseEvent):void
	{
		var pt:Point = new Point(event.stageX, event.stageY);
		pt = globalToLocal(pt);
			
		thumb.x = Math.min(Math.max(pt.x - xOffset, boundMin), boundMax);
		
		e = new SliderEvent(SliderEvent.THUMB_DRAG);
        e.value = value;
        dispatchEvent(e);
        
        invalidateDisplayList();
	}
	
	private function bound_onMouseDown(event:MouseEvent):void
	{
		moveTween.xTo = Math.min(Math.max(event.localX, boundMin), boundMax);
		moveTween.play()
	}
	
	private function onTweenUpdate(event:TweenEvent):void
	{
		invalidateDisplayList();
	}
	
	private function onTweenEnd(event:TweenEvent):void
	{
		e = new SliderEvent(SliderEvent.CHANGE);
        e.value = value;
        dispatchEvent(e);
	}
}
}