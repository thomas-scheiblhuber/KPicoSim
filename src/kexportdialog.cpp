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
#include "kexportdialog.h"
#include <kfiledialog.h>

KExportDialog::KExportDialog( QWidget *parent, const char *name ) : QDialog(parent, name)
{
	m_templateFile = "" ;
	m_outputDir = "" ;
	
	QLabel *label = new QLabel( this ) ;
	label->setText( "Template file" ) ;
	label->move( 10, 10 ) ;
	
	label = new QLabel( this ) ;
	label->setText( "Output directory" ) ;
	label->move( 10, 35 ) ;
	
	label = new QLabel( this ) ;
	label->setText( "Entity name" ) ;
	label->move( 10, 60 ) ;
		
	m_lineTemplateFile = new KLineEdit( this ) ;
	m_lineTemplateFile->setText( "" ) ;
	m_lineTemplateFile->setFixedSize( 150, 20 ) ;
	m_lineTemplateFile->move( 110, 10 ) ;
	
	m_lineOutputDir = new KLineEdit( this ) ;
	m_lineOutputDir->setText( "" ) ;
	m_lineOutputDir->setFixedSize( 150, 20 ) ;
	m_lineOutputDir->move( 110, 35 ) ;
	
	m_lineEntityName = new KLineEdit( this ) ;
	m_lineEntityName->setText( "" ) ;
	m_lineEntityName->setFixedSize( 150, 20 ) ;
	m_lineEntityName->move( 110, 60 ) ;
		
	QPushButton *button = new QPushButton( this ) ;
	button->setText( "OK" ) ;
	button->setFixedSize( 60, 25 ) ;
	button->move( 100, 90 ) ;
	connect( button, SIGNAL( clicked() ), this, SLOT( btnOKClicked() ) ) ;
	
	button = new QPushButton( this ) ;
	button->setText( "Cancel" ) ;
	button->setFixedSize( 60, 25 ) ;
	button->move( 200, 90 ) ;
	connect( button, SIGNAL( clicked() ), this, SLOT( btnCancelClicked() ) ) ;

	button = new QPushButton( this ) ;
	button->setText( "..." ) ;
	button->setFixedSize( 25, 20 ) ;
	button->move( 270, 10 ) ;
	connect( button, SIGNAL( clicked() ), this, SLOT( showFileDialog() ) )  ;
	
	button = new QPushButton( this ) ;
	button->setText( "..." ) ;
	button->setFixedSize( 25, 20 ) ;
	button->move( 270, 35 ) ;
	connect( button, SIGNAL( clicked() ), this, SLOT( showDirDialog() )  ) ;
		
	setFixedSize( 340, 130 ) ;
	setCaption( "Export to VHDL" ) ;
	m_bCanceled = true ;
}

KExportDialog::~KExportDialog()
{
}

void KExportDialog::modal()
{
	exec() ;
}

void KExportDialog::showFileDialog()
{
	KFileDialog dlg( QString::null, "*.vhd|vhdl template file", this, "template dlg", true ) ;
	dlg.exec() ;
	
	if ( dlg.selectedFile() != "" )
		m_lineTemplateFile->setText( dlg.selectedFile() ) ;
}

void KExportDialog::showDirDialog()
{
	QString dir = KFileDialog::getExistingDirectory ( QString::null, this, "Export directory" ) ;
	if ( dir != "" )
		m_lineOutputDir->setText( dir ) ;
}

void KExportDialog::btnOKClicked()
{
	m_templateFile = m_lineTemplateFile->text() ;
	m_outputDir = m_lineOutputDir->text() ;
	m_entityName = m_lineEntityName->text() ;
	m_bCanceled = false ;
	close() ;
}

void KExportDialog::btnCancelClicked()
{
	m_outputDir = "" ;
	m_templateFile = "" ;
	m_entityName = "" ;
	
	close() ;
}

void KExportDialog::setTemplateFile( QString file )
{
	m_lineTemplateFile->setText( file ) ;
}

void KExportDialog::setOutputDir( QString dir )
{
	m_lineOutputDir->setText( dir ) ;
}

void KExportDialog::setEntityName( QString name ) 
{
	m_lineEntityName->setText( name ) ;
}

QString KExportDialog::getTemplateFile()
{
	return m_templateFile ;
}

QString KExportDialog::getOutputDir()
{
	return m_outputDir ;
}

QString KExportDialog::getEntityName()
{
	return m_entityName ;
}


#include "kexportdialog.moc"
