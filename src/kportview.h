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
#ifndef KPORTVIEW_H
#define KPORTVIEW_H

#include <ktoolbar.h>
#include <qcheckbox.h>
#include <klineedit.h>
#include "kport.h"
#include <ksimpleconfig.h>

class KPortView : public KToolBar
{
	Q_OBJECT
	public:
    	KPortView( CPicoBlaze *cpu, QWidget *parent = 0, const char *name = 0);
	   	~KPortView();
		
		KPort	* m_port ;

	public slots:
		void read() ;
		void write( unsigned char value ) ;
		
		void writeableToggled( bool on ) ;
		void readableToggled( bool on ) ;
		
		void setID( const QString & newID ) ;
		QString id() ;

		void readConfig( KSimpleConfig &config, QString group ) ;
		void writeConfig( KSimpleConfig &config, QString group ) ;

	signals:
//		void closing( KPortView *ioport ) ;
		
	protected:
//		virtual void closeEvent ( QCloseEvent * e ) ;

		QCheckBox * m_bits[ 8 ], * m_readable, * m_writeable ;
		KLineEdit * m_editID ;
		
		CPicoBlaze * m_cpu ;

		QColor m_backgroundColor ;
		
};

#endif
