var g_imgDir = '';

function button_getButton(id)
{
  if (isNS4)
    return eval("document." + id);
  else
    return document.getElementById(id);
}

function button_enableButton(id, bEnable)
{
  if (bEnable)
  {
    eval('g_' + id + '_isEnabled = true;');
    if (button_getButton(id))
    {
      button_getButton(id).src = eval('g_' + id + eval('g_' + id + '_state') + '_imgNormal.src');
    }
  }
  else
  {
    eval('g_' + id + '_isEnabled = false;');
    if (button_getButton(id))
    {
      button_getButton(id).src = eval('g_' + id + eval('g_' + id + '_state') + '_imgDisabled.src');
    }
  }
}

function button_isEnabled(id)
{
  return eval('g_' + id + '_isEnabled');
}

function button_setButtonState(id, state)
{
  eval('g_' + id + '_state = ' + state + ';');
  button_enableButton(id, button_isEnabled(id));
}

function button_getButtonState(id)
{
  return eval('g_' + id + '_state');
}

function button_setImageDir(dir)
{
  g_imgDir = dir;
}

function button_prepareButton(id, clickEvent, 
  imgSrcNormal, imgSrcMouseOver, imgSrcMouseDown, imgSrcDisabled, bEnable, title)
{
  button_prepareInteractionButton(id, title, bEnable, 
    clickEvent, "", "", "", "", 
    "", imgSrcNormal, imgSrcMouseOver, imgSrcMouseDown, imgSrcDisabled);
}

function button_prepareInteractionArea(id, title, bEnable, 
  clickEvent, mouseOverEvent, mouseOutEvent, mouseDownEvent, mouseUpEvent)
{
  button_prepareInteractionButton(id, title, bEnable, 
    clickEvent, mouseOverEvent, mouseOutEvent, mouseDownEvent, mouseUpEvent, 
    "", "", "", "", "");
}

