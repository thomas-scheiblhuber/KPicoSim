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
#include "ksimulator.h"

KSimulator::KSimulator(QObject *parent, const char *name )
 : QObject(parent, name)
{
	m_picoBlaze = new CPicoBlaze() ;
	m_assembler = new CAssembler() ;
	
	m_assembler->setCode( m_picoBlaze->code ) ;
	m_timer = new QTimer( this ) ;
	m_bInterrupt = FALSE ;
	
	m_timer->stop() ;
	m_run = false ;
	connect( m_timer, SIGNAL( timeout() ), this, SLOT( next() ) ) ;
}

KSimulator::~KSimulator()
{

	delete m_picoBlaze ;
	delete m_assembler ;
}

void KSimulator::setMessageList( KListView *messageList ) 
{
	m_assembler->setMessageList( messageList ) ;
}

void KSimulator::reset()
{
	m_picoBlaze->Reset() ; 
	emit stepped( m_picoBlaze->GetNextSourceLine() ) ;
}

void KSimulator::clear()
{
	m_picoBlaze->code->ClearCode() ;
	m_assembler->clear() ;
}

void KSimulator::interrupt()
{
	m_bInterrupt = TRUE ;
}

unsigned int KSimulator::getNextSourceLine()
{
	return m_picoBlaze->GetNextSourceLine() ;
}

void KSimulator::next()
{
	if ( m_bInterrupt ) {
		m_bInterrupt = FALSE ;
		m_picoBlaze->Interrupt() ;
	} else
		m_picoBlaze->Next() ;
	emit stepped( m_picoBlaze->GetNextSourceLine() ) ;
}

CPicoBlaze * KSimulator::getCpu()
{
	return m_picoBlaze ;
}

unsigned char KSimulator::getFlags()
{
	unsigned char flags = 0 ;
	
	if ( m_picoBlaze->flags.carry )
		flags |= CARRY_FLAG ;
	if ( m_picoBlaze->flags.zero )
		flags |= ZERO_FLAG ;
	if ( m_picoBlaze->flags.interrupt_enable )
		flags |= INTERRUPT_FLAG ;
		
	return flags ;
}

void KSimulator::setFlags( unsigned char flags ) 
{
	m_picoBlaze->flags.carry = flags & CARRY_FLAG ? TRUE : FALSE ;
	m_picoBlaze->flags.zero = flags & ZERO_FLAG ? TRUE : FALSE ;
	m_picoBlaze->flags.interrupt_enable = flags & INTERRUPT_FLAG ? TRUE : FALSE ;
}

void KSimulator::run()
{
	m_timer->start( 1 ) ;
	m_run = true ;
}

void KSimulator::stop()
{
	m_timer->stop() ;
	m_run = false ;
}

bool KSimulator::isRunning()
{
	return m_run ;
}

void KSimulator::getRegisterValues( unsigned char *values )
{
	int i ;
	
	for ( i = 0 ; i < 16 ; i++ ) {
		values[ i ] = m_picoBlaze->s[ i ] ;
	}

}

void KSimulator::setRegisterValues( unsigned char *values )
{
	int i ;
	
	for ( i = 0 ; i < 16 ; i++ ) {
		 m_picoBlaze->s[ i ] = values[ i ] ;
	}
}

void KSimulator::getScratchpad( unsigned char * values )
{
	int i ;
	
	for ( i = 0 ; i < SCRATCHPAD_SIZE ; i++ )
		values[ i ] = m_picoBlaze->scratch->Get( i ) ;
}

#include "ksimulator.moc"
