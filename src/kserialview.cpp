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
#include "kserialview.h"
#include <qlabel.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qframe.h>
#include <klocale.h>
KSerialView::KSerialView( CPicoBlaze *cpu, QWidget * parent ) : QWidget( parent )
{
	m_cpu = cpu ;
	
	txPort = new KPort( 1 ) ;
	txPort->setMode( PortWriteable ) ;
	
	rxPort = new KPort( 1 ) ;
	rxPort->setMode( PortReadable ) ;
	
	statusPort = new KPort( 0 ) ;
	statusPort->setMode( PortReadable ) ;
	
	view = new KSerialWindow( this ) ;
	QWidget *settings = new QWidget( this ) ;
	settings->setMinimumSize( 90, 90 ) ;
	
	QVBoxLayout *layout = new QVBoxLayout( this ) ;
	layout->addWidget( view ) ;
	layout->addWidget( settings ) ;
	
	QGroupBox *groupBox = new QGroupBox( i18n( "Serial Settings" ), settings ) ;
	groupBox->setFixedSize( 200, 80 ) ;
	groupBox->move( 10, 0 ) ;
	
	QLabel *label = new QLabel( groupBox ) ;
	label->setText( "Transmit" ) ;
	label->move( 5, 15 ) ;
	label->setFixedSize( 55, 18 ) ;
	
	label = new QLabel( groupBox ) ;
	label->setText( "Receive" ) ;
	label->move( 5, 35 ) ;
	label->setFixedSize( 55, 18 ) ;
	
	label = new QLabel( groupBox ) ;
	label->setText( "Status" ) ;
	label->move( 5, 55 ) ;
	label->setFixedSize( 55, 18 ) ;
	
	txPortID = new KLineEdit( groupBox ) ;
	txPortID->move( 65, 15 ) ;
	txPortID->setFixedSize( 40, 18 ) ;
	txPortID->setText( "1" ) ;
	
	rxPortID = new KLineEdit(  groupBox ) ;
	rxPortID->move( 65, 35 ) ;
	rxPortID->setText( "1" ) ;
	rxPortID->setFixedSize( 40, 18 ) ;
	
	statusPortID = new KLineEdit( groupBox );
	statusPortID->move( 65, 55 ) ;
	statusPortID->setText( "0" ) ;
	statusPortID->setFixedSize( 40, 18 ) ;
	
	statusPort->setReadValue( 0 ) ;	// Buffers are empty, nothing received.
	connect( txPort, SIGNAL( write( unsigned char ) ), this, SLOT( transmit( unsigned char ) ) ) ;
	connect( rxPort, SIGNAL( read() ), this, SLOT( receive() ) ) ;
	connect( view, SIGNAL( keyPressed( int ) ), this, SLOT( keyPressed( int ) ) ) ;

	connect( txPortID, SIGNAL( textChanged( const QString & ) ), this, SLOT( updateTxId( const QString & ) ) ) ;
	connect( rxPortID, SIGNAL( textChanged( const QString & ) ), this, SLOT( updateRxId( const QString & ) ) ) ;
	connect( statusPortID, SIGNAL( textChanged( const QString & ) ), this, SLOT( updateStatusId( const QString & ) ) ) ;
	
	groupBox = new QGroupBox( "Status Register", settings ) ;
	groupBox->setFixedSize( 200, 80 ) ;
	groupBox->move( 250, 0 ) ;
	
	label = new QLabel( groupBox ) ;
	label->setText( "RX" ) ;
	label->move( 106, 15 ) ;
	label->setFixedSize( 20, 20 ) ;

	label = new QLabel( groupBox ) ;
	label->setText( "TX" ) ;
	label->move( 168, 15 ) ;
	label->setFixedSize( 20, 20 ) ;
	
	label = new QLabel( groupBox ) ;
	label->setText( "DR" ) ;
	label->move( 80, 30 ) ;
	label->setFixedSize( 20, 20 ) ;
	
	label = new QLabel( groupBox ) ;
	label->setText( "F" ) ;
	label->move( 110, 30 ) ;
	label->setFixedSize( 20, 20 ) ;
	
	label = new QLabel( groupBox ) ;
	label->setText( "HF" ) ;
	label->move( 130, 30 ) ;
	label->setFixedSize( 20, 20 ) ;
		
	label = new QLabel( groupBox ) ;
	label->setText( "F" ) ;
	label->move( 160, 30 ) ;
	label->setFixedSize( 20, 20 ) ;
	
	label = new QLabel( groupBox ) ;
	label->setText( "HF" ) ;
	label->move( 179, 30 ) ;
	label->setFixedSize( 20, 20 ) ;
	
	int i ;
	for ( i = 0 ; i < 8 ; i++ ) {
		m_statusBits[ i ] = new QCheckBox( groupBox ) ;
		m_statusBits[ i ]->move( 5 + i * 25, 50 ) ;
		m_statusBits[ i ]->setFixedSize( 15, 20 ) ;
	}
	
	for ( i = 0 ; i < 6 ; i++ ) {
		m_statusBits[ i ]->setEnabled( false ) ;
	}
	
	QFrame *frame = new QFrame( groupBox ) ;
	frame->setFrameRect( QRect( 0, 0, 1, 50 ) ) ;
	frame->setFrameShape( QFrame::VLine ) ;
	frame->move( 73, 20 ) ;
	frame->setFixedSize( 1, 50 ) ;
	
	frame = new QFrame( groupBox ) ;
	frame->setFrameRect( QRect( 0, 0, 1, 50 ) ) ;
	frame->setFrameShape( QFrame::VLine ) ;
	frame->move( 149, 20 ) ;
	frame->setFixedSize( 1, 50 ) ;
		
	connect( m_statusBits[ 6 ], SIGNAL( toggled( bool ) ), this, SLOT( txFlagsChanged( bool ) ) ) ;
	connect( m_statusBits[ 7 ], SIGNAL( toggled( bool ) ), this, SLOT( txFlagsChanged( bool ) ) ) ;
	
	fifoPtr = 0 ;
	
	m_cpu->addPort( txPort ) ;
	m_cpu->addPort( rxPort ) ;
	m_cpu->addPort( statusPort ) ;

	m_backgroundColor = rxPortID->backgroundColor() ;
}


