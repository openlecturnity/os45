// Global variables
var g_bFeedbackMessageVisible = false;
var g_nFeedbackIndex = -1;
var g_nFeedbackIndexTemp = -1;
var g_nFeedbackTestingIndex = -1;
var g_nQuestionnaireIndex = -1;
var g_bIsQuestionnaire = false;
var g_bIsQuestionnaireTemp = false;
var g_bAllQuestionsAnswered = false;
var g_bQuestionIsDeactivated = false;
var g_TimeoutId = null;
var g_FeedbackId = null;

var g_nTotalMinAchievablePoints = 0;
var g_nTotalMaxAchievablePoints = 0;
var g_nTotalPassAchievablePoints = 0;
var g_nTotalCurrentlyAchievePoints = 0;

var g_interactionType = "";
var g_questionFillIndex = 0;
var g_questionDragIndex = 0;


var g_ScopeStrSeparator = "<,>";
var g_MajorStrSeparator = "{,}";
var g_MinorStrSeparator = "[,]";

var g_Responses = "ABCDEFGHIJKLMNOPRS";





/* ------------------------------------------------------------------------
 * The "QuestionnaireEntry" object
 * ------------------------------------------------------------------------ */
function QuestionnaireEntry(nNumOfQuestions, nTotalPoints, nPassPoints, 
  nPageStart, nPageEnd, nStartMs, nEndMs)
{
  this.nNumOfQuestions = nNumOfQuestions;
  this.nTotalPoints = nTotalPoints;
  this.nPassPoints = nPassPoints;
  this.nPageStart = nPageStart;
  this.nPageEnd = nPageEnd;
  this.nStartMs = nStartMs;
  this.nEndMs = nEndMs;
  g_nTotalPassAchievablePoints += nPassPoints;
  g_nTotalMaxAchievablePoints += nTotalPoints;
}

/* ------------------------------------------------------------------------
 * The "QuestionEntry" object
 * ------------------------------------------------------------------------ */
function QuestionEntry(nQuestionnaireIndex, strId, nAchievablePoints, bSuccessfullyAnswered, bIsDeactivated, 
  nMaximumAttempts, nTakenAttempts, nTimeoutSec, nViewedSec, 
  nPage, nStartMs, nEndMs)
{
  this.nQuestionnaireIndex = nQuestionnaireIndex;
  this.strId = strId;
  this.nAchievablePoints = nAchievablePoints;
  this.bSuccessfullyAnswered = bSuccessfullyAnswered;
  this.bIsDeactivated = bIsDeactivated;
  this.nMaximumAttempts = nMaximumAttempts;
  this.nTakenAttempts = nTakenAttempts;
  this.nTimeoutSec = nTimeoutSec;
  this.nViewedSec = nViewedSec;
  this.nPage = nPage;
  this.nStartMs = nStartMs;
  this.nEndMs = nEndMs;
  
  this.nQuestionAddedLMS = "false";
}

/* ------------------------------------------------------------------------
 * The "FeedbackEntry" object
 * ------------------------------------------------------------------------ */
function FeedbackEntry(nX, nY, nWidth, nHeight, bHasQuestionnaire, nIdx, nType, strTextPassed, strText, 
  strAction, clrBorder, clrBackground, clrText, strFontFamily, nFontSizeHeader, nFontSize, nFontSizeTable, 
  strFontStyle, strFontWeight, nOkWidth, nOkHeight)
{
  this.nX = nX;
  this.nY = nY;
  this.nWidth = nWidth;
  this.nHeight = nHeight;
  this.bHasQuestionnaire = bHasQuestionnaire;
  this.nIdx = nIdx;
  this.nType = nType;
  this.strTextPassed = strTextPassed;
  this.strText = strText;
  this.strAction = strAction;
  this.clrBorder = clrBorder;
  this.clrBackground = clrBackground;
  this.clrText = clrText;
  this.strFontFamily = strFontFamily;
  this.nFontSizeHeader = nFontSizeHeader;
  this.nFontSize = nFontSize;
  this.nFontSizeTable = nFontSizeTable;
  this.strFontStyle = strFontStyle;
  this.strFontWeight = strFontWeight;
  this.nOkWidth = nOkWidth;
  this.nOkHeight = nOkHeight;
}

/* ------------------------------------------------------------------------
 * The "RadioDynamicEntry" object
 * ------------------------------------------------------------------------ */
function RadioDynamicEntry(nX, nY, nSize, bIsChecked, strId, nIdx, nPage, nStartMs, nEndMs)
{
  this.nX = nX;
  this.nY = nY;
  this.nSize = nSize;
  this.bIsChecked = bIsChecked;
  this.strId = strId;
  this.nIdx = nIdx;
  this.nPage = nPage;
  this.nStartMs = nStartMs;
  this.nEndMs = nEndMs;
}

/* ------------------------------------------------------------------------
 * The "TextFieldEntry" object
 * ------------------------------------------------------------------------ */
function TextFieldEntry(nX, nY, nSize, strText, strId, nIdx, nPage, nStartMs, nEndMs)
{
  this.nX = nX;
  this.nY = nY;
  this.nSize = nSize;
  this.strText = strText;
  this.strId = strId;
  this.nIdx = nIdx;
  this.nPage = nPage;
  this.nStartMs = nStartMs;
  this.nEndMs = nEndMs;
}

/* ------------------------------------------------------------------------
 * The "TargetPointEntry" object
 * ------------------------------------------------------------------------ */
function TargetPointEntry(nX, nY, nWidth, nHeight, nCenterX, nCenterY, nRadius, strObjectId, nPage, nStartMs, nEndMs)
{
  this.nX = nX;
  this.nY = nY;
  this.nWidth = nWidth;
  this.nHeight = nHeight;
  this.nCenterX = nCenterX;
  this.nCenterY = nCenterY;
  this.nRadius = nRadius;
  this.strObjectId = strObjectId;
  this.nPage = nPage;
  this.nStartMs = nStartMs;
  this.nEndMs = nEndMs;
}

/* ------------------------------------------------------------------------
 * The "ChangeTextEntry" object
 * ------------------------------------------------------------------------ */
