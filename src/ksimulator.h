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
#ifndef KSIMULATOR_H
#define KSIMULATOR_H

#include <qobject.h>
#include "cpicoblaze.h"
#include "cassembler.h"

#include <string>
#include <qtimer.h>
#include <klistview.h>

#include "kprocessorview.h"

#define ZERO_FLAG		0x01
#define CARRY_FLAG		0x02
#define INTERRUPT_FLAG	0x04

class KSimulator : public QObject
{
	Q_OBJECT
	public:
    	KSimulator(QObject *parent = 0, const char *name = 0);
	   	~KSimulator();
		
		void setFilename( string filename ) { m_assembler->setFilename(  filename  ) ; }				
		bool compile() { return m_assembler->assemble() ; } 
		bool exportVHDL( string templateFile, string outputDir, string entityName ) 
		{ 
			return m_assembler->exportVHDL( templateFile, outputDir, entityName ) ; 
		}

		bool exportHEX( string filename, bool mem )
		{
			return m_assembler->exportHEX( filename, mem ) ;
		}
		
		void setRegisterValues( unsigned char *values ) ;
		void getRegisterValues( unsigned char *values ) ;
		unsigned char getFlags() ;
		void setFlags( unsigned char flags ) ;
		void getScratchpad( unsigned char * values ) ;
		
		unsigned int getNextSourceLine() ;
		
		void assemblerError( unsigned int line, const char * str ) ;
		void setMessageList( KListView *messageList ) ;
		
		bool isRunning() ;
		
		CPicoBlaze * getCpu() ;
		
	signals:
		void stepped( unsigned int currentSourceLine ) ;
		
	public slots:
		void run() ;
		void stop() ;
		void next() ;
		void interrupt() ;
		void reset() ;
		void clear() ;
	
	private:
		CPicoBlaze * m_picoBlaze  ;
		CAssembler * m_assembler ;
		QTimer	   * m_timer ;
		bool		m_bInterrupt ;
		KListView  *m_messageList ;
		bool		m_run ;
};

#endif
