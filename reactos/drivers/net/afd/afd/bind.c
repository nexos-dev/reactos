/* $Id: bind.c,v 1.1.2.1 2004/07/09 04:41:18 arty Exp $
 * COPYRIGHT:        See COPYING in the top level directory
 * PROJECT:          ReactOS kernel
 * FILE:             drivers/net/afd/afd/bind.c
 * PURPOSE:          Ancillary functions driver
 * PROGRAMMER:       Art Yerkes (ayerkes@speakeasy.net)
 * UPDATE HISTORY:
 * 20040708 Created
 */

#include "afd.h"
#include "tdi_proto.h"
#include "tdiconn.h"
#include "debug.h"

NTSTATUS WarmSocketForBind( PAFD_FCB FCB ) {
    NTSTATUS Status = STATUS_UNSUCCESSFUL;

    AFD_DbgPrint(MID_TRACE,("Called\n"));

    if( FCB->LocalAddress ) {
	Status = TdiOpenAddressFile
	    ( &FCB->TdiDeviceName,
	      FCB->LocalAddress,
	      &FCB->AddressFile.Handle, 
	      &FCB->AddressFile.Object );
    }

    if( !NT_SUCCESS(Status) ) {
	TdiCloseDevice( &FCB->AddressFile.Handle,
			FCB->AddressFile.Object );
	RtlZeroMemory( &FCB->AddressFile, sizeof( FCB->AddressFile ) );
    }

    AFD_DbgPrint(MID_TRACE,("Returning %x\n", Status));

    return Status;
}

NTSTATUS STDCALL
AfdBindSocket(PDEVICE_OBJECT DeviceObject, PIRP Irp,
	      PIO_STACK_LOCATION IrpSp) {
    NTSTATUS Status = STATUS_SUCCESS;
    PFILE_OBJECT FileObject = IrpSp->FileObject;
    PAFD_FCB FCB = FileObject->FsContext;
    PAFD_BIND_REQ BindReq = Irp->AssociatedIrp.SystemBuffer;
    
    AFD_DbgPrint(MID_TRACE,("Called\n"));
    
    if( !SocketAcquireStateLock( FCB ) ) return LostSocket( Irp );
    
    FCB->LocalAddress = TaCopyTransportAddress( &BindReq->Address );

    if( FCB->LocalAddress ) 
	Status = WarmSocketForBind( FCB );
    else Status = STATUS_NO_MEMORY;

    if( NT_SUCCESS(Status) ) 
	FCB->State = SOCKET_STATE_BOUND;

    return UnlockAndMaybeComplete( FCB, Status, Irp, 0, NULL );
}

