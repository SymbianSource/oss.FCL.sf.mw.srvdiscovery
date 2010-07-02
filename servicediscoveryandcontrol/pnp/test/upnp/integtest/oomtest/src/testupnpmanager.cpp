// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Contains implementation of CTestUPnPManager class
// Contains implemenatation of CTestUPnPManager, CUPnPObserver, CUPnPTimer, 
// and CAction classes
// 
//

#include "testupnpmanager.h"


/*
  Constructor:
  @internalTechnology
  @test
*/
CTestUPnPManager::CTestUPnPManager()
:	CTestStep()
	{
	iCancelDiscovery = EFalse;
	iExpectedData = EFalse;
	iDiscoverySuccess = EFalse;
	iNotifyCount = 0;
	iPublishCount = 0;
	iDescribeResponseCount = 0;
	iRenewCount = 0;
	iFailedNumUris = 0;
	}

/*
   Initializes all member vaiables.
   @param		None.
   @return		None.
*/
void CTestUPnPManager::InitializeL()
	{
	iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	
	_LIT(KInfoLogFile, "SequenceOperationsL().... \n");
	INFO_PRINTF1(KInfoLogFile);
	TPtrC listOfSequences;
	_LIT(KListOfSequences, "listofsequences");
	GetStringFromConfig(ConfigSection(), KListOfSequences, listOfSequences);

	TokenizeStringL(listOfSequences, iSequenceList);
	iNumOperations = iSequenceList.Count();

	}

/*
   Destructor
   @internalTechnology
   @test
*/
CTestUPnPManager::~CTestUPnPManager()
	{
	}

void CTestUPnPManager::ClosePublisher()
	{
	_LIT(KInfoLogFile, "ClosePublisher....\n");
	INFO_PRINTF1(KInfoLogFile);
	CancelPublish();
	}

void CTestUPnPManager::CloseDiscoverer()
	{
	iControlPoint.Close();
	}

void CTestUPnPManager::CleanUpObserverArray()
	{
	_LIT(KInfoLogFile, "CleanUpObserverArray....\n");
	INFO_PRINTF1(KInfoLogFile);
	if (iObserverArray.Count() > 0 )
		{
		iObserverArray.ResetAndDestroy();
		}
	else
		{
		iObserverArray.Close();
		}
	_LIT(KInfoLogFile1, "CleanUpObserverArray End....\n");
	INFO_PRINTF1(KInfoLogFile1);
	}

void CTestUPnPManager::CleanUpResultsArray()
	{
	_LIT(KInfoLogFile, "CleanUpResultsArray....\n");
	INFO_PRINTF1(KInfoLogFile);
	TInt count = iResultsArray.Count();
		for (TInt index(0); index < count ; index++ )
			{
			iResultsArray[index]->Close();
			}
	iResultsArray.Close();
	_LIT(KInfoLogFile1, "CleanUpResultsArray End....\n");
	INFO_PRINTF1(KInfoLogFile1);
	}

void CTestUPnPManager::DescriptionSuiteCleanUp()
	{
	_LIT(KInfoLogFile, "DescriptionSuiteCleanUp....\n");
	INFO_PRINTF1(KInfoLogFile);
	TInt count = iDescriptionSuiteArray.Count();
	for (TInt index(0); index < count ; index++ )
		{
		iDescriptionSuiteArray[index].iBaseUrl.Close();
		iDescriptionSuiteArray[index].iDescription.Close();
		DeleteSeriveUrlsList(index);
		}
	iDescriptionSuiteArray.Close();
	_LIT(KInfoLogFile1, "DescriptionSuiteCleanUp End...\n");
	INFO_PRINTF1(KInfoLogFile1);
	}

void CTestUPnPManager::ScpdUriArrayCleanUp()
	{
	_LIT(KInfoLogFile, "ScpdUriArrayCleanUp....\n");
	INFO_PRINTF1(KInfoLogFile);
	TInt count = iScpdUriArray.Count();
	for (TInt index(0); index < count ; index++ )
		{
		iScpdUriArray[index].Close();
		}
	iScpdUriArray.Close();
	_LIT(KInfoLogFile1, "ScpdUriArrayCleanUp End....\n");
	INFO_PRINTF1(KInfoLogFile1);
	}

void CTestUPnPManager::EventUriArrayCleanUp()
	{
	_LIT(KInfoLogFile, "EventUriArrayCleanUp....\n");
	INFO_PRINTF1(KInfoLogFile);
	TInt count = iEventUriArray.Count();
	for (TInt index(0); index < count ; index++ )
		{
		iEventUriArray[index].Close();
		}
	iEventUriArray.Close();
	_LIT(KInfoLogFile1, "EventUriArrayCleanUp End....\n");
	INFO_PRINTF1(KInfoLogFile1);
	}

void CTestUPnPManager::ControlUriArrayCleanUp()
	{
	_LIT(KInfoLogFile, "ControlUriArrayCleanUp....\n");
	INFO_PRINTF1(KInfoLogFile);
	TInt count = iControlUriArray.Count();
	for (TInt index(0); index < count ; index++ )
		{
		iControlUriArray[index].Close();
		}
	iControlUriArray.Close();
	_LIT(KInfoLogFile1, "ControlUriArrayCleanUp End....\n");
	INFO_PRINTF1(KInfoLogFile1);
	}

/*
   Implementation of CTestStep base class virtual and this is used for doing all
   initialisation common
    to derived classes here.
   @param  None
   @return TVerdict
 */
TVerdict CTestUPnPManager::doTestStepPreambleL()
	{
	InitializeL();
	SavePreviousLogFileL();
	return CTestStep::doTestStepPreambleL();
	}

