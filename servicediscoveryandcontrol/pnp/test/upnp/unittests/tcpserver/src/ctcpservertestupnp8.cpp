/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include <ss_glob.h>
#include <comms-infras/ss_thread.h>
#include <comms-infras/ss_nodemessages_factory.h>
#include <comms-infras/ss_nodemessages_dataclient.h>
#include "ptcpservertests.h"
#include "upnp_cf_msgs.h"
#include "upnpflowbase.h"
#include "upnpflowfactory.h"

#include "CTcpServerTestUPnP8.h"

using namespace Messages;
const TUint16 KActivityNull = 0;
const TUint KHttpDefaultPort = 80;

//------------------------------------------------------------//	
/////////////////// CTcpServerTestUPnP8 /////////////////////////
////// Testcase: Send the test data from the test client/////////
//------------------------------------------------------------//

_LIT8 ( KTestUPnPServiceUri,"http://127.0.0.1:80/testserviceuri1" );
_LIT8 ( KSubscribe, "GET1 /testserviceuri1 HTTP/1.1\r\nHOST: 127.0.0.1:80\r\n\r\n");
_LIT8 ( KExpectedResponse, "HTTP/1.1 501 Not Implemented");

CTcpServerTestUPnP8* CTcpServerTestUPnP8::NewL ( CProxyProvd* aProvd )
	{
	CTcpServerTestUPnP8* ret = new ( ELeave ) CTcpServerTestUPnP8 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CTcpServerTestUPnP8::CTcpServerTestUPnP8 ( CProxyProvd* aProvd ) :
	CTcpServerTestBase ( aProvd ), iSocketHandler ( *this ), iHostAddr ( KInetAddrLoop, KHttpDefaultPort ), iState ( ECreateServer )
	{
	}
	
CTcpServerTestUPnP8::~CTcpServerTestUPnP8 ()
	{
	iServiceUri.Close ();
		
	iSocketHandler.CancelAll ();
	iClientSocket.Close ();
	delete iSocketOpener;
	
	iTimer.Close ();
	
	TUpnpMessage::DeRegister ();
	}
	
void CTcpServerTestUPnP8::ConstructL ()
	{
	TUpnpMessage::RegisterL ( SockManGlobals::Get ()->SelfWorker ());
	
	iServiceUri.CreateL ( KTestUPnPServiceUri );
	
	iTimer.CreateLocal ();
	TRAPD ( err, iSocketOpener = CSocketOpener::NewL ( *this ) );			
	
	User::LeaveIfError ( err );		
	}

TVerdict CTcpServerTestUPnP8::RunTestL ()
	{
	switch ( iState )
		{
		case ECreateServer:
			{
			const TUint8* uriPtr = NULL;
			uriPtr = iServiceUri.Ptr ();
			
			const TUid reqUid = { CUPnPFlowFactory::iUid };		
			THttpServerFlowQuery flowQuery ( uriPtr,
										 iServiceUri.Size (),
										 Id (),
										 EHttpServerFlow, iChunkManager );
			TCFPlayerRole playerRole ( TCFPlayerRole::EDataPlane );
			
			TCFFactory::TFindOrCreatePeer msg ( TCFPlayerRole::EDataPlane, reqUid, &flowQuery );
			TNodeId factoryContainer = SockManGlobals::Get()->GetPlaneFC( playerRole );
			
			RClientInterface::OpenPostMessageClose ( Id (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, reqUid, &flowQuery ).CRef () );
			
			iState  = EStartServer;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
			
		case EStartServer:
			{
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iTcpServer ), TCFDataClient::TStart ().CRef () );
			
			iState  = EOpenClientSocketAndConnect;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
			
		case EOpenClientSocketAndConnect:
			{
			iSocketOpener->MakeSocket ( KAfInet, KSockStream, KProtocolInetTcp );
			
			iState  = ESendData;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
			
		case ESendData:
			{			
			iSendChain.CreateL ( KSubscribe );
			iSocketHandler.Send ( iSendChain );
			
			iState = ESendStopServer;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		// just to make sure production objects are cleaned-up properly.
		case ESendStopServer:
			{	
			// cleanup tcp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iTcpServer ), TCFDataClient::TStop ( KErrNone ).CRef () );
			
			iState = EServerStopped;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case EServerStopped:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iTcpServer ), TEChild::TDestroy ().CRef () );
			
			iTimer.After ( iStatus, 1000000 ); //1secs
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case ECleanup:
			{
			return iResponse; // Here the final test case is passed back to the RSocket::Ioctl
			}
					
		default:
			{
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
			}
		}
	}
	
void CTcpServerTestUPnP8::OpenComplete ( RInternalSocket& aSocket )
	{
	iClientSocket = aSocket;
	iSocketHandler.Attach ( aSocket );	
	
	iSocketHandler.Connect ( iHostAddr );
	}
	
void CTcpServerTestUPnP8::ConnectComplete ()
	{	
	CompleteSelf ( KErrNone );
	}
	
void CTcpServerTestUPnP8::SendComplete ( TInt /*aLength*/ )
	{
	iSendChain.Init ();
	iSocketHandler.Recv ();
	}
	
void CTcpServerTestUPnP8::RecvComplete ( RMBufChain& aData )
	{
	RBuf8 responseBuf;
	responseBuf.CreateMax ( aData.Length () );
	aData.CopyOut ( responseBuf );
	aData.Free ();
	
	if ( responseBuf.FindF ( KExpectedResponse ) == KErrNotFound )
		{
		iResponse = EFail; // test case failed
		}
	else
		{
		iResponse = EPass;	// test case passed
		}
	
	responseBuf.Close ();
	CompleteSelf ( KErrNone );
	}
	
void CTcpServerTestUPnP8::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iTcpServer = msg.iNodeId;
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStarted> () 
				|| aMessage.IsMessage<TCFDataClient::TStopped> () )
		{
		CompleteSelf ( KErrNone );
		}
	}
	
