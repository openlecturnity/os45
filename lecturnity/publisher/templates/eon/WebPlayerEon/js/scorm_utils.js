function scutils_PreInit()
{
	if(g_scormEnabled && g_scormLoaded )
	{
	 scutils_checkResumeSupported();
	 if(g_LMSResumeSupported)
           {
	     doLMSSetValue( "cmi.score.min", g_nTotalMinAchievablePoints ) ;
             doLMSSetValue( "cmi.score.max", g_nTotalMaxAchievablePoints ) ;
	    }
	  if (doLMSGetValue( "cmi.entry") == "resume")
		{
		var pos = doLMSGetValue("cmi.location");
		g_returnPosition = parseInt(pos);
		if(g_LMSResumeSupported)
		   {
		     scutils_loadAchiveScore();
		     scutils_loadSuspendData();
		    }
		}
	else
		{
		if(g_LMSResumeSupported)
			doLMSSetValue( "cmi.score.raw", g_nTotalCurrentlyAchievePoints ) ;
		scutils_updateSuccessStatus();
		scutils_updateCompletionStatus();
		}

	}	
}

function delayedSeekTime(msTime)
{
  if (InitImageStyle.visibility == 'hidden')
  {
    g_returnPosition = msTime;
    setTimeout("control_play()", 0);
    setTimeout("control_pause()", 50);
    setTimeout("layout_onGotoTimestamp(g_returnPosition, 'pause')", 500);
    setTimeout("control_stop()", 520);
  }
  else
    setTimeout("delayedSeekTime(g_returnPosition)", 500);
}

function scutils_PostInit()
{
if(g_scormEnabled && g_scormLoaded )
	{
	  if (doLMSGetValue( "cmi.entry") == "resume")
		{
		
			if(g_LMSResumeSupported)
			{
				scutils_loadLearnerData();	
			}
			if (control_getType() == "RM")
			   delayedSeekTime(g_returnPosition);
			else
			   setTimeout("layout_onGotoTimestamp(g_returnPosition, 'pause')",  500);
		}
	}
}

function scutils_Finish()
{
	if(g_scormEnabled && g_scormLoaded )
	{
	if(g_LMSResumeSupported)
		scutils_saveSuspendData();
	var time = getPlayTime();
	doLMSSetValue("cmi.session_time", time);
	var a_successStatus = scutils_updateSuccessStatus();
	var a_completionStatus = scutils_updateCompletionStatus();
	
	if (a_successStatus && a_completionStatus )
		{
		// should be doLMSSetValue("cmi.exit", "normal"); but are some problems with status reporting in CLIX
		doLMSSetValue("cmi.exit", "suspend");
		}
	else
		{
		doLMSSetValue("cmi.exit", "suspend");
		}
		
	}
}

function getPlayTime()
{
	var hours = parseInt(g_sessionTime / 3600);
	var minutes = parseInt((g_sessionTime % 3600) / 60);
	var seconds = (g_sessionTime % 3600) % 60;
	var retVal = "PT"+ hours+"H"+minutes+"M"+seconds+"S";
	return retVal;
}

function scutils_setLocation(a_scormLocation)
{
  if (g_scormEnabled && g_scormLoaded  )
	{
	doLMSSetValue( "cmi.location", a_scormLocation ) ;
	}
}