function ChangeTextEntry(nX, nY, nType, strId, nPage, nStartMs, nEndMs)
{
  this.nX = nX;
  this.nY = nY;
  this.nType = nType;
  this.strId = strId;
  this.nPage = nPage;
  this.nStartMs = nStartMs;
  this.nEndMs = nEndMs;
}

/* ------------------------------------------------------------------------
 * The "QuestionTextEntry" object
 * ------------------------------------------------------------------------ */
function QuestionTextEntry(nX, nY, nType, strId, nPage, nStartMs, nEndMs)
{
  this.nX = nX;
  this.nY = nY;
  this.nType = nType;
  this.strId = strId;
  this.nPage = nPage;
  this.nStartMs = nStartMs;
  this.nEndMs = nEndMs;
}

/* ------------------------------------------------------------------------ */

function testingInit()
{
  g_TimeoutId = window.setInterval("checkForTimeout()", 1000)
}

//
// Button interactions
//

function testing_resetQuestion(pageNumber)
{
  resetDragAndDrop(pageNumber);
  resetMultipleChoice(pageNumber);
  resetFillInBlank(pageNumber);

  var qIdx = getQuestionIndex(pageNumber);
  if (qIdx >= 0)
    g_questionEntries[qIdx].bSuccessfullyAnswered = false;
  else
    alert("Error: corrupt testing data!");
}

function testing_submitAnswer(pageNumber)
{
  var qIdx = getQuestionIndex(pageNumber); 
  if (qIdx >= 0)
  {
    if(g_scormEnabled && g_scormLoaded && g_LMSResumeSupported)
    var rightNow = new Date();
    var qqIdx = g_questionEntries[qIdx].nQuestionnaireIndex;
    
    g_questionEntries[qIdx].nTakenAttempts++;
    updateTriesFromQuestionIndex(qIdx);
        
    var bIsCorrect = false;
    if (checkMultipleChoice(pageNumber) == true)
      bIsCorrect = true;
    if (checkDragAndDrop(pageNumber) == true)
      bIsCorrect = true;
    if (checkFillInBlank(pageNumber) == true)
      bIsCorrect = true;
   
    if(g_scormEnabled && g_scormLoaded && g_LMSResumeSupported)
      var interactionLearnerResponse = scutils_makeLearnerResponse(pageNumber, g_interactionType);
    if (bIsCorrect)
    {
      g_questionEntries[qIdx].bSuccessfullyAnswered = true;     
      g_nTotalCurrentlyAchievePoints += g_questionEntries[qIdx].nAchievablePoints;
      if(g_scormEnabled && g_scormLoaded && g_LMSResumeSupported)
      {
        scutils_setAchiveScore(g_nTotalCurrentlyAchievePoints); 
        scutils_setInteractionResult(g_questionEntries[qIdx].strId, "correct", interactionLearnerResponse, rightNow);
      }
      // Deactivate question
      g_bQuestionIsDeactivated = true;
      deactivateQuestion(g_questionEntries[qIdx].nPage);
      g_bAllQuestionsAnswered = areAllQuestionsAnswered(qqIdx);
      var fbIdx = getFeedbackIndexFromQuestionIndex(qIdx, FEEDBACK_TYPE_Q_CORRECT, false);
      if (fbIdx >= 0)
        testing_openFeedbackMessage(fbIdx, false);
    }
    else
    {
      g_questionEntries[qIdx].bSuccessfullyAnswered = false;
      if(g_scormEnabled && g_scormLoaded && g_LMSResumeSupported)
          scutils_setInteractionResult(g_questionEntries[qIdx].strId, "incorrect", interactionLearnerResponse, rightNow);
      if ((g_questionEntries[qIdx].nMaximumAttempts > 0) 
        && (g_questionEntries[qIdx].nTakenAttempts >= g_questionEntries[qIdx].nMaximumAttempts))
      {
        if (!g_questionEntries[qIdx].bIsDeactivated)
        {
          // Deactivate question
          g_bQuestionIsDeactivated = true;
          deactivateQuestion(g_questionEntries[qIdx].nPage);
          g_bAllQuestionsAnswered = areAllQuestionsAnswered(qqIdx);
          var fbIdx = getFeedbackIndexFromQuestionIndex(qIdx, FEEDBACK_TYPE_Q_WRONG, false);
          if (fbIdx >= 0)
            testing_openFeedbackMessage(fbIdx, false);
        }
      }
      else
      {
        g_bQuestionIsDeactivated = false;
        var fbIdx = getFeedbackIndexFromQuestionIndex(qIdx, FEEDBACK_TYPE_Q_REPEAT, false);
        if (fbIdx >= 0)
          testing_openFeedbackMessage(fbIdx, false);
      }
    }

    if (g_bAllQuestionsAnswered)
      submitTesting(qIdx);
  }
  else
    alert("Error: corrupt testing data!");
}

// Not used:
function testing_resetQuestionnaire(pageNumber)
{
  var qqIdx = getQuestionnaireIndex(pageNumber);
  if (qqIdx >= 0)
  {
    var qqPageStart = g_questionnaireEntries[qqIdx].nPageStart;
    var qqPageEnd = g_questionnaireEntries[qqIdx].nPageEnd;
    for (var page = qqPageStart; page <= qqPageEnd; ++page)
      testing_resetQuestion(page);
  }
}

// Not used:
function testing_submitTesting(pageNumber)
{
  var qqIdx = getQuestionnaireIndex(pageNumber);
  if (qqIdx >= 0)
  {
    var qqPageStart = g_questionnaireEntries[qqIdx].nPageStart;
    var qqPageEnd = g_questionnaireEntries[qqIdx].nPageEnd;
    for (var page = qqPageStart; page <= qqPageEnd; ++page)
      testing_submitAnswer(page);
  }
}

/* ------------------------------------------------------------------------ */

//
// Timeout, Deactivation
//

