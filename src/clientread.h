/***************************************************************************
 *   Copyright (C) 2004 by Flameeyes                                       *
 *   dgp85@users.sourceforge.net                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef TFTPCLIENTREAD_H
#define TFTPCLIENTREAD_H

#include "readsession.h"
#include "client.h"

namespace TFTP {

	/*!
	\author Flameeyes
	\brief Single Transfer Session (Read)
	
	This class represent a single read session, and stores information
	about the file and the transfer itself.
	*/
	class Client::ReadSession : public TFTP::ReadSession
	{
	public:
		ReadSession(TransferInfo T, const QString &local, const QString &remote, Mode trmode = Octet);
		ReadSession(TransferInfo T, QIODevice *data, const QString &remote, Mode trmode = Octet);
		~ReadSession();
	protected:
		void sendRequest(const QString &remote);
	
	};

};

#endif
