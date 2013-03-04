package imc.lecturnity.converter;

import imc.epresenter.filesdk.Questionnaire;
import imc.epresenter.player.whiteboard.InteractionArea;
import imc.epresenter.player.whiteboard.Picture;
import imc.epresenter.player.whiteboard.RadioDynamic;
import imc.epresenter.player.whiteboard.TextFieldDynamic;
import imc.epresenter.player.whiteboard.TargetPoint;
import imc.epresenter.player.whiteboard.Text;
import imc.lecturnity.converter.MyButtonArea;

public class DocumentInteractionInfo
{
   public SlideEntry[]       aSlideEntries;
   public TelepointerEntry[] aTelepointerEntries;
   public ThumbnailEntry[]   aThumbnailEntries;
   public InteractionArea[]  aInteractionAreas;
   public MyButtonArea[]     aMyButtonAreas;
   public RadioDynamic[]     aTestingRadioButtons;
   public TextFieldDynamic[] aTestingTextFields;
   public Picture[]          aTestingMoveablePictures;
   public TargetPoint[]      aTestingTargetPoints;
   public Text[]             aTestingChangeTexts;
   public Text[]             aTestingQuestionTexts;
   public Questionnaire[]    aQuestionnaires;
   public int[]              aStopmarkTimes;
   public int[]              aTargetmarkTimes;
   
}