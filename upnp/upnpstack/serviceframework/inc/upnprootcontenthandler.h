/** @file
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Declares the CUpnpRootContentHandler class
*
*/



#ifndef __UPNPROOTCONTENTHANDLER_H__
#define __UPNPROOTCONTENTHANDLER_H__

#include <babitflags.h>

#include "upnpcontenthandler.h"

/**
*  This class handles root tag of xml device descriptions
*  It extends CUpnpContentHandler class, thus it is subsequent implementation of MContentHandler interface
*  It is designed to be used by upnp controler that delegates to it events from xml sax parser
*
*  @since Series60 2.6
*/
class CUpnpDevice;

using namespace Xml;

NONSHARABLE_CLASS( CUpnpRootContentHandler ) : public CUpnpContentHandler
{
protected: 
    enum TState
        { 
        EInitial,
        EURLBase
        //ESpecVersion not used now
        };
public:

    /**
    * Two-phased constructor
    * @since Series60 3.2 
    * @return instance of CUpnpRootContentHandler class
    */  
    static CUpnpRootContentHandler* NewL(
            CUpnpContentHandlersController& aController, CUpnpDevice& aResultDevice,
            TParseType aParseType );
    
    /**
    * Two-phased constructor. Leaves the object on CleanupStack
    * @since Series60 3.2 
    * @return instance of CUpnpRootContentHandler class
    */  
    static CUpnpRootContentHandler* NewLC( 
            CUpnpContentHandlersController& aController, CUpnpDevice& aResultDevice,
            TParseType aParseType );
    
    /**
    * Destructor of CUpnpRootContentHandler class
    * @since Series60 3.2     
    */	
    virtual ~CUpnpRootContentHandler();
    
public:	// from MContentHandler
    
    /**
    * This method is a callback to indicate an element has been parsed.
    * @param				aElement is a handle to the element's details.
    * @param				aAttributes contains the attributes for the element.
    */
    virtual void OnStartElementL( const RTagInfo& aElement, 
    							  const RAttributeArray& aAttributes );

    /**
    * This method is a callback to indicate the end of the element has been reached.
    * @param              aElement is a handle to the element's details.
    */
    virtual void OnEndElementL( const RTagInfo& aElement );
    
    /**
    * This method is a callback that sends the content of the element.
    * aErrorCode should be KErrNone, and that aBytes should contains complete
    * content (one chunk).
    * @param              aBytes is the raw content data for the element in one chunk
    */
     virtual void OnContentL( const TDesC8& aBytes ); 
    
protected:

    /**
    * Default C++ constructor 
    * @since Series60 3.2     
    */  
    CUpnpRootContentHandler( CUpnpContentHandlersController& aController, 
        CUpnpDevice& aResultDevice, TParseType aParseType );

private:
    CUpnpDevice& iResultDevice;
    TState iCurrentState;
    TBitFlags8 iFoundTags;
    TParseType iParseType;
    
};

#endif //__UPNPROOTCONTENTHANDLER_H__