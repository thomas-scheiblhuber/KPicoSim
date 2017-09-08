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
#ifndef JTAGPROGRAMMER_H
#define JTAGPROGRAMMER_H

#include <qobject.h>
#include <string.h>
#include "jtag.h"
class JTAGProgrammer : public QObject 
{
	Q_OBJECT

	public:
    	JTAGProgrammer( QObject *parent );
	    ~JTAGProgrammer();

		void setBitFile( std::string filename ) ;

		void program() ;
	
	signals:
		void progress( int percent ) ;
		void message( const char *msg ) ;


	protected:
		int getDevice( bool *id ) ;

		std::string m_bitFilename ;

		CJTAG *m_dev ;
		QObject *m_parent ;

};

#endif