function scutils_addInteraction(a_InteractionID, a_InteractionType, a_InteractionTimeStamp, a_InteractionWeighting, a_interactionPattern)
{
  if (g_scormEnabled && g_scormLoaded  )
	{
	var t_InteractionTimeStamp = scutils_GMTtoLMStime(a_InteractionTimeStamp); 
	var crtCout = doLMSGetValue("cmi.interactions._count");
	doLMSSetValue( "cmi.interactions."+crtCout+".id", "urn:IMC:interaction_"+a_InteractionID) ;
	doLMSCommit();
	doLMSSetValue( "cmi.interactions."+crtCout+".type", a_InteractionType) ;
	doLMSCommit();
	doLMSSetValue( "cmi.interactions."+crtCout+".timestamp", t_InteractionTimeStamp );
	doLMSCommit();
	doLMSSetValue( "cmi.interactions."+crtCout+".weighting", a_InteractionWeighting );
	doLMSCommit();
	if(a_InteractionType == "fill-in")
		{
			var i=0;
			var j=0;
			var k=0;
			var str = "";
			var fill = new Array();	
			
			while(i < a_interactionPattern.length)
			{
			fill[j] = new Array();
			
				while(a_interactionPattern.charAt(i) != '[' && i < a_interactionPattern.length)
				{
					if(a_interactionPattern.charAt(i) != ';' && a_interactionPattern.charAt(i) != '[')
						str += a_interactionPattern.charAt(i);
					else 
					{
						fill[j][k] = str;
						str = "";
						k ++;	
					}
				i ++;	
				}
			fill[j][k] = str;
			str = "";	
			k = 0;
			i += 3;
			j ++;	
			}
		
			k = 0;
			var index = 0;
			var as = false;
			var st = new Array();
			st[k] = -1;
			while(k > -1)
			{
				if(st[k] < fill[k].length-1)
				{
					
					st[k] = st[k] + 1;
					as = true;
				}
				else
					as = false;
				if(as)
				{
					if(k == fill.length-1)
					{
						a_interactionPattern = "";
						for(i=0; i < st.length; i++)
						{
							if(i != st.length-1)
								a_interactionPattern += fill[i][st[i]] + "[,]";
							else
								a_interactionPattern += fill[i][st[i]];
						}
						doLMSSetValue( "cmi.interactions."+crtCout+".correct_responses."+index+".pattern", a_interactionPattern ) ;
						index ++;	
					}
					else
					{
						k ++;
						st[k] = -1;
					}
				}
				else
					k --; 
			}	
		}
		else
		doLMSSetValue( "cmi.interactions."+crtCout+".correct_responses.0.pattern", a_interactionPattern ) ;
		doLMSCommit();

	}
}
function scutils_setInteractionResult(a_InteractionID , a_InteractionResult, a_InteractionLearnerResponse, a_InteractionTimeLatency)
{
  if (g_scormEnabled && g_scormLoaded  )
	{
	var t_index = scutils_getInteractionIndex(a_InteractionID)
	if(t_index != null)
		{
		
		doLMSSetValue( "cmi.interactions."+t_index+".result", a_InteractionResult ) ;
		doLMSSetValue( "cmi.interactions."+t_index+".learner_response", a_InteractionLearnerResponse) ;
		var t_timeStamp = doLMSGetValue( "cmi.interactions."+t_index+".timestamp");
		var t_TimeStampGMT = scutils_LMStoGMTtime(t_timeStamp);
		var t_InteractionTimeLatencyGMT = new Date(a_InteractionTimeLatency.toUTCString());
		var t_TimeLatencyGMT = t_InteractionTimeLatencyGMT-t_TimeStampGMT;
		var t_TimeLatencyLMS = scutils_GMTtoLMS_TimeInterval(t_TimeLatencyGMT);
		doLMSSetValue( "cmi.interactions."+t_index+".latency", t_TimeLatencyLMS ) ;
		doLMSCommit();
		}
	}
}

function scutils_getInteractionIndex(a_InteractionID)
{
 if (g_scormEnabled && g_scormLoaded  )
	{
	var t_crtCout = doLMSGetValue("cmi.interactions._count");
	var t_InteractionID;
	for (var i = 0; i < t_crtCout; ++i)
		{
		t_InteractionID = doLMSGetValue("cmi.interactions."+i+".id");
		if(("urn:IMC:interaction_"+a_InteractionID) == t_InteractionID)
			{
			return i;
			}
		}
	return null;
	}
return null;	
}

function scutils_setAchiveScore(a_AchiveScore)
{
  if (g_scormEnabled && g_scormLoaded  )
	  {
	  doLMSSetValue( "cmi.score.raw", a_AchiveScore ) ;
	  }
}

function scutils_loadAchiveScore()
{
  if (g_scormEnabled && g_scormLoaded)
	  {
	  var a_crtAchiveScore =  doLMSGetValue( "cmi.score.raw" ) ;
	  if(a_crtAchiveScore != null)
		  g_nTotalCurrentlyAchievePoints = parseInt(a_crtAchiveScore);
	  }
return;	  
}

