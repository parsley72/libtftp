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
#include "tftp.h"
#include "utils.h"

#include <netinet/in.h>

#include <qcstring.h>
 
namespace TFTP {
 
/*!
\brief Sends an error via the socket
\param sd Socket to send the error to
\param ec Error code to send
\param errMsg Error message string

The packet format is this:
<pre>
	2 bytes  2 bytes        string    1 byte
	----------------------------------------
ERROR | 05    |  ErrorCode |   ErrMsg   |   0  |
	----------------------------------------
</pre>

*/
void sendError(TransferInfo &ti, ErrorCode ec, const QString &errMsg)
{
	qWarning("About to send error %d [%s]", ec, errMsg.latin1());
	QByteArray dgram( errMsg.length() + 5 );
	
	wordOfArray(dgram)[0] = htons( (uint16_t)ERROR );
	wordOfArray(dgram)[1] = htons( (uint16_t)ec );
	
	memcpy( dgram.data() + 4, errMsg.ascii(), errMsg.length()+1 );
	
	if( ti.sd->writeBlock(dgram.data(), dgram.size(), ti.dAddr, ti.dPort) == -1 )
		qWarning( "Error sending error packet o_O %d", ti.sd->error() );
}

};