function checkForTimeout()
{
  // actualTimestamp: see slides.js
  g_sessionTime++;
  var qIdx = getQuestionIndexFromTimestamp(actualTimestamp);
  if (qIdx >= 0)
  {
    if ((g_questionEntries[qIdx].nMaximumAttempts >= 0)
      && (!g_questionEntries[qIdx].bIsDeactivated))
    {
      updateTriesFromQuestionIndex(qIdx);
    }

    if ((g_questionEntries[qIdx].nTimeoutSec > 0)
      && (!g_questionEntries[qIdx].bIsDeactivated))
    {
      g_questionEntries[qIdx].nViewedSec += 1;
      updateTimerFromQuestionIndex(qIdx);
      if (g_questionEntries[qIdx].nViewedSec > g_questionEntries[qIdx].nTimeoutSec)
      {
        // Pause replay (if necessary)
        if (g_wasPlaying)
          control_pause();
        // Deactivate question
        g_bQuestionIsDeactivated = true;
        deactivateQuestion(g_questionEntries[qIdx].nPage);
        var qqIdx = g_questionEntries[qIdx].nQuestionnaireIndex;
        g_bAllQuestionsAnswered = areAllQuestionsAnswered(qqIdx);
        var fbIdx = getFeedbackIndexFromQuestionIndex(qIdx, FEEDBACK_TYPE_Q_TIME, false);
        if (fbIdx >= 0)
          testing_openFeedbackMessage(fbIdx, false);

        if (g_bAllQuestionsAnswered)
          submitTesting(qIdx);
      }
    }
  }
  
}

function deactivateQuestion(pageNumber)
{
  var qIdx = getQuestionIndex(pageNumber);
  if (qIdx >= 0)
  {
    deactivateDragAndDrop(pageNumber);
    deactivateMultipleChoice(pageNumber);
    deactivateFillInBlank(pageNumber);
    deactivateTestingButtons(pageNumber);
    g_questionEntries[qIdx].bIsDeactivated = true;
  }
}

//
// Data interpretation
//

function resetDragAndDrop(pageNumber)
{
  for (var i = 0; i < g_targetPointEntries.length; ++i)
  {
    var pageNr = g_targetPointEntries[i].nPage;
    if (pageNr == pageNumber)
    {
      if (g_targetPointEntries[i].strObjectId != "")
      {
        var strId = "" + g_targetPointEntries[i].strObjectId + "Layer";
        var imgObject = document.getElementById(strId);
        imgObject.style.left = originalX[i];
        imgObject.style.top = originalY[i];
      }
    }
  }
}

function resetMultipleChoice(pageNumber)
{
  var radioGroup = eval("document.TestingForm.Radio" + pageNumber);
  if (radioGroup)
  {
    if (radioGroup.length)
    {
      for (var i = 0; i < radioGroup.length; ++i)
        radioGroup[i].checked = false;
    }
    else
      radioGroup.checked = false;
  }

  var checkGroup = eval("document.TestingForm.Check" + pageNumber);
  if (checkGroup)
  {
    if (checkGroup.length)
    {
      for (var i = 0; i < checkGroup.length; ++i)
        checkGroup[i].checked = false;
    }
    else
      checkGroup.checked = false;
  }
}

function resetFillInBlank(pageNumber)
{
  for (var i = 0; i < g_textFieldEntries.length; ++i)
  {
    var pageNr = g_textFieldEntries[i].nPage;
    if (pageNr == pageNumber)
    {
      var textFieldObj = eval("document.TestingForm." + g_textFieldEntries[i].strId);
      textFieldObj.value = "";
    }
  }
}

function checkDragAndDrop(pageNumber)
{
  var res = false;
  var bHasTargetPoints = false;

  var bIsCorrect = true;
  for (var i = 0; i < g_targetPointEntries.length; ++i)
  {
    var pageNr = g_targetPointEntries[i].nPage;
    if (pageNr == pageNumber)
    {
      g_interactionType = "matching";
      bHasTargetPoints = true;
      if (g_targetPointEntries[i].strObjectId != "")
      {
        var strLayer1 = "" + g_targetPointEntries[i].strObjectId + "Layer";
        var layer1 = document.getElementById(strLayer1);
        bIsCorrect = targetPointInLayer(g_targetPointEntries[i], layer1);
        if (!bIsCorrect)
          break;
      }
    }
  }
  
  if (bHasTargetPoints && bIsCorrect)
    res = true;

  return res;
}

function checkMultipleChoice(pageNumber)
{
  var res = false;
  var bHasRadioButtons = false;

  var bIsCorrect = true;
  for (var i = 0; i < g_radioDynamicEntries.length; ++i)
  {
    var pageNr = g_radioDynamicEntries[i].nPage;
    if (pageNr == pageNumber)
    {
      g_interactionType = "choice";
      bHasRadioButtons = true;
      var idx = g_radioDynamicEntries[i].nIdx; 
      var radioGroup = eval("document.TestingForm.Radio" + pageNumber);
      if (radioGroup)
      {
        if (radioGroup.length)
        {
          if (radioGroup[idx].checked && !g_radioDynamicEntries[i].bIsChecked)
            bIsCorrect = false;
          else if (!radioGroup[idx].checked && g_radioDynamicEntries[i].bIsChecked)
            bIsCorrect = false;
        }
        else
        {
          if (radioGroup.checked && !g_radioDynamicEntries[i].bIsChecked)
            bIsCorrect = false;
          else if (!radioGroup.checked && g_radioDynamicEntries[i].bIsChecked)
            bIsCorrect = false;
        }
      }
      var checkGroup = eval("document.TestingForm.Check" + pageNumber);
      if (checkGroup)
      {
        if (checkGroup.length)
        {
          if (checkGroup[idx].checked && !g_radioDynamicEntries[i].bIsChecked)
            bIsCorrect = false;
          else if (!checkGroup[idx].checked && g_radioDynamicEntries[i].bIsChecked)
            bIsCorrect = false;
        }
        else
        {
          if (checkGroup.checked && !g_radioDynamicEntries[i].bIsChecked)
            bIsCorrect = false;
          else if (!checkGroup.checked && g_radioDynamicEntries[i].bIsChecked)
            bIsCorrect = false;
        }
      }
      if(!bIsCorrect)
        break;
    }
  }

  if (bHasRadioButtons && bIsCorrect)
    res = true;

  return res;
}

function checkFillInBlank(pageNumber)
{
  var res = false;
  var bHasTextFields = false;

  var bIsCorrect = true;  
  for (var i = 0; i < g_textFieldEntries.length; ++i)
  {
    var pageNr = g_textFieldEntries[i].nPage;
    if (pageNr == pageNumber)
    {
      g_interactionType = "fill-in";
      bHasTextFields = true;
      var textFieldObj = eval("document.TestingForm." + g_textFieldEntries[i].strId);
      bIsCorrect = isAnswerStringInStringArray(textFieldObj.value, g_textFieldEntries[i].strText);
      if(!bIsCorrect)
        break;
    }
  }

  if (bHasTextFields && bIsCorrect)
    res = true;

  return res;
}

