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
#include "kport.h"

unsigned char KPort::In() { 			// Called by Picoblaze
	emit read() ;
	return m_readValue ;
}

void KPort::Out( unsigned char value ) { // Called by Picoblaze
	m_writeValue = value ; 
	emit write( value ) ; 
}

unsigned char KPort::getWriteValue() { 
	return m_writeValue ; 
}

unsigned char KPort::getReadValue() {
	return m_readValue ;
} 

void KPort::setWriteValue( uint8_t value ) { 
	m_writeValue = value ; 
}

void KPort::setReadValue( uint8_t value ) {
	m_readValue = value ;
}