KSerialView::~KSerialView()
{
	m_cpu->deletePort( txPort ) ;
	m_cpu->deletePort( rxPort ) ;
	m_cpu->deletePort( statusPort ) ;
	
	delete txPort ;
	delete rxPort ;
	delete statusPort ;
	delete view ;
}

void KSerialView::updateTxId( const QString & str )
{
	bool ok ;
	int val ;
	QString s ;
	
	val = str.toInt( &ok ) ;
	if ( ok && val >= 0 && val <= 255 ) {
		txPortID->setBackgroundColor( m_backgroundColor ) ;
		txPort->setID( val ) ;
	} else {
		txPortID->setBackgroundColor( QColor( 255, 128, 128 ) ) ; 
	}


	/*else {
		s.sprintf( "%u", txPort->getID() ) ;
		txPortID->setText( s ) ;
	}
	*/
}

void KSerialView::updateRxId( const QString & str )
{
	bool ok ;
	int val ;
	QString s ;
	
	val = str.toInt( &ok ) ;
	if ( ok && val >= 0 && val <= 255 ) {
		rxPortID->setBackgroundColor( m_backgroundColor ) ;
		rxPort->setID( val ) ;
	} else {
		rxPortID->setBackgroundColor( QColor( 255, 128, 128 ) ) ; 
	}
	
	/*else {
		s.sprintf( "%u", rxPort->getID() ) ;
		rxPortID->setText( s ) ;
	}
	*/
}

void KSerialView::updateStatusId( const QString & str )
{
	bool ok ;
	int val ;
	QString s ;
	
	val = str.toInt( &ok ) ;
	if ( ok && val >= 0 && val <= 255 ) {
		statusPort->setID( val ) ;
		statusPortID->setBackgroundColor( m_backgroundColor ) ;
	} else {
		statusPortID->setBackgroundColor( QColor( 255, 128, 128 ) ) ; 
	}
	/*else {
		s.sprintf( "%u", statusPort->getID() ) ;
		statusPortID->setText( s ) ;
	}
*/
}

void KSerialView::transmit( unsigned char b )
{
	if ( b == '\r' )
		b = '\n' ;
	
	if ( b == 0x08 ) { // Backspace
		view->doKeyboardAction( QTextEdit::ActionBackspace ) ;
	} else {
		QString str ;
		view->insert( (str+=b)  ) ;
	}
}

unsigned char KSerialView::getReceiveFlags()
{	
	unsigned char flags = 0 ;
	if ( fifoPtr != 0 ) flags |= 0x10 ;			// Receive
	else                flags &= ~0x10 ;
		
	if ( fifoPtr > 7 ) flags |= 0x04 ;			// Halffull Marker
	else               flags &= ~0x04 ;
	
	if ( fifoPtr == 15 ) flags |= 0x08 ;		// Full Marker
	else                 flags &= ~0x08 ;
	
	return flags ;			
}		

unsigned char KSerialView::getTransmitFlags()
{
	unsigned char flags = 0 ;
	if ( m_statusBits[ 6 ]->isChecked() )
		flags |= 0x02 ;
	if ( m_statusBits[ 7 ]->isChecked() )
		flags |= 0x01 ;
		
	return flags ;
}

void KSerialView::receive()
{	
	int i ;	
	
	if ( fifoPtr == 0 ) {					// Fifo empty
		statusPort->setReadValue( 0x00 ) ;
		return ;
	}

	rxPort->setReadValue( rxFifo[ 0 ] ) ;
	for ( i = 1 ; i < 16 ; i++ )
		rxFifo[ i - 1 ] = rxFifo[ i ] ;
	fifoPtr -= 1 ; 

	statusPort->setReadValue( getReceiveFlags() | getTransmitFlags() ) ;
	setStatusBits( getReceiveFlags() ) ;
}

void KSerialView::keyPressed( int key )
{
	if ( key == '\n' || key == 0 )
		return ;

	rxFifo[ fifoPtr ] = key ;
	if ( fifoPtr != 15 )
		fifoPtr += 1 ;
	
	statusPort->setReadValue( getReceiveFlags() | getTransmitFlags() ) ;
	setStatusBits( getReceiveFlags() ) ;
}

void KSerialView::txFlagsChanged( bool en )
{
	en = en ;
	statusPort->setReadValue( getReceiveFlags() | getTransmitFlags() ) ;	
}

void KSerialView::setStatusBits( unsigned char value )
{
	m_statusBits[ 3 ]->setChecked( (value & 0x10) != 0 ) ;
	m_statusBits[ 4 ]->setChecked( (value & 0x08) != 0 ) ;		
	m_statusBits[ 5 ]->setChecked( (value & 0x04) != 0 ) ;
}
