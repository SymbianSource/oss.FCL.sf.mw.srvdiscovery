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
// @file
// @internalComponent
// 
//

#if ( !defined __TEST08MULTIPLETHREADS_H__ )
#define __TEST08MULTIPLETHREADS_H__

#include "chunkmgrteststepbase.h"

_LIT ( KTest08, "ChunkMgrTest08" );

class CTest08MultipleThreads: public CChunkMgrTestStepBase
	{
public:
	TVerdict doTestStepL ();
	static TInt ChildThreadStep ( TAny* aParams );
private:
	TInt iMainTryCount;
	TInt iChildTryCount;
	};

#endif // __TEST08MULTIPLETHREADS_H__