function scutils_updateSuccessStatus()
{
  if (g_scormEnabled && g_scormLoaded  )
	  {
	  if(g_nTotalCurrentlyAchievePoints >= g_nTotalPassAchievablePoints)
		  {
		  doLMSSetValue( "cmi.success_status", "passed" ) ;
		  return true;
		  }
	 else
		 {
		 doLMSSetValue( "cmi.success_status", "failed" ) ;
		 return false;
		 }
	}
}

function scutils_updateCompletionStatus()
{
  if (g_scormEnabled && g_scormLoaded  )
	  {
	  var a_visitedCount = 0;
	  for ( var i = 0; i < g_visitedPages.length; i++)
		  if(g_visitedPages[i] == "true")
			  a_visitedCount++;
	  if(a_visitedCount < g_thumbCount)
		{
		doLMSSetValue( "cmi.completion_status", "incomplete" ) ;
		return false;
		}
	 else
		{
		doLMSSetValue( "cmi.completion_status", "completed" ) ;
		return true;
		}
	 }
}

function scutils_getSuccessStatus()
{
  if (g_scormEnabled && g_scormLoaded )
	  {
	  return doLMSGetValue("cmi.success_status" ) ;
	  }
return;	  
}

function scutils_saveSuspendData()
{
  if (g_scormEnabled && g_scormLoaded )
	 {
		  // wil contain comma separated information for every question and every data information
		  // comma separated for all questions
		  // Question1 -> QuestionnaireIndex [,]QuestionEntryIndex[,]QuestionAddedLMS[,]QuestionActivatedStatus[,]QuestionTakenAttempts[,]QuestionViewedSec
		  // Question2 -> QuestionnaireIndex[,]QuestionEntryIndex[,]QuestionAddedLMS[,]QuestionActivatedStatus[,]QuestionTakenAttempts[,]QuestionViewedSec
		  // Question1{,}Question2[,]Question3
		  //DataInfo1<,>DataInfo2<,>DataInfo3
		  var a_suspendData = "";
		  
		  if(g_hasTesting == true)
		{
			 var qLooping = false;
			 for(var j = 0; j < g_questionEntries.length; ++j )
			  {
			  if(qLooping)
				  a_suspendData+=g_MajorStrSeparator;
			  else
				  qLooping = true;
			  a_suspendData += j + g_MinorStrSeparator + 
			  g_questionEntries[j].nQuestionAddedLMS + g_MinorStrSeparator +
			  g_questionEntries[j].bIsDeactivated + g_MinorStrSeparator +
			  g_questionEntries[j].nTakenAttempts + g_MinorStrSeparator +
			  g_questionEntries[j].nViewedSec ;
			 }
		}	 
		
		a_suspendData += g_ScopeStrSeparator;
		
			  for(var i = 0; i < g_visitedPages.length-1; i++)
				  a_suspendData += g_visitedPages[i] + g_MinorStrSeparator;
		 if(g_visitedPages.length >= 1)
			 a_suspendData += g_visitedPages[g_visitedPages.length-1];	
			 
		  doLMSSetValue( "cmi.suspend_data", a_suspendData) ;

	  }
	  
}

