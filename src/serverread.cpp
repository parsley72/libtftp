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
#include "serverread.h"
#include "utils.h"

#include <netinet/in.h>

#include <qfile.h>

namespace TFTP {

Session::Remapped Server::ReadSession::mapped;

/*!
\brief Default constructor
\param T Information of the peer to send the packets to
\param dgram WRQ packet received
*/
Server::ReadSession::ReadSession(TransferInfo &T, const QByteArray &dgram) :
	TFTP::ReadSession(T)
{
	if ( ntohs( wordOfArray(dgram)[0] ) != RRQ )
		qFatal("WriteSession created with a non-RRQ packet");
	
	m_currentFilename = QString(dgram.data() + 2);
	QString mode = QString( dgram.data() + 2 + m_currentFilename.length() + 1 ).lower();
	
	if ( mode == "netascii" ) // We need work for this
		currentMode = NetAscii;
	else if ( mode == "octet" ) // Good
		currentMode = Octet;
	else if ( mode == "mail" ) { // Not supported
		sendError(ti, IllegalOp, "Mail transfer not supported");
		return;
	} else {
		sendError(ti, IllegalOp, "Illegal transfer mode");
		return;
	}
	
	if ( mapped.contains(m_currentFilename) )
	{
		m_currentFile = mapped[m_currentFilename].dev;
		if ( mapped[m_currentFilename].oneshot )
			mapped.erase(m_currentFilename);
	} else {
		QString filename = m_currentFilename;
		filename.replace('\\', '_');
		filename.replace('/', '_');
		m_currentFile = new QFile(filename);
		m_currentFile->open(IO_ReadOnly);
		
	}
	
	loadFile();
	
	currentBlock = 0;
	
	sendData();
}


Server::ReadSession::~ReadSession()
{
}

/*!
\brief Remap a filename received from the server to a new one [WRQ only]
\param oldname Filename supplied by the client
\param newname Actual filename (and path) where to save the file
\param oneshot If true, the mapping of the filename will be removed after one
	file is transferred (useful to get timestamped files)
*/
void Server::ReadSession::remapFilename(const QString &oldname, QIODevice *dev, bool oneshot)
{
	mapped[oldname].dev = dev;
	mapped[oldname].oneshot = oneshot;
}

/*!
\brief Removes a mapping for a filename [WRQ only]
\param oldname Filename supplied by the client
*/
void Server::ReadSession::unmapFilename(const QString &oldname)
{
	mapped.erase(oldname);
}

};