function button_prepareInteractionButton(id, title, bEnable, 
  clickEvent, mouseOverEvent, mouseOutEvent, mouseDownEvent, mouseUpEvent, 
  dirImg, imgSrcNormal, imgSrcMouseOver, imgSrcMouseDown, imgSrcDisabled)
{
  var imgDir = (dirImg != "") ? dirImg : g_imgDir;

  document.writeln('<script type="text/javascript" language="JavaScript">');
  document.writeln('');
  document.writeln('  var g_' + id + '_title = "' + title + '";');
  if (bEnable)
    document.writeln('  var g_' + id + '_isEnabled = true;');
  else
    document.writeln('  var g_' + id + '_isEnabled = false;');
  // "Normal" button: 'state' is "0" by default
  document.writeln('  var g_' + id + '_state = 0;');
  // Define button images
  document.writeln('  var g_' + id + '0_imgNormal = new Image();');
  if (imgSrcNormal != "")
    document.writeln('  g_' + id + '0_imgNormal.src = "' + imgDir + imgSrcNormal + '";');
  else
    document.writeln('  g_' + id + '0_imgNormal.src = "' + g_imgDir + 'trans.gif' + '";');
  document.writeln('  var g_' + id + '0_imgMouseOver = new Image();');
  if (imgSrcMouseOver != "")
    document.writeln('  g_' + id + '0_imgMouseOver.src = "' + imgDir + imgSrcMouseOver + '";');
  else
    document.writeln('  g_' + id + '0_imgMouseOver.src = "' + g_imgDir + 'trans.gif' + '";');
  document.writeln('  var g_' + id + '0_imgMouseDown = new Image();');
  if (imgSrcMouseDown != "")
    document.writeln('  g_' + id + '0_imgMouseDown.src = "' + imgDir + imgSrcMouseDown + '";');
  else
    document.writeln('  g_' + id + '0_imgMouseDown.src = "' + g_imgDir + 'trans.gif' + '";');
  document.writeln('  var g_' + id + '0_imgDisabled = new Image();');
  if (imgSrcDisabled != "")
    document.writeln('  g_' + id + '0_imgDisabled.src = "' + imgDir + imgSrcDisabled + '";');
  else
    document.writeln('  g_' + id + '0_imgDisabled.src = "' + g_imgDir + 'trans.gif' + '";');
  document.writeln('  ');
  // Define button actions
  document.writeln('  function btn_' + id + '_onClick()');
  document.writeln('  {');
  document.writeln('    if (g_' + id + '_isEnabled)');
  if (clickEvent != "")
  	 document.writeln('      ' + clickEvent + ((clickEvent.charAt(clickEvent.length-1) == ")") ? ';' : '();'));
  else
  	 document.writeln('      ;');
  document.writeln('  }');
  document.writeln('  ');
  document.writeln('  function btn_' + id + '_mouseOver()');
  document.writeln('  {');
  document.writeln('    if (g_' + id + '_isEnabled)');
  document.writeln('    {');
  document.writeln('      var bt = button_getButton("' + id + '");');
  document.writeln('      if (bt.src == eval("g_' + id + '" + g_' + id + '_state + "_imgNormal.src"))');
  document.writeln('        bt.src = eval("g_' + id + '" + g_' + id + '_state + "_imgMouseOver.src");');
  if (mouseOverEvent != "")
  	 document.writeln('      ' + mouseOverEvent + ((mouseOverEvent.charAt(mouseOverEvent.length-1) == ")") ? ';' : '();'));
  document.writeln('    }');
  document.writeln('  }');
  document.writeln('  ');
  document.writeln('  function btn_' + id + '_mouseOut()');
  document.writeln('  {');
  document.writeln('    if (g_' + id + '_isEnabled)');
  document.writeln('    {');
  document.writeln('      var bt = button_getButton("' + id + '");');
  document.writeln('      bt.src = eval("g_' + id + '" + g_' + id + '_state + "_imgNormal.src");');
  if (mouseOutEvent != "")
  	 document.writeln('      ' + mouseOutEvent + ((mouseOutEvent.charAt(mouseOutEvent.length-1) == ")") ? ';' : '();'));
  document.writeln('    }');
  document.writeln('  }');
  document.writeln('  ');
  document.writeln('  function btn_' + id + '_mouseDown()');
  document.writeln('  {');
  document.writeln('    if (g_' + id + '_isEnabled)');
  document.writeln('    {');
  document.writeln('      var bt = button_getButton("' + id + '");');
  document.writeln('      bt.src = eval("g_' + id + '" + g_' + id + '_state + "_imgMouseDown.src");');
  if (mouseDownEvent != "")
  	 document.writeln('      ' + mouseDownEvent + ((mouseDownEvent.charAt(mouseDownEvent.length-1) == ")") ? ';' : '();'));
  document.writeln('    }');
  document.writeln('  }');
  document.writeln('  ');
  document.writeln('  function btn_' + id + '_mouseUp()');
  document.writeln('  {');
  document.writeln('    if (g_' + id + '_isEnabled)');
  document.writeln('    {');
  document.writeln('      var bt = button_getButton("' + id + '");');
  document.writeln('      bt.src = eval("g_' + id + '" + g_' + id + '_state + "_imgMouseOver.src");');
  if (mouseUpEvent != "")
  	 document.writeln('      ' + mouseUpEvent + ((mouseUpEvent.charAt(mouseUpEvent.length-1) == ")") ? ';' : '();'));
  document.writeln('    }');
  document.writeln('  }');
  
  document.writeln('');
  document.writeln('</script>');
}