function scutils_loadSuspendData()
{
 if (g_scormEnabled && g_scormLoaded )
	  {
	
		  // wil contain comma separated information for every question
		  // comma separated for all questions
		  // Question1 ->  QuestionEntryIndex, QuestionAddedLMS, QuestionActivatedStatus, QuestionTakenAttempts, QuestionViewedSec
		  // Question2 ->  QuestionEntryIndex, QuestionAddedLMS, QuestionActivatedStatus, QuestionTakenAttempts, QuestionViewedSec
		  // Question3 ->  QuestionEntryIndex, QuestionAddedLMS, QuestionActivatedStatus, QuestionTakenAttempts, QuestionViewedSec
		  // Question1, Question2, Question3

		var a_suspendDataInformation = doLMSGetValue("cmi.suspend_data" ) ;
		if (a_suspendDataInformation != null)
			{
				var a_suspendData = new Array();
				a_suspendData = a_suspendDataInformation.split(g_ScopeStrSeparator);
				// recompose g_questionEntries data
				if(g_hasTesting == true)
				{
				var	a_QuestionData = new Array();
				a_QuestionData = a_suspendData[0].split(g_MajorStrSeparator);
				var a_QuestionValue;
				for ( var j = 0; j < a_QuestionData.length; j++)
					{		
						a_QuestionValue =  new Array();
						a_QuestionValue = a_QuestionData[j].split(g_MinorStrSeparator);
						var a_QuestionIndex = a_QuestionValue[0];			
						g_questionEntries[a_QuestionIndex] .nQuestionAddedLMS = a_QuestionValue[1];
						g_questionEntries[a_QuestionIndex] .nTakenAttempts = parseInt(a_QuestionValue[3]);
						g_questionEntries[a_QuestionIndex] .nViewedSec = parseInt(a_QuestionValue[4]);
						if(a_QuestionValue[2] == "true")			
							deactivateQuestion(g_questionEntries[a_QuestionIndex].nPage);				
					}
				}
			// recompose g_visitedPages data
				var a_visitedPages = new Array()	;
				a_visitedPages = a_suspendData[1].split(g_MinorStrSeparator);
				for ( var i = 0; i < a_visitedPages.length; i++)
					g_visitedPages[i] = a_visitedPages[i];
		
			}
	  }
}