function deactivateDragAndDrop(pageNumber)
{
  // Mouse events for dragable images prove themselves for deactivation
  // --> nothing to do here
}

function deactivateMultipleChoice(pageNumber)
{
  var radioGroup = eval("document.TestingForm.Radio" + pageNumber);
  if (radioGroup)
  {
    if (radioGroup.length)
    {
      for (var i = 0; i < radioGroup.length; ++i)
        radioGroup[i].disabled = true;
    }
    else
      radioGroup.disabled = true;
  }

  var checkGroup = eval("document.TestingForm.Check" + pageNumber);
  if (checkGroup)
  {
    if (checkGroup.length)
    {
      for (var i = 0; i < checkGroup.length; ++i)
        checkGroup[i].disabled = true;
    }
    else
      checkGroup.disabled = true;
  }
}

function deactivateFillInBlank(pageNumber)
{
  for (var i = 0; i < g_textFieldEntries.length; ++i)
  {
    var pageNr = g_textFieldEntries[i].nPage;
    if (pageNr == pageNumber)
    {
      var textFieldObj = eval("document.TestingForm." + g_textFieldEntries[i].strId);
      textFieldObj.style.borderColor = "#C0C0C0";
      textFieldObj.disabled = true;
    }
  }
}

function deactivateTestingButtons(pageNumber)
{
  for (var i = 0; i < g_interactionEntries.length; ++i)
  {
    if (g_interactionEntries[i].bIsTestingButton)
    {
      var pageNr = getPageFromActivityTime(g_interactionEntries[i].timeActiveBegin);
      if (pageNr == pageNumber)
      {
        var strId = g_interactionEntries[i].strId;
        button_enableButton(strId, false);
        g_interactionEntries[i].timeActiveBegin = -1;
        g_interactionEntries[i].timeActiveEnd = -1;
      }
    }
  }
}

function areAllQuestionsAnswered(qqIndex)
{
  var questionnaire = g_questionnaireEntries[qqIndex];
  var qqPageStart = questionnaire.nPageStart;
  var qqPageEnd = questionnaire.nPageEnd;
  var bAllQuestionsAnswered = true;
  for (var page = qqPageStart; page <= qqPageEnd; ++page)
  {
    var bIsDeactivated = g_questionEntries[getQuestionIndex(page)].bIsDeactivated;
    var bSuccessfullyAnswered = g_questionEntries[getQuestionIndex(page)].bSuccessfullyAnswered;
    var bQuestionAnswered = (bIsDeactivated || bSuccessfullyAnswered) ? true : false;
    if (!bQuestionAnswered)
    {
      bAllQuestionsAnswered = false;
      break;
    }
  }
  return bAllQuestionsAnswered;
}

function submitTesting(nQuestionIndex)
{
  window.clearInterval(g_TimeoutId);
  var fbIdx = -1;
  var qqIdx = g_questionEntries[nQuestionIndex].nQuestionnaireIndex;
  if (getIsQuestionnairePassed(qqIdx))
    fbIdx = getFeedbackIndexFromQuestionIndex(nQuestionIndex, FEEDBACK_TYPE_QQ_PASSED, false);
  else
    fbIdx = getFeedbackIndexFromQuestionIndex(nQuestionIndex, FEEDBACK_TYPE_QQ_FAILED, false);

  if (fbIdx >= 0)
  {
    g_nFeedbackTestingIndex = fbIdx;
    testing_openFeedbackMessage(fbIdx, false);
  }

  if (g_bFeedbackSummaryEnabled)
    submitSummary(qqIdx);
}

function submitSummary(nQuestionnaireIndex)
{
  if (g_FeedbackId != null)
  {
    g_nQuestionnaireIndex = nQuestionnaireIndex;
    window.setTimeout("submitSummary(g_nQuestionnaireIndex)", 333);
    return;
  }

  var fbIdx = -1;
  fbIdx = getFeedbackIndexFromQuestionIndex(nQuestionnaireIndex, FEEDBACK_TYPE_QQ_EVAL, true);

  g_nQuestionnaireIndex = -1;
  if (fbIdx >= 0)
    testing_openFeedbackMessage(fbIdx, true);
}

//
// Feedback
//

