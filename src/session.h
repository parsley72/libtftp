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
#ifndef TFTPSESSION_H
#define TFTPSESSION_H

#include "tftp.h"

#include <qstring.h>
#include <qcstring.h>
#include <qmap.h>

class QIODevice;

namespace TFTP {

	/*!
	\brief TFTP transfers base class
	
	This class is the parent class of all the TFTP transfer classes
	( Server::WriteSession Server::ReadSession
	Client::WriteSession Client::ReadSession )
	
	\warning Be sure you connect the sentFile and receivedFile slots of Client and Server,
		because this class doesn't close the device anymore!
	
	*/
	class Session{
	public:
		virtual ~Session();
		void closeDevice();
		
		//! Gets the current TransferInfo (Server::WriteSession::ti)
		inline const TransferInfo transferInfo() const
		{ return ti; }
	protected:
		Session(TransferInfo T) : ti(T) { };
		
		TransferInfo ti;	//!< Transfer peer information
		Mode currentMode;	//!< Mode for current transfer (should be Invalid when not in transfer)
		uint16_t currentBlock;	//!< Index of the current block (last one acknowledged)
		QString m_currentFilename;//!< String containing the current file as supplied by
					//!< the client (not remapped)
		QIODevice *m_currentFile;//!< Pointer to the QFile (or QBuffer) instance which
					//!< represent the current file being read or wrote
	public:
		//! Gets the current device
		QIODevice *currentFile() const
		{ return m_currentFile; }
		
		//! Gets the remote filename
		QString currentFilename() const
		{ return m_currentFilename; }
		
//@{
/*!
\name remap
\brief Filename remapping stuff

These functions are used both for write and read files. Obviously, 
*/
		//! Struct representing a mapped filename
		struct MappedFN {
			QIODevice *dev;	//!< Device to save the file to (can be a QBuffer for instance)
			bool oneshot;	//!< If true, the mapping should be removed after one file is transferred
		};
		
		typedef QMap<QString, MappedFN> Remapped; //!< A map of Server::MappedFN
//@}
	
	};

};

#endif
