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
#include "readsession.h"
#include "utils.h"

#include <netinet/in.h>

#include <qfile.h>

namespace TFTP {

ReadSession::~ReadSession()
{
	delete ba;
}

/*!
\brief Loads data from the file opened into the memory
\note This function allocates all the space needed by the file. This can be
large up to 32Mb. Please be sure you have enough memory to support this.
*/
void ReadSession::loadFile()
{
	ba = new QByteArray( m_currentFile->size() );
	
	switch(currentMode)
	{
		case NetAscii:
			qWarning("NetAscii transfers not supported");
			break;
		case Octet:
			uint32_t actual = m_currentFile->readBlock(ba->data(), ba->size());
			if ( actual != ba->size() ) // Shouldn't be used
				ba->resize(actual);
			break;
	}
}

/*!
\brief Sends the data to the peer
\return This function returns true if all the packets are already sent

The packet format is this:
<pre>
          2 bytes    2 bytes       n bytes
          ---------------------------------
   DATA  | 03    |   Block #  |    Data    |
          ---------------------------------
</pre>

*/
bool ReadSession::sendData()
{
	uint32_t offset = currentBlock * 512;
	int32_t length = ba->size() - offset;
	if ( length < 0 )
		return true;
	
	length = length > 512 ? 512 : length;
	

	QByteArray dgram( length + 4 );
	
	wordOfArray(dgram)[0] = htons( (uint16_t)DATA );
	wordOfArray(dgram)[1] = htons( currentBlock+1 );
	
	memcpy(dgram.data() + 4, ba->data() + offset, length);
	
	if( ti.sd->writeBlock(dgram.data(), dgram.size(), ti.dAddr, ti.dPort) == -1 )
		qWarning("Error sending data packet [%d]", ti.sd->error() );
	
	return false;
}

/*!
\brief Parses an acknowledge packet and send the right data
\param dgram Datagram to parse
\return false if the ack packet is accepted, and no error is thrown, else true
\note The calling function should delete the object if parseAck() returned true
*/
bool ReadSession::parseAck(const QByteArray &dgram)
{
	if ( ntohs(wordOfArray(dgram)[0]) != ACK )
	{
		qWarning("Not an ACK packed passed to parseData()");
		return true;
	}
	
	if ( currentBlock != ntohs(wordOfArray(dgram)[1]) -1 )
	{
		qWarning("Expected %d, got %d", currentBlock+1, ntohs(wordOfArray(dgram)[1]));
		sendError(ti, IllegalOp, "Error in block sequence");
		return true;
	}
	
	currentBlock = ntohs(wordOfArray(dgram)[1]);
	
	return sendData();
}

};