function scutils_loadLearnerData()
{
   if (g_scormEnabled && g_scormLoaded )
	  {
			var t_crtCout = doLMSGetValue("cmi.interactions._count");
			var t_InteractionID;
			
			for (var qIdx = 0; qIdx < g_questionEntries.length; qIdx++)
			{	
			var n = scutils_getInteractionIndex(g_questionEntries[qIdx].strId);
			if( n != null)
			{
			var result = doLMSGetValue("cmi.interactions."+n+".result");
			if(result == "correct")
				g_questionEntries[qIdx].bSuccessfullyAnswered = true;
			else
				g_questionEntries[qIdx].bSuccessfullyAnswered = false;		
			var response = doLMSGetValue("cmi.interactions."+n+".learner_response");
			
				if(response != null)
					{
					var type = doLMSGetValue("cmi.interactions."+n+".type");
					if(type == "choice")
					{
						var radioGroup = eval("document.TestingForm.Radio" +g_questionEntries[qIdx].nPage);
						var j = 0;
						while(j < response.length)
						{
							radioGroup[g_Responses.indexOf(response.charAt(j))].checked = true;
							j += 4;
						} 
					}
					else 
					if(type == "fill-in")
					{
						var str = "";
						var j = 0;
						
						while(j<response.length)
						{
							if (j!=0) j += 3;
							while (response.charAt(j)!='[' && j<response.length)
							{
								str += response.charAt(j);
								j ++;
							}
							var textFieldObj = eval("document.TestingForm." + g_textFieldEntries[g_questionFillIndex].strId);
							textFieldObj.value = str;
							g_questionFillIndex ++;
							str = "";
							
						}	
					}
					else
					if(type == "matching")
					{		
						// remember initial moveLayer positions
						if (originalX == null) 
						{
							var count = g_targetPointEntries.length;
							originalX = new Array(count);
							originalY = new Array(count);
							for (i = 0; i < count; i++)
							{
								var strId = "" + g_targetPointEntries[i].strObjectId + "Layer";		
								originalX[i] = posX(document.getElementById(strId));
								originalY[i] = posY(document.getElementById(strId));	
							}
						}
							
						var count = g_questionDragIndex;
						var startMs = g_targetPointEntries[count].nStartMs;
						while(count < g_targetPointEntries.length && g_targetPointEntries[count].nStartMs == startMs)
							count ++;
															  
						var vector = new Array();
						for(var k = g_questionDragIndex; k < count; k++)
							vector[k] = parseInt(g_targetPointEntries[k].nX);
								
						for(var k = g_questionDragIndex; k < count-1; k++)
							for(var l = k+1; l < count; l++)
							  if(vector[l] < vector[k])
							  {
								var temp = vector[l];
								vector[l] = vector[k]
								vector[k] = temp;								 
							  }
			 	 
						var j=0;
						var index = g_questionDragIndex;	
						while(j < response.length)
						{
							var strId = "" + g_targetPointEntries[g_questionDragIndex].strObjectId + "Layer";
      						var imgObject = document.getElementById(strId);
  																										
							j += 4;					
							var str = response.charAt(j);
							var k = index;	
							while(k < count && !(str.length==1 && str.charAt(0)=='-'))
							{
								if(g_targetPointEntries[k].nX == vector[g_Responses.indexOf(str)]) 
								{
									imgObject.style.left = g_targetPointEntries[k].nX + layout_getSlidesLeft();
   									imgObject.style.top = g_targetPointEntries[k].nY + layout_getSlidesTop();
									arrangeLayer(imgObject, g_targetPointEntries[k]);
									break;
								}
						  		else k++;
							}
							j += 4;
							g_questionDragIndex ++;
						}
								
					}	
					}
				}
			}
	  }
}	  
function scutils_GMTtoLMStime(a_TimeStamp)
{
var a_Year = a_TimeStamp.getFullYear();
var a_Month =  a_TimeStamp.getUTCMonth()+1;
if(a_Month<10)
	a_Month = "0"+a_Month;
var a_Date =  a_TimeStamp.getUTCDate();
if(a_Date<10)
	a_Date = "0"+a_Date;
var a_Hour =  a_TimeStamp.getUTCHours();
if(a_Hour<10)
	a_Hour = "0"+a_Hour;
var a_Minute=  a_TimeStamp.getUTCMinutes();
if(a_Minute<10)
	a_Minute = "0"+a_Minute;
var a_Second=  a_TimeStamp.getUTCSeconds();
if(a_Second<10)
	a_Second = "0"+a_Second;	
var timeLMS = a_Year+"-"+a_Month+"-"+a_Date+"T"+a_Hour+":"+a_Minute+":"+a_Second;
return timeLMS;
}
function scutils_LMStoGMTtime(a_TimeStamp)
{
var a_DateTime = new Array();
a_DateTime = a_TimeStamp.split("T");
var a_Date = new Array();
a_Date = a_DateTime[0].split("-");

var a_Time = new Array();
a_Time = a_DateTime[1].split(":");

var t_DateTimeGMT = new Date();  

var a_Year = parseInt(a_Date[0]);
t_DateTimeGMT.setYear(a_Year);

var a_Month = parseInt(a_Date[1])-1;
t_DateTimeGMT.setUTCMonth(a_Month);

var a_Date = parseInt(a_Date[2]);
t_DateTimeGMT.setUTCDate(a_Date);

var a_Hour = parseInt(a_Time[0]);
t_DateTimeGMT.setUTCHours(a_Hour);

var a_Minute = parseInt(a_Time[1]);
t_DateTimeGMT.setUTCMinutes(a_Minute);

var a_Second = parseInt(a_Time[2]); 
t_DateTimeGMT.setUTCSeconds(a_Second);

return new Date(t_DateTimeGMT.toUTCString());

}