function testing_openFeedbackMessage(nFeedbackIndex, bIsQuestionnaire)
{
  window.clearInterval(g_FeedbackId);
  g_FeedbackId = null;
  g_nFeedbackIndexTemp = nFeedbackIndex;
  g_bIsQuestionnaireTemp = bIsQuestionnaire;

  if (g_bFeedbackMessageVisible)
  {
    // setTimeout() needs global variables as parameters
    g_FeedbackId = window.setInterval("testing_openFeedbackMessage(g_nFeedbackIndexTemp, g_bIsQuestionnaireTemp)", 125);
    return;
  }

  g_bFeedbackMessageVisible = true;
  g_nFeedbackIndex = nFeedbackIndex;
  g_bIsQuestionnaire = bIsQuestionnaire;

  // Setting some default values
  var strFeedback = "";
  var nMsgWidth = 260;
  var nMsgHeight = 140;
  var clrBorder = g_feedbackEntries[nFeedbackIndex].clrBorder;
  var clrBackground = g_feedbackEntries[nFeedbackIndex].clrBackground;
  var clrText = g_feedbackEntries[nFeedbackIndex].clrText;
  var strFontFamily = g_feedbackEntries[nFeedbackIndex].strFontFamily;
  var nFontSizeHeader = g_feedbackEntries[nFeedbackIndex].nFontSizeHeader;
  var nFontSize = g_feedbackEntries[nFeedbackIndex].nFontSize;
  var nFontSizeTable = g_feedbackEntries[nFeedbackIndex].nFontSizeTable;
  var strFontStyle = g_feedbackEntries[nFeedbackIndex].strFontStyle;
  var strFontWeight = g_feedbackEntries[nFeedbackIndex].strFontWeight;
  var nOkWidth = g_feedbackEntries[nFeedbackIndex].nOkWidth;
  var nOkHeight = g_feedbackEntries[nFeedbackIndex].nOkHeight;


  if (bIsQuestionnaire)
  {
    var dScale = (1.0 * nFontSize / 14.0);
    strFeedback = getSummaryString(nFeedbackIndex); 
    nMsgWidth = Math.round(dScale * 260);
    nMsgHeight = Math.round(dScale * 240);
    var qqIdx = g_feedbackEntries[nFeedbackIndex].nIdx;
    var nRowHeight = Math.round(12.0 * nFontSizeTable / 9.0);
    nMsgHeight += (nRowHeight * g_questionnaireEntries[qqIdx].nNumOfQuestions);
    if (dScale < 1.0)
      nMsgHeight += (13.0 / dScale);
  }
  else
  {
    strFeedback = g_feedbackEntries[nFeedbackIndex].strText;
    nMsgWidth = g_feedbackEntries[nFeedbackIndex].nWidth;
    nMsgHeight = g_feedbackEntries[nFeedbackIndex].nHeight;
  }

  var left = g_feedbackEntries[nFeedbackIndex].nX;
  var top = g_feedbackEntries[nFeedbackIndex].nY;
  if ((left+nMsgWidth) > g_slidesWidth)
    left = g_slidesWidth - nMsgWidth;
  if ((top+nMsgHeight) > g_slidesHeight)
    top = g_slidesHeight - nMsgHeight;
  left += layout_getSlidesLeft();
  top += layout_getSlidesTop();

  var ProtectiveLayer = document.getElementById("protectiveLayer");
  ProtectiveLayer.style.visibility = "visible"; 
  var MessageWindow = document.getElementById("feedbackLayer");
  MessageWindow.style.left = left;
  MessageWindow.style.top = top;
  MessageWindow.style.width = nMsgWidth + 32; // +32 for maybe suppressed scrollbar
  MessageWindow.style.height = nMsgHeight + 12; // +12 for maybe cut bottom line
  MessageWindow.style.visibility = "visible";

  // resize iframe table
  var iFrame = this.iframeMessage;
  iFrame.document.getElementById("msg_table").width = nMsgWidth;
  iFrame.document.getElementById("msg_table").height = nMsgHeight;
  iFrame.document.getElementById("msg_table").style.borderColor = clrBorder;
  iFrame.document.getElementById("msg_table").style.backgroundColor = clrBackground;
  iFrame.document.getElementById("spacer_top").width = nMsgWidth - 32;
  iFrame.document.getElementById("spacer_left").height = nMsgHeight - 32;
  iFrame.document.getElementById("spacer_right").height = nMsgHeight - 32;
  iFrame.document.getElementById("spacer_bottom").width = nMsgWidth - 32;
  // adapt "OK" button
  iFrame.document.getElementById("button_ok").style.width = nOkWidth;
  iFrame.document.getElementById("button_ok").style.height = nOkHeight;
  iFrame.document.getElementById("button_ok").style.paddingTop = 1;
  if (!document.all)
    iFrame.document.getElementById("td_ok").width = 10;

  var strMessage = "<div style='color:" + clrText + ";font-family:" + strFontFamily + ";font-size:" + nFontSize + "px;font-style:" + strFontStyle + ";font-weight:" + strFontWeight + ";'>";
  strMessage += strFeedback;
  strMessage += "<\/div>"
  iFrame.document.getElementById("Feedback").innerHTML = strMessage;
}

function testing_closeFeedbackMessage()
{
  var ProtectiveLayer = document.getElementById("protectiveLayer");
  ProtectiveLayer.style.visibility = "hidden"; 
  var MessageWindow = document.getElementById("feedbackLayer");
  MessageWindow.style.visibility = "hidden";
  g_bFeedbackMessageVisible = false;

  var bQuestionAnswered = (!g_bAllQuestionsAnswered && g_bQuestionIsDeactivated);
  var bTestingEvaluation = (g_bAllQuestionsAnswered && !g_bFeedbackSummaryEnabled && g_feedbackEntries[g_nFeedbackIndex].nType>=FEEDBACK_TYPE_QQ_PASSED);
  var bTestingSummary = (g_bAllQuestionsAnswered && g_bIsQuestionnaire && g_bFeedbackSummaryEnabled);
  if ( bQuestionAnswered || bTestingEvaluation || bTestingSummary)
  {
    if (bTestingSummary)
      eval(g_feedbackEntries[g_nFeedbackTestingIndex].strAction);
    else
      eval(g_feedbackEntries[g_nFeedbackIndex].strAction);

    if (g_wasPlaying)
      control_play();
  }
  //g_nFeedbackIndex = -1;
  //g_bIsQuestionnaire = false;
}

function getIsQuestionnairePassed(nQuestionnaireIndex)
{
  var qqPageStart = g_questionnaireEntries[nQuestionnaireIndex].nPageStart;
  var qqPageEnd = g_questionnaireEntries[nQuestionnaireIndex].nPageEnd;
  var nSumPoints = 0;
  for (var page = qqPageStart; page <= qqPageEnd; ++page)
  {
    var bPassed = g_questionEntries[getQuestionIndex(page)].bSuccessfullyAnswered;
    var nPoints = (bPassed) ? (g_questionEntries[getQuestionIndex(page)].nAchievablePoints) : (0);
    nSumPoints += nPoints;
  }
  if (nSumPoints >= g_questionnaireEntries[nQuestionnaireIndex].nPassPoints)
    return true;
  else
    return false;
}

