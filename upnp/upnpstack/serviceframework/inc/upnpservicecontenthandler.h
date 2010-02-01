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
* Description:  Declares the CUpnpServiceContentHandler class
*
*/


#ifndef __UPNPSERVICECONTENTHANDLER_H__
#define __UPNPSERVICECONTENTHANDLER_H__

#include "upnpcontenthandler.h"

/**
 *  This class handles xml service descriptions
 *  It extends CUpnpContentHandler class, thus it is subsequent implementation of MContentHandler interface
 *  It is designed to be used by upnp controler that delegates to it events from xml sax parser
 *
 *  @since Series60 2.6
 */
class CUpnpDevice;
class CUpnpService;

using namespace Xml;

NONSHARABLE_CLASS(CUpnpServiceContentHandler) : public CUpnpContentHandler
    {
public:

    /**
     * Two-phased constructor
     * @since Series60 3.2 
     * @return instance of CUpnpServiceContentHandler class
     */
    static CUpnpServiceContentHandler* NewL(
            CUpnpContentHandlersController& aController, 
            CUpnpDevice* aParentDevice );

    /**
     * Two-phased constructor. Leaves the object on CleanupStack
     * @since Series60 3.2 
     * @return instance of CUpnpServiceContentHandler class
     */
    static CUpnpServiceContentHandler* NewLC(
            CUpnpContentHandlersController& aController, 
            CUpnpDevice* aParentDevice );

    /**
     * Two-phased constructor
     * @since Series60 3.2 
     * @return instance of CUpnpServiceContentHandler class
     */
    static CUpnpServiceContentHandler* NewL(
            CUpnpContentHandlersController& aController, 
            CUpnpService& aNotOwnedResultService );

    /**
     * Two-phased constructor. Leaves the object on CleanupStack
     * @since Series60 3.2 
     * @return instance of CUpnpServiceContentHandler class
     */
    static CUpnpServiceContentHandler* NewLC(
            CUpnpContentHandlersController& aController, 
            CUpnpService& aNotOwnedResultService );

    /**
     * Destructor of CUpnpServiceContentHandler class
     * @since Series60 3.2     
     */
    virtual ~CUpnpServiceContentHandler();

    /**
     * Returns parsed CUpnpService object, and pass ownership of it to the caller.
     * One shouldn't call this method if CUpnpServiceContentHandler dosn't have
     * ownership CUpnpService object.
     */
    CUpnpService* ResultService();

public: // from MContentHandler

    /**
     * This method is a callback to indicate an element has been parsed.
     * @param				aElement is a handle to the element's details.
     * @param				aAttributes contains the attributes for the element.
     */
    virtual void OnStartElementL( const RTagInfo& aElement, 
        const RAttributeArray& aAttributes );

    /**
     This method is a callback to indicate the end of the element has been reached.
     @param              aElement is a handle to the element's details.
     */
    virtual void OnEndElementL( const RTagInfo& aElement );

    /**
     This method is a callback that sends the content of the element.
     @param              aBytes is the raw content data for the element in one chunk
     */
    virtual void OnContentL( const TDesC8& aBytes );
    
protected:

    /**
     * Default C++ constructor 
     * @since Series60 3.2     
     */
    CUpnpServiceContentHandler( CUpnpContentHandlersController& aController, 
        CUpnpDevice* aParentDevice );

    /**
     * Default C++ constructor 
     * @since Series60 3.2     
     */
    CUpnpServiceContentHandler( CUpnpContentHandlersController& aController, 
        CUpnpService& aNotOwnedResultService );

private:
    CUpnpDevice* iParentDevice; //not owned
    CUpnpService* iResultService;
    TBool iIsServiceOwned;

    };

#endif //__UPNPSERVICECONTENTHANDLER_H__