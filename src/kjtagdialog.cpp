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
#include "kjtagdialog.h"
#include <qmessagebox.h>
#include <kfiledialog.h>
#include <kiconloader.h>

KJTAGDialog::KJTAGDialog( QWidget *parent )
 : QDialog( parent )
{

	setFixedSize( 330, 300 ) ;
	m_groupConfigure = new QGroupBox( "Configure", this ) ;
	m_groupConfigure->setFixedSize( 310, 45 ) ;
	m_groupConfigure->move( 10, 10 ) ;

	m_selectFileBtn  = new QPushButton( m_groupConfigure ) ;
	m_selectFileBtn->setFixedSize( 30, 25 ) ;
	m_selectFileBtn->move( 205, 15 ) ;
	m_selectFileBtn->setPixmap( KGlobal::iconLoader()->loadIcon( "fileopen", KIcon::Small ) ) ;
	connect( m_selectFileBtn, SIGNAL( clicked() ), this, SLOT( selectFileName() ) ) ;

	m_configureBtn = new QPushButton( "Configure", m_groupConfigure ) ;
	m_configureBtn->setFixedSize( 60, 25 ) ;
	m_configureBtn->move( 240, 15 ) ;

	connect( m_configureBtn, SIGNAL( clicked() ), this, SLOT( configure() ) ) ;
	
	m_bitFileLabel = new QLabel( "Bit file", m_groupConfigure ) ;
	m_bitFileLabel->setFixedSize( 50, 25 ) ;
	m_bitFileLabel->move( 10, 15 ) ;
	
	m_bitFileEdit = new KLineEdit( m_groupConfigure ) ;
	m_bitFileEdit->setFixedSize( 150, 25 ) ;
	m_bitFileEdit->move( 50, 15 ) ;

	m_progress = new KProgress( this ) ;
	m_progress->setFixedSize( width(), 20 ) ;	
	m_progress->move( 0, 280 ) ;
	m_progress->setTotalSteps( 100 ) ;

	m_debug = new KTextEdit( this ) ;
	m_debug->setFixedSize( 310, 200 ) ;
	m_debug->move( 10, 60 ) ;
	
	m_programmer = new JTAGProgrammer( this ) ;
	connect( m_programmer, SIGNAL( progress(int) ), this, SLOT( progress(int) ) ) ;
	connect( m_programmer, SIGNAL( message(const char*) ), this, SLOT( addMessage(const char*) ) ) ;

	m_debug->insert( "This option is still very experimental!!\n"
                     "The code is tested with the Spartan-3 Development Board.\n"
					 "Assuming the following setup:\n"
                     " TDI--->[XC3SXXX]--->[XCF]--->TDO\n"
                     "========================================================\n" ) ;
}

KJTAGDialog::~KJTAGDialog()
{
	delete m_programmer ;
}

void KJTAGDialog::configure()
{
	m_debug->clear() ;
	m_programmer->setBitFile( m_bitFileEdit->text() ) ;
	m_programmer->program() ;
	m_progress->setProgress( 0 ) ;
}

void KJTAGDialog::progress( int percent )
{
	m_progress->setProgress( percent ) ;
}

void KJTAGDialog::setFilename( QString filename )
{
	m_bitFileEdit->setText( filename ) ;
}

QString KJTAGDialog::getFilename()
{
	return m_bitFileEdit->text() ;
}

void KJTAGDialog::selectFileName()
{
	QString filename = KFileDialog::getOpenFileName( QString::null, 
                                                    "*.bit|bit files\n*|All files", 
		   										     this,
  												     "Select configuration file" ) ;
	if ( filename != "" )	
		m_bitFileEdit->setText( filename ) ;
}

void KJTAGDialog::addMessage( const char *msg )
{
	m_debug->insert( msg ) ;
}


