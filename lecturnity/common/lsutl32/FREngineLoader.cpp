#include "StdAfx.h"
//
//// ( C ) 2008 ABBYY Software. All rights reserved.
//// ABBYY FineReader Engine 9.0 Sample
//
//// Helper functions for loading and unloading FREngine.dll
//
//#include "FREngineLoader.h"
//
//// Return full path to FRE dll
//static const char* GetFreDllPath() {
//    return "G:\\ABBYY FineReader Engine 9.0\\Bin\\FREngine.dll";
//}
//
//// Unicode version of the same function
//static wchar_t* GetFreDllPathU() {
//    return L"G:\\ABBYY FineReader Engine 9.0\\Bin\\FREngine.dll";
//}
//
//// Return developer serial number for FRE
//static wchar_t* GetFreDeveloperSN() {
//    return L"FEDT-9012-0002-3482-1350";
//}
//
//// Return full path to Samples directory
//static wchar_t* GetSamplesFolder() {
//    return L"G:\\ABBYY FineReader Engine 9.0\\Samples";
//}
//
//// char* version of the same function
//static const char* GetSamplesFolderChar() {
//    return "G:\\ABBYY FineReader Engine 9.0\\Samples";
//}
//
//#define VisualComponentsDllPath "G:\\ABBYY FineReader Engine 9.0\\Bin\\VisualComponents.dll"
//
//// Global FineReader Engine object.
//FREngine::IEnginePtr Engine;
//
//// HANDLE to FREngine.dll
//static HMODULE libraryHandle = 0;
//
////	This function loads and initializes FineReader Engine.
//void LoadFREngine()
//{
//	if( Engine != 0 ) {
//		// Already loaded
//		return;
//	}
//
//	// First step: load FREngine.dll
//	if( libraryHandle == 0 ) {
//
//		libraryHandle = LoadLibraryEx( ::GetFreDllPathU(), 0, LOAD_WITH_ALTERED_SEARCH_PATH );
//		if( libraryHandle == 0 ) {
//			throw L"Error while loading ABBYY FineReader Engine";			
//		}
//	}
//
//	// Second step: obtain FineReader Engine object
//	typedef HRESULT ( STDAPICALLTYPE* GetEngineObjectFunc )( BSTR, BSTR, BSTR, FREngine::IEngine** );
//	GetEngineObjectFunc pGetEngineObject = 
//		( GetEngineObjectFunc )GetProcAddress( libraryHandle, "GetEngineObject" );
//
//	if( pGetEngineObject == 0 || pGetEngineObject( ::GetFreDeveloperSN(), 0, 0, &Engine ) != S_OK ) {
//		UnloadFREngine();
//		throw L"Error while loading ABBYY FineReader Engine";		
//	}
//}
//
////	This function deinitializes and unloads FineReader Engine
//void UnloadFREngine()
//{
//	if( libraryHandle == 0 ) {
//		return;
//	}
//	
//	// Release Engine object
//	Engine = 0;
//
//	// Deinitialize FineReader engine
//	typedef HRESULT ( STDAPICALLTYPE* DeinitializeEngineFunc )();
//	DeinitializeEngineFunc pDeinitializeEngine = 
//		( DeinitializeEngineFunc )GetProcAddress( libraryHandle, "DeinitializeEngine" );
//
//	if( pDeinitializeEngine == 0 || pDeinitializeEngine() != S_OK ) {
//		throw L"Error while unloading ABBYY FineReader Engine";		
//	}
//
//	// Now it's safe to free the FREngine.dll library
//	FreeLibrary( libraryHandle );
//	libraryHandle = 0;
//}
//
//#include "..\drawsdk\objects.h"
//#include "ScreengrabbingExtended.h"
//
//// It seems that it's bug in Native COM Support.
//// Enum names are not prefixed by namespace in compiler-generated .tli file.
//// So we should say 'using' to them.
//using FREngine::BlockTypeEnum;
//
//// Forward declaration
//void clearStatistics();
//void calculateStatisticsForLayout( FREngine::ILayoutPtr layout );
//void calculateStatisticsForBlock( FREngine::IBlockPtr block );
//void calculateStatisticsForTextBlock( FREngine::ITextBlockPropertiesPtr textBlockProperties );
//void calculateStatisticsForTableBlock( FREngine::ITableBlockPropertiesPtr tableBlockProperties );
//void calculateStatisticsForText( FREngine::ITextPtr text );
//void calculateStatisticsForParagraph( FREngine::IParagraphPtr paragraph );
//void calculateCharStatisticsForParagraph( FREngine::IParagraphPtr paragraph );
//void calculateStatisticsForChar( FREngine::IParagraphPtr paragraph, int charIndex );
//void calculateWordStatisticsForParagraph( FREngine::IParagraphPtr paragraph );
//bool isUnrecognizedSymbol( char symbol );
//
////Statistics counters 
//
////Count of all suspicious symbols in layout
//int suspiciousSymbolsCount;
////Count of all unrecognized symbols in layout
//int unrecognizedSymbolsCount;
////Count of all nonspace symbols in layout
//int allSymbolsCount;
////Count of all words in layout
//int allWordsCount;
////Count of all not dictionary word in layout
//int notDictionaryWordsCount;
//// Text objects
//CList<DrawSdk::Text*,DrawSdk::Text*> *textObjects;
//
//CList<DrawSdk::Text*,DrawSdk::Text*>* GetTextFromImage(CString imageFilename)
//{           
//   TRACE("FR OCR : %ls\n", imageFilename);
//   // Load ABBYY FineReader Engine
//   LoadFREngine();
//
//   // Set parent window and caption for Engine's dialog and message boxes
//   //Engine->ParentWindow = ( long ) hDlg;
//   //Engine->ApplicationTitle = appTitle;
//
//   // Create document from image file
//   BSTR bstrFilename = imageFilename.AllocSysString();
//   _bstr_t imagePath(bstrFilename,TRUE);
//
//   FREngine::IFRDocumentPtr frDocument = Engine->CreateFRDocumentFromImage( imagePath, 0 );
//
//   //Recognize document
//   frDocument->Process( 0, 0, 0 );
//
//   // calculate statistics
//   calculateStatisticsForLayout( frDocument->Pages->Item( 0 )->Layout );
//
//   // Unload ABBYY FineReader Engine
//   //UnloadFREngine();
//
//   return textObjects;
//}
//
//void clearStatistics()
//{
//	suspiciousSymbolsCount = unrecognizedSymbolsCount = allSymbolsCount = allWordsCount =
//		notDictionaryWordsCount = 0;
//   if(textObjects)
//      textObjects->RemoveAll();
//}
//
//void calculateStatisticsForLayout( FREngine::ILayoutPtr layout )
//{
//	clearStatistics();
//			
//	FREngine::IBlocksCollectionPtr blocks = layout->Blocks;
//	for( int index = 0; index < blocks->Count; index++ ) {
//		calculateStatisticsForBlock( blocks->Item( index ) );
//	}
//}
//
//void calculateStatisticsForBlock( FREngine::IBlockPtr block )
//{			
//	if( block->Type == FREngine::BT_Text ) {			
//		calculateStatisticsForTextBlock( block->TextBlockProperties );
//	} else if( block->Type == FREngine::BT_Table ) {
//		calculateStatisticsForTableBlock( block->TableBlockProperties );
//	}
//}
//
//void calculateStatisticsForTextBlock( FREngine::ITextBlockPropertiesPtr textBlockProperties )
//{			
//	calculateStatisticsForText( textBlockProperties->Text );
//}
//
//void calculateStatisticsForTableBlock( FREngine::ITableBlockPropertiesPtr tableBlockProperties )
//{			
//	for( int index = 0; index < tableBlockProperties->Cells->Count; index++ ) {
//		calculateStatisticsForText( tableBlockProperties->Cells->Item( index )->Text );
//	}			
//}
//
//void calculateStatisticsForText( FREngine::ITextPtr text ) 
//{
//	FREngine::IParagraphsPtr paragraphs = text->Paragraphs;
//	for( int index = 0; index < paragraphs->Count; index++ ) {
//		calculateStatisticsForParagraph( paragraphs->Item( index ) );
//	}
//}
//
//void calculateStatisticsForParagraph( FREngine::IParagraphPtr paragraph )
//{	
//	calculateCharStatisticsForParagraph( paragraph );
//
//	calculateWordStatisticsForParagraph( paragraph );
//}
//
//void calculateCharStatisticsForParagraph( FREngine::IParagraphPtr paragraph )
//{	
//	for( int index = 0; index < paragraph->Text.length(); index++ )
//	{
//		calculateStatisticsForChar( paragraph, index );
//	}
//}
//
//void calculateStatisticsForChar( FREngine::IParagraphPtr paragraph, int charIndex )
//{
//	FREngine::ICharParamsPtr charParams = Engine->CreateCharParams();
//	paragraph->GetCharParams( charIndex, charParams );
//	if( charParams->IsSuspicious ) 
//	{
//		suspiciousSymbolsCount++;
//	}
//
//	if( isUnrecognizedSymbol( ( (const wchar_t*) paragraph->Text )[charIndex] ) ) 
//	{
//		unrecognizedSymbolsCount++;
//	}
//
//	if( ( (const wchar_t*) paragraph->Text )[charIndex] != ' ' ) 
//	{
//		allSymbolsCount++;
//	}
//}
//
//void calculateWordStatisticsForParagraph( FREngine::IParagraphPtr paragraph )
//{
//	allWordsCount += paragraph->Words->Count;
//
//	for( int index = 0; index < paragraph->Words->Count; index++ ) 
//	{
//		FREngine::IWordPtr word = paragraph->Words->Item( index );
//		if( !word->IsWordFromDictionary ) 
//		{
//			notDictionaryWordsCount ++;
//		}
//      FREngine::ICharParamsPtr charParams = Engine->CreateCharParams();
//      int top = 1000000;
//      int bottom = 0;
//      int left = 1000000;
//      int right = 0;
//      int fontSize = 0;
//
//      bool doPrint = true;
//      for( int i = 0; i < word->Text.length(); i++ ) 
//      {
//   	   paragraph->GetCharParams( word->FirstSymbolPosition+i, charParams );
//         if(charParams->Top < top)
//            top = charParams->Top;
//         if(charParams->Bottom > bottom)
//            bottom = charParams->Bottom;
//         if(charParams->Left < left)
//            left = charParams->Left;
//         if(charParams->Right > right)
//            right = charParams->Right;
//         fontSize = charParams->FontSize / 20; // Engine delivers twips which is 1/20 points
//         // TODO: maybe use average font size of all characters of current word
//      }
//
//         int nUtfSize = ::WideCharToMultiByte(CP_UTF8, 0, word->Text,word->Text.length(), NULL, 0, NULL, NULL);
//         char *szTemp = new char[nUtfSize + 1];
//         memset(szTemp, 0, nUtfSize + 1);
//         ::WideCharToMultiByte(CP_UTF8, 0, word->Text,word->Text.length(), szTemp, nUtfSize + 1, NULL, NULL);
//         TRACE("[%i,%i,%i,%i] Font:%i <%s>\n", left, top, right, bottom, fontSize, szTemp );
//
//         DrawSdk::Text *textObject = CscreengrabbingDoc::CreateTextObject(szTemp, left, top, right-left+1, bottom-top+1, fontSize);
//         if(!textObjects)
//            textObjects = new CList<DrawSdk::Text*,DrawSdk::Text*>();
//         if(textObjects)
//            textObjects->AddTail(textObject);
//
//         delete[] szTemp;
//   }
//}
//
//bool isUnrecognizedSymbol( char symbol )
//{
//	//it is special constant used by FREngine recogniser
//	return ( symbol == 0x005E );
//}
