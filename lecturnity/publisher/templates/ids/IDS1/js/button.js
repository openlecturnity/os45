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
      button_getButton(id).src = eval('g_' + id + eval('g_' + id + '_state') + '_img.src');
    }
  }
  else
  {
    eval('g_' + id + '_isEnabled = false;');
    if (button_getButton(id))
    {
      button_getButton(id).src = eval('g_' + id + eval('g_' + id + '_state') + '_imgDis.src');
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

function button_prepareButton(id, clickEvent, imgSrc, imgMoSrc, imgPrSrc, imgDisSrc, bEnable, title)
{
  document.writeln('<script type="text/javascript" language="JavaScript">');
  document.writeln('var g_' + id + '_title = "' + title + '";');
  if (bEnable)
    document.writeln('var g_' + id + '_isEnabled = true;');
  else
    document.writeln('var g_' + id + '_isEnabled = false;');
  document.writeln('var g_' + id + '_state = 0;');
  document.writeln('var g_' + id + '0_img = new Image();');
  document.writeln('g_' + id + '0_img.src = "' + g_imgDir + imgSrc + '";');
  document.writeln('var g_' + id + '0_imgMo = new Image();');
  document.writeln('g_' + id + '0_imgMo.src = "' + g_imgDir + imgMoSrc + '";');
  document.writeln('var g_' + id + '0_imgPr = new Image();');
  document.writeln('g_' + id + '0_imgPr.src = "' + g_imgDir + imgPrSrc + '";');
  if (imgDisSrc != "")
  {
    document.writeln('  var g_' + id + '0_imgDis = new Image();');
    document.writeln('  g_' + id + '0_imgDis.src = "' + g_imgDir + imgDisSrc + '";');
  }
  document.writeln('');
  document.writeln('function button' + id + 'mouseOver()');
  document.writeln('{');
  document.writeln('  if (g_' + id + '_isEnabled)');
  document.writeln('    {');
  document.writeln('      var bt = button_getButton("' + id + '");');
  document.writeln('      if (bt.src == eval("g_' + id + '" + g_' + id + '_state + "_img.src"))');
  document.writeln('        bt.src = eval("g_' + id + '" + g_' + id + '_state + "_imgMo.src")');
  //  document.writeln('      if (bt.src == g_' + id + '_img.src)');
  //  document.writeln('        bt.src = g_' + id + '_imgMo.src;');
  document.writeln('    }');
  document.writeln('}');
  document.writeln('');
  document.writeln('function button' + id + 'mouseOut()');
  document.writeln('{');
  document.writeln('  if (g_' + id + '_isEnabled)');
  document.writeln('    {');
  document.writeln('      var bt = button_getButton("' + id + '");');
  document.writeln('      bt.src = eval("g_' + id + '" + g_' + id + '_state + "_img.src");');
  document.writeln('    }');
  document.writeln('}');
  document.writeln('');
  document.writeln('function button' + id + 'mouseDown()');
  document.writeln('{');
  document.writeln('  if (g_' + id + '_isEnabled)');
  document.writeln('    {');
  document.writeln('      var bt = button_getButton("' + id + '");');
  document.writeln('      bt.src = eval("g_' + id + '" + g_' + id + '_state + "_imgPr.src");');
  document.writeln('    }');
  document.writeln('}');
  document.writeln('');
  document.writeln('function button' + id + 'mouseUp()');
  document.writeln('{');
  document.writeln('  if (g_' + id + '_isEnabled)');
  document.writeln('    {');
  document.writeln('      var bt = button_getButton("' + id + '");');
  document.writeln('      bt.src = eval("g_' + id + '" + g_' + id + '_state + "_imgMo.src");');
  document.writeln('    }');
  document.writeln('}');
  document.writeln('');
  document.writeln('function button' + id + 'onClick()');
  document.writeln('{');
  document.writeln('  if (g_' + id + '_isEnabled)');
  document.writeln('    ' + clickEvent + '();');
  document.writeln('}');
  document.writeln('</script>');
}

function button_addButtonState(id, state, imgSrc, imgMoSrc, imgPrSrc, imgDisSrc)
{
  document.writeln('<script type="text/javascript" language="JavaScript">');
  document.writeln('var g_' + id + state + '_isEnabled = true;');
  document.writeln('var g_' + id + state + '_state = 0;');
  document.writeln('var g_' + id + state + '_img = new Image();');
  document.writeln('g_' + id + state + '_img.src = "' + g_imgDir + imgSrc + '";');
  document.writeln('var g_' + id + state + '_imgMo = new Image();');
  document.writeln('g_' + id + state + '_imgMo.src = "' + g_imgDir + imgMoSrc + '";');
  document.writeln('var g_' + id + state + '_imgPr = new Image();');
  document.writeln('g_' + id + state + '_imgPr.src = "' + g_imgDir + imgPrSrc + '";');
  if (imgDisSrc != "")
    {
      document.writeln('  var g_' + id + state + '_imgDis = new Image();');
      document.writeln('  g_' + id + state + '_imgDis.src = "' + g_imgDir + imgDisSrc + '";');
    }
  document.writeln('');
  document.writeln('</script>');
}

function button_outputButton(id, bW, bH)
{
  var imgSrc = '';
  if (eval('g_' + id + '_isEnabled'))
    imgSrc = eval('g_' + id + '0_img.src');
  else
    imgSrc = eval('g_' + id + '0_imgDis.src');
  var imgTitle = eval('g_' + id + '_title');
  if (isNS4)
    document.write('<a href="javascript: button' + id + 'onClick();" onMouseOver="button' + id + 'mouseOver();" onMouseOut="button' + id + 'mouseOut();" onMouseDown="button' + id + 'mouseDown();" onMouseUp="button' + id + 'mouseUp();"><img name="' + id + '" id="' + id + '" src="' + imgSrc + '" border="0" width="' + bW + '" height="' + bH + '" alt="' + imgTitle + '"></a>');
  else
    document.write('<img id="' + id + '" src="' + imgSrc + '" border="0" width="' + bW + '" height="' + bH + '" onClick="javascript: button' + id + 'onClick();" onMouseOver="button' + id + 'mouseOver();" onMouseOut="button' + id + 'mouseOut();" onMouseDown="button' + id + 'mouseDown();" onMouseUp="button' + id + 'mouseUp();" alt="' + imgTitle + '">');
}
