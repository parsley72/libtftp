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
#ifndef TFTPCLIENTWRITE_H
#define TFTPCLIENTWRITE_H

#include "writesession.h"
#include "client.h"

namespace TFTP {

	/*!
	\author Flameeyes
	\brief Single Transfer Session (Write)
	
	This class represent a single write session, and stores information
	about the file and the transfer itself.
	*/
	class Client::WriteSession : public TFTP::WriteSession
	{
	public:
		WriteSession(TransferInfo T, const QString &local, const QString &remote, Mode trmode = Octet);
		WriteSession(TransferInfo T, QIODevice *data, const QString &remote, Mode trmode = Octet);
		~WriteSession();
	protected:
		void sendRequest(const QString &remote);
	
	};

};

#endif
