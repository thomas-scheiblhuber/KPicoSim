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
#ifndef KJTAGDIALOG_H
#define KJTAGDIALOG_H

#include <qdialog.h>
#include <qpushbutton.h>
#include <klineedit.h>
#include <kprogress.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <ktextedit.h>

#include "jtagprogrammer.h"




class KJTAGDialog : public QDialog
{	
	Q_OBJECT
	public:
	    KJTAGDialog(QWidget *parent);
    	~KJTAGDialog();

		void setFilename( QString filename ) ;
		QString getFilename() ;

	public slots:
		void configure() ;
		void progress( int progress ) ;
		void selectFileName() ;
		void addMessage( const char *msg ) ;

	protected:
		QPushButton *m_configureBtn, *m_selectFileBtn ;
		KLineEdit *m_bitFileEdit ;
		KProgress *m_progress ;
		QLabel *m_bitFileLabel ;
		QGroupBox *m_groupConfigure ;
		KTextEdit *m_debug ;

		JTAGProgrammer *m_programmer ;
};

#endif
