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
//


#ifndef __UPNPPARAMSEXTFACTORY_H__
#define __UPNPPARAMSEXTFACTORY_H__

#include <e32base.h>

class CUPnPParamSetBase;

class CUPnPParamsExtFactory : public CBase
{
	public:
	static CUPnPParamSetBase* NewL ( TAny* aConstructionParams );
};

#endif // __UPNPPARAMSEXTFACTORY_H__
