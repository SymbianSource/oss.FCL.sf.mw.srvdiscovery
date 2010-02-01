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

#ifndef __UPNPCONTROLSCPRFACTORY_H
#define __UPNPCONTROLSCPRFACTORY_H

#include <comms-infras/ss_subconnprov.h>

class CUPnPControlSubConnectionProviderFactory : public ESock::CSubConnectionProviderFactoryBase
	{		
public:
	// ECOM interface implementation ID
	static const TUint iUid = 0x2000D057;

public:
	static CUPnPControlSubConnectionProviderFactory* NewL(TAny* aParentContainer);

protected:
	CUPnPControlSubConnectionProviderFactory(
		TUid aFactoryId, 
		ESock::CSubConnectionFactoryContainer& aParentContainer);

protected:
    // CSubConnectionProviderFactoryBase overrides
	virtual ESock::ACommsFactoryNodeId* DoCreateObjectL(ESock::TFactoryQueryBase& aQuery);
	};

#endif // UPNPCONTROLSCPRFACTORY_H
