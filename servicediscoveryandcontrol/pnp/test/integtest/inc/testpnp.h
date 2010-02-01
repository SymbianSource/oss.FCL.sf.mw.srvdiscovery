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
// Contains declarations of CTestPnP class
// 
//

#ifndef __TEST_PNP_H__
#define __TEST_PNP_H__

#include <testexecuteserverbase.h>

/*
CTestPnP class which is derived from the Test server and provides the functionalities to 
perform ability to run individual test steps (test cases or part testcases) either sequentially 
or concurrently within scripts (test harnesses). 
*/

class CTestPnP: public CTestServer
	{
public:
	static CTestPnP* NewL();
	// Base class pure virtual override
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);

protected:
	CTestStep*    CreateTestStepL(const TDesC& aStepName);
	};

#endif // __TEST_PNP_H__