function scutils_GMTtoLMS_TimeInterval(a_TimeInterval)
{
var a_DateTimeGMT = new Date(a_TimeInterval);
var a_Year;
if(a_DateTimeGMT.getTimezoneOffset()>=0)
	a_Year = a_DateTimeGMT.getFullYear()-1969;
else
	a_Year = a_DateTimeGMT.getFullYear()-1970;
var a_Date = a_DateTimeGMT.getUTCDate()-1;
var retDateTime = "P"+a_Year+"Y"+a_DateTimeGMT.getUTCMonth()+"M"+a_Date+"DT"
					+a_DateTimeGMT.getUTCHours()+"H"+a_DateTimeGMT.getUTCMinutes()+"M"+a_DateTimeGMT.getUTCSeconds()+"S";
return retDateTime;
}
function scutils_makeLearnerResponse(a_pageNumber, a_interactionType)
{
 var t_interactionLearnerResponse = "";
 if(a_interactionType == "choice")
	{
	var commaResponse = 0;
	for (var i = 0; i < g_radioDynamicEntries.length; ++i)
  		{
    		var pageNr = g_radioDynamicEntries[i].nPage;
    				
			if (pageNr == a_pageNumber)
			{
				var idx = g_radioDynamicEntries[i].nIdx;
				var choiceChecked = eval("document.TestingForm.Radio" + a_pageNumber);
				if (choiceChecked[idx].checked && commaResponse == 0) 
					{
					t_interactionLearnerResponse += g_Responses.charAt(g_radioDynamicEntries[i].nIdx); 
					commaResponse = 1;
					}
				else 
					if(choiceChecked[idx].checked && commaResponse==1) 
						t_interactionLearnerResponse += "[,]" + g_Responses.charAt(g_radioDynamicEntries[i].nIdx);	

			}
		}	
	}
   else
	if(a_interactionType == "fill-in")
	   {
	   var commaResponse = 0;
	
	   for (var i = 0; i < g_textFieldEntries.length; ++i)
  	      {
    		var pageNr = g_textFieldEntries[i].nPage;
    		if (pageNr == a_pageNumber)
			{			
			 var textFieldObj = eval("document.TestingForm." + g_textFieldEntries[i].strId);
			 if(commaResponse == 0)
			    {
			     t_interactionLearnerResponse += textFieldObj.value;
			     commaResponse =1;
			    }
			 else
			     t_interactionLearnerResponse += "[,]" + textFieldObj.value;
			}	
		}

	   }
	   	else
	if(a_interactionType == "matching")
	{
	var commaResponse = 0;
	var index = 0;
	
	while(index < g_targetPointEntries.length && g_targetPointEntries[index].nPage != a_pageNumber)
		index++;
	
	var count = index;
	while(count < g_targetPointEntries.length && g_targetPointEntries[count].nPage == a_pageNumber)
		count++;
										  
	var vector = new Array();
	for(var k = index; k < count; k++)
		vector[k] = parseInt(g_targetPointEntries[k].nX);
	
	for(var k = index; k < count-1; k++)
		for(var l = k+1; l < count; l++)
		  if(vector[l] < vector[k])
		  {
		  	var temp = vector[l];
		  	vector[l] = vector[k]
		  	vector[k] = temp;
		  }	  
		 		
		for (var i = 0; i < g_targetPointEntries.length; ++i)
  		{
    		var pageNr = g_targetPointEntries[i].nPage;
    		if (pageNr == a_pageNumber)
    		{						
				var strLayer1 = "" + g_targetPointEntries[i].strObjectId + "Layer";
      			        var layer1 = document.getElementById(strLayer1);
				
				var startX1 = posX(layer1);
  				var startY1 = posY(layer1);
  				var endX1 = startX1 + layer1.clientWidth-1;
  				var endY1 = startY1+ layer1.clientHeight-1;
				
				var j=0;
				var k=0;
				var bIsInside = false;
				while(j < g_targetPointEntries.length)
				{
					var tpX = g_targetPointEntries[j].nCenterX + layout_getSlidesLeft();
  					var tpY = g_targetPointEntries[j].nCenterY + layout_getSlidesTop();
					bIsInside = tpX >= startX1 && tpX <= endX1 && tpY >= startY1 && tpY <= endY1;
					
					if(bIsInside && commaResponse == 0)
				 	{
						while(k < count && g_targetPointEntries[j].nX != vector[k])
							k ++;				
						t_interactionLearnerResponse += "" + i +'[.]' + g_Responses.charAt(k);//g_targetPointEntries[j].strObjectId;	
						commaResponse =1;
						break;
				 	}
				 	else
				 	if(bIsInside && commaResponse == 1)
				 	{
				 		while(k < count && g_targetPointEntries[j].nX != vector[k])
							k ++;		
						t_interactionLearnerResponse += "[,]" + i +'[.]' + g_Responses.charAt(k);//g_targetPointEntries[j].strObjectId;
						break;
				 	}
					else j++;
				}
				if(!bIsInside && commaResponse == 0) 
				{
					t_interactionLearnerResponse += "" + i +'[.]'+ '-';
				 	commaResponse =1;
				}
				else
				if(!bIsInside && commaResponse == 1) 
					t_interactionLearnerResponse += "[,]" + i +'[.]'+ '-';
			}
		}
	}
return t_interactionLearnerResponse;
}

