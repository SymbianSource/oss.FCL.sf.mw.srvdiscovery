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


#if ( !defined __TEST02ALLOCDEALLOC_H__ )
#define __TEST02ALLOCDEALLOC_H__

#include "chunkmgrteststepbase.h"

_LIT ( KTest02, "ChunkMgrTest02" );

class CTest02AllocDealloc: public CChunkMgrTestStepBase
	{
public:
	TVerdict doTestStepL ();
	};

#endif // __TEST02ALLOCDEALLOC_H__
