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
#include "writesession.h"
#include "utils.h"

#include <netinet/in.h>

#include <qfile.h>

namespace TFTP {

WriteSession::~WriteSession()
{
}

/*!
\brief Sends an acknoledge via the socket

The packet format is this:
<pre>
          2 bytes    2 bytes
          --------------------
   ACK   | 04    |   Block #  |
          --------------------
</pre>

*/
void WriteSession::sendAck()
{
	QByteArray dgram(4);
	wordOfArray(dgram)[0] = htons( (uint16_t)ACK );
	wordOfArray(dgram)[1] = htons( currentBlock );
	
	if( ti.sd->writeBlock(dgram.data(), dgram.size(), ti.dAddr, ti.dPort) == -1 )
		qWarning("Error sending acknowledge packet [%d]", ti.sd->error() );
}

/*!
\brief This function is used by Server::WriteSession and Client::WriteSession to write
	the data to the local file, changing to the right mode if needed
*/
void WriteSession::actualWriteBlock(char *data, uint32_t size)
{
	switch(currentMode)
	{
		case NetAscii:
			qWarning("NetAscii transfers not supported");
			break;
		case Octet:
			m_currentFile->writeBlock( data, size );
			break;
	}
}

/*!
\brief Parses a data packet and acknowledge it
\param dgram Datagram to parse
\return false if the data packet is accepted, and no error is thrown, else true
\note The calling function should delete the object if parseData returned true
*/
bool WriteSession::parseData(const QByteArray &dgram)
{
	if ( ntohs(wordOfArray(dgram)[0]) != DATA )
	{
		qWarning("Not a data packed passed to parseData()");
		return true;
	}
	
	if ( currentBlock != ntohs(wordOfArray(dgram)[1]) -1 )
	{
		qWarning("Expected %d, got %d", currentBlock+1, ntohs(wordOfArray(dgram)[1]));
		sendError(ti, IllegalOp, "Error in block sequence");
		return true;
	}
	
	currentBlock = ntohs(wordOfArray(dgram)[1]);
	
	int length = dgram.size() - 4;
	
	actualWriteBlock( dgram.data() +4, length );
	
	sendAck();
	
	if ( length != 512 )
		return true;

	return false;
}

};