function button_addButtonState(id, state, imgSrcNormal, imgSrcMouseOver, imgSrcMouseDown, imgSrcDisabled)
{
  // 'state' begins at "1" because "0" is the default state
  document.writeln('<script type="text/javascript" language="JavaScript">');
  document.writeln('');
  document.writeln('  var g_' + id + state + '_isEnabled = true;');
  document.writeln('  var g_' + id + state + '_state = 0;');
  document.writeln('  var g_' + id + state + '_imgNormal = new Image();');
  document.writeln('  g_' + id + state + '_imgNormal.src = "' + g_imgDir + imgSrcNormal + '";');
  document.writeln('  var g_' + id + state + '_imgMouseOver = new Image();');
  document.writeln('  g_' + id + state + '_imgMouseOver.src = "' + g_imgDir + imgSrcMouseOver + '";');
  document.writeln('  var g_' + id + state + '_imgMouseDown = new Image();');
  document.writeln('  g_' + id + state + '_imgMouseDown.src = "' + g_imgDir + imgSrcMouseDown + '";');
  document.writeln('  var g_' + id + state + '_imgDisabled = new Image();');
  if (imgSrcDisabled != "")
    document.writeln('  g_' + id + state + '_imgDisabled.src = "' + g_imgDir + imgSrcDisabled + '";');
  else
    document.writeln('  g_' + id + state + '_imgDisabled.src = "' + g_imgDir + 'trans.gif' + '";');
  document.writeln('');
  document.writeln('</script>');
}

function button_outputButton(id, bW, bH)
{
  var imgSrc = '';
  if (eval('g_' + id + '_isEnabled'))
    imgSrc = eval('g_' + id + '0_imgNormal.src');
  else
    imgSrc = eval('g_' + id + '0_imgDisabled.src');
  var imgTitle = eval('g_' + id + '_title');

  if (isNS4)
    document.write('<a href="javascript: btn_' + id + '_onClick();" onMouseOver="btn_' + id + '_mouseOver();" onMouseOut="btn_' + id + '_mouseOut();" onMouseDown="btn_' + id + '_mouseDown();" onMouseUp="btn_' + id + '_mouseUp();"><img name="' + id + '" id="' + id + '" src="' + imgSrc + '" border="0" width="' + bW + '" height="' + bH + '" title="' + imgTitle + '" alt="' + imgTitle + '"></a>');
  else
    document.write('<img id="' + id + '" src="' + imgSrc + '" border="0" width="' + bW + '" height="' + bH + '" onClick="javascript: btn_' + id + '_onClick();" onMouseOver="btn_' + id + '_mouseOver();" onMouseOut="btn_' + id + '_mouseOut();" onMouseDown="btn_' + id + '_mouseDown();" onMouseUp="btn_' + id + '_mouseUp();" title="' + imgTitle + '" alt="' + imgTitle + '">');
}

function button_outputInteractionArea(id, x, y, w, h, zIndex, bUseHandCursor)
{
  var imgSrc = '' + g_imgDir + 'trans.gif';

  var imgTitle = eval('g_' + id + '_title');
  zIndex += (g_slideEntries.length + 10);

  if (bUseHandCursor)
    document.write('<div id="' + id + 'Layer" style="position:absolute;left:' + x + 'px;top:' + y + 'px;z-index:' + zIndex + ';visibility:hidden;"><a href="javascript: btn_' + id + '_onClick();" onMouseOver="btn_' + id + '_mouseOver();" onMouseOut="btn_' + id + '_mouseOut();" onMouseDown="btn_' + id + '_mouseDown();" onMouseUp="btn_' + id + '_mouseUp();"><img name="' + id + '" id="' + id + '" src="' + imgSrc + '" border="0" width="' + w + '" height="' + h + '" title="' + imgTitle + '" alt="' + imgTitle + '"></a></div>');
  else
    document.write('<div id="' + id + 'Layer" style="position:absolute;left:' + x + 'px;top:' + y + 'px;z-index:' + zIndex + ';visibility:hidden;"><img id="' + id + '" src="' + imgSrc + '" border="0" width="' + w + '" height="' + h + '" onClick="javascript: btn_' + id + '_onClick();" onMouseOver="btn_' + id + '_mouseOver();" onMouseOut="btn_' + id + '_mouseOut();" onMouseDown="btn_' + id + '_mouseDown();" onMouseUp="btn_' + id + '_mouseUp();" title="' + imgTitle + '" alt="' + imgTitle + '"></div>');
}