function getSummaryString(nFeedbackIndex)
{
  var nFontSizeHeader = g_feedbackEntries[nFeedbackIndex].nFontSizeHeader;
  var nFontSize = g_feedbackEntries[nFeedbackIndex].nFontSize;
  var nFontSizeTable = g_feedbackEntries[nFeedbackIndex].nFontSizeTable;
  var nRowHeight = Math.round(12.0 * nFontSizeTable / 9.0);
  var strFontFamily = g_feedbackEntries[nFeedbackIndex].strFontFamily;
  var strFontStyle = g_feedbackEntries[nFeedbackIndex].strFontStyle;
  var strFontWeight = g_feedbackEntries[nFeedbackIndex].strFontWeight;
  var strTextColor = g_feedbackEntries[nFeedbackIndex].clrText;
  var nQuestionnaireIndex = g_feedbackEntries[nFeedbackIndex].nIdx;
  var qqPageStart = g_questionnaireEntries[nQuestionnaireIndex].nPageStart;
  var qqPageEnd = g_questionnaireEntries[nQuestionnaireIndex].nPageEnd;
  var nSumPoints = 0;
  var nMaximumPoints = g_questionnaireEntries[nQuestionnaireIndex].nTotalPoints;
  var nPassPoints = g_questionnaireEntries[nQuestionnaireIndex].nPassPoints;
  var idx = 0;

  var strSummary = "<div style='color:" + g_colorFeedbackText + ";font-family:" + strFontFamily + ";font-size:" + nFontSizeHeader + "px;font-style:" + strFontStyle + ";font-weight:" + strFontWeight + ";'>";
  strSummary += g_strTestingResult;
  strSummary += "<\/div>";
  strSummary += "<table><tr><td><\/td><\/tr><\/table>";
  strSummary += "<table width=\"100%\" cellpadding=\"0\" cellspacing=\"2\" border=\"0\" style='text-align:left;color:" + strTextColor + ";font-family:" + strFontFamily + ";font-size:" + nFontSize + "px;'>";
  strSummary += "<tr height='" + nRowHeight + "' style=\"color:" + strTextColor + "\">";
  strSummary += "<td>";
  strSummary += g_strTestingQuestion;
  strSummary += "<\/td>";
  strSummary += "<td>";
  strSummary += g_strTestingAnswer;
  strSummary += "<\/td>";
  strSummary += "<td>";
  strSummary += g_strTestingPoints;
  strSummary += "<\/td>";
  strSummary += "<\/tr>";
  strSummary += "<tr>";
  strSummary += "<td colspan=\"3\" height=\"1\" style=\"background-color:" + strTextColor + "\"><img src=\"images\trans.gif\" width=\"1\" height=\"1\" alt=\"\"><\/td>";
  strSummary += "<\/tr>";
  for (var page = qqPageStart; page <= qqPageEnd; ++page)
  {
    var bCorrect = g_questionEntries[getQuestionIndex(page)].bSuccessfullyAnswered;
    var nPoints = (bCorrect) ? (g_questionEntries[getQuestionIndex(page)].nAchievablePoints) : (0);
    strSummary += "<tr height='" + nRowHeight + "' style=\"font-size:" + nFontSizeTable + "\">";
    strSummary += "<td>";
    strSummary += (g_strTestingQuestion + " " + ++idx);
    strSummary += "<\/td>";
    strSummary += "<td>";
    strSummary += (bCorrect) ? (g_strTestingCorrect) : (g_strTestingWrong);
    strSummary += "<\/td>";
    strSummary += "<td>";
    strSummary += nPoints;
    strSummary += "<\/td>";
    strSummary += "<\/tr>";
    nSumPoints += nPoints;
  }
  strSummary += "<tr style=\"font-size:" + nFontSizeTable + "\">";
  strSummary += "<td colspan=\"3\" height=\"1\" style=\"background-color:" + strTextColor + "\"><img src=\"trans.gif\" width=\"1\" height=\"1\" alt=\"\"><\/td>";
  strSummary += "<\/tr>";
  strSummary += "<tr>";
  strSummary += "<td colspan=\"3\" height=\"1\" style=\"background-color:" + strTextColor + "\"><img src=\"trans.gif\" width=\"1\" height=\"1\" alt=\"\"><\/td>";
  strSummary += "<\/tr>";
  strSummary += "<tr height='" + nRowHeight + "' style=\"font-size:" + nFontSizeTable + "\">";
  strSummary += "<td colspan=\"2\">";
  strSummary += g_strTestingMaximumPoints;
  strSummary += "<\/td>";
  strSummary += "<td>";
  strSummary += nMaximumPoints;
  strSummary += "<\/td>";
  strSummary += "<\/tr>";
  strSummary += "<tr height='" + nRowHeight + "' style=\"font-size:" + nFontSizeTable + "\">";
  strSummary += "<td colspan=\"2\">";
  strSummary += g_strTestingRequiredPoints;
  strSummary += "<\/td>";
  strSummary += "<td>";
  strSummary += nPassPoints;
  strSummary += "<\/td>";
  strSummary += "<\/tr>";
  strSummary += "<tr height='" + nRowHeight + "' style=\"font-size:" + nFontSizeTable + "\">";
  strSummary += "<td colspan=\"2\">";
  strSummary += g_strTestingRequiredPercentage;
  strSummary += "<\/td>";
  strSummary += "<td>";
  strSummary += (nMaximumPoints>0) ? (Math.round(100.0 * nPassPoints/nMaximumPoints) + "%") : ("0%");
  strSummary += "<\/td>";
  strSummary += "<\/tr>";
  strSummary += "<tr height='" + nRowHeight + "' style=\"font-size:" + nFontSizeTable + "\">";
  strSummary += "<td colspan=\"2\">";
  strSummary += g_strTestingAchievedPoints;
  strSummary += "<\/td>";
  strSummary += "<td>";
  strSummary += nSumPoints;
  strSummary += "<\/td>";
  strSummary += "<\/tr>";
  strSummary += "<tr height='" + nRowHeight + "' style=\"font-size:" + nFontSizeTable + "\">";
  strSummary += "<td colspan=\"2\">";
  strSummary += g_strTestingAchievedPercentage;
  strSummary += "<\/td>";
  strSummary += "<td>";
  strSummary += (nMaximumPoints>0) ? (Math.round(100.0 * nSumPoints/nMaximumPoints) + "%") : ("0%");
  strSummary += "<\/td>";
  strSummary += "<\/tr>";
  strSummary += "<\/table>";
  strSummary += "<table><tr><td><\/td><\/tr><\/table>";
  strSummary += "<div style='text-align:left;margin-left:2px;color:" + strTextColor + ";font-family:" + strFontFamily + ";font-size:" + nFontSize + "px;'>";
  var strFeedbackPassed = g_feedbackEntries[nFeedbackIndex].strTextPassed;
  var strFeedbackFailed = g_feedbackEntries[nFeedbackIndex].strText;
  strSummary += (nSumPoints >= g_questionnaireEntries[nQuestionnaireIndex].nPassPoints) ? 
    (strFeedbackPassed) : (strFeedbackFailed) ;
  strSummary += "<\/div><br>";

  return strSummary;
}

