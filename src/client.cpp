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
#include "client.h"
#include "clientread.h"
#include "clientwrite.h"
#include "utils.h"

#include <qsocketnotifier.h>

#include <netinet/in.h>

namespace TFTP {

/*!
\brief Finds the write instance which has the provided TransferInfo data
\param ti TransferInfo data to find the write instance of
\return A pointer to the write session, or NULL if not foundable
*/
Client::WriteSession *Client::findWSession(const TransferInfo &ti)
{
	for(QValueList<WriteSession*>::iterator it = writes.begin(); it != writes.end(); it++)
		if ( (*it)->transferInfo() == ti )
			return (*it);
	
	return NULL;
}

/*!
\brief Finds the read instance which has the provided TransferInfo data
\param ti TransferInfo data to find the read instance of
\return A pointer to the read session, or NULL if not foundable
*/
Client::ReadSession *Client::findRSession(const TransferInfo &ti)
{
	for(QValueList<ReadSession*>::iterator it = reads.begin(); it != reads.end(); it++)
		if ( (*it)->transferInfo() == ti )
			return (*it);
	
	return NULL;
}

/*!
\brief Default constructor
\param addr Address of the server to connect to
\param port Port of the server to connect to (default 69)

\note You should create one client per server you want to connect to
*/
Client::Client(const QHostAddress &addr, uint16_t port)
 : QObject(), sd(QSocketDevice::Datagram)
{
	sd.setBlocking(false);
	sd.connect(addr, port);
	
	sn = new QSocketNotifier(sd.socket(), QSocketNotifier::Read, this);
	connect(sn, SIGNAL(activated(int)), this, SLOT(dataReceived()));
}

Client::~Client()
{
}

//! Slot called when data is received from the socket
void Client::dataReceived()
{
	QByteArray dgram( sd.bytesAvailable() );
	sd.readBlock( dgram.data(), dgram.size() );
	
	TransferInfo ti(&sd, sd.peerAddress(), sd.peerPort());
	switch( ntohs(wordOfArray(dgram)[0]) )
	{
		case ACK: {
				ReadSession *rs = findRSession(ti);
				if ( !rs )
				{
					sendError(ti, IllegalOp, "ACK packet when no file is being uploaded");
					return;
				}
				
				if ( rs->parseAck(dgram) )
				{
					emit sentFile(rs->currentFile(), rs->currentFilename());
					reads.remove(rs);
					delete rs;
				}
			} break;
		case DATA: {
				WriteSession *ws = findWSession(ti);
				if ( !ws )
				{
					sendError(ti, IllegalOp, "DATA packet when no file is being downloaded");
					return;
				}
			
				if ( ws->parseData(dgram) )
				{
					emit receivedFile(ws->currentFile(), ws->currentFilename());
					writes.remove(ws);
					delete ws;
				}
			} break;
		case ERROR: {
				Session *s;
				if ( (s = findWSession(ti) ) )
					writes.remove(reinterpret_cast<WriteSession*>(s));
				else if ( (s = findRSession(ti) ) )
					reads.remove(reinterpret_cast<ReadSession*>(s));
				else
					qWarning("Error received without a session opened for the peer");
				
				if ( s )
					qWarning(
						"Error packet received, peer session aborted\n%s [%d]",
						dgram.data() + 4,
						ntohs(wordOfArray(dgram)[1])
						);
			} break;
		default:
			sendError(ti, IllegalOp, "Illegal TFTP opcode");
			return;
	}
}

/*!
\brief Initiate an upload transfer to the server
\param localname Name of the file in the local filesystem
\param remotename Name of the file on the remote side

\note For client, the concept of Read and Write are inverted: to a ReadSession
	on one side, the other side is always using a WriteSesssion!
*/
void Client::putFile(const QString &localname, const QString &remotename, Mode trmode)
{
	reads.push_back( new ReadSession(TransferInfo(&sd), localname, remotename, trmode) );
}

/*!
\brief Initiate a download transfer to the server
\param localname Name of the file in the local filesystem
\param remotename Name of the file on the remote side

\note For client, the concept of Read and Write are inverted: to a ReadSession
	on one side, the other side is always using a WriteSesssion!
*/
void Client::getFile(const QString &localname, const QString &remotename, Mode trmode)
{
	writes.push_back( new WriteSession(TransferInfo(&sd), localname, remotename, trmode) );
}

};
#include "client.moc"
