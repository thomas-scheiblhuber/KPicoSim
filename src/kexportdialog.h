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
#ifndef KEXPORTDIALOG_H
#define KEXPORTDIALOG_H

#include <qobject.h>
#include <klineedit.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <qdialog.h>

class KExportDialog : public QDialog
{
	Q_OBJECT
	public:
    	KExportDialog(QWidget *parent = 0, const char *name = 0 );
    	~KExportDialog();
	
		void setOutputDir( QString dir ) ;
		void setTemplateFile( QString file ) ;
		void setEntityName( QString name ) ;
		
		QString getTemplateFile() ;
		QString getOutputDir() ;
		QString getEntityName() ;
		
		void modal() ;
		
		bool isCanceled() { return m_bCanceled ; } 
	protected:
		QString m_outputDir ;
		QString m_templateFile ;
		QString m_entityName ;
		
		KLineEdit * m_lineTemplateFile, * m_lineOutputDir, *m_lineEntityName ;
		bool m_bCanceled ;
		
	public slots:
		void btnOKClicked() ;
		void btnCancelClicked() ;
		void showFileDialog() ;
		void showDirDialog() ;
};

#endif