//
// Utils for testing stuff
//

function getQuestionnaireIndex(pageNumber)
{
  var qqIdx = -1;
  for (var i = 0; i < g_questionnaireEntries.length; ++i)
  {
    if ((pageNumber >= g_questionnaireEntries[i].nPageStart) 
      && (pageNumber <= g_questionnaireEntries[i].nPageEnd))
    {
      qqIdx = i;
      break;
    }
  }
  return qqIdx;
}

function getQuestionIndex(pageNumber)
{
  var qIdx = -1;
  for (var i = 0; i < g_questionEntries.length; ++ i)
  {
    if (pageNumber == g_questionEntries[i].nPage)
    {
      qIdx = i;
      break;
    }
  }
  return qIdx;
}

function getQuestionIndexFromTimestamp(timeMs)
{
  var qIdx = -1;
  for (var i = 0; i < g_questionEntries.length; ++ i)
  {
    if ((timeMs >= g_questionEntries[i].nStartMs) && (timeMs <= g_questionEntries[i].nEndMs))
    {
      qIdx = i;
      break;
    }
  }
  return qIdx;
}

function getFeedbackIndexFromQuestionIndex(qIdx, nFeedbackType, bIsQuestionnaire)
{
  var fbIdx = -1;
  for (var i = 0; i < g_feedbackEntries.length; ++i)
  {
    if ((g_feedbackEntries[i].nIdx == qIdx) 
      && (g_feedbackEntries[i].nType == nFeedbackType) 
      && (g_feedbackEntries[i].bHasQuestionnaire == bIsQuestionnaire))
    {
      fbIdx = i;
      break;
    }
  }
  return fbIdx;
}

function getPageFromActivityTime(timeActiveBeginMs)
{
  var nPage = -1;
  for (var i = 0; i < (g_slideEntries.length - 1); ++i)
  {
    if ((timeActiveBeginMs >= g_slideEntries[i].slideTime) && (timeActiveBeginMs < g_slideEntries[i+1].slideTime))
    {
      nPage = g_slideEntries[i].slideNumber;
      break; 
    }
  }
  if (timeActiveBeginMs < g_slideEntries[0].slideTime)
    nPage = 0;
  i = (g_slideEntries.length - 1);
  if (timeActiveBeginMs >= g_slideEntries[i].slideTime)
    nPage = i;

  if (nPage < 0)
    return nPage;
  else
    return (nPage + 1);
}

// General: array separator is ';'
function isAnswerStringInStringArray(strAnswer, strAllAnswers)
{
  var bIsInArray = false;
  var alAnswers = strAllAnswers.split(";");
  for (var i = 0; i < alAnswers.length; ++i)
  {
    if (alAnswers[i] == strAnswer)
    {
      bIsInArray = true;
      break;
    }  
  }
  return bIsInArray;
}
function getBooleanArrayFromString(strBooleanData)
{
  var alBoolean = strBooleanData.split(";");
  var aBooleanData = new Array(alBoolean.length)
  for (var i = 0; i < alBoolean.length; ++ i)
  {
    if (alBoolean[i] == "true")
      aBooleanData[i] = true;
    else
      aBooleanData[i] = false;
  }

  return aBooleanData;
}

function getIntegerArrayFromString(strIntegerData)
{
  var alInteger = strIntegerData.split(";");
  var aIntegerData = new Array(alInteger.length)
  for (var i = 0; i < alInteger.length; ++ i)
    aIntegerData[i] = parseInt(alInteger);

  return aIntegerData;
}

function updateTriesFromQuestionIndex(qIdx)
{
  var nTries = g_questionEntries[qIdx].nMaximumAttempts - g_questionEntries[qIdx].nTakenAttempts;
  if (nTries < 0)
    return;
  if (g_questionEntries[qIdx].nMaximumAttempts == 0)
    nTries = 1;
  var strTries = "" + nTries;
  var actualPage = g_questionEntries[qIdx].nPage;
  for (var i = 0; i < g_changeTextEntries.length; ++i)
  {
    if ( (g_changeTextEntries[i].nPage == actualPage) 
      && (g_changeTextEntries[i].nType == TEXT_TYPE_CHANGE_TRIES) )
    {
      var strId = "" + g_changeTextEntries[i].strId + "Layer";
      document.getElementById(strId).innerHTML = strTries;
    }
  }
}

function updateTimerFromQuestionIndex(qIdx)
{
  var nSec = g_questionEntries[qIdx].nTimeoutSec - g_questionEntries[qIdx].nViewedSec;
  if (nSec < 0)
    return;
  var strTime = getTimeFromSeconds(nSec);
  var actualPage = g_questionEntries[qIdx].nPage;
  for (var i = 0; i < g_changeTextEntries.length; ++i)
  {
    if ( (g_changeTextEntries[i].nPage == actualPage) 
      && (g_changeTextEntries[i].nType == TEXT_TYPE_CHANGE_TIME) )
    {
      var strId = "" + g_changeTextEntries[i].strId + "Layer";
      document.getElementById(strId).innerHTML = strTime;
    }
  }
}

function getTimeFromSeconds(nSec)
{
  var strTime = "";
//  var hrs = Math.floor(nSec / 3600.0);
//  nSec = nSec - (3600 * hrs);
  var min = Math.floor(nSec / 60.0);
  nSec = nSec - (60 * min);
  var sec = nSec;
//  if (hrs < 10)
//    strTime += "0";
//  strTime += hrs + ":";
  if (min < 10)
    strTime += "0";
  strTime += min + ":";
  if (sec < 10)
    strTime += "0";
  strTime += sec;
  
  return strTime;
}
//
// Drag & Drop
//

var downX, downY;
var isSnappedIn = false;
var layer = null;
var layerIndex = -1;
var originalX = null;
var originalY = null;

function posX(obj)
{
  return parseInt(obj.style.left);
}

function posY(obj)
{
  return parseInt(obj.style.top);
}

