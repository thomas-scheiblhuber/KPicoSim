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
#include "kportview.h"
#include <qlayout.h>
#include <qlabel.h>

KPortView::KPortView( CPicoBlaze *cpu, QWidget *parent, const char *name)
 : KToolBar(parent, name)
{
	m_cpu = cpu ;
	
	QWidget *widget = new QWidget( this ) ;
	widget->setMinimumSize( 200, 65 ) ;
	
	QLabel *label = new QLabel( "ID", widget ) ;
	label->move( 3, 0 ) ;
	
	label = new QLabel( "b7", widget ) ;
	label->move( 32, 0 ) ;

	label = new QLabel( "b0", widget ) ;
	label->move( 137, 0 ) ;
	
	label = new QLabel( "I", widget ) ;
	label->move( 160, 0 ) ;
	
	label = new QLabel( "O", widget ) ;
	label->move( 175, 0 ) ;
	
	m_editID = new KLineEdit( widget ) ;
	m_editID->setText( "0" ) ;
	m_editID->setFixedSize( 30, 20 ) ;
	m_editID->move( 2, 22 );
	connect( m_editID, SIGNAL( textChanged( const QString &) ), this, SLOT( setID( const QString &) ) ) ;
	
	int i ;
	for ( i = 0 ; i < 8 ; i++ ) {
		m_bits[ i ] = new QCheckBox( widget ) ;
		m_bits[ i ]->move( 35 + i * 15, 22 ) ;
	}
	
	m_readable = new QCheckBox( widget ) ;
	m_readable->move( 160, 22 ) ;
	
	m_writeable = new QCheckBox( widget ) ;
	m_writeable->move( 175, 22 ) ;

	m_port = new KPort( 0 ) ;
	m_cpu->addPort( m_port ) ;
	
	connect( m_port, SIGNAL( read() ), this, SLOT( read() ) ) ;
	connect( m_port, SIGNAL( write(unsigned char) ), this, SLOT( write(unsigned char) ) ) ;
	connect( m_readable, SIGNAL( toggled(bool) ), this, SLOT( readableToggled(bool) ) ) ;
	connect( m_writeable, SIGNAL( toggled(bool) ), this, SLOT( writeableToggled(bool) ) ) ;
	
	setWidget( widget ) ;
	setCloseMode( Always ) ;

	m_writeable->setChecked( true ) ;
	writeableToggled( true ) ;
	m_readable->setChecked( true ) ;
	readableToggled( true ) ;

	setResizeEnabled( true ) ;

	m_backgroundColor = m_editID->backgroundColor() ;
}

KPortView::~KPortView()
{
	m_cpu->deletePort( m_port ) ;
}
/*
void KPortView::closeEvent ( QCloseEvent * e ) 
{
	emit closing( this ) ;
}
*/
void KPortView::setID( const QString &newID )
{
	QString str ;
	bool ok ;
	int id ;
	
	id = newID.toInt( &ok ) ;
	
	if ( ok && id >= 0 && id <= 255 ) {
		m_port->setID( id ) ;				// change it back to the old id
		m_editID->setText( newID ) ;
		m_editID->setBackgroundColor( m_backgroundColor ) ;
	} else {
		m_editID->setBackgroundColor( QColor( 255, 128, 128 ) ) ;
	}


	/*else {
		str.sprintf( "%d", m_port->getID() ) ;
		m_editID->setText( str ) ;	
	}*/
}

QString KPortView::id()
{
	return m_editID->text() ;
}

void KPortView::read()
{
	unsigned char value = 0 ;
	int i ;
	
	for ( i = 0 ; i < 8 ; i++ ) 
		if ( m_bits[ i ]->isChecked() )
			value |= ( 0x80 >> i ) ;
	
	m_port->setReadValue( value ) ;
}

void KPortView::write( unsigned char value )
{
	int i;
	for ( i = 0 ; i < 8 ; i++ ) 
		if  ( value & ( 0x80 >> i ) )
			m_bits[ i ]->setChecked( true ) ;
		else	
			m_bits[ i ]->setChecked( false ) ;
}

void KPortView::readableToggled( bool on )
{
	int mode = m_port->getMode() ;
	if ( on ) mode |= PortReadable ;
	else      mode &= ~PortReadable ;
	m_port->setMode( mode ) ;
}

void KPortView::writeableToggled( bool on )
{
	int mode = m_port->getMode() ;
	if ( on ) mode |= PortWriteable ;
	else      mode &= ~PortWriteable ;
	m_port->setMode( mode ) ;
}

void KPortView::readConfig( KSimpleConfig &config, QString group ) 
{
	config.setGroup( group ) ;

	int mode = config.readPropertyEntry( "Mode", QVariant::Int ).toInt() ;
	m_port->setMode( mode ) ;
	m_readable->setChecked( (mode & PortReadable) != 0 ) ;
	m_writeable->setChecked( (mode & PortWriteable ) != 0 ) ;

	QString id = config.readEntry( "Id" ) ;
	setID( id ) ;
}

void KPortView::writeConfig( KSimpleConfig &config, QString group ) 
{
	config.setGroup( group ) ;
	
	config.writeEntry( "Mode", m_port->getMode() ) ;
	config.writeEntry( "Id", m_port->getID() ) ;
}



#include "kportview.moc"