/*
 Save the previous log file...if any
*/
void CTestUPnPManager::SavePreviousLogFileL()
	{
	_LIT(KInfoLogFile, "Saving Previous logs If any.... \n");
	INFO_PRINTF1(KInfoLogFile);
	RFs fileserv;
	TInt error = fileserv.Connect();
	if(!error)
		{
		_LIT(KlogFile, "C:\\logs\\log.txt");
		_LIT(KPrefixLog, "C:\\upnplogs\\");
		_LIT(KSuffix, "_UDEB");
		_LIT(KExtn, ".txt");
		
		CFileMan* fMan = NULL;
		TRAP_IGNORE(fMan = CFileMan::NewL(fileserv));
		if(fMan != NULL)
			{
			CleanupStack::PushL(fMan);
			RBuf newFileName;
			const TInt KTwentyFour = 24;
			newFileName.Create(ConfigSection().Length() + KTwentyFour );
			newFileName.Copy(KPrefixLog);
			//Script file name
			iScriptName.Copy(ConfigSection());
			newFileName.Append(iScriptName);
			newFileName.Append(KSuffix);
			newFileName.Append(KExtn);
			
			TInt error = fMan->Rename(KlogFile, newFileName);
			if (error != KErrNone)
				{
				_LIT(KErrInfo, "Unable to Save Previous logs...May be NO Previous logs exist \n");
				ERR_PRINTF1(KErrInfo);
				}
			else
				{
				_LIT(KSavingInfo, "<font color=0000ff><b>Saved Previous logs with File Name: <u>%S</u> </b></font> \n");
				INFO_PRINTF2(KSavingInfo, &newFileName);
				}
			CleanupStack::PopAndDestroy(fMan);
			newFileName.Close();
			}
		}
	fileserv.Close();
	_LIT(KInfoLogFile1, "Saving Previous logs End.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	}

/*
 return Script File name..if any
*/
const TDesC& CTestUPnPManager::ScriptFileName()
	{
	return iScriptName;
	}

/*
   Implementation of CTestStep base class virtual and it is used for doing all
   after test treatment common to derived classes in here.
   @return TVerdict
 */
TVerdict CTestUPnPManager::doTestStepPostambleL()
	{
		_LIT(KInfoDesctructorBegin, "Begin: In Destructor CTestUpnPanager....\n");
	INFO_PRINTF1(KInfoDesctructorBegin);
	if(iStringPoolMgr)
		{
		iStringPoolMgr->Release();
		//this will tkae care of clean up of iStringPool
		}
	_LIT(KInfoDesctructorBegin2, "Releasing String Pool Manager....\n");
	INFO_PRINTF1(KInfoDesctructorBegin2);
		
	CleanUpObserverArray();
	iDeviceLocationArray.Close();
	CleanUpResultsArray();
	DescriptionSuiteCleanUp();
	ScpdUriArrayCleanUp();
	ControlUriArrayCleanUp();
	EventUriArrayCleanUp();
	CloseDiscoverer();
	//#ifdef _DEBUG
		ClosePublisher();
		CActiveScheduler::Install(NULL); //Testing for ONB hangs
		_LIT(KInfoDesctructorBegin1, "Deleting the Active Schedulers....\n");
		INFO_PRINTF1(KInfoDesctructorBegin1);
		delete iScheduler;
		iScheduler = NULL;	
	//#endif //_Debug

	iPublisherObserverMap.Close();	
	_LIT(KInfoDesctructorEnd, "End: ....In Destructor CTestUpnPanager\n");
	INFO_PRINTF1(KInfoDesctructorEnd);

	return CTestStep::doTestStepPostambleL();
	}


TVerdict CTestUPnPManager::doTestStepL()
	{
	SequenceOperationsL();
	return TestStepResult();
	}

/*
   This function performs operations based on sequence specified in the ini file
   @param  None
   @return None
*/
void CTestUPnPManager::SequenceOperationsL()
	{
	for(TInt i(0); i < iNumOperations; i++)
		{
		SetCurrentSection(iSequenceList[i]);
		TPtrC operationType;
		_LIT(KOperationType, "operationtype");
		GetStringFromConfig(iSequenceList[i], KOperationType, operationType);
		PerformDesiredOperationsL(iSequenceList[i], operationType);
		}
	}

/*
   This function performs desired operations specified as a parameter
   @param  aOperation string representing operation to be performed
   @return None
*/
void CTestUPnPManager::PerformDesiredOperationsL(const TDesC& aOperation, const TDesC& operationType)
	{
	const TInt KMatch = 0;
//	TPtrC operationType;
//	_LIT(KOperationType, "operationtype");
//	GetStringFromConfig(aOperation, KOperationType, operationType);
	
	if(operationType.Compare(KOpenDiscoverer) == KMatch)
		{
		DoTestOOML(EOpenDiscover);
		}
	else if(operationType.Compare(KDiscovery) == KMatch)
		{
		TPtrC listOfServiceTypes;
		TPtrC listOfMxTimes;
		_LIT(KListOfServiceTypes, "listofservicetypes");
		_LIT(KListOfMxTimes, "listofmxtimes");
		if(!GetStringFromConfig(GetCurrentSection(), KListOfServiceTypes, listOfServiceTypes)
			|| !GetStringFromConfig(GetCurrentSection(), KListOfMxTimes, listOfMxTimes))
			{
			_LIT(KErrInfo, "Problem in reading values from ini. \n Expected fields are: \n%S, \n%S \n");
			ERR_PRINTF3( KErrInfo, &KListOfServiceTypes, &KListOfMxTimes );
			//SetTestStepResult(EFail);
			return;
			}
		TokenizeStringL(listOfServiceTypes, iServiceTypeList);
		TokenizeStringL(listOfMxTimes, iMxTimeList);

		DoTestOOML(EDiscover);
		}
	else if(operationType.Compare(KCancelDiscover) == KMatch)
		{
		CancelDiscoverL(aOperation);
		}
	else if(operationType.Compare(KCancelDescribe) == KMatch)
		{
		CancelDescribeL(aOperation);
		}
	else if(operationType.Compare(KPublishService) == KMatch)
		{
		TPtrC iServiceType;
		GetStringFromConfig(aOperation, KServiceType, iServiceType);

		TPtrC iUsn;
		GetStringFromConfig(aOperation, KUSN, iUsn);
		
		TInt iDuration;
		GetIntFromConfig(aOperation, KCacheControl, iDuration);
		
		TPtrC iDescription;
		GetStringFromConfig(aOperation, KDescription, iDescription);
		
		TPtrC descriptionPath;
		_LIT(KDescriptionPath, "Description_Path");
		GetStringFromConfig(aOperation, KDescriptionPath, iDescriptionPath);

		DoTestOOML(EPublishService);
//		PublishServiceL(aOperation);
		}
	else if(operationType.Compare(KPublishDevice) == KMatch)
		{
		GetStringFromConfig(aOperation, KParentDeviceUid, iParentdeviceuid);
		GetStringFromConfig(aOperation, KDeviceType, iDevicetype);
		GetStringFromConfig(aOperation, KFriendlyName, iFriendlyname);
		GetStringFromConfig(aOperation, KManufacturer, iManufacturer);
		GetStringFromConfig(aOperation, KManufacturerURL, iManufacturerurl);
		GetStringFromConfig(aOperation, KModelDescription, iModeldescription);
		GetStringFromConfig(aOperation, KModelName, iModelname);
		GetStringFromConfig(aOperation, KModelNumber, iModelnumber);
		GetStringFromConfig(aOperation, KModelURL, iModelurl);
		GetStringFromConfig(aOperation, KPresentationURL, iPresentationurl);
		GetStringFromConfig(aOperation, KSerialNumber, iSerialnumber);
		GetStringFromConfig(aOperation, KUDN, iUdn);
		GetStringFromConfig(aOperation, KUPC, iUpc);
		GetIntFromConfig(aOperation, KCacheControl, iDuration);
		DoTestOOML(EPublishDevice);
		}
	else if(operationType.Compare(KDescribe) == 0)
		{
		DescribeL();
		}
	else if(operationType.Compare(KDescribeService) == 0)
		{
		DescribeServiceL();
		}
	else if(operationType.Compare(KSubscribe) == 0)
		{
		SubscribeForStateChangeNotificationsL();
		}
	else if(operationType.Compare(KUnsubscribe) == 0)
		{
		UnsubscribeForStateChangeNotificationsL(aOperation);
		}
	else if(operationType.Compare(KRegisterAnnouncement) == KMatch)
		{
		RegisterForAnnouncementsL(aOperation);
		}
	else if(operationType.Compare(KCancelNotify) == KMatch)
		{
		CancelNotifyAnnouncementL(aOperation);
		}
	else if(operationType.Compare(KInitiateControl) == KMatch)
		{
		InitiateActionL(aOperation);
		}
	else if(operationType.Compare(KStartScheduler) == KMatch)
		{
		CActiveScheduler::Start();
		}
	else if(operationType.Compare(KStopScheduler) == KMatch)
		{
		CActiveScheduler::Stop();
		}
	else if(operationType.Compare(KStartTimer) == KMatch)
		{
		StartTimerL(aOperation);
		}
	else if(operationType.Compare(KPrintResults) == 0)
		{
		PrintAndValidateResultsL(aOperation);
		}
	}

/*
   Parses a comma separated string and constructs a list out of the values
   @param	aString a reference to a string to be tokenized
   @param	aList is an out parameter to store a list of tokens in an arry
   @param	aSeparator is a character used to delimit the tokens
  */
void CTestUPnPManager::TokenizeStringL(const TDesC& aString, RArray<TPtrC>& aList, TChar aSeparator)
	{
	TLex lexer(aString);

	while(!lexer.Eos())
		{
		lexer.SkipSpaceAndMark();

		while(!lexer.Eos() && lexer.Get() != aSeparator)
			{
			}

		if(!lexer.Eos())
			{
			lexer.UnGet();
			}
		aList.AppendL(lexer.MarkedToken());
		lexer.Inc();
		}
	}

/*
   Starts a timer for a wait time and triggers CallBack function as timer expires.
   @param	None
   @return	None
 */
void CTestUPnPManager::StartTimerL(const TDesC& aOperationType)
	{
	_LIT(KInfoLogFile, "StartTimerL.... \n");
	INFO_PRINTF1(KInfoLogFile);
	_LIT(KWaitTime,"waittime");
	TInt waitTime = 0;
	GetIntFromConfig(aOperationType, KWaitTime, waitTime);
//	TTimeIntervalMicroSeconds32 interval(waitTime);
	CUPnPTimer* timer = CUPnPTimer::NewL(this);
	CleanupStack::PushL(timer);
	timer->IssueRequestL(waitTime);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(timer);
	_LIT(KInfoLogFile1, "StartTimerL End.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	}

/*
   Sets the current config section.
   @param	aCurrentSection - a reference string that represent current config section
   @return	None
 */
void CTestUPnPManager::SetCurrentSection(const TDesC& aCurrentSection)
	{
	iCurrentSection = aCurrentSection;
	}

/*
   Retrieve the current config section.
   @param
   @return	Reference to current config section
 */
TDesC& CTestUPnPManager::GetCurrentSection()
	{
	return	iCurrentSection;
	}

/*
	Tests whether discovering services/devices is performing as specified in UPnP specifications.
	@param		aOperationType is reference to a section name in ini file where required parameters
				needs to be referred for this operation.
	@return		None.
 */
void CTestUPnPManager::DiscoverL()
	{
	CUPnPObserver* upnpObserver = CUPnPObserver::NewL(this);
	iObserverArray.AppendL(upnpObserver);
	CPnPParameterBundle* pnpBundle = CPnPParameterBundle::NewL( );
	CleanupClosePushL( *pnpBundle );
	pnpBundle->SetPnPObserver((MPnPObserver*)upnpObserver);

	CUPnPParameterFamily* pnpFamily = CUPnPParameterFamily::NewL ( *pnpBundle , EUPnPDiscoverRequestParamSet );
	TInt numUris = iServiceTypeList.Count();
	iFailedNumUris = iServiceTypeList.Count(); //This should be of Unique Uri's count
	TInt numMxs = iMxTimeList.Count();
	TLex responseTimeObj;
	RBuf8 buf;
	buf.CreateL(KMaxUriSize);
	for(TInt i(0); i < numUris; i++)
		{
		CUPnPDiscoverRequestParamSet* discoverParamSet = CUPnPDiscoverRequestParamSet::NewL(pnpFamily );
		buf.Copy(iServiceTypeList[i]);
		discoverParamSet->SetUriL( buf );
		//response time
		responseTimeObj.Assign(iMxTimeList[i]);
		TInt responseTime(0);
		responseTimeObj.Val(responseTime);
		discoverParamSet->SetResponseTime(responseTime);
		}
	iControlPoint.Discover(*pnpBundle);
	CActiveScheduler::Start();

	buf.Close();
	iMxTimeList.Close();
	iServiceTypeList.Close();
	CleanupStack::PopAndDestroy( pnpBundle );
	}

TBool CTestUPnPManager::IsDiscoverySuccess()
	{
	return iDiscoverySuccess;
	}

void CTestUPnPManager::SetDiscoveryStatus(TBool aSuccess)
	{
	_LIT(KInfoDiscoverSetStatus, "Setting iDiscoverySuccesss Flag to %d \n");
	INFO_PRINTF2(KInfoDiscoverSetStatus, aSuccess );
	iDiscoverySuccess = aSuccess;
	}

/*
	Tests whether Publishing a Service/Device is performing as specified in UPnP specifications.
	@param			aOperationType is reference to a section name in ini file where required parameters
					needs to be referred for this operation.
	@return			None.
 */
void CTestUPnPManager::PublishServiceL ()
	{
	CUPnPObserver* upnpObserver = CUPnPObserver::NewL(this);
	iObserverArray.Append(upnpObserver);
	//To keep track of mapping between Observer and Publisher
	TInt observerIndex = iObserverArray.Count() - 1;
	CPnPParameterBundle* pnpBundle = CPnPParameterBundle::NewL( );
    CleanupClosePushL( *pnpBundle );
    pnpBundle->SetPnPObserver((MPnPObserver*)upnpObserver);
	CUPnPParameterFamily* pnpFamily = CUPnPParameterFamily::NewL (*pnpBundle , EUPnPServiceRegisterParamSet );
   	CUPnPServiceRegisterParamSet* registerServiceParamSet = CUPnPServiceRegisterParamSet::NewL(pnpFamily );
	RBuf8 serviceTypeBuf;
	serviceTypeBuf.Create(iServiceType.Length());
	serviceTypeBuf.Copy(iServiceType);

	RBuf8 usnBuf;
	usnBuf.Create(iUsn.Length());
	usnBuf.Copy(iUsn);

	RBuf8 descriptionBuf;
	descriptionBuf.Create(iDescription.Length());
	descriptionBuf.Copy(iDescription);

	registerServiceParamSet->SetUriL ( serviceTypeBuf );
	registerServiceParamSet->SetDeviceUidL ( usnBuf );
	registerServiceParamSet->SetCacheControlData ( iDuration );
	registerServiceParamSet->SetSubscriptionDuration(300);
	
	ExtractServiceDescriptionL (*registerServiceParamSet);

	_LIT8(KInitialMessage, "Initial Service Message");
	registerServiceParamSet->SetInitialMessageL(KInitialMessage);

	//To keep track of mapping between Observer and Publisher
	TInt publisherIndex = OpenPublisherL();
	//Create mapping of Publsiher and observer
	TPublisherObserverMap publisherObserverMapElement;
	publisherObserverMapElement.iPublisherIndex = publisherIndex;
	publisherObserverMapElement.iObserverIndex =  observerIndex;
	publisherObserverMapElement.iPublisherControl = KService;
	iPublisherObserverMap.Append(publisherObserverMapElement);

	//Publish service nad update publish count
	iPublishCount++;
	iPublisherArray[publisherIndex].Publish(*pnpBundle);

	descriptionBuf.Close();
	usnBuf.Close();
	serviceTypeBuf.Close();
	CleanupStack::PopAndDestroy( pnpBundle );
	}
	
/*
 * Get publisher mapping index for the given observer index
 */
TInt CTestUPnPManager::GetPublisherIndex (TInt aObserverIndex)
	{
	_LIT(KInfoLogFile, "GetPublisherIndex.... \n");
	INFO_PRINTF1(KInfoLogFile);
	
	TInt count = iPublisherObserverMap.Count();
	for (TInt index(0); index < count ; index++ )
		{
		if (iPublisherObserverMap[index].iObserverIndex == aObserverIndex)
			{
			return iPublisherObserverMap[index].iPublisherIndex;
			}
		}
	return KErrNotFound;
	}

/*
 * Gets publisher control i.e, Device/Service for the given observer index
 */
TPublisherControl CTestUPnPManager::GetPublisherControl (TInt aObserverIndex)
	{
	_LIT(KInfoLogFile, "GetPublisherControl.... \n");
	INFO_PRINTF1(KInfoLogFile);
	
	TInt count = iPublisherObserverMap.Count();
	for (TInt index(0); index < count ; index++ )
		{
		if (iPublisherObserverMap[index].iObserverIndex == aObserverIndex)
			{
			return iPublisherObserverMap[index].iPublisherControl;
			}
		}
	return KErrNoControl;
	}

/*
 * Gets publisher control i.e, Device/Service for the given observer index
 */
TPublisherControl CTestUPnPManager::PublisherType (TInt aPublisherIndex)
	{
	_LIT(KInfoLogFile, "PublisherType.... \n");
	INFO_PRINTF1(KInfoLogFile);
	TInt count = iPublisherObserverMap.Count();
	TPublisherControl publisherControl = KErrNoControl;
	for (TInt index(0); index < count ; index++ )
		{
		if (iPublisherObserverMap[index].iPublisherIndex == aPublisherIndex)
			{
			switch (iPublisherObserverMap[index].iPublisherControl)
				{
				case KService:
					{
					publisherControl = KService;
					break;
					}
				case KDevice:
					{
					publisherControl = KDevice;
					break;
					}
				default :
					{
					publisherControl = KErrNoControl;
					break;
					}
				}
			}
		}
	_LIT(KInfoLogFile2, "PublisherType....End \n");
	INFO_PRINTF1(KInfoLogFile2);
	return publisherControl;
	}

TInt CTestUPnPManager::NotifyCount()
	{
	return iNotifyCount;
	}

TBool CTestUPnPManager::SendNotificationsL (CUPnPObserver* aObserver)
	{
	_LIT(KInfoLogFile, "SendNotificationsL.... \n");
	INFO_PRINTF1(KInfoLogFile);
	
    TInt observerIndex = iObserverArray.Find(aObserver);
    TInt publisherIndex = GetPublisherIndex(observerIndex);
    TPublisherControl publisherControl = GetPublisherControl(observerIndex);

    if(publisherControl != KService)
    	{
    	_LIT(KSendNotifyErr,"SEND NOTIFY NOT EXECUTED -- Can be Executed only ON Services...\n");
    	ERR_PRINTF1(KSendNotifyErr);
    	return EFalse;
    	}

    CPnPParameterBundle* pnpBundle = CPnPParameterBundle ::NewL( );
    CleanupClosePushL( *pnpBundle );
	pnpBundle->SetPnPObserver((MPnPObserver*)this);

    CUPnPParameterFamily* pnpFamily = CUPnPParameterFamily::NewL (*pnpBundle , EUPnPServiceNotificationParamSet);
    CUPnPServiceNotificationParamSet* ServiceNotifyParamSet = CUPnPServiceNotificationParamSet::NewL (pnpFamily );

	RBuf8 notificationDataBuf;
	notificationDataBuf.CreateL(_L8("notification"));

	ServiceNotifyParamSet->SetNotificationL (notificationDataBuf );
	iNotifyCount++;
	if(publisherIndex >= 0)
		{
		iPublisherArray[publisherIndex].SendNotify(*pnpBundle );
		_LIT(KSendNotify,"Info: SEND NOTIFY SUCCESSFUL...\n");
		INFO_PRINTF1(KSendNotify);
		}
	else
		{
		_LIT(KSendNotifyFailed,"Err: SEND NOTIFY UNSUCCESSFUL...\n");
		ERR_PRINTF1(KSendNotifyFailed);
		}

	CleanupStack::PopAndDestroy( pnpBundle );
	notificationDataBuf.Close();
	_LIT(KInfoLogFile1, "SendNotificationsL End.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	
	return ETrue;
	}

/*
	Extract service description available in a file, copy it to the buffer and append the same to service register paramset.
	@param			aConfigSection - reference to a section name in ini file where required parameters that needs to be referred for this operation.
					aServiceRegisterParamSet - Storage for the description buffer
	@return			None.
 */

void CTestUPnPManager::ExtractServiceDescriptionL (CUPnPServiceRegisterParamSet& aServiceRegisterParamSet)
	{
	if((iDescriptionPath == KNullDesC) && (iDescription == KNullDesC)) 
		{
		_LIT8(KDefaultServiceDescription, "defaultservicedescription");
		TBuf8<KMaxBufLength> descBuf;
		descBuf.Copy(KDefaultServiceDescription);
		aServiceRegisterParamSet.SetServiceDescriptionL (descBuf);
		return;
		}
	else
		{
		RFs fs;
		RFile file;
		TInt err;
		RBuf8 buf;
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
		if(iDescriptionPath != KNullDesC)
			{
			err = file.Open(fs, iDescriptionPath, EFileShareReadersOnly);
			}
		else 
			{
			err = file.Open(fs, iDescription, EFileShareReadersOnly);
			}

		if (err != KErrNone)
			{
			User::LeaveIfError(err);
			}
		CleanupClosePushL(file);
		TInt fileSize = 0;
		file.Size(fileSize);
		buf.Create(fileSize);
		err = file.Read(buf, fileSize);
		aServiceRegisterParamSet.SetServiceDescriptionL ( buf );
		_LIT(KServiceDescription, "ServiceDescription is.... %S \n");
		INFO_PRINTF2(KServiceDescription, &buf);
		CleanupStack::PopAndDestroy(2);
		buf.Close();
		}
	_LIT(KInfoLogFile1, "ExtractServiceDescriptionL() End.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	}

void CTestUPnPManager::ResolveScpdUrisL(TInt aIndex)
	{
	_LIT(KInfoLogFile, "ResolveScpdUrisL .... \n");
	INFO_PRINTF1(KInfoLogFile);
	if ( aIndex < 0 )
		{
		_LIT(KBadIndex, "BadIndex...ResolveScpdUrisL Cannot be executed \n");
		ERR_PRINTF1(KBadIndex);
		return;
		}
	const TInt KSlashLenghth = 1;
	TUriParser8 uri;
	uri.Parse(iDescriptionSuiteArray[aIndex].iBaseUrl);
	TBuf8<KMaxUriSize> buf;
	buf.Copy(uri.Extract(EUriScheme));

	buf.Append(KColonSlashSlash);
	buf.Append(uri.Extract(EUriHost));

	buf.Append(KColon);
	buf.Append(uri.Extract(EUriPort));

	TInt count = iDescriptionSuiteArray[aIndex].iServiceUrls.Count();
	for (TInt scpdIndex(0); scpdIndex < count; scpdIndex++ )
		{
		RBuf8 scpdUri;
		scpdUri.Create(buf.Length() + KSlashLenghth + iDescriptionSuiteArray[aIndex].iServiceUrls[scpdIndex].scpdUrl.Length());
		scpdUri.Copy(buf);
		 if (iDescriptionSuiteArray[aIndex].iServiceUrls[scpdIndex].scpdUrl.Find(KSlash)!=0)
			 {
			 scpdUri.Append(KSlash);
			 }
		 scpdUri.Append(iDescriptionSuiteArray[aIndex].iServiceUrls[scpdIndex].scpdUrl);
		 AppendScpdUriL(scpdUri);
		}
	_LIT(KInfoLogFile1, "ResolveScpdUrisL End .... \n");
	INFO_PRINTF1(KInfoLogFile1);
	}

void CTestUPnPManager::ResolveEventUrisL(TInt aIndex)
	{
	_LIT(KInfoLogFile, "ResolveEventUrisL.... \n");
	INFO_PRINTF1(KInfoLogFile);
	if ( aIndex < 0 )
		{
		_LIT(KBadIndex, "BadIndex...ResolveEventUrisL Cannot be executed \n");
		ERR_PRINTF1(KBadIndex);
		return;
		}
	const TInt KSlashLenghth = 1;
	TUriParser8 uri;
	uri.Parse(iDescriptionSuiteArray[aIndex].iBaseUrl);
	TBuf8<KMaxUriSize> buf;
	buf.Copy(uri.Extract(EUriScheme));

	buf.Append(KColonSlashSlash);
	buf.Append(uri.Extract(EUriHost));

	buf.Append(KColon);
	buf.Append(uri.Extract(EUriPort));

	TInt count = iDescriptionSuiteArray[aIndex].iServiceUrls.Count();
	for (TInt eventIndex(0); eventIndex < count; eventIndex++ )
		{
		RBuf8 eventUri;
		eventUri.Create(buf.Length() + KSlashLenghth + iDescriptionSuiteArray[aIndex].iServiceUrls[eventIndex].eventSubUrl.Length());
		eventUri.Copy(buf);
		 if (iDescriptionSuiteArray[aIndex].iServiceUrls[eventIndex].eventSubUrl.Find(KSlash)!=0)
			 {
			 eventUri.Append(KSlash);
			 }
		 eventUri.Append(iDescriptionSuiteArray[aIndex].iServiceUrls[eventIndex].eventSubUrl);
		 AppendEventUriL(eventUri);
		}
	_LIT(KInfoLogFile1, "ResolveEventUrisL End.... \n"); 
	INFO_PRINTF1(KInfoLogFile1);
		
	}

void CTestUPnPManager::ResolveControlUrisL(TInt aIndex)
	{
	_LIT(KInfoLogFile, "ResolveControlUrisL().... \n");
	INFO_PRINTF1(KInfoLogFile);
	if ( aIndex < 0 )
		{
		_LIT(KBadIndex, "BadIndex...ResolveControlUrisL Cannot be executed \n");
		ERR_PRINTF1(KBadIndex);
		return;
		}
	const TInt KSlashLenghth = 1;
	TUriParser8 uri;
	uri.Parse(iDescriptionSuiteArray[aIndex].iBaseUrl);
	TBuf8<KMaxUriSize> buf;
	buf.Copy(uri.Extract(EUriScheme));

	buf.Append(KColonSlashSlash);
	buf.Append(uri.Extract(EUriHost));

	buf.Append(KColon);
	buf.Append(uri.Extract(EUriPort));

	TInt count = iDescriptionSuiteArray[aIndex].iServiceUrls.Count();
	for (TInt ControlIndex(0); ControlIndex < count; ControlIndex++ )
		{
		RBuf8 controlUri;
		controlUri.Create(buf.Length() + KSlashLenghth + iDescriptionSuiteArray[aIndex].iServiceUrls[ControlIndex].controlUrl.Length());
		controlUri.Copy(buf);
		 if (iDescriptionSuiteArray[aIndex].iServiceUrls[ControlIndex].controlUrl.Find(KSlash)!=0)
			 {
			 controlUri.Append(KSlash);
			 }
		 controlUri.Append(iDescriptionSuiteArray[aIndex].iServiceUrls[ControlIndex].controlUrl);
		 AppendControlUriL(controlUri);
		}
	_LIT(KInfoLogFile1, "ResolveControlUrisL() End.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	}

/*
	Opens a handle to a RPnPServiceDiscovery object
	@param			None.
	@return			None.
 */
void CTestUPnPManager::OpenDiscovererL()
	{
	User::LeaveIfError(iControlPoint.Open(KTierId));
	}

/*
	Opens a handle to a RPnPServicePublisher object and append the same to Publisher array
	@param			None.
	@return			TBool representing success or failure of the operation.
 */
TInt CTestUPnPManager::OpenPublisherL()
	{
  	RPnPServicePublisher publisher;
	TInt openStatus = 0;
	User::LeaveIfError(publisher.Open(KTierId));
	iPublisherArray.AppendL(publisher);
	TInt index = iPublisherArray.Count() - 1;
	return index;
	}
	
/*
	Publishing of device as per UPnP specifications.
	@param			aOperationType is reference to a section name in ini file where required parameters
					needs to be referred for this operation.
	@return			None.
 */

void CTestUPnPManager::PublishDeviceL ()
	{
	CUPnPObserver* upnpObserver = CUPnPObserver::NewL(this);
	iObserverArray.AppendL(upnpObserver);

	//To keep track of mapping between Observer and Publisher
	TInt observerIndex = iObserverArray.Count() - 1;

	CPnPParameterBundle* pnpBundle = CPnPParameterBundle::NewL( );
    CleanupClosePushL( *pnpBundle );
	pnpBundle->SetPnPObserver((MPnPObserver*)upnpObserver);
	CUPnPParameterFamily* pnpFamily = CUPnPParameterFamily::NewL (*pnpBundle , EUPnPDeviceRegisterParamSet );
	CUPnPDeviceRegisterParamSet* registerDeviceParamSet = CUPnPDeviceRegisterParamSet::NewL(pnpFamily );

	RBuf8 parentdeviceuidBuf;
	parentdeviceuidBuf.CreateL(iParentdeviceuid.Length());
	parentdeviceuidBuf.Copy(iParentdeviceuid);

	RBuf8 devicetypeBuf;
	devicetypeBuf.CreateL(iDevicetype.Length());
	devicetypeBuf.Copy(iDevicetype);

	RBuf8 friendlynameBuf;
	friendlynameBuf.CreateL(iFriendlyname.Length());
	friendlynameBuf.Copy(iFriendlyname);

	RBuf8 manufacturerBuf;
	manufacturerBuf.CreateL(iManufacturer.Length());
	manufacturerBuf.Copy(iManufacturer);

	RBuf8 manufacturerurlBuf;
	manufacturerurlBuf.CreateL(iManufacturerurl.Length());
	manufacturerurlBuf.Copy(iManufacturerurl);

	RBuf8 modeldescriptionBuf;
	modeldescriptionBuf.CreateL(iModeldescription.Length());
	modeldescriptionBuf.Copy(iModeldescription);

	RBuf8 modelnameBuf;
	modelnameBuf.CreateL(iModelname.Length());
	modelnameBuf.Copy(iModelname);

	RBuf8 modelnumberBuf;
	modelnumberBuf.CreateL(iModelnumber.Length());
	modelnumberBuf.Copy(iModelnumber);


	RBuf8 modelurlBuf;
	modelurlBuf.CreateL(iModelurl.Length());
	modelurlBuf.Copy(iModelurl);

	RBuf8 presentationurlBuf;
	presentationurlBuf.CreateL(iPresentationurl.Length());
	presentationurlBuf.Copy(iPresentationurl);

	RBuf8 serialnumberBuf;
	serialnumberBuf.CreateL(iSerialnumber.Length());
	serialnumberBuf.Copy(iSerialnumber);

	RBuf8 udnBuf;
	udnBuf.CreateL(iUdn.Length());
	udnBuf.Copy(iUdn);

	RBuf8 upcBuf;
	upcBuf.CreateL(iUpc.Length());
	upcBuf.Copy(iUpc);


	TInt publisherIndex = OpenPublisherL();
	registerDeviceParamSet-> SetParentDeviceUidL(parentdeviceuidBuf);
	registerDeviceParamSet-> SetUriL (devicetypeBuf);
	registerDeviceParamSet-> SetFriendlyNameL (friendlynameBuf);
	registerDeviceParamSet-> SetManufacturerL (manufacturerBuf );
	registerDeviceParamSet-> SetManufactureUrlL (manufacturerurlBuf);
	registerDeviceParamSet-> SetModelDescL (modeldescriptionBuf);
	registerDeviceParamSet-> SetModelNameL (modelnameBuf);
	registerDeviceParamSet-> SetModelNumberL (modelnumberBuf);
	registerDeviceParamSet-> SetModelUriL (modelurlBuf);
	registerDeviceParamSet-> SetPresentationL (presentationurlBuf);
	registerDeviceParamSet-> SetUDNL (udnBuf);
	registerDeviceParamSet-> SetUPCL (upcBuf);
	registerDeviceParamSet-> SetCacheControlData (iDuration);
	registerDeviceParamSet-> SetSerialNumberL (serialnumberBuf);

	//To keep track of mapping between Observer and Publisher
	//Create mapping of Publsiher and observer
	TPublisherObserverMap publisherObserverMapElement;
	publisherObserverMapElement.iPublisherIndex = publisherIndex;
	publisherObserverMapElement.iObserverIndex =  observerIndex;
	publisherObserverMapElement.iPublisherControl = KDevice;
	iPublisherObserverMap.AppendL(publisherObserverMapElement);

	//Publish Device and update publish count
	iPublishCount++;
	iPublisherArray[publisherIndex].Publish(*pnpBundle);

	CleanUpObserverArray();
	upcBuf.Close();
	udnBuf.Close();
	serialnumberBuf.Close();
	presentationurlBuf.Close();
	modelurlBuf.Close();
	modelnumberBuf.Close();
	modelnameBuf.Close();
	manufacturerurlBuf.Close();
	manufacturerBuf.Close();
	friendlynameBuf.Close();
	devicetypeBuf.Close();
	parentdeviceuidBuf.Close();
	CleanupStack::PopAndDestroy( pnpBundle );
	_LIT(KInfoLogFile1, "PublishDeviceL End.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	}

/*
Test for Register announcements ssdp:alive and ssdp:byebye messages
	@param			aOperationType is reference to a section name in ini file where required parameters
					needs to be referred for this operation.
	@return			None.
*/
void  CTestUPnPManager::RegisterForAnnouncementsL (const TDesC& aOperationType)
	{
	_LIT(KInfoLogFile, "RegisterForAnnouncementsL().... \n");
	INFO_PRINTF1(KInfoLogFile);
	RArray<TPtrC> uriTypeList;
	TPtrC listOfUriTypes;
	_LIT(KListOfUriTypes, "listofuritypes");
	CUPnPObserver* upnpObserver = CUPnPObserver::NewL(this);
	iObserverArray.Append(upnpObserver);
	CPnPParameterBundle* pnpBundle = CPnPParameterBundle::NewL( );
	CleanupClosePushL( *pnpBundle );
	pnpBundle->SetPnPObserver((MPnPObserver*)upnpObserver);

	CUPnPParameterFamily* pnpFamily = CUPnPParameterFamily::NewL ( *pnpBundle , EUPnPRegisterAnnouncementParamSet );
	if(!GetStringFromConfig(GetCurrentSection(), KListOfUriTypes, listOfUriTypes))
		{
		_LIT(KErrInfo, "Problem in reading values from ini. \n Expected fields are: \n%S, \n%S \n");
		ERR_PRINTF2( KErrInfo, &KListOfUriTypes);
		return;
		}

	TokenizeStringL(listOfUriTypes, uriTypeList);
	TInt numUris = uriTypeList.Count();
	RBuf8 buf;
	buf.Create(KMaxUriSize);
	for(TInt i(0); i < numUris; i++)
		{
		CUPnPRegisterAnnouncementParamSet* announcementParamSet = CUPnPRegisterAnnouncementParamSet::NewL(pnpFamily );
		buf.Copy(uriTypeList[i]);
		announcementParamSet->SetUriL( buf );
		}
	buf.Close();
	uriTypeList.Close();
	iControlPoint.RegisterNotify(*pnpBundle );
	TBool schedulerStart = EFalse;
	_LIT(KSchedulerStart, "schedulerstart");
	GetBoolFromConfig(aOperationType, KSchedulerStart, schedulerStart);
	if(schedulerStart)
		{
		CActiveScheduler::Start();
		}
	CleanupStack::PopAndDestroy(pnpBundle);
	_LIT(KInfoLogFile1, "RegisterForAnnouncementsL() End.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	
	
	}

/*
	Tests whether subscribtion to event change notifications is performing as specified in UPnP specifications.
	@param			None.
	@return			None.
 */
void  CTestUPnPManager::SubscribeForStateChangeNotificationsL()
	{
	_LIT(KInfoLogFile, "SubscribeForStateChangeNotificationsL() start.... \n");
	INFO_PRINTF1(KInfoLogFile);
	CUPnPObserver* upnpObserver = CUPnPObserver::NewL(this);
	CleanupStack::PushL( upnpObserver );
	CPnPParameterBundle* pnpBundle = CPnPParameterBundle::NewL( );
    CleanupClosePushL( *pnpBundle );
	pnpBundle->SetPnPObserver((MPnPObserver*)upnpObserver);
	CUPnPParameterFamily* pnpFamily = CUPnPParameterFamily::NewL (*pnpBundle , EUPnPSubscribeRequestParamSet);
	TInt subscriptionDuration;
    _LIT(KSubscriptionDuration, "subscriptionduration");
	GetIntFromConfig(GetCurrentSection(), KSubscriptionDuration, subscriptionDuration);
	TBool readIni = EFalse;
	GetBoolFromConfig(GetCurrentSection(), KReadIni, readIni);
	if(readIni)
		{
		CUPnPSubscribeRequestParamSet * subscribeParamSet = CUPnPSubscribeRequestParamSet::NewL (pnpFamily );
		TPtrC subscriptionUri;
		GetStringFromConfig(GetCurrentSection(), KSubscriptionUri, subscriptionUri);
		RBuf8 subscriptionUriBuf;
		subscriptionUriBuf.Create(subscriptionUri.Length());
		subscriptionUriBuf.Copy(subscriptionUri);
		subscribeParamSet->SetDuration(subscriptionDuration);
		subscribeParamSet->SetUriL(subscriptionUriBuf);
		subscriptionUriBuf.Close();
		}
	else
		{
		//Multiple uri's can be submitted for description
		ResolveAllUrisL();
		TInt eventUriCount = iEventUriArray.Count();
		if ( eventUriCount==0 )
			{
			_LIT(KEventSubscription,"Event Subscription Cannot be performed \n");
			INFO_PRINTF1(KEventSubscription);
			CleanupStack::PopAndDestroy( pnpBundle );
			CleanupStack::PopAndDestroy( upnpObserver );
			//SetTestStepResult(EFail);
			return;
			}
		for(TInt index(0); index < eventUriCount ; index++)
			{
			//for (TInt j(0); j < 10 ; j++)
			//{//Pass Duplicate subscritption reuqest multiple times in single bundle
				CUPnPSubscribeRequestParamSet * subscribeParamSet = CUPnPSubscribeRequestParamSet::NewL (pnpFamily );
				subscribeParamSet->SetUriL(iEventUriArray[index]);
				subscribeParamSet->SetDuration(subscriptionDuration);
				RBuf eventUri16;
				eventUri16.Create(iEventUriArray[index].Length());
				eventUri16.Copy(iEventUriArray[index]);
				_LIT(KEventSubUriInfo,"Event Uri Submitted for Subscription Request is: %S \n");
				INFO_PRINTF2(KEventSubUriInfo, &eventUri16);
				eventUri16.Close();
			//	}
			}
		}
	iControlPoint.Subscribe( *pnpBundle );
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy( pnpBundle );
	CleanupStack::PopAndDestroy( upnpObserver );
	_LIT(KInfoLogFile1, "SubscribeForStateChangeNotificationsL() End.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	}

void  CTestUPnPManager::ReSubscribeForStateChangeNotificationsL()
	{
	_LIT(KInfoLogFile, "Re-SubscribeForStateChangeNotificationsL() start.... \n");
	INFO_PRINTF1(KInfoLogFile);
	CUPnPObserver* upnpObserver = CUPnPObserver::NewL(this);
	CleanupStack::PushL( upnpObserver );
	CPnPParameterBundle* pnpBundle = CPnPParameterBundle::NewL( );
    CleanupClosePushL( *pnpBundle );
	pnpBundle->SetPnPObserver((MPnPObserver*)upnpObserver);
	CUPnPParameterFamily* pnpFamily = CUPnPParameterFamily::NewL (*pnpBundle , EUPnPSubscribeRequestParamSet);
	TInt subscriptionDuration;
    _LIT(KSubscriptionDuration, "subscriptionduration");
	GetIntFromConfig(GetCurrentSection(), KSubscriptionDuration, subscriptionDuration);
	
	TInt eventUriCount = iEventUriArray.Count();
	if ( eventUriCount==0 )
		{
		_LIT(KEventSubscription,"Event Subscription Cannot be performed \n");
		INFO_PRINTF1(KEventSubscription);
		CleanupStack::PopAndDestroy( pnpBundle );
		CleanupStack::PopAndDestroy( upnpObserver );
		//SetTestStepResult(EFail);
		return;
		}
	for(TInt index(0); index < eventUriCount ; index++)
		{
		CUPnPSubscribeRequestParamSet * subscribeParamSet = CUPnPSubscribeRequestParamSet::NewL (pnpFamily );
		subscribeParamSet->SetUriL(iEventUriArray[index]);
		subscribeParamSet->SetDuration(20);
		RBuf eventUri16;
		eventUri16.Create(iEventUriArray[index].Length());
		eventUri16.Copy(iEventUriArray[index]);
		_LIT(KEventSubUriInfo,"Event Uri Submitted for Re-Subscription  is: %S \n");
		INFO_PRINTF2(KEventSubUriInfo, &eventUri16);
		eventUri16.Close();
		}
	iControlPoint.Subscribe( *pnpBundle );
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy( pnpBundle );
	CleanupStack::PopAndDestroy( upnpObserver );
	_LIT(KInfoLogFile1, "Re-SubscribeForStateChangeNotificationsL() End.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	}
/*
	Tests whether unsubscribtion to subscribed events is performing as specified in UPnP specifications.
	@param			None
	@return			None.
 */
void  CTestUPnPManager::UnsubscribeForStateChangeNotificationsL(const TDesC& aOperationType)
	{
	_LIT(KInfoLogFile, "UnsubscribeForStateChangeNotificationsL().... \n");
	INFO_PRINTF1(KInfoLogFile);
	
	CUPnPObserver* upnpObserver = CUPnPObserver::NewL(this);
	iObserverArray.Append(upnpObserver);
	CPnPParameterBundle* pnpBundle = CPnPParameterBundle::NewL();
	CleanupClosePushL( *pnpBundle );
	pnpBundle->SetPnPObserver((MPnPObserver*)upnpObserver);
	CUPnPParameterFamily* pnpFamily = CUPnPParameterFamily::NewL (*pnpBundle , EUPnPCancelSubscribeParamSet);
	TBool readIni = EFalse;
	GetBoolFromConfig(aOperationType, KReadIni, readIni);
	if(readIni)
		{
	CUPnPCancelSubscribeParamSet * unsubscribeParamSet = CUPnPCancelSubscribeParamSet::NewL(pnpFamily);
	TPtrC unsubscribepath;
	_LIT(KUnsubscribePath, "unsubscribepath");
		GetStringFromConfig(aOperationType, KUnsubscribePath, unsubscribepath);
	RBuf8 unsubscribepathBuf;
	unsubscribepathBuf.Create(unsubscribepath.Length());
	unsubscribepathBuf.Copy(unsubscribepath);
	unsubscribeParamSet->SetUriL(unsubscribepathBuf);
		_LIT(KCanCelSubscribeInfo,"Executing Cancel Subscribe for URI:  %S \n");
		INFO_PRINTF2(KCanCelSubscribeInfo, &unsubscribepath);
		iControlPoint.Cancel(*pnpBundle );
	unsubscribepathBuf.Close();
	}
	else
	{
		TInt eventUriCount = iEventUriArray.Count();
		if ( eventUriCount==0 )
			{
			_LIT(KEventUnSubscription,"Event UnSubscription Cannot be performed \n");
			INFO_PRINTF1(KEventUnSubscription);
			CleanupStack::PopAndDestroy( pnpBundle );
			return;
			}
		for(TInt index(0); index < eventUriCount ; index++)//eventUriCount
			{
			CUPnPCancelSubscribeParamSet * unsubscribeParamSet = CUPnPCancelSubscribeParamSet::NewL(pnpFamily);
			unsubscribeParamSet ->SetUriL(iEventUriArray[index]);
			RBuf eventUri16;
			eventUri16.Create(iEventUriArray[index].Length());
			eventUri16.Copy(iEventUriArray[index]);
			_LIT(KEventSubUriInfo,"Event Uri Submitted for UnSubscription Request is: %S \n");
			INFO_PRINTF2(KEventSubUriInfo, &eventUri16);
			eventUri16.Close();
			}
		iControlPoint.Cancel(*pnpBundle );
		}
	CleanupStack::PopAndDestroy( pnpBundle );
	_LIT(KInfoLogFile1, "UnsubscribeForStateChangeNotificationsL() End.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	}

/*
	Test for Intiatting a Control Action from Control Point side
	@param			aOperationType is reference to a section name in ini file where required parameters
					needs to be referred for this operation.
	@return			None.
 */
void CTestUPnPManager::InitiateActionL(const TDesC& /* aOperationType */)
	{
	_LIT(KInfoLogFile, "InitiateActionL().... \n");
	INFO_PRINTF1(KInfoLogFile);
	ResolveAllUrisL();
	if(!iAction)
		{
		iAction= CAction::NewL(this);
		}
	for(TInt i(0); i < iControlUriArray.Count(); i++)
		{
		RBuf controlUri16;
		controlUri16.Create(iControlUriArray[i].Length());
		controlUri16.Copy(iControlUriArray[i]);
		_LIT(KContolUri, "<font color=00ff00><b><u>Control Uri Submitted is: %S </b></u></font>");
		INFO_PRINTF2(KContolUri, &controlUri16);
		iAction->InitiateActionControlL(iControlUriArray[i],iControlPoint);
		controlUri16.Close();		
		}
	_LIT(KInfoLogFile1, "InitiateActionL() End.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	}

/*
	Test for Cancelling Discovery Operation
	@param			aOperationType is reference to a section name in ini file where required parameters
					needs to be referred for this operation.
	@return			None.
 */
void CTestUPnPManager::CancelDiscoverL(const TDesC& aOperationType)
	{
	iCancelDiscovery = ETrue;
	const TInt KBufLength = 1000;
	CUPnPObserver* upnpObserver = CUPnPObserver::NewL(this);
	iObserverArray.Append(upnpObserver);
	CPnPParameterBundle* pnpBundle = CPnPParameterBundle::NewL( );
    CleanupClosePushL( *pnpBundle );
	pnpBundle->SetPnPObserver((MPnPObserver*)upnpObserver);
	CUPnPParameterFamily* pnpFamily = CUPnPParameterFamily::NewL ( *pnpBundle , EUPnPCancelDiscoverParamSet );
	CUPnPCancelDiscoverParamSet* cancelDiscoverParamSet = CUPnPCancelDiscoverParamSet::NewL(pnpFamily );
	TPtrC serviceType;
	GetStringFromConfig(aOperationType, KServiceType, serviceType);
	RBuf8 buf;
	buf.Create(KBufLength);
	buf.Copy(serviceType);
	cancelDiscoverParamSet->SetUriL( buf );
	iControlPoint.Cancel (*pnpBundle );
	buf.Close();
	CleanupStack::PopAndDestroy( pnpBundle );
	
	TBool flag=ETrue;
	_LIT(KStopScheduler,"stopscheduler");
	GetBoolFromConfig(aOperationType,KStopScheduler,flag);
	if(flag)		
		{
		CActiveScheduler::Stop();	
		}
	_LIT(KInfoLogFile1, "CancelDiscoverL() End.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	}

void CTestUPnPManager::CancelDescribeL(const TDesC& aOperationType)
	{
	//Modify this later
	const TInt KBufLength = 1000;
	_LIT(KInfoLogFile, "CancelDescribeL().... \n");
	INFO_PRINTF1(KInfoLogFile);
	CUPnPObserver* upnpObserver = CUPnPObserver::NewL(this);
	iObserverArray.Append(upnpObserver);
	CPnPParameterBundle* pnpBundle = CPnPParameterBundle::NewL( );
    CleanupClosePushL( *pnpBundle );
	pnpBundle->SetPnPObserver((MPnPObserver*)upnpObserver);
	CUPnPParameterFamily* pnpFamily = CUPnPParameterFamily::NewL ( *pnpBundle , EUPnPCancelDescribeParamSet );
	CUPnPCancelDescribeParamSet* cancelDescribeParamSet = CUPnPCancelDescribeParamSet::NewL(pnpFamily );
	TPtrC serviceType;
	GetStringFromConfig(aOperationType, KServiceType, serviceType);
	_LIT(KCanCelDiscoveryInfo,"Executing Cancel Describe for URI:  %S \n");
	INFO_PRINTF2(KCanCelDiscoveryInfo, &serviceType);
	RBuf8 buf;
	buf.Create(KBufLength);
	buf.Copy(serviceType);
	cancelDescribeParamSet->SetUriL( buf );
	iControlPoint.Cancel (*pnpBundle );
	buf.Close();
	CleanupStack::PopAndDestroy( pnpBundle );
	_LIT(KInfoLogFile1, "CancelDescribeL() End.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	}

/*
 Cancells all persistent Device Description Requests.
*/
void CTestUPnPManager::CancelAllDeviceDescribeL()
	{
	_LIT(KInfoLogFile, "CancelAllDeviceDescribeL().... \n");
	INFO_PRINTF1(KInfoLogFile);
	
	//this perofrms only Cancellation of all Device descriptions only
	CUPnPObserver* upnpObserver = CUPnPObserver::NewL(this);
	iObserverArray.Append(upnpObserver);
	CPnPParameterBundle* pnpBundle = CPnPParameterBundle::NewL( );
    CleanupClosePushL( *pnpBundle );
	pnpBundle->SetPnPObserver((MPnPObserver*)upnpObserver);
	CUPnPParameterFamily* pnpFamily = CUPnPParameterFamily::NewL ( *pnpBundle , EUPnPCancelDescribeParamSet );
	TInt DevLocCount = iDeviceLocationArray.Count();
	for(TInt index(0); index < DevLocCount  ; index++)
		{
		TBuf8<256> url;
		TUriParser8 uri;
		uri.Parse(iDeviceLocationArray[index]);
		url.Copy(_L8("http://"));
		const TDesC8& scheme = uri.Extract(EUriScheme);
		_LIT8(KScheme, "http");
		//Uri must be of scheme "http" only
		if (scheme.CompareF(KScheme) == 0)
			{
			const TDesC8& host = uri.Extract(EUriHost);
			url.Append(host);
			url.Append(_L8(":"));
			const TDesC8& portStr = uri.Extract ( EUriPort );
			url.Append(portStr);
			url.Append(_L8("/")) ;
			CUPnPCancelDescribeParamSet* cancelDescribeParamSet = CUPnPCancelDescribeParamSet::NewL(pnpFamily );
			cancelDescribeParamSet->SetUriL(url);
			RBuf uri16;
			uri16.Create(url.Length());
			uri16.Copy(url);
			_LIT(KCanCelDescribeInfo,"Executing Cancel Describe for URI:  %S \n");
			INFO_PRINTF2(KCanCelDescribeInfo, &uri16);
			uri16.Close();
			}
		}
	iControlPoint.Cancel (*pnpBundle );
	CleanupStack::PopAndDestroy( pnpBundle );
	CActiveScheduler::Stop();
	_LIT(KInfoLogFile1, "CancelAllDeviceDescribeL() End.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	
	}

/*
 * this perofrms only Cancellation of all Service descriptions only
*/
void CTestUPnPManager::CancelAllServiceDescribeL()
	{
	_LIT(KInfoLogFile, "CancelAllServiceDescribeL().... \n");
	INFO_PRINTF1(KInfoLogFile);
	
	CUPnPObserver* upnpObserver = CUPnPObserver::NewL(this);
	iObserverArray.Append(upnpObserver);
	CPnPParameterBundle* pnpBundle = CPnPParameterBundle::NewL( );
    CleanupClosePushL( *pnpBundle );
	pnpBundle->SetPnPObserver((MPnPObserver*)upnpObserver);
	CUPnPParameterFamily* pnpFamily = CUPnPParameterFamily::NewL ( *pnpBundle , EUPnPCancelDescribeParamSet );
	TInt scpdCount = iScpdUriArray.Count();
	for(TInt index(0); index < scpdCount  ; index++)
		{
		RBuf uri16;
		uri16.Create(iScpdUriArray[index].Length());
		uri16.Copy(iScpdUriArray[index]);
		_LIT(KCanCelDescribeServiceInfo,"Executing Cancel DescribeService for URI:  %S \n");
		INFO_PRINTF2(KCanCelDescribeServiceInfo, &uri16);
		uri16.Close();
		CUPnPCancelDescribeParamSet* cancelDescribeParamSet = CUPnPCancelDescribeParamSet::NewL(pnpFamily );
		cancelDescribeParamSet->SetUriL(iScpdUriArray[index]);
		}
	iControlPoint.Cancel (*pnpBundle );
	CleanupStack::PopAndDestroy( pnpBundle );
	CActiveScheduler::Stop();
	_LIT(KInfoLogFile1, "CancelAllServiceDescribeL() End.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	
	}

/*
	Test for Cancelling Register Announcement
	@param			aOperationType is reference to a section name in ini file where required parameters
					needs to be referred for this operation.
	@return			None.
 */
void  CTestUPnPManager::CancelNotifyAnnouncementL(const TDesC& aOperationType)
	{
	_LIT(KInfoLogFile, "CancelNotifyAnnouncementL().... \n");
	INFO_PRINTF1(KInfoLogFile);
	CUPnPObserver* upnpObserver = CUPnPObserver::NewL(this);
	iObserverArray.Append(upnpObserver);
	CPnPParameterBundle* pnpBundle = CPnPParameterBundle::NewL( );
	 CleanupClosePushL( *pnpBundle );
	pnpBundle->SetPnPObserver((MPnPObserver*)upnpObserver);
	CUPnPParameterFamily* pnpFamily = CUPnPParameterFamily::NewL (*pnpBundle , EUPnPCancelRegisterNotifyParamSet);
	CUPnPCancelRegisterNotifyParamSet * cancelNotifyParamSet = CUPnPCancelRegisterNotifyParamSet::NewL(pnpFamily);
	TPtrC uri;
	GetStringFromConfig(aOperationType, KUri, uri);
	RBuf8 buf;
	buf.Create(uri.Length());
	buf.Copy(uri);
	cancelNotifyParamSet->SetUriL(buf);
	iControlPoint.Cancel (*pnpBundle);
	buf.Close();
	CleanupStack::PopAndDestroy(pnpBundle);
	_LIT(KInfoLogFile1, "CancelNotifyAnnouncementL() End.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	}

void CTestUPnPManager::AppendBundlesL(CPnPParameterBundleBase* aPnPBundle)
	{
	iResultsArray.AppendL(aPnPBundle);
	}

/*
   Validates the discovery response received from the callback against the expected discovery response.
   @param	aIndex is an integer refers to an index in the ResultsArray of CResults object.
   @param	aResultsList is an array for section names for expected results section.
   @param	aExpctedResults is an array of Bools and each of the element indicates whether the expected results
   			in each section are matched against actual results.
   @return	None
 */

void CTestUPnPManager::PrintAndValidateResultsL(const TDesC& aOperationType)
	{
	_LIT(KInfoLogFile, "PrintAndValidateResultsL().... \n");
	INFO_PRINTF1(KInfoLogFile);
	TInt resultsCount = 0;
	const TInt bundleCount = iResultsArray.Count();
	_LIT(KPrintStatement, "printing results \n");
	_LIT(KPrintDesign,"	\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ \n");
	INFO_PRINTF1(KPrintStatement);
	INFO_PRINTF1(KPrintDesign);
	for(TInt i(0); i < bundleCount; i++)
		{
		CUPnPParameterFamily* paramFamily = static_cast<CUPnPParameterFamily*> ( iResultsArray[i]->GetFamilyByIndex() );
		if( paramFamily )
			{
			switch ( paramFamily->Id() )
				{
				case EUPnPDiscoverResponseParamSet:
					{
					PrintDiscoveryResultsL(paramFamily);
					ValidateDiscoveryResultsL(aOperationType);
					}
				break;

				case EUPnPPresenceAnnouncementParamSet:
					{
					TParamSetIter<CUPnPPresenceAnnouncementParamSet> iter;
					CUPnPPresenceAnnouncementParamSet* paramSet  = NULL;
					paramFamily->InitializeIter(iter);
					while ( !iter.AtEnd()  )
						{
						//Paramset at current position
						paramSet = iter();
						if(paramSet->UPnPEvent().iStatus == TUPnPEvent::EFail)
							{
							TInt error = paramSet->UPnPEvent().iErrorCode;
							_LIT(KRegisterAliveFailed,"Failed to Send Register Alive Notifications with...Error Code  = %d \n");
							ERR_PRINTF2(KRegisterAliveFailed, error);
							//SetTestStepResult(EFail); 
							++iter;
							continue;
							}
						TPtrC8 serviceLocation(paramSet->ServiceLocation());
						HBufC* devloc = HBufC::NewLC(serviceLocation.Length());	//1
						devloc->Des().Copy(serviceLocation);
						TPtrC devLocPointer (devloc->Des());
						_LIT(KInfoUri,"The URI found is %S \n");
						INFO_PRINTF2(KInfoUri, &devLocPointer);

						TInt availabilityDuration = 0;
						availabilityDuration = paramSet->Duration();
						_LIT(KInfoDuration,"The service availability duration is %d \n");
						INFO_PRINTF2(KInfoDuration, &availabilityDuration);

						_LIT(KInfoAlive,"ssdp:alive notification got from %S AND Duration is %d\n");
						INFO_PRINTF3(KInfoAlive, &devLocPointer, &availabilityDuration);

						TPtrC8 usn(paramSet->USN());
						HBufC* usnVal = HBufC::NewLC(usn.Length());	//2
						usnVal->Des().Copy(usn);
						TPtrC usnValPointer (usnVal->Des());
						_LIT(KInfoUSN,"The USN found is %S \n");
						INFO_PRINTF2(KInfoUSN, &usnValPointer);

						TPtrC8 uri(paramSet->Uri());
						HBufC* uriVal = HBufC::NewLC(uri.Length());	//3
						uriVal->Des().Copy(uri);
						TPtrC uriValPointer (uriVal->Des());
						_LIT(KInfoURI,"The Uri found is %S \n");
						INFO_PRINTF2(KInfoURI, &uriValPointer);
						resultsCount++;
						CleanupStack::PopAndDestroy(uriVal);
						CleanupStack::PopAndDestroy(usnVal);
						CleanupStack::PopAndDestroy(devloc);
						++iter;
						}
					}
				break;
				case EUPnPDescribeResponseParamSet:
					{
					TParamSetIter<CUPnPDescribeResponseParamSet> iter;
					CUPnPDescribeResponseParamSet* paramSet  = NULL;
					paramFamily->InitializeIter(iter);
					while ( !iter.AtEnd()  )
						{
						paramSet = iter();
						if(paramSet->UPnPEvent().iStatus == TUPnPEvent::EFail)
							{
							TInt error = paramSet->UPnPEvent().iErrorCode;
							_LIT(KDescribeFailed,"Failed to Describe with...Error Code  = %d \n");
							ERR_PRINTF2(KDescribeFailed, error);
							//SetTestStepResult(EFail); 
							return;
							}
						TPtrC8 description(paramSet->Description());
						HBufC* descriptionValue = HBufC::NewLC(description.Length());
						descriptionValue->Des().Copy(description);
						TPtrC descriptionPointer (descriptionValue->Des());
						_LIT(KInfoDescribe,"content is  %S \n");
						INFO_PRINTF2(KInfoDescribe, &descriptionPointer);
						CleanupStack::PopAndDestroy(descriptionValue);
						++iter;
						resultsCount++;
						}
					//Reached Eof the Bundles
					}
				break;
				case EUPnPAbsenceAnnouncementParamSet:
					{
					TParamSetIter<CUPnPAbsenceAnnouncementParamSet> iter;
					CUPnPAbsenceAnnouncementParamSet* paramSet  = NULL;
					paramFamily->InitializeIter(iter);

					while ( !iter.AtEnd()  )
						{
						//Paramset at current position
						paramSet = iter();
						if(paramSet->UPnPEvent().iStatus == TUPnPEvent::EFail)
							{
							TInt error = paramSet->UPnPEvent().iErrorCode;
							_LIT(KRegisterByeByeFailed,"Failed to Send Register ByeBye Notifications with...Error Code  = %d \n");
							ERR_PRINTF2(KRegisterByeByeFailed, error);
							//SetTestStepResult(EFail); 
							++iter;
							continue;
							}
						TPtrC8 usn(paramSet->USN());
						HBufC* usnVal = HBufC::NewLC(usn.Length());
						usnVal->Des().Copy(usn);
						TPtrC usnValPointer (usnVal->Des());

						_LIT(KInfoBye,"ssdp:byebye notification received from %S \n");
						INFO_PRINTF2(KInfoBye, &usnValPointer);
						resultsCount++;
						CleanupStack::PopAndDestroy(usnVal);
						++iter;
						}
					}
				break;
				case EUPnPSubscribeResponseParamSet:
					{
					TParamSetIter<CUPnPSubscribeResponseParamSet> iter;
					CUPnPSubscribeResponseParamSet* paramSet  = NULL;
					paramFamily->InitializeIter(iter);
					while ( !iter.AtEnd() )
						{
						//Paramset at current position
						paramSet = iter();
						RBuf subscriptionUri16;
						subscriptionUri16.Create(paramSet->Uri().Length());
						subscriptionUri16.Copy(paramSet->Uri());
						if(paramSet->UPnPEvent().iStatus == TUPnPEvent::EFail)
							{
							_LIT(KInfoSubscribeFailed, "Err: Failed to Subscribe.\n %S \n");
							INFO_PRINTF2(KInfoSubscribeFailed,&subscriptionUri16);
							TInt error = paramSet->UPnPEvent().iErrorCode;
							_LIT(KErrSubscribe,"Failed to Subscribe with...Error Code  = %d \n");
							ERR_PRINTF2(KErrSubscribe, error);
							//SetTestStepResult(EFail); 
							}
						else if(paramSet->UPnPEvent().iStatus == TUPnPEvent::ESubscriptionTimeout)
							{
							_LIT(KInfoSubscribeTimedOut, "Info: SubscribeTimed out \n %S \n");
							INFO_PRINTF2(KInfoSubscribeTimedOut,&subscriptionUri16);
							}
						else if(paramSet->UPnPEvent().iStatus == TUPnPEvent::ESuccess)
							{
							_LIT(KInfoSubscribeSuccess, "<font color=00ff00><b><u>Info: Subscribtion SUCCESS</u></b></font>\n %S \n");
							INFO_PRINTF2(KInfoSubscribeSuccess,&subscriptionUri16);
							}
						subscriptionUri16.Close();
						++iter;
						}
					}
				break;
				case EUPnPNotifyEventParamSet:
					{//receives event notifications sent at client
					TParamSetIter<CUPnPNotifyEventParamSet> iter;
					CUPnPNotifyEventParamSet* paramSet  = NULL;
					paramFamily->InitializeIter(iter);
					while ( !iter.AtEnd() )
						{
						//Paramset at current position
						paramSet = iter();
						RBuf subscriptionUri16;
						subscriptionUri16.Create(paramSet->Uri().Length());
						subscriptionUri16.Copy(paramSet->Uri());
						if(paramSet->UPnPEvent().iStatus == TUPnPEvent::EFail)
							{
							++iter;
							_LIT(KInfoNotificationsFailed, "Err: Failed to Get Event notifications \n %S \n");
							INFO_PRINTF2(KInfoNotificationsFailed,&subscriptionUri16);
							TInt error = paramSet->UPnPEvent().iErrorCode;
							_LIT(KNotificationsFailed,"Failed to Receive Notifications with...Error Code  = %d \n");
							ERR_PRINTF2(KNotificationsFailed, error);
							//SetTestStepResult(EFail); 
							}
						else if (paramSet->UPnPEvent().iStatus == TUPnPEvent::ESuccess)
							{
							_LIT(KInfoNotifications, "Notifications for subscribed Uri: %S \t is received \n");
							INFO_PRINTF2(KInfoNotifications, &subscriptionUri16);
							++iter;
							}
						subscriptionUri16.Close();
						}
					}
				break;
				default:
					{
					//Do Nothing
					}
				break;
				}
			}
		}
	_LIT(KEndPrintStatement,"Done with printing results \n");
	INFO_PRINTF1(KEndPrintStatement);
	INFO_PRINTF1(KPrintDesign);
	}

/*
  Constructor:
*/
CUPnPObserver::CUPnPObserver()
	:iSequence(0),iResultsCount(0)
	{
	}

/*
  Destructor
*/
CUPnPObserver::~CUPnPObserver()
	{
	iFileServ.Close();
	}


/*
	Static factory constructor. Uses two phase construction and leaves nothing on the
	CleanupStack. Creates a CTestControlPointService object.
	@param			aManager	A pointer to a CTestUPnPManager object.
	@return			A pointer to the newly created CPnPObserver object.
	@post			A fully constructed and initialized CPnPObserver object.
 */
CUPnPObserver* CUPnPObserver::NewL(CTestUPnPManager* aManager)
	{
	CUPnPObserver* self = new(ELeave) CUPnPObserver;
	CleanupStack::PushL(self);
	self->ConstructL(aManager);
	CleanupStack::Pop(self);
	return self;
	}

/*
	Second phase of two-phase construction method. Does any allocations required to fully construct
	the object.
	@pre 		First phase of construction is complete.
	@param		aManager	A pointer to a CTestUPnPManager object.
	@post		The object is fully constructed and initialized.
 */
void CUPnPObserver::ConstructL(CTestUPnPManager* aManager)
	{
	iManager = aManager;
	User::LeaveIfError(iFileServ.Connect());
	}



/*
   This method returns a reference to the CTestUPnPManager object.
   @param None
   return a reference to the CTestUPnPManager object
 */
CTestUPnPManager& CUPnPObserver::Manager()
	{
	return *iManager;
	}

void CTestUPnPManager::DoTestOOML (TInt aTestType)
	{
		TInt iTryCount = 1;
		TInt err=0;
//		__UHEAP_MARK;
		for ( ;; )
			{
			__UHEAP_SETFAIL(RHeap::EFailNext, iTryCount);
			
			switch(aTestType)
				{
			case EPublishDevice:
				TRAP( err, PublishDeviceL());
				break;
			case EOpenDiscover:
				TRAP(err,OpenDiscovererL());
				break;
			case EDiscover:
				TRAP(err, DiscoverL());
				break;
			case EPublishService:
				TRAP(err,PublishServiceL());
				break;
			default:
				break;
				}
					
			if ( err == KErrNone ) 
				{
				__UHEAP_RESET;
				INFO_PRINTF1(_L("Memory allocation testing completed. No of allocations "));
				break;
				}
			if ( err != KErrNoMemory )
				{	
				SetTestStepResult(EFail);
				User::Leave(err);
				}
			__UHEAP_SETFAIL(RHeap::ENone, 0);
			if(aTestType == EPublishDevice && iTryCount == 25)		
				{
				iTryCount = 30;
				}
			iTryCount++;
			}
//		__UHEAP_MARKEND;	
		INFO_PRINTF2(_L("TRYCOUNT %d"), iTryCount);
	}

/*
   This is called when callback hits. And this method stores the bundle results
   and meta information related to that results into results array maintained by manager
   for later verification.
   @param  aParameterBundle a reference to CPnPParameterBundleBase
   @return None
 */
void CUPnPObserver::OnPnPEventL ( CPnPParameterBundleBase& aParameterBundle)
	{
	_LIT(KInfoLogFile, "OnPnPEventL ().... \n");
	Manager().INFO_PRINTF1(KInfoLogFile);
	
	CopyResultBundlesL(aParameterBundle);
	CUPnPParameterFamily* paramFamily = static_cast<CUPnPParameterFamily*> ( aParameterBundle.GetFamilyByIndex() );
	if( paramFamily )
		{
		switch ( paramFamily->Id() )
			{
			case EUPnPDiscoverResponseParamSet:
				{
				TParamSetIter<CUPnPDiscoverResponseParamSet> iter;
				CUPnPDiscoverResponseParamSet* paramSet  = NULL;
				paramFamily->InitializeIter(iter);
				while (!iter.AtEnd())
					{
					paramSet = iter();
					//Printig of Search target
					TPtrC8 uriST(paramSet->Uri());
					HBufC* stUri = HBufC::NewLC(uriST.Length());
					stUri->Des().Copy(uriST);
					TPtrC uriStPtr (stUri->Des());
					CleanupStack::PopAndDestroy(stUri);
					
					if(paramSet->UPnPEvent().iStatus == TUPnPEvent::EDiscoveryTimeout)
						{
						CActiveScheduler::Stop();
						return;
						}
					}
				}
			break;
			case EUPnPPresenceAnnouncementParamSet:
				{
				TParamSetIter<CUPnPPresenceAnnouncementParamSet> iter;
				CUPnPPresenceAnnouncementParamSet* paramSet  = NULL;
				paramFamily->InitializeIter(iter);

				while ( !iter.AtEnd()  )
					{
					paramSet = iter();
					//Printig of Search target
					TPtrC8 uriST(paramSet->Uri());
					HBufC* stUri = HBufC::NewLC(uriST.Length());
					stUri->Des().Copy(uriST);
					TPtrC uriStPtr (stUri->Des());
					_LIT(KInfoST,"<b>The RegisterNotify URI was:  <u>%S</u> </b>\n");
					Manager().INFO_PRINTF2(KInfoST, &uriStPtr);
					CleanupStack::PopAndDestroy(stUri);
					
					if(paramSet->UPnPEvent().iStatus == TUPnPEvent::EFail)
						{
						TInt error = paramSet->UPnPEvent().iErrorCode;
						_LIT(KRegisterNotifyFail,"Resgitser Notify Failed: TUPnPEvent::EFail Error Code is = %d \n");
						Manager().ERR_PRINTF2(KRegisterNotifyFail,error);
						CActiveScheduler::Stop();
						//Manager().SetTestStepResult(EFail); 
						return;
						}
					TPtrC8 serviceLocation(paramSet->ServiceLocation());
					HBufC* servloc = HBufC::NewLC(serviceLocation.Length()); //1
					servloc->Des().Copy(serviceLocation);
					TPtrC servlocPointer (servloc->Des());
					TInt duration = paramSet->Duration();
					_LIT(KInfoAlive,"<font color=1f6633><b>ssdp:alive notification got from %S AND Duration is %d </b></font>\n");
					iManager->INFO_PRINTF3(KInfoAlive, &servlocPointer, &duration);
					CleanupStack::PopAndDestroy(servloc);
					++iter;
					iResultsCount++;
					}
				_LIT(KInfoLogFile, "EUPnPPresenceAnnouncementParamSet.... \n");
				Manager().INFO_PRINTF1(KInfoLogFile);
				}
			break;

			case EUPnPAbsenceAnnouncementParamSet:
				{
				TParamSetIter<CUPnPAbsenceAnnouncementParamSet> iter;
				CUPnPAbsenceAnnouncementParamSet* paramSet  = NULL;
				paramFamily->InitializeIter(iter);

				while ( !iter.AtEnd()  )
					{
					paramSet = iter();
					//Printig of Search target
					TPtrC8 uriST(paramSet->Uri());
					HBufC* stUri = HBufC::NewLC(uriST.Length());
					stUri->Des().Copy(uriST);
					TPtrC uriStPtr (stUri->Des());
					_LIT(KInfoST,"<b>The RegisterNotify URI was:  <u>%S</u> </b>\n");
					Manager().INFO_PRINTF2(KInfoST, &uriStPtr);
					CleanupStack::PopAndDestroy(stUri);
					if(paramSet->UPnPEvent().iStatus == TUPnPEvent::EFail)
						{
						TInt error = paramSet->UPnPEvent().iErrorCode;
						_LIT(KRegisterNotifyByeByeFail,"Resgitser Notify ByeBye Failed: TUPnPEvent::EFail Error Code is = %d \n");
						Manager().ERR_PRINTF2(KRegisterNotifyByeByeFail, error);
						CActiveScheduler::Stop();
						//Manager().SetTestStepResult(EFail); 
						return;
						}
					TPtrC8 usn(paramSet->USN());
					HBufC* usnvalue = HBufC::NewLC(usn.Length());	//1
					usnvalue->Des().Copy(usn);
					TPtrC usnPointer (usnvalue->Des());
					_LIT(KInfoBye,"<font color=33121f><b>ssdp:byebye notification received from %S </b></font>\n");
					iManager->INFO_PRINTF2(KInfoBye, &usnPointer);
					CleanupStack::PopAndDestroy(usnvalue);
					++iter;
					}
					_LIT(KInfoLogFile, "EUPnPAbsenceAnnouncementParamSet.... \n");
					Manager().INFO_PRINTF1(KInfoLogFile);
				}
			break;

			case EUPnPPublishResponseParamSet:
				{
				TBool flag=EFalse;
				_LIT(KStopScheduler,"stopscheduler");
				TInt responseCount = 0;
				CUPnPPublishResponseParamSet* paramSet  = NULL;
				Manager().GetBoolFromConfig(Manager().GetCurrentSection(),KStopScheduler,flag);
				_LIT(KExpectedPublishEesponseCount,"expectedpublishresponsecount");
				Manager().GetIntFromConfig(Manager().GetCurrentSection(),KExpectedPublishEesponseCount,responseCount);
				TParamSetIter<CUPnPPublishResponseParamSet> iter;
				paramFamily->InitializeIter(iter);
				while ( !iter.AtEnd()  )
					{
					paramSet = iter();
					Manager().iPublishCount--;
					if(paramSet->UPnPEvent().iStatus == TUPnPEvent::ESuccess)
						{
						TBool sendNotify= EFalse;
						TInt notifyCount(0);
						_LIT(KInfoPublish, "Info: Succesfully Published a Device/Service.");
						Manager().INFO_PRINTF1(KInfoPublish);
						_LIT(KSendNotify,"sendnotifications");
						Manager().GetBoolFromConfig(Manager().GetCurrentSection(),KSendNotify,sendNotify);

						if(sendNotify)
							{
							RBuf16 descriptionBaseUri;
							descriptionBaseUri.Create(paramSet->Uri().Length());
							descriptionBaseUri.Copy(paramSet->Uri());
							_LIT(KInfoSendNotify, "Info: Calling Send Notify on Uri %S \n");
							Manager().INFO_PRINTF2(KInfoSendNotify, &descriptionBaseUri);
							descriptionBaseUri.Close();
							Manager().SendNotificationsL(this);
							}
						_LIT(KExpctedNotifications,"expectednotifications");
						Manager().GetIntFromConfig(Manager().GetCurrentSection(),KExpctedNotifications,notifyCount);
						TInt publishCount = Manager().iPublishCount;
						if(flag && notifyCount == Manager().NotifyCount()&& publishCount <= 0 )
							{
							_LIT(KInfoLogFile, "EUPnPPublishResponseParamSet Success....And Stopping Active Scheduler after %d Publishes \n");
							Manager().INFO_PRINTF2(KInfoLogFile, publishCount );
							CActiveScheduler::Stop();
							}
						}
					else if(paramSet->UPnPEvent().iStatus == TUPnPEvent::EFail)
						{
						TInt error = paramSet->UPnPEvent().iErrorCode;
						_LIT(KErrPublish,"Failed to Published a Device/Service...Error Code is = %d \n");
						Manager().ERR_PRINTF2(KErrPublish, error);
						CActiveScheduler::Stop();
						_LIT(KInfoLogFile, "EUPnPPublishResponseParamSet Fail.... \n");
						Manager().INFO_PRINTF1(KInfoLogFile);
						//Manager().SetTestStepResult(EFail); 
						return;
						}
					++iter;
					}
				}
			break;

			case EUPnPDescribeResponseParamSet:
				{
				//abruptdescribeclosecp
				//opt for Abrupt close of CP
				TBool abruptDiscribeCloseCP = EFalse;
				_LIT(KAbruptDiscribeCloseCP,"abruptdescribeclosecp");
				Manager().GetBoolFromConfig(Manager().GetCurrentSection(),KAbruptDiscribeCloseCP,abruptDiscribeCloseCP );
				if(abruptDiscribeCloseCP)
					{
					//Closing Control point Abruptly
					_LIT(KInfoSubScribe, "In Describe Response \n");
					_LIT(KInfoCloseCP, "Closing Control point Abruptly.... \n");
					Manager().INFO_PRINTF1(KInfoSubScribe);
					Manager().INFO_PRINTF1(KInfoCloseCP);
					CActiveScheduler::Stop();
					Manager().CloseDiscoverer();
					return;
					}
				
				TParamSetIter<CUPnPDescribeResponseParamSet> iter;
				CUPnPDescribeResponseParamSet* paramSet  = NULL;
				paramFamily->InitializeIter(iter);

				while ( !iter.AtEnd()  )
					{
					//Only one response expected for Describe and no iter increment is required
					//Paramset at current position
					paramSet = iter();

					RBuf8 descriptionBuf;
					RBuf8 descriptionBaseUri;
					descriptionBaseUri.Create(paramSet->Uri());
					descriptionBuf.Create(paramSet->Description());

					RBuf description16;
					description16.Create(descriptionBuf.Length());
					description16.Copy(descriptionBuf);

					RBuf descriptionUri16;
					descriptionUri16.Create(descriptionBaseUri.Length());
					descriptionUri16.Copy(descriptionBaseUri);

					if(paramSet->UPnPEvent().iStatus == TUPnPEvent::EFail)
						{
						TInt error = paramSet->UPnPEvent().iErrorCode;
						_LIT(KDescribeFail,"Describe Response Fail: TUPnPEvent::EFail...Error Code is = %d \n");
						Manager().ERR_PRINTF2(KDescribeFail, error);
						_LIT(KDescriptionUri,"Description Uri is  %S \n");
						Manager().ERR_PRINTF1(KDescribeFail);
						Manager().INFO_PRINTF2(KDescriptionUri, &descriptionUri16  );
						_LIT(KInfoLogFile, "EUPnPDescribeResponseParamSet Fail.... \n");
						Manager().INFO_PRINTF1(KInfoLogFile);
						CActiveScheduler::Stop();
						descriptionBuf.Close();
						descriptionBaseUri.Close();
						description16.Close();
						descriptionUri16.Close();
						//Manager().SetTestStepResult(EFail);
						return;
						}
					Manager().AppendDescriptionL(descriptionBaseUri, descriptionBuf);
					TFileName filename;
					filename.Format(KDefaultFileFormat, 'C');
					TBuf<KMaxBufLength> filenameBuffer(filename);
					//get scriptName
					filenameBuffer.Append(Manager().ScriptFileName());
					_LIT(KUnderScore, "_");
					filenameBuffer.Append(KUnderScore);
					filenameBuffer.AppendNumFixedWidth(iSequence,EDecimal, 4);
					iSequence++;
					filenameBuffer.Append(KDefaultExtension);
					RFile file;
					TInt err=file.Replace(iFileServ,filenameBuffer,EFileRead|EFileWrite|EFileShareAny);
					if (err == KErrNone)
						{
						TInt writeErr= file.Write(descriptionBuf);
						if(writeErr != KErrNone )
							{
							_LIT(KErrWrite,"Unable to Write the description to the File \n");
							Manager().ERR_PRINTF1(KErrWrite);
							}
						_LIT(KDescriptionUri,"Description Uri is  %S \n");
						_LIT(KInfoDescribe,"content is  %S \n");
						Manager().INFO_PRINTF2(KDescriptionUri, &descriptionUri16  );
						Manager().INFO_PRINTF2(KInfoDescribe, &description16);
						Manager().iDescribeResponseCount++;
						}
					descriptionBuf.Close();
					descriptionBaseUri.Close();
					description16.Close();
					descriptionUri16.Close();
					file.Close();
					++iter;
					iResultsCount++;
					}
				TBool cancelAll = EFalse;
				Manager().GetBoolFromConfig(Manager().GetCurrentSection(), KCancelAll, cancelAll );
				TBool cancelAllService = EFalse;
				Manager().GetBoolFromConfig(Manager().GetCurrentSection(), KCancelAllService, cancelAllService );
				TInt descCount = 1;
				Manager().GetIntFromConfig(Manager().GetCurrentSection(), KExpctedDescriptions, descCount );

				if(Manager().iDescribeResponseCount >= descCount )// Modify this later
					{
					if(cancelAll)
						{
						Manager().CancelAllDeviceDescribeL();
						return;
						}
					else if(cancelAllService)
						{
						Manager().CancelAllServiceDescribeL();
						return;
					}
					else
					{
					CActiveScheduler::Stop();
					_LIT(KInfoLogFile, "EUPnPDescribeResponseParamSet.... \n");
					Manager().INFO_PRINTF1(KInfoLogFile);
					return;
					}
				}
				}
			break;
			case EUPnPSubscribeResponseParamSet:
				{
				TParamSetIter<CUPnPSubscribeResponseParamSet> iter;
				CUPnPSubscribeResponseParamSet* paramSet  = NULL;
				paramFamily->InitializeIter(iter);

				while ( !iter.AtEnd() )
					{
					//Paramset at current position
					paramSet = iter();
					RBuf subscriptionUri16;
					subscriptionUri16.Create(paramSet->Uri().Length());
					subscriptionUri16.Copy(paramSet->Uri());
					if(paramSet->UPnPEvent().iStatus == TUPnPEvent::EFail)
						{
						_LIT(KInfoSubscribeFailed, "Info: Failed to Subscribe.\n %S \n");
						Manager().INFO_PRINTF2(KInfoSubscribeFailed,&subscriptionUri16);
						subscriptionUri16.Close();
						TInt error = paramSet->UPnPEvent().iErrorCode;
						_LIT(KSubscribeFailed,"Failed to Subscribe with...Error Code  = %d \n");
						Manager().ERR_PRINTF2(KSubscribeFailed, error);
						CActiveScheduler::Stop();
						_LIT(KInfoLogFile, "EUPnPSubscribeResponseParamSet Fail.... \n");
						Manager().INFO_PRINTF1(KInfoLogFile);
						//Manager().SetTestStepResult(EFail); 
						return;
						}
					else if(paramSet->UPnPEvent().iStatus == TUPnPEvent::ESubscriptionTimeout)
						{
						_LIT(KInfoSubscribeTimeOut, "Info: Subscription about to expire\n %S \n");
						Manager().INFO_PRINTF2(KInfoSubscribeTimeOut, &subscriptionUri16);
						subscriptionUri16.Close();
						_LIT(KInfoSubscribeTimeOut3, "subscription uri deleted\n");
						Manager().INFO_PRINTF1(KInfoSubscribeTimeOut3);
						
						//Implementation of Resubscribe is here
						TBool renewFlag = EFalse;
						_LIT(KRenewFlag,"renewflag");
						Manager().GetBoolFromConfig(Manager().GetCurrentSection(),KRenewFlag,renewFlag );
						
						_LIT(KInfoSubscribeTimeOut1, "renew flag value %d renew count   %d  \n");
						Manager().INFO_PRINTF3(KInfoSubscribeTimeOut1, renewFlag,Manager().iRenewCount);
						
						if( renewFlag && Manager().iRenewCount == 0)
							{//go to renew here only once
							subscriptionUri16.Close();//cleanup activity
							CActiveScheduler::Stop();//this is for start made in prev subscribe
							_LIT(KInfoLogFile, "EUPnPSubscribeResponseParamSet Timeout1.... \n");
							Manager().INFO_PRINTF1(KInfoLogFile);
							Manager().iRenewCount++;
							//renew subscriptions
							_LIT(KInfoSubscribeTimeOut2, "renew count %d \n");
							Manager().INFO_PRINTF2(KInfoSubscribeTimeOut2,Manager().iRenewCount);
							_LIT(KInfoReSubscribe, "Request to RESUBSCRIPTION is Executed\n %S \n");
							Manager().INFO_PRINTF1(KInfoReSubscribe);
							Manager().SubscribeForStateChangeNotificationsL();
							return;
							}
						else
							{
							//Unsubscribe before closing
							_LIT(KInfoUnSubscribe, "Request to UNSUBSCRIPTION is Executed As Subscription TIMED OUT \n %S \n");
							Manager().INFO_PRINTF1(KInfoUnSubscribe);
							TPtrC referSection;
							_LIT(KReferSection,"refersection");
							Manager().GetStringFromConfig(Manager().GetCurrentSection(),KReferSection,referSection );
							Manager().UnsubscribeForStateChangeNotificationsL(referSection);
							subscriptionUri16.Close();//cleanup activity
							CActiveScheduler::Stop();
							_LIT(KInfoLogFile, "EUPnPSubscribeResponseParamSet Timeout2.... \n");
							Manager().INFO_PRINTF1(KInfoLogFile);
							return;
							}
						}
					else if(paramSet->UPnPEvent().iStatus == TUPnPEvent::ESuccess)
						{
						_LIT(KInfoSubscribeSuccess, "Info: SUBSCRIPTION REQUEST IS ACCEPTED FOR SUBSCRIPTION\n %S \n");
						Manager().INFO_PRINTF2(KInfoSubscribeSuccess, &subscriptionUri16);
						
						//opt for Abrupt close of CP
						TBool abruptCloseCP = EFalse;
						_LIT(KAbruptCloseCP,"abruptclosecp");
						Manager().GetBoolFromConfig(Manager().GetCurrentSection(),KAbruptCloseCP,abruptCloseCP );
						if(abruptCloseCP)
							{
							//Closing Control point Abruptly
							_LIT(KInfoSubScribe, "In Subscribe Response \n");
							_LIT(KInfoCloseCP, "Closing Control point Abruptly.... \n");
							Manager().INFO_PRINTF1(KInfoSubScribe);
							Manager().INFO_PRINTF1(KInfoCloseCP);
							CActiveScheduler::Stop();
							Manager().CloseDiscoverer();
							return;
							}
						//Opt for Unsubscribe
						TBool unSubscribeFlag = EFalse;
						_LIT(KUnsubscribeFlag,"unsubscribeflag");
						Manager().GetBoolFromConfig(Manager().GetCurrentSection(),KUnsubscribeFlag,unSubscribeFlag );
						if(unSubscribeFlag)
							{//go to unsubscribe here
							_LIT(KInfoUnSubscribe, "Request to UNSUBSCRIPTION is Executed Before Subscription Expiry\n  \n");
							Manager().INFO_PRINTF1(KInfoUnSubscribe);
							TPtrC referSection;
							_LIT(KReferSection,"refersection");
							Manager().GetStringFromConfig(Manager().GetCurrentSection(),KReferSection,referSection );
							//Unsubscribe
							Manager().UnsubscribeForStateChangeNotificationsL(referSection);
							subscriptionUri16.Close();//cleanup activity
							CActiveScheduler::Stop();//this is for start made in prev subscribe
							_LIT(KInfoLogFile, "EUPnPSubscribeResponseParamSet Success.... \n");
							Manager().INFO_PRINTF1(KInfoLogFile);
							return;
							}
						}
					++iter;
					}
				}
			break;
			case EUPnPNotifyEventParamSet:
				{//receives event notifications sent at client
				_LIT(KInfoLogFile, "EUPnPNotifyEventParamSet.... \n");
				Manager().INFO_PRINTF1(KInfoLogFile);
				
				TParamSetIter<CUPnPNotifyEventParamSet> iter;
				CUPnPNotifyEventParamSet* paramSet  = NULL;
				paramFamily->InitializeIter(iter);

				while ( !iter.AtEnd() )
					{
					//Paramset at current position
					paramSet = iter();
					RBuf subscriptionUri16;
					subscriptionUri16.Create(paramSet->Uri().Length());
					subscriptionUri16.Copy(paramSet->Uri());
					if(paramSet->UPnPEvent().iStatus == TUPnPEvent::EFail)
						{
						++iter;
						_LIT(KInfoNotificationsFailed, "Info: Failed to Get Event notifications\n %S \n");
						Manager().INFO_PRINTF2(KInfoNotificationsFailed,&subscriptionUri16);
						subscriptionUri16.Close();
						TInt error = paramSet->UPnPEvent().iErrorCode;
						_LIT(KNotificationsFailed,"Failed to Receive Notifications with...Error Code  = %d \n");
						Manager().ERR_PRINTF2(KNotificationsFailed, error);
						//Manager().SetTestStepResult(EFail);
						return;
						}
					else if (paramSet->UPnPEvent().iStatus == TUPnPEvent::ESuccess)
						{
						_LIT(KInfoNotifications, "Notifications for subscribed Uri: %S \t is received \n");
						Manager().INFO_PRINTF2(KInfoNotifications, &subscriptionUri16);
						subscriptionUri16.Close();//cleanup activity
						++iter;
						}
					}
				_LIT(KInfoLogFile1, "EUPnPNotifyEventParamSet End.... \n");
				Manager().INFO_PRINTF1(KInfoLogFile1);
				}
			break;
			default:
				{
				//Do Nothing
				}
			break;
			}
		}
	}

/*
   Error handling,asynchronous one.
   @param  aError Error Code
   @return None
 */
void CUPnPObserver::OnPnPError(TInt aError)
	{
	if(aError == KErrNoMemory)
		{
		//User::Leave(KErrNoMemory);
		}
	_LIT(KPositiveCase,"positivecase");
	TBool positiveCase = ETrue;
	Manager().GetBoolFromConfig(Manager().GetCurrentSection(), KPositiveCase, positiveCase);
	// Note down the error and stop scheduler
	//If not postive case, client.dll will come back before active scheduler start
	//so, don't execute activescheduler::stop
	if (positiveCase)
		{
		CActiveScheduler::Stop();
		}
	_LIT(KErrorMessage, "Error occurred in the UPnP Observer : %d \n");
	Manager().ERR_PRINTF2(KErrorMessage, aError);
	//Manager().SetTestStepResult(EFail);
	}

/*
   This function copies result bundles into results array, which inturn is used in printing results
   @param  aPnpBundle a reference to CPnPParameterBundleBase
   @return None
 */
void CUPnPObserver::CopyResultBundlesL(const RPnPParameterBundle& aPnpBundle)
	{
	_LIT(KInfoLogFile, "CUPnPObserver::CopyResultBundlesL().... \n");
	Manager().INFO_PRINTF1(KInfoLogFile);
	
	HBufC8* holdBundle = HBufC8::NewLC(aPnpBundle.Length());

	if (!holdBundle)
        {
        User::Leave(KErrNoMemory);
        }

    TPtr8 storePtr(holdBundle->Des());
	TInt retOnStore = aPnpBundle.Store(storePtr);
	User::LeaveIfError(retOnStore);

	TPtrC8 loadPtr(holdBundle->Des());
	CPnPParameterBundleBase* newPnpBundle = CPnPParameterBundleBase::NewL();
	TInt retOnLoad = newPnpBundle->Load(loadPtr);
	User::LeaveIfError(retOnLoad);
	CleanupStack::PushL(newPnpBundle);
	Manager().AppendBundlesL(newPnpBundle);
	CleanupStack::Pop(newPnpBundle);
	CleanupStack::PopAndDestroy(holdBundle);
	_LIT(KInfoLogFile1, "CUPnPObserver::CopyResultBundlesL() End.... \n");
	Manager().INFO_PRINTF1(KInfoLogFile1);
	}

/*
   This function Prints Discovery results by getting it from Results bundle
   @param  aParamFamily a pointer to CUPnPParameterFamily
   @return None
 */
void CTestUPnPManager::PrintDiscoveryResultsL(CUPnPParameterFamily* aParamFamily)
	{
	_LIT(KInfoLogFile, "CTestUPnPManager::PrintDiscoveryResultsL().... \n");
	INFO_PRINTF1(KInfoLogFile);

	_LIT(KPrintStatement, "printing results \n");
	_LIT(KPrintDesign,"	\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ \n");

	INFO_PRINTF1(KPrintStatement);
	INFO_PRINTF1(KPrintDesign);

	TParamSetIter<CUPnPDiscoverResponseParamSet> iter;
	CUPnPDiscoverResponseParamSet* paramSet  = NULL;
	aParamFamily->InitializeIter(iter);

	while ( !iter.AtEnd() )
		{
		paramSet = iter();

		if(paramSet->UPnPEvent().iStatus == TUPnPEvent::EFail)
			{
			TInt error = paramSet->UPnPEvent().iErrorCode;
			_LIT(KDiscoverFailed,"Failed to Discover with...Error Code  = %d \n");
			ERR_PRINTF2(KDiscoverFailed, error);
			++iter;
			continue;
			}
		else if(paramSet->UPnPEvent().iStatus == TUPnPEvent::EDiscoveryTimeout)
			{
			_LIT(KDiscoverTimeOut,"Discover Response TimeOut: TUPnPEvent::EDiscoveryTimeout \n");
			INFO_PRINTF1(KDiscoverTimeOut);
			++iter;
			continue;
			}
		TPtrC8 deviceLocation(paramSet->DeviceLocation());
		HBufC* devloc = HBufC::NewLC(deviceLocation.Length());	//1
		devloc->Des().Copy(deviceLocation);
		TPtrC devLocPointer (devloc->Des());
		_LIT(KInfoUri,"The Location: %S \n");
		INFO_PRINTF2(KInfoUri, &devLocPointer);

		TPtrC8 usn(paramSet->USN());
		HBufC* usnVal = HBufC::NewLC(usn.Length());	//2
		usnVal->Des().Copy(usn);
		TPtrC usnValPointer (usnVal->Des());
		_LIT(KInfoUSN,"The USN found is %S \n");
		INFO_PRINTF2(KInfoUSN, &usnValPointer);

		TInt expiryTime = paramSet->ExpiryTime();
		_LIT(KInfoexpiryTime,"The Expiry Time or CACHE-CONTROL: max-age =  %d \n");
		INFO_PRINTF2(KInfoexpiryTime, expiryTime);

		CleanupStack::PopAndDestroy(usnVal);
		CleanupStack::PopAndDestroy(devloc);
		++iter;
		continue;
		}
	_LIT(KEndPrintStatement,"Done with printing results \n");
	INFO_PRINTF1(KEndPrintStatement);
	INFO_PRINTF1(KPrintDesign);
	}

void CTestUPnPManager::ValidateDiscoveryResultsL(const TDesC& aOperationType)
	{
	_LIT(KInfoLogFile, "CTestUPnPManager::ValidateDiscoveryResultsL().... \n");
	INFO_PRINTF1(KInfoLogFile);
	
	TInt DeviceLocationCount = NULL;
	TPtrC ExpectedURL;
	GetStringFromConfig(aOperationType, KExpectedURL, ExpectedURL);

	TInt totalCount(0);
	GetIntFromConfig(aOperationType, KExpectedCount, totalCount);
	const TInt bundleCount = iResultsArray.Count();

	for(TInt i(0); i < bundleCount; i++)
		{
		CUPnPParameterFamily* paramFamily = static_cast<CUPnPParameterFamily*> ( iResultsArray[i]->GetFamilyByIndex() );
		if( paramFamily )
			{
	 		if(paramFamily->Id() ==EUPnPDiscoverResponseParamSet)
				{
				TParamSetIter<CUPnPDiscoverResponseParamSet> iter;
				CUPnPDiscoverResponseParamSet* paramSet  = NULL;
				paramFamily->InitializeIter(iter);

				while ( !iter.AtEnd() )
					{
					paramSet = iter();
					if(paramSet->UPnPEvent().iStatus == TUPnPEvent::EFail)
						{
						TInt error = paramSet->UPnPEvent().iErrorCode;
						_LIT(KDiscoverFailed,"Failed to Discover with...Error Code  = %d \n");
						ERR_PRINTF2(KDiscoverFailed, error);
						++iter;
						continue;
						}
					TPtrC8 deviceLocation(paramSet->DeviceLocation());
					HBufC* devloc = HBufC::NewLC(deviceLocation.Length());
					devloc->Des().Copy(deviceLocation);
					TPtrC devLocPointer (devloc->Des());
					DeviceLocationCount++;
					if((ExpectedURL!=KNullDesC) && (ExpectedURL.Compare(devLocPointer) == KErrNone))
						{
						SetTestStepResult(EPass);
						}
					CleanupStack::PopAndDestroy(devloc);
					++iter;
					continue;
					}
				}
			}
		}
	if((DeviceLocationCount != NULL) &&(DeviceLocationCount == totalCount))
		{
		SetTestStepResult(EPass);
		}
	_LIT(KInfoLogFile1, "CTestUPnPManager::ValidateDiscoveryResultsL() End.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	}

void CTestUPnPManager::CancelPublish()
	{
	_LIT(KInfoLogFile, "CTestUPnPManager::CancelPublish() Begin....  \n");
	INFO_PRINTF1(KInfoLogFile);
	TInt count = iPublisherArray.Count();
	for(TInt i(count - 1); i >= 0 ; i--)
		{
		TPublisherControl publisherType = PublisherType(i);
		_LIT(KInfoCancelLog, "CTestUPnPManager::CancelPublish()...Closing Publisher....  %d\n");
		INFO_PRINTF2(KInfoCancelLog,i);
		switch(publisherType)
			{
			case KService:
				{
				_LIT(KInfoService, "Closing Publisher.... of Type---->SERVICE\n");
				INFO_PRINTF1(KInfoService);
				break;
				}
			case KDevice:
				{
				_LIT(KInfoDevice, "Closing Publisher.... of Type---->DEVICE\n");
				INFO_PRINTF1(KInfoDevice);
				break;
				}
			default :
				{
				_LIT(KInfoErr, "Closing Publisher.... of Type---->InValid\n");
				ERR_PRINTF1(KInfoErr);
				break;
				}
			}
		iPublisherArray[i].Close();
		}
	_LIT(KInfoLogFile2, "CancelPublish() Array Closed.... \n");
	INFO_PRINTF1(KInfoLogFile2);
	iPublisherArray.Close();
	_LIT(KInfoLogFile3, "CTestUPnPManager::CancelPublish() End.... \n");
	INFO_PRINTF1(KInfoLogFile3);
	}

void CTestUPnPManager::DescribeServiceL()
	{
	_LIT(KInfoLogFile, "CTestUPnPManager::DescribeServiceL().... \n");
	INFO_PRINTF1(KInfoLogFile);
	
	ResolveAllUrisL();
	TInt scpdUriCount = iScpdUriArray.Count();
	//__ASSERT_DEBUG ( scpdUriCount > 0, User::Panic (KDescribeTestPanicCategory,KErrNoDescriptionPath ) );
	if ( scpdUriCount==0 )
		{
		_LIT(KDescription,"Description Cannot be performed \n");
		INFO_PRINTF1(KDescription);
		//SetTestStepResult(EFail);
		return;
		}
	CUPnPObserver* upnpObserver = CUPnPObserver::NewL(this);
	CleanupStack::PushL( upnpObserver );
	CPnPParameterBundle* pnpBundle = CPnPParameterBundle::NewL( );
	CleanupClosePushL( *pnpBundle );
	pnpBundle->SetPnPObserver((MPnPObserver*)upnpObserver);
	CUPnPParameterFamily* pnpFamily = CUPnPParameterFamily::NewL (*pnpBundle , EUPnPDescribeRequestParamSet);

	// Read the ScpdUrl from ScpdUri Array
	//Make sure that Before this Describe Device, and Discovery is executes Succesfully.
	for(TInt index(0); index < scpdUriCount ; index++)//scpdUriCount
		{
		CUPnPDescribeRequestParamSet* describeParamSet = CUPnPDescribeRequestParamSet::NewL (pnpFamily );
		describeParamSet->SetUriL(iScpdUriArray[index]);
		RBuf scpdUri16;
		scpdUri16.Create(iScpdUriArray[index].Length());
		scpdUri16.Copy(iScpdUriArray[index]);
		_LIT(KScpdUriInfo,"SCPD Uri Submitted for Description Request is: %S \n");
		INFO_PRINTF2(KScpdUriInfo, &scpdUri16);
		scpdUri16.Close();
		}
	iControlPoint.Describe(*pnpBundle );
	CActiveScheduler::Start();
	_LIT(KInfoLogFile1, "CTestUPnPManager::DescribeServiceL() End.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	
	CleanupStack::PopAndDestroy( pnpBundle );
	CleanupStack::PopAndDestroy( upnpObserver );
	}

void CTestUPnPManager::DescribeL()
	{
	_LIT(KInfoLogFile, "CTestUPnPManager::DescribeL().... \n");
	INFO_PRINTF1(KInfoLogFile);
	
	CUPnPObserver* upnpObserver = CUPnPObserver::NewL(this);
	CleanupStack::PushL( upnpObserver );
	CPnPParameterBundle* pnpBundle = CPnPParameterBundle::NewL( );
	CleanupClosePushL( *pnpBundle );
	pnpBundle->SetPnPObserver((MPnPObserver*)upnpObserver);
	CUPnPParameterFamily* pnpFamily = CUPnPParameterFamily::NewL (*pnpBundle , EUPnPDescribeRequestParamSet);
	RBuf8 descriptionPathBuf;

	TBool readIni = ETrue;
	GetBoolFromConfig(GetCurrentSection(), KReadIni, readIni);
	if(readIni)
		{
		// Get the Description path from .ini File
		TPtrC descriptionPath;
		GetStringFromConfig(GetCurrentSection(), KDescriptionPath, descriptionPath);
		CUPnPDescribeRequestParamSet* describeParamSet = CUPnPDescribeRequestParamSet::NewL (pnpFamily );
		descriptionPathBuf.Create(descriptionPath.Length());
		descriptionPathBuf.Copy(descriptionPath);
		describeParamSet->SetUriL(descriptionPathBuf);
		descriptionPathBuf.Close();
		iControlPoint.Describe(*pnpBundle );
		TBool SchedulerStatus = ETrue;
		_LIT(KSchedulerStatus, "schedulerstart");
		GetBoolFromConfig(GetCurrentSection(), KSchedulerStatus, SchedulerStatus);
		if(SchedulerStatus)
			{
			CActiveScheduler::Start();
			}
		}
	else
		{
		// Read the Description path from DevLoaction array
		//Make sure that Before this Describe, Successful Discovery is happend.
		TInt DevLocCount = iDeviceLocationArray.Count();
		if ( DevLocCount==0 )
			{
			_LIT(KDescription,"Description Cannot be performed as NO Uri available to Describe\n");
			INFO_PRINTF1(KDescription);
			CleanupStack::PopAndDestroy( pnpBundle );
			CleanupStack::PopAndDestroy( upnpObserver );
			//SetTestStepResult(EFail);
			return;
			}

		//__ASSERT_DEBUG ( DevLocCount > 0, User::Panic (KDescribeTestPanicCategory,KErrNoDescriptionPath ) );

		for(TInt iter(0); iter < DevLocCount  ; iter++)
			{
			TBuf8<256> url;
			TUriParser8 uri;
			uri.Parse(iDeviceLocationArray[iter]);
			const TDesC8& scheme = uri.Extract(EUriScheme);
			_LIT8(KScheme, "http");
			//Uri must be of scheme "http" only
			if (scheme.CompareF(KScheme) == 0)
				{
				url.Copy(iDeviceLocationArray[iter]);
				CUPnPDescribeRequestParamSet* describeParamSet = CUPnPDescribeRequestParamSet::NewL (pnpFamily );
				describeParamSet->SetUriL(url);
				}
			else
				{
				_LIT(KInavlidUri, "Describe Request for InvalidURi");
				INFO_PRINTF1(KInavlidUri);
				return;
				}
			}
			
		iControlPoint.Describe(*pnpBundle );
		TBool SchedulerStatus = ETrue;
		_LIT(KSchedulerStatus, "schedulerstart");
		GetBoolFromConfig(GetCurrentSection(), KSchedulerStatus, SchedulerStatus);
		if(SchedulerStatus)
			{
			CActiveScheduler::Start();
			}
		}
	_LIT(KInfoLogFile1, "CTestUPnPManager::DescribeL() End.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	CleanupStack::PopAndDestroy( pnpBundle );
	CleanupStack::PopAndDestroy( upnpObserver );
	}

void CTestUPnPManager::ResolveAllUrisL()
	{
	_LIT(KInfoLogFile, "CTestUPnPManager::ResolveAllUrisL().... \n");
	INFO_PRINTF1(KInfoLogFile);
	
	TInt count = iDescriptionSuiteArray.Count();
	//__ASSERT_DEBUG ( count > 0, User::Panic (KDescribeTestPanicCategory, KErrNoServiceUris ) );
	if( count <=0 )
		{
		_LIT(KErrResolve, "No Description is exist to Resolve URIs");
		ERR_PRINTF1(KErrResolve);
		return;
		}
	//Fill the all types of Uris for each base Uri
	for (TInt index(0); index < count ; index++ )
		{
		GetServiceUrlsListL(index);
		ResolveScpdUrisL(index);
		ResolveEventUrisL(index);
		ResolveControlUrisL(index);
		}
	}

void CTestUPnPManager::GetServiceUrlsListL(TInt aIndex)
	{
	_LIT(KInfoLogFile, "CTestUPnPManager::GetServiceUrlsListL().... \n");
	INFO_PRINTF1(KInfoLogFile);
	
	CUPnPDeviceDescription*	rootDeviceDescription = NULL;
	//Release the content of StringPool if any
	if(iStringPoolMgr)
		iStringPoolMgr->Release();
	DeleteSeriveUrlsList(aIndex);

	 iStringPoolMgr = CStringPoolManager::NewL();
	 //Store the Parsed node info
	 iStringPool = iStringPoolMgr->StringPool();
	_LIT(KInfoLogFile4, "Creating Stringpool and its manager.... \n");
	INFO_PRINTF1(KInfoLogFile4);
	
	//Parse the device description
    CUPnPDescriptionParser* app = CUPnPDescriptionParser::NewL( iStringPool, CUPnPDescriptionParser::EDevice);
	_LIT(KInfoLogFile2, "CTestUPnPManager::CUpnpDeviceXmlParser::NewL().... \n");
	INFO_PRINTF1(KInfoLogFile2);
    TRAPD(error,(rootDeviceDescription = static_cast<CUPnPDeviceDescription*>(app->ParseDescriptionBufL(iDescriptionSuiteArray[aIndex].iDescription))));
	if (error != KErrNone)
		{
		_LIT(KErrLogFile1, "CTestUPnPManager::GetServiceUrlsListL() is Terminating.... And Error is %d \n");
		ERR_PRINTF2(KErrLogFile1, error);
		return;
		}
	_LIT(KInfoLogFile3, "rootDeviceDescription.... \n");
	INFO_PRINTF1(KInfoLogFile3);
	
   
    
    delete app;
	app = NULL;
	_LIT(KInfoLogFile5, "Deleting CUpnpDeviceXmlParser().... \n");
	INFO_PRINTF1(KInfoLogFile5);

    //Retrieve the Tag for Base
    RString string = iStringPool.String(UPNPDESCRIPTIONXMLTAGS::EUrlBase,iStringPoolMgr->GetUPnPTable());
    //Get the node info for the Base i.e., value between the start and end Base tags
    //If BaseUri exist in Description replace already existing BaseUri
	if(rootDeviceDescription->Property(string).Compare(KNullDesC8)>0)
		{
		//Delete already stored Base uri
		iDescriptionSuiteArray[aIndex].iBaseUrl.Close();
		iDescriptionSuiteArray[aIndex].iBaseUrl.Create(rootDeviceDescription->Property(string).Length());
		iDescriptionSuiteArray[aIndex].iBaseUrl.Copy(rootDeviceDescription->Property(string));
		}

	//Catch hold of first level of Device
	CUPnPDevice* device = rootDeviceDescription->DeviceObject();

	TInt count =device->CountOfEmbeddedDeviceInfoTable();
	if(count>0)
		{
		for(TInt i=0;i<count;i++)
			{
			GetEmbeddedDeviceServicesL(aIndex,device->AtEmbeddedDeviceInfoTable(i));
			}
		}
	GetServiceValuesL(aIndex, device);
	_LIT(KInfoLogFile6, "Finished GetServiceValuesL.... \n");
	INFO_PRINTF1(KInfoLogFile6);
	delete rootDeviceDescription;
	rootDeviceDescription = NULL;
	_LIT(KInfoLogFile1, "CTestUPnPManager::GetServiceUrlsListL() End.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	}

void CTestUPnPManager::DeleteSeriveUrlsList(TInt aIndex)
	{
	_LIT(KInfoLogFile, "CTestUPnPManager::DeleteSeriveUrlsList().... \n");
	INFO_PRINTF1(KInfoLogFile);
	
	for(TInt i=0;i<iDescriptionSuiteArray[aIndex].iServiceUrls.Count();i++)
		{
		iDescriptionSuiteArray[aIndex].iServiceUrls[i].serviceType.Close();
		iDescriptionSuiteArray[aIndex].iServiceUrls[i].scpdUrl.Close();
		iDescriptionSuiteArray[aIndex].iServiceUrls[i].controlUrl.Close();
		iDescriptionSuiteArray[aIndex].iServiceUrls[i].eventSubUrl.Close();
		}
	iDescriptionSuiteArray[aIndex].iServiceUrls.Close();
	_LIT(KInfoLogFile1, "CTestUPnPManager::DeleteSeriveUrlsList() End.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	}

void CTestUPnPManager::GetEmbeddedDeviceServicesL(TInt aIndex, CUPnPDevice* aDevice)
	{
	_LIT(KInfoLogFile, "CTestUPnPManager::GetEmbeddedDeviceServicesL().... \n");
	INFO_PRINTF1(KInfoLogFile);
	
	if(aDevice->CountOfEmbeddedDeviceInfoTable() > 0)
		{
		for(TInt i=0;i<aDevice->CountOfEmbeddedDeviceInfoTable();i++)
			{
			GetEmbeddedDeviceServicesL(aIndex, aDevice->AtEmbeddedDeviceInfoTable(i));
			GetServiceValuesL(aIndex, aDevice);
			}
		}
	else
		{
		GetServiceValuesL(aIndex, aDevice);
		}
	}


void CTestUPnPManager::AppendDeviceLocationL(TDesC8& aDevLocPointer)
	{
	User::LeaveIfError(	iDeviceLocationArray.Append(aDevLocPointer) );
	}

void CTestUPnPManager::AppendScpdUriL(RBuf8& aScpdUri)
	{
	User::LeaveIfError(	iScpdUriArray.Append(aScpdUri) );
	}

void CTestUPnPManager::AppendEventUriL(RBuf8& aEventUri)
	{
	User::LeaveIfError(	iEventUriArray.Append(aEventUri) );
	}

void CTestUPnPManager::AppendControlUriL(RBuf8& aControlUri)
	{
	User::LeaveIfError(	iControlUriArray.Append(aControlUri) );
	}

void CTestUPnPManager::AppendDescriptionL(RBuf8& aBaseUri,RBuf8& aDescription)
	{
	TDescriptionSuite buf;
	buf.iDescription.CreateL(aDescription);
	buf.iBaseUrl.CreateL(aBaseUri);
	iDescriptionSuiteArray.Append(buf);
	}

void CTestUPnPManager::GetServiceValuesL(TInt aIndex, CUPnPDevice* aDevice)
	{
	_LIT(KInfoLogFile, "CTestUPnPManager::GetServiceValuesL().... \n");
	INFO_PRINTF1(KInfoLogFile);
	
	TUint count;
	const CUPnPServiceInfo* servInfo = NULL;
	TServiceUrl serviceurl;
	count = aDevice->CountOfServiceInfoTable();
	if(count > 0)
		{
		for(TInt i = 0;  i< aDevice->CountOfServiceInfoTable(); i++)
			{
			servInfo = aDevice->AtServiceInfoTable(i);

			RString string = iStringPool.String(UPNPDESCRIPTIONXMLTAGS::EServiceType, iStringPoolMgr->GetUPnPTable());
			serviceurl.serviceType.Create(servInfo->Property(string).Length());
			serviceurl.serviceType.Copy(servInfo->Property(string));

			string = iStringPool.String(UPNPDESCRIPTIONXMLTAGS::EScpdUrl, iStringPoolMgr->GetUPnPTable());
			serviceurl.scpdUrl.Create(servInfo->Property(string).Length());
			serviceurl.scpdUrl.Copy(servInfo->Property(string));

			string = iStringPool.String(UPNPDESCRIPTIONXMLTAGS::EContorlUrl, iStringPoolMgr->GetUPnPTable());
			serviceurl.controlUrl.Create(servInfo->Property(string).Length());
			serviceurl.controlUrl.Copy(servInfo->Property(string));

			string = iStringPool.String(UPNPDESCRIPTIONXMLTAGS::EEventSubUrl, iStringPoolMgr->GetUPnPTable());
			serviceurl.eventSubUrl.Create(servInfo->Property(string).Length());
			serviceurl.eventSubUrl.Copy(servInfo->Property(string));

			iDescriptionSuiteArray[aIndex].iServiceUrls.AppendL(serviceurl);
			}
		}
	_LIT(KInfoLogFile1, "CTestUPnPManager::GetServiceValuesL() End.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	}


CUPnPTimer::CUPnPTimer(CTestUPnPManager* aManager)
: CActive(EPriorityHigh),iManager(aManager)
    {
    CActiveScheduler::Add(this);
    }


CUPnPTimer::~CUPnPTimer()
    {
	Cancel();
    }


CUPnPTimer* CUPnPTimer::NewL(CTestUPnPManager* aManager)
    {
	CUPnPTimer* self = new (ELeave) CUPnPTimer(aManager);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
    }

void CUPnPTimer::ConstructL()
	{
	}


void CUPnPTimer::IssueRequestL(TTimeIntervalMicroSeconds32 anInterval)
    {
	iTimer.CreateLocal();
    iTimer.After(iStatus, anInterval);
    SetActive();
	_LIT(KInfoLogFile, "CUPnPTimer::IssueRequestL() End.... \n");
	iManager->INFO_PRINTF1(KInfoLogFile);
    }


void CUPnPTimer::RunL()
	{
	CActiveScheduler::Stop();
	_LIT(KInfoLogFile, "CUPnPTimer::RunL() End.... \n");
	iManager->INFO_PRINTF1(KInfoLogFile);
	
	}

void CUPnPTimer::DoCancel()
	{
	iTimer.Cancel();
	}

CAction* CAction::NewL(CTestUPnPManager* aManager)
	{
	CAction* self = new (ELeave) CAction(aManager);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;

	}

CAction::CAction(CTestUPnPManager* aManager)
: CActive(CActive::EPriorityStandard),iManager(aManager)
	{

	}

CAction::~CAction()
	{
	Cancel();
	iFile.Close ( );
	iFs.Close ( );
	}

void CAction::ConstructL()
	{
	_LIT(KInfoLogFile, "CAction::ConstructL() .... \n");
	iManager->INFO_PRINTF1(KInfoLogFile);
	CActiveScheduler::Add(this);
	_LIT(KPath, "c:\\upnp\\testupnp\\client\\testdata\\");
	_LIT(KFile, "c:\\upnp\\testupnp\\client\\testdata\\actionresponse.xml");
	User::LeaveIfError ( iFs.Connect ( ) );
    TInt err = iFile.Open ( iFs, KFile, EFileWrite );
    if(err == KErrNotFound)
		{
		err = iFile.Create(iFs,KFile,EFileWrite);
		}
	else if(err == KErrPathNotFound)
		{
		err= iFs.MkDirAll(KPath);
		if(err==KErrNone || err== KErrAlreadyExists)
			{
			iFile.Create(iFs,KFile,EFileWrite);
			}
		}
	else
    	{
    	User::LeaveIfError(err);
    	}
	}

void CAction::RunL()
	{
	switch(iActionState)
		{
		case ESendData:
			iActionState = EReceiveData;
			iControlChannel.Recv ( iCtrlMsg, iStatus );
			SetActive();
			_LIT(KInfoLogFile, "CAction::RunL Send Data.... \n");
			iManager->INFO_PRINTF1(KInfoLogFile);
			break;
		case EReceiveData:
		    TInt err = iFile.Write(iCtrlMsg.MessageDes ());
		    if ( !iCtrlMsg.IsLast () )
		    	{
				iCtrlMsg.ClearFlags();
		    	iControlChannel.Recv ( iCtrlMsg, iStatus );
				SetActive();
				_LIT(KInfoLogFile, "CAction::RunL Receive Data... \n");
				iManager->INFO_PRINTF1(KInfoLogFile);
		    	}
			else
				{
				iControlChannel.Close( );
				CActiveScheduler::Stop();
				_LIT(KInfoLogFile, "CAction::RunL End.... \n");
				iManager->INFO_PRINTF1(KInfoLogFile);
				}
			break;
		}
	}

void CAction::DoCancel()
	{
	}

void CAction::InitiateActionControlL(TDesC8& aUri,RPnPServiceDiscovery& aControlPoint)
	{
	RFs fs;
    RFile	file;
    TInt err;
    RBuf8 buf;
	_LIT(KInfoLogFile2, "InitiateActionControlL .... \n");
	iManager->INFO_PRINTF1(KInfoLogFile2);

    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    err = file.Open(fs,_L("z:\\upnp\\testupnp\\client\\testdata\\action.xml"),EFileRead);
	if (err != KErrNone)
	    {
	    User::LeaveIfError(err);
	    }
	CleanupClosePushL(file);
	TInt fileSize = 0;
	file.Size(fileSize);
	buf.Create(fileSize);
	err = file.Read(buf, fileSize);
	err= -1;
	err = aControlPoint.InitiateControl ( iControlChannel, aUri );
	_LIT(KInfoLogFile, "InitiateControl() on Control Point Done with %d.... \n");
	iManager->INFO_PRINTF2(KInfoLogFile,err);
	iCtrlMsg.SetMessageDes ( buf );
	iCtrlMsg.SetLast ();
	TInt newLen = buf.Length();
	iCtrlMsg.SetMaxLength (newLen);
	iControlChannel.Send ( iCtrlMsg, iStatus );
	SetActive();
	CActiveScheduler::Start();
	_LIT(KInfoLogFile1, "InitiateActionControlL End.... \n");
	iManager->INFO_PRINTF1(KInfoLogFile1);
	CleanupStack::PopAndDestroy(2 );
	}


