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
#include "clientwrite.h"
#include "utils.h"

#include <netinet/in.h>

#include <qfile.h>

namespace TFTP {

/*!
\brief Default constructor
\param T TranferInfo struct with peer data
\param local Local filename
\param remote Remote filename
\param trmode Transfer mode

\note This library support ONLY octet transfer mode
*/
Client::WriteSession::WriteSession(TransferInfo T, const QString &local, const QString &remote, Mode trmode)
 : TFTP::WriteSession(T)
{
	currentMode = trmode;
	currentBlock = 0;
	m_currentFile = new QFile(local);
	m_currentFile->open(IO_WriteOnly | IO_Append);
	
	sendRequest(remote);
}

/*!
\brief Alternative version of constructor
\param T TransferInfo struct with peer data
\param data a QIODevice instance (QFile or QBuffer) to save the data received
\param remote Remote filename to use
\param trmode Transfer mode

\note This library support ONLY octet transfer mode
*/
Client::WriteSession::WriteSession(TransferInfo T, QIODevice *data, const QString &remote, Mode trmode)
 : TFTP::WriteSession(T)
{
	currentMode = trmode;
	currentBlock = 0;
	m_currentFile = data;
	
	sendRequest(remote);
}

/*!
\brief Actual write request function called by the two constructors
\param remote The filename to use on the remote side

\note This function is called by the two constructors, to avoid code redundancy

This function send a RRQ to the server to initiate transfer. The RRQ format
is this:

<pre>
          2 bytes    string   1 byte     string   1 byte
          -----------------------------------------------
   RRQ   |  01   |  Filename  |   0  |    Mode    |   0  |
          -----------------------------------------------
</pre>
*/
void Client::WriteSession::sendRequest(const QString &remote)
{
	QString modestring;
	switch( currentMode )
	{
		case Octet:
			modestring = "octet";
			break;
		case NetAscii:
			modestring = "netascii";
			break;
		default:
			qFatal("ERROR! Invalid mode in transfer operation!");
	}
	
	QByteArray dgram( 2 + remote.length() + 1 + modestring.length() + 1 );
	wordOfArray(dgram)[0] = htons( (uint16_t)RRQ );
	
	memcpy( dgram.data()+2, remote.ascii(), remote.length() +1 );
	memcpy( dgram.data()+2+remote.length()+1, modestring.ascii(), modestring.length() +1 );
	
	if( ti.sd->writeBlock(dgram.data(), dgram.size(), ti.dAddr, ti.dPort) == -1 )
		qWarning("Error sending RRQ packet [%d]", ti.sd->error() );
}

Client::WriteSession::~WriteSession()
{
}

};
