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
#ifndef TFTPWRITESESSION_H
#define TFTPWRITESESSION_H

#include "session.h"

namespace TFTP {

	/*!
	\brief Class for inbound TFTP transfers
	*/
	class WriteSession : public Session
	{
	public:
		virtual ~WriteSession();
		bool parseData(const QByteArray &dgram);
	protected:
		WriteSession(TransferInfo T) : Session(T) { };
		void actualWriteBlock(char *data, uint32_t size);
		void sendAck();	
	};

};

#endif
