package utils.iconcombobox
{
import flash.display.DisplayObject;
import mx.controls.ComboBox;
import mx.core.IFlexDisplayObject;
import mx.core.UIComponent;

public class IconComboBox extends ComboBox
{
    public function IconComboBox() {
        super();
    }

    private var iconHolder:UIComponent;

    override protected function createChildren():void {
        super.createChildren();

        iconHolder = new UIComponent();
        addChild(iconHolder);
    }

    override protected function measure():void {
        super.measure();
        if (iterator) {
            var iconClass:Class = document[iterator.current.icon];
            var icon:IFlexDisplayObject = new iconClass() as IFlexDisplayObject;
            while (iconHolder.numChildren > 0)
                iconHolder.removeChildAt(0);
            iconHolder.addChild(DisplayObject(icon));
            measuredWidth += icon.measuredWidth;
            measuredHeight = Math.max(measuredHeight, icon.measuredHeight + borderMetrics.top + borderMetrics.bottom);
        }
    }

    override protected function updateDisplayList(unscaledWidth:Number,
                                                  unscaledHeight:Number):void {
        super.updateDisplayList(unscaledWidth, unscaledHeight);

        var iconClass:Class = document[selectedItem.icon];
        var icon:IFlexDisplayObject = new iconClass() as IFlexDisplayObject;
        while (iconHolder.numChildren > 0)
            iconHolder.removeChildAt(0);
        iconHolder.addChild(DisplayObject(icon));
        iconHolder.y = (unscaledHeight - icon.measuredHeight) / 2;
        iconHolder.x = borderMetrics.left;
        textInput.x = iconHolder.x + icon.measuredWidth;
        textInput.setActualSize(textInput.width - icon.measuredWidth, textInput.height);
    } 
}

}