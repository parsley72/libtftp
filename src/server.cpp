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
#include "server.h"
#include "serverwrite.h"
#include "serverread.h"
#include "utils.h"

#include <qsocketnotifier.h>
#include <qfile.h>

#include <netinet/in.h>

namespace TFTP {

QValueList<Server::WriteSession*> Server::writes;

/*!
\brief Finds the write instance which has the provided TransferInfo data
\param ti TransferInfo data to find the write instance of
\return A pointer to the write session, or NULL if not foundable
*/
Server::WriteSession *Server::findWSession(const TransferInfo &ti)
{
	for(QValueList<WriteSession*>::iterator it = writes.begin(); it != writes.end(); it++)
		if ( (*it)->transferInfo() == ti )
			return (*it);
	
	return NULL;
}

QValueList<Server::ReadSession*> Server::reads;

/*!
\brief Finds the read instance which has the provided TransferInfo data
\param ti TransferInfo data to find the read instance of
\return A pointer to the read session, or NULL if not foundable
*/
Server::ReadSession *Server::findRSession(const TransferInfo &ti)
{
	for(QValueList<ReadSession*>::iterator it = reads.begin(); it != reads.end(); it++)
		if ( (*it)->transferInfo() == ti )
			return (*it);
	
	return NULL;
}

/*!
\brief Default constructor
\param addr Address to bind the server to
\param port Port to bind the server to (default = 69, as in RFC)
\note Root access should be needed to access port &lt; 1024
*/
Server::Server(const QHostAddress &addr, uint16_t port)
 : QObject(), sd(QSocketDevice::Datagram)
{
	sd.setBlocking(false);
	sd.bind(addr, port);
	
	sn = new QSocketNotifier(sd.socket(), QSocketNotifier::Read, this);
	connect(sn, SIGNAL(activated(int)), this, SLOT(dataReceived()));
}

Server::~Server()
{
	delete sn;
	for(QValueList<WriteSession*>::iterator it = writes.begin(); it != writes.end(); it++)
		delete (*it);
}

//! Slot called when data is received from the socket
void Server::dataReceived()
{
	QByteArray dgram( sd.bytesAvailable() );
	sd.readBlock( dgram.data(), dgram.size() );
	
	TransferInfo ti(&sd);
	switch( ntohs(wordOfArray(dgram)[0]) )
	{
		case RRQ: {
				ReadSession *rs = findRSession(ti);
				if ( rs )
				{
					sendError(ti, IllegalOp, "Duplicate write request from same peer");
					return;
				}
				
				reads.push_back( new ReadSession(ti, dgram) );
			} break;
		case ACK: {
				ReadSession *rs = findRSession(ti);
				if ( !rs )
				{
					sendError(ti, IllegalOp, "ACK packet without a RRQ");
					return;
				}
				
				if ( rs->parseAck(dgram) )
				{
					emit sentFile(rs->currentFile(), rs->currentFilename());
					reads.remove(rs);
					delete rs;
				}
			} break;
		case WRQ: {
				WriteSession *ws = findWSession(ti);
				if ( ws )
				{
					qWarning("Duplicate write request from same peer");
					sendError(ti, IllegalOp, "Duplicate write request from same peer");
					return;
				}
				
				writes.push_back( new WriteSession(ti, dgram) );
			} break;
		case DATA: {
				WriteSession *ws = findWSession(ti);
				if ( !ws )
				{
					qWarning("DATA packet without a WRQ");
					sendError(ti, IllegalOp, "DATA packet without a WRQ");
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

};

#include "server.moc"
