/***************************************************************************
 *   Copyright (C) 2004 by Flameeyes                                       *
 *   dgp85@users.sourceforge.net                                           *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License version 2.1 as published by the Free Software Foundation.     *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Lesser General Public License for more details.                   *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef TFTP_H
#define TFTP_H

#include <stdint.h>

#include <qstring.h>
#include <qsocketdevice.h>
#include <qhostaddress.h>

/*!
\brief TFTP Management namespace

This namespace contains classes, enums, functions for TFTP management
*/
namespace TFTP {
	//! Default port of TFTP service
	static const uint32_t defaultPort = 69;
	
	/*!
	\brief Transfer parameters
	
	This struct contains all the field needed by a transfer session to
	find itself and to send the data to the right peer.
	*/
	struct TransferInfo {
		QSocketDevice *sd;		//!< QSocketDevice to use
		const QHostAddress dAddr;	//!< Address of the transfer peer
		const uint16_t dPort;		//!< Port of the transfer peer
		
		TransferInfo() :
			sd(0), dAddr(QHostAddress()), dPort(0)
		{ }
		
		/*!
		\brief Short constructor
		\param sd Pointer to the QSocketDevice to use and to take the info from
		*/
		TransferInfo(QSocketDevice *D) :
			sd(D), dAddr( sd->peerAddress() ), dPort( sd->peerPort() )
		{ }
		
		/*!
		\brief Parameterized constructor
		\param D Pointer to the QSocketDevice to use
		\param A Address of the transfer peer
		\param P Port of the transfer peer
		*/
		TransferInfo(QSocketDevice *D, const QHostAddress &A, const uint16_t P) :
			sd(D), dAddr(A), dPort(P)
		{ }
	};
	
	/*!
	\brief Compare operator between two TransferInfo structs
	\param a First instance to compare
	\param b Second instance to compare
	\return true if all the fields of the structs are equal
	*/
	inline bool operator ==(const TransferInfo &a, const TransferInfo &b)
	{
		return	a.sd == b.sd &&
			a.dAddr == b.dAddr &&
			a.dPort == b.dPort;
	}
	
	//! List of opcodes which can be found in a TFTP header
	enum Opcodes {
		OInvalid,	//!< Invalid opcode
		RRQ,		//!< Read request
		WRQ,		//!< Write request
		DATA,		//!< Data packet
		ACK,		//!< Acknowledge
		ERROR		//!< Error
	};
	
	//! List of error codes which can be found in a TFtp error datagram
	enum ErrorCode {
		Undefined,	//!< Not defined, see error message (if any).
		FileNotFound,	//!< File not found.
		AccessViolation,//!< Access violation.
		DiskFull,	//!< Disk full or allocation exceeded.
		IllegalOp,	//!< Illegal TFTP operation.
		UnknownTID,	//!< Unknown transfer ID.
		FileExists,	//!< File already exists.
		NoUser		//!< No such user.
	};
	
	//! List of transfer modes
	enum Mode {
		MInvalid,	//!< Invalid mode
		NetAscii,	//!< ASCII-mode transfer
		Octet,		//!< Binary-mode transfer
		Mail		//!< Mail transfer (not supported)
	};

	void sendError(TransferInfo &ti, ErrorCode ec, const QString &errMsg);
	void sendAck(TransferInfo &ti, uint16_t block);
};

#endif