function button_outputInteractionButton(id, x, y, w, h, zIndex, bUseHandCursor)
{
  var imgSrc = '';
  if (eval('g_' + id + '_isEnabled'))
    imgSrc = eval('g_' + id + '0_imgNormal.src');
  else
    imgSrc = eval('g_' + id + '0_imgDisabled.src');

  var imgTitle = eval('g_' + id + '_title');
  zIndex += (g_slideEntries.length + 10);

  if (bUseHandCursor || isNS4)
    document.write('<div id="' + id + 'Layer" style="position:absolute;left:' + x + 'px;top:' + y + 'px;z-index:' + zIndex + ';visibility:hidden;"><a href="javascript: btn_' + id + '_onClick();" onMouseOver="btn_' + id + '_mouseOver();" onMouseOut="btn_' + id + '_mouseOut();" onMouseDown="btn_' + id + '_mouseDown();" onMouseUp="btn_' + id + '_mouseUp();"><img name="' + id + '" id="' + id + '" src="' + imgSrc + '" border="0" width="' + w + '" height="' + h + '" title="' + imgTitle + '" alt="' + imgTitle + '"></a></div>');
  else
    document.write('<div id="' + id + 'Layer" style="position:absolute;left:' + x + 'px;top:' + y + 'px;z-index:' + zIndex + ';visibility:hidden;"><img id="' + id + '" src="' + imgSrc + '" border="0" width="' + w + '" height="' + h + '" onClick="javascript: btn_' + id + '_onClick();" onMouseOver="btn_' + id + '_mouseOver();" onMouseOut="btn_' + id + '_mouseOut();" onMouseDown="btn_' + id + '_mouseDown();" onMouseUp="btn_' + id + '_mouseUp();" title="' + imgTitle + '" alt="' + imgTitle + '"></div>');
}

function button_outputOkButton(id, x, y, w, h, zIndex, bUseHandCursor)
{
  var imgSrc = '';
  if (eval('g_' + id + '_isEnabled'))
    imgSrc = eval('g_' + id + '0_imgNormal.src');
  else
    imgSrc = eval('g_' + id + '0_imgDisabled.src');

  var imgTitle = eval('g_' + id + '_title');
  zIndex += (g_slideEntries.length + 10);

  if (bUseHandCursor || isNS4)
    document.write('<div id="' + id + 'Layer" style="position:relative;left:+' + x + 'px;top:+' + y + 'px;z-index:' + zIndex + ';visibility:visible;"><a href="javascript: btn_' + id + '_onClick();" onMouseOver="btn_' + id + '_mouseOver();" onMouseOut="btn_' + id + '_mouseOut();" onMouseDown="btn_' + id + '_mouseDown();" onMouseUp="btn_' + id + '_mouseUp();"><img name="' + id + '" id="' + id + '" src="' + imgSrc + '" border="0" width="' + w + '" height="' + h + '" title="' + imgTitle + '" alt="' + imgTitle + '"></a></div></td>');
  else
    document.write('<div id="' + id + 'Layer" style="position:relative;left:+' + x + 'px;top:+' + y + 'px;z-index:' + zIndex + ';visibility:visible;"><img id="' + id + '" src="' + imgSrc + '" border="0" width="' + w + '" height="' + h + '" onClick="javascript: btn_' + id + '_onClick();" onMouseOver="btn_' + id + '_mouseOver();" onMouseOut="btn_' + id + '_mouseOut();" onMouseDown="btn_' + id + '_mouseDown();" onMouseUp="btn_' + id + '_mouseUp();" title="' + imgTitle + '" alt="' + imgTitle + '"></div></td>');
}
