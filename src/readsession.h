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
#ifndef TFTPREADSESSION_H
#define TFTPREADSESSION_H

#include "session.h"

namespace TFTP {

	/*!
	\brief Class for inbound TFTP transfers
	*/
	class ReadSession : public Session
	{
	public:
		~ReadSession();
		bool parseAck(const QByteArray &dgram);
	protected:
		ReadSession(TransferInfo T) : Session(T) { };
		void loadFile();
		bool sendData();
		
		QByteArray *ba; //!< Byte array with file's content
	};

};

#endif