function contains(targetPointEntry, x, y)
{
  if ( (g_lastPositionMs < targetPointEntry.nStartMs) || (g_lastPositionMs > targetPointEntry.nEndMs) )
    return false;

  var tpX = targetPointEntry.nCenterX + layout_getSlidesLeft();
  var tpY = targetPointEntry.nCenterY + layout_getSlidesTop();
  var distance = Math.sqrt( (tpX-x)*(tpX-x) + (tpY-y)*(tpY-y) );
  return (distance <= targetPointEntry.nRadius);
}

function intersectionExists(layer1, layer2)
{
  if (layer1 == layer2)
    return true;
  var startX1 = posX(layer1);
  var startY1 = posY(layer1);
  var startX2 = posX(layer2);
  var startY2 = posY(layer2);
  var endX1 = startX1 + layer1.clientWidth-1;
  var endY1 = startY1 + layer1.clientHeight-1;
  var endX2 = startX2 + layer2.clientWidth-1;
  var endY2 = startY2 + layer2.clientHeight-1;
  var bNoIntersection = endX1 < startX2 || endY1 < startY2 || startX1 > endX2 || startY1 > endY2;
  return !bNoIntersection;
}

function arrangeLayer(centerLayer, targetPointEntry)
{
  centerLayer.style.left = targetPointEntry.nCenterX - (0.5 * centerLayer.clientWidth) + layout_getSlidesLeft();
  centerLayer.style.top = targetPointEntry.nCenterY - (0.5 * centerLayer.clientHeight) + layout_getSlidesTop();
}

function targetPointInLayer(targetPointEntry, layer1)
{
  var tpX = targetPointEntry.nCenterX + layout_getSlidesLeft();
  var tpY = targetPointEntry.nCenterY + layout_getSlidesTop();
  var startX1 = posX(layer1);
  var startY1 = posY(layer1);
  var endX1 = startX1 + layer1.clientWidth-1;
  var endY1 = startY1 + layer1.clientHeight-1;
  var bIsInside = tpX >= startX1 && tpX <= endX1 && tpY >= startY1 && tpY <= endY1;
  return bIsInside;
}

function mouseDown(e)
{
  if (!e)
    e = window.event;
  
  var src = (document.all) ? (e.srcElement) : (e.target);

  if (originalX == null)
  {
    // remember initial moveLayer positions
    var count = g_targetPointEntries.length;
    originalX = new Array(count);
    originalY = new Array(count);
    for (i = 0; i < count; ++i)
    {
      if (g_targetPointEntries[i].strObjectId != "")
      {
        strId = "" + g_targetPointEntries[i].strObjectId + "Layer";
        originalX[i] = posX(document.getElementById(strId));
        originalY[i] = posY(document.getElementById(strId));
      }
    }
  }
  
  var bIsDeactivated = false;
  // actualTimestamp: see slides.js
  var qIdx = getQuestionIndexFromTimestamp(actualTimestamp);
  if (qIdx >= 0)
    bIsDeactivated = g_questionEntries[qIdx].bIsDeactivated;
  if(!bIsDeactivated)
  {
    var count = g_targetPointEntries.length;
    for (i = 0; i < count; ++i)
    {
      if (g_targetPointEntries[i].strObjectId != "")
      {
        strId = "" + g_targetPointEntries[i].strObjectId + "Layer";
        if (document.getElementById(strId) == src)
        {
          layer = src;
          layerIndex = i;
          layer.style.zIndex++;
          downX = (document.all) ? (e.offsetX) : (e.layerX);
          downY = (document.all) ? (e.offsetY) : (e.layerY);
        }
      }
    }
  }
}

function mouseMove(e)
{
  if (!e)
    e = window.event;

  var myX = (document.all) ? (e.clientX) : (e.pageX);
  var myY = (document.all) ? (e.clientY) : (e.pageY);

  var bIsDeactivated = false;
  // actualTimestamp: see slides.js
  var qIdx = getQuestionIndexFromTimestamp(actualTimestamp);
  if (qIdx >= 0)
    bIsDeactivated = g_questionEntries[qIdx].bIsDeactivated;
  if (layer && !bIsDeactivated)
  {
    trueX = myX - downX + document.body.scrollLeft;
    trueY = myY - downY + document.body.scrollTop;
    var middleX = trueX + layer.clientWidth/2;
    var middleY = trueY + layer.clientHeight/2;
    isSnappedIn = false;
    var count = g_targetPointEntries.length;
    for (i = 0; i < count; ++i)
    {
      if ((actualTimestamp >= g_targetPointEntries[i].nStartMs) && (actualTimestamp <= g_targetPointEntries[i].nEndMs))
      {
        if (contains(g_targetPointEntries[i], middleX, middleY))
        {
          arrangeLayer(layer, g_targetPointEntries[i]);
          isSnappedIn = true;
          break;
        }
      }
    }
    if (!isSnappedIn)
    {
      layer.style.left = trueX;
      layer.style.top = trueY;
    }
    return false;
  }
}

function mouseUp(e)
{
  var bIsDeactivated = false;
  // actualTimestamp: see slides.js
  var qIdx = getQuestionIndexFromTimestamp(actualTimestamp);
  if (qIdx >= 0)
    bIsDeactivated = g_questionEntries[qIdx].bIsDeactivated;
  if (layer && !bIsDeactivated)
  {
    // Check, if one Image is displayed above another
    var isMasked = false;
    var count = g_targetPointEntries.length;
    for (i = 0; i < count; ++i)
    {
      // actualTimestamp: see slides.js
      if ((actualTimestamp >= g_targetPointEntries[i].nStartMs) && (actualTimestamp <= g_targetPointEntries[i].nEndMs))
      {
        if (g_targetPointEntries[i].strObjectId != "")
        {
          strId = "" + g_targetPointEntries[i].strObjectId + "Layer";
          var layer2 = document.getElementById(strId);
          if (intersectionExists(layer, layer2) && layer != layer2)
            isMasked = true;
        }
      }
    }
    if ( (!isSnappedIn) || (isMasked) )
    {
      layer.style.left = originalX[layerIndex];
      layer.style.top = originalY[layerIndex];
    }
    layer.style.zIndex--;
    layer = null;
    layerIndex = -1;
  }
}

document.onmousedown = mouseDown;
document.onmousemove = mouseMove;
document.onmouseup = mouseUp;