function scutils_makeInteractionPattern(a_pageNumber, a_interactionType)
{
var t_interactionPattern = ""; 
    if(g_interactionType == "choice")
	{
	var commaPattern = 0;
	for (var i = 0; i < g_radioDynamicEntries.length; ++i)
  		{
    		var pageNr = g_radioDynamicEntries[i].nPage;
    				
			if (pageNr == a_pageNumber)
				{
				if(g_radioDynamicEntries[i].bIsChecked && commaPattern == 0) 
					{
					var idx = g_radioDynamicEntries[i].nIdx;
					t_interactionPattern += "" + g_Responses.charAt(idx);
					commaPattern = 1;
					}
				else 
					if(g_radioDynamicEntries[i].bIsChecked && commaPattern == 1)
						{ 
						var idx = g_radioDynamicEntries[i].nIdx;
						t_interactionPattern += "[,]" + g_Responses.charAt(idx);
						}


				}
		}	
	}
   else
	if(g_interactionType == "fill-in")
	   {
	  var commaPattern = 0;	
	   for (var i = 0; i < g_textFieldEntries.length; ++i)
  	      {
    		var pageNr = g_textFieldEntries[i].nPage;
    		if (pageNr == a_pageNumber)
			{			
			if(commaPattern == 0)
				{	 
					var str = g_textFieldEntries[i].strText;
					t_interactionPattern += g_textFieldEntries[i].strText;
					commaPattern = 1;
				}
				else
					t_interactionPattern += "[,]" + g_textFieldEntries[i].strText;

			}	
		}

	   }
	   	else
	if(g_interactionType == "matching")
	{
	var commaPattern = 0;	
	var index = 0;
	
	while(index < g_targetPointEntries.length && g_targetPointEntries[index].nPage != a_pageNumber)
		index++;
	
	var count = index;
	while(count < g_targetPointEntries.length && g_targetPointEntries[count].nPage == a_pageNumber)
		count++;
										  
	var vector = new Array();
	for(var k = index; k < count; k++)
		vector[k] = parseInt(g_targetPointEntries[k].nX);
	
	for(var k = index; k < count-1; k++)
		for(var l = k+1; l < count; l++)
		  if(vector[l] < vector[k])
		  {
		  	var temp = vector[l];
		  	vector[l] = vector[k]
		  	vector[k] = temp;
		  }	  
		 		
		for (var i = 0; i < g_targetPointEntries.length; ++i)
  		{
    		var pageNr = g_targetPointEntries[i].nPage;
    		if (pageNr == a_pageNumber)
			{			
				var j=0;
				while(j < count && g_targetPointEntries[i].nX != vector[j])
					j ++;	
				if(commaPattern == 0)
				 	{
				 		t_interactionPattern += "" + i +'[.]'+ g_Responses.charAt(j);	
						commaPattern = 1;
				 	}
				else
					t_interactionPattern += "[,]" + i +'[.]'+ g_Responses.charAt(j);
		
				
			}
		}
	}

return t_interactionPattern;
}
function scutils_checkResumeSupported()
{
// sets the g_LMSResumeSupported
if(g_scormEnabled && g_scormLoaded )
	{
	var supportedInteractionsDM =  doLMSGetValue("cmi.interactions._children");
	var supportedScoreDM =  doLMSGetValue("cmi.score._children");
	if((supportedInteractionsDM != null)&&(supportedScoreDM != null))	
		if((supportedInteractionsDM.search("id")  != -1) &&
		   (supportedInteractionsDM.search("type") != -1) &&
		   (supportedInteractionsDM.search("learner_response") != -1) &&
		   (supportedInteractionsDM.search("result") != -1) &&
		   (supportedScoreDM.search("raw") != -1 ))
			   g_LMSResumeSupported = true;
		else
			alert("                                          Warning!\nYour Learning Management System dose not support course resuming.\n\nIf lesson is closed before finish all progress will be lost!");	
	}

}


