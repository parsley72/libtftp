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
#ifndef TFTPCLIENT_H
#define TFTPCLIENT_H

#include "tftp.h"

#include <qobject.h>
#include <qsocketdevice.h>

class QSocketNotifier;

namespace TFTP {

	/*!
	\brief TFTP Client implementation
	
	This class is used to creare a minimal TFTP client, to receive
	and send files.
	*/
	class Client : public QObject
	{
	Q_OBJECT
	public:
		Client(const QHostAddress &addr, uint16_t port = defaultPort);
		~Client();
		
		void putFile(const QString &localname, const QString &remotename, Mode trmode = Octet);
		void getFile(const QString &localname, const QString &remotename, Mode trmode = Octet);
		class WriteSession;
		class ReadSession;
	
	signals:
		/*!
		\brief Receiving file completed
		\param dev QIODevice instance where the file was wrote
		\param remotefilename Name of the file on the remote side
		
		This signal is emitted when the receiving of a file is completed
		*/
		void receivedFile(QIODevice *dev, const QString remotefilename);
		
		/*!
		\brief Sending file completed
		\param dev QIODevice instance from which the file was read
		\param remotefilename Name of the file on the remote side
		
		This signal is emitted when the sending of a file is completed
		*/
		void sentFile(QIODevice *dev, QString remotefilename);
	
	protected slots:
		void dataReceived();
	
	protected:
		QValueList<WriteSession*> writes;
			//!< List of all opened write instances
		QValueList<ReadSession*> reads;
			//!< List of all opened read instances
			
		WriteSession *findWSession(const TransferInfo &ti);
		ReadSession *findRSession(const TransferInfo &ti);
		
		QSocketDevice sd;	//!< Socket device bound to the port
		QSocketNotifier *sn;	//!< Socket notifier opened on Server::sd
	
	};

};

#endif
