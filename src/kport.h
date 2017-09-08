/***************************************************************************
 *   Copyright (C) 2005 by Mark Six                                        *
 *   marksix@xs4all.nl                                                     *
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
#ifndef KPORT_H
#define KPORT_H

#include <qobject.h>
#include "cpicoblaze.h"

class KPort :  public QObject, public CIOPort
{
	Q_OBJECT
	public:
		KPort( unsigned char portID ) : CIOPort( portID ) {}
		virtual ~KPort() {} ;
	
		unsigned char In() ;			// Called by Picoblaze
		void Out( unsigned char value ) ; // Called by Picoblaze
		
		unsigned char getWriteValue() ;
		unsigned char getReadValue() ;
		
		void setWriteValue( uint8_t value ) ;
		void setReadValue( uint8_t value ) ;
		
	signals:
		void write( unsigned char value ) ;
		void read() ;
		
	private:		
		unsigned char m_readValue, m_writeValue ;
} ;


#endif
