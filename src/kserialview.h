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
#ifndef KSERIALVIEW_H
#define KSERIALVIEW_H

#include <ktoolbar.h>
#include <ktextedit.h>
#include <klineedit.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qfont.h>
#include <qpopupmenu.h>

#include "kport.h"

class KSerialWindow : public KTextEdit 
{
	Q_OBJECT
	
	public:
		KSerialWindow( QWidget *parent ) : KTextEdit( parent ) {
			setWrapColumnOrWidth( 80 ) ; 				// Serial window is a terminal
			setWrapPolicy( QTextEdit::Anywhere ) ;
			setWordWrap( QTextEdit::FixedColumnWidth ) ; 
			setFont( KGlobalSettings::fixedFont()  ) ;  // Use default fixed font
		}
		virtual ~KSerialWindow() {} ;
		
	protected:
		virtual void keyPressEvent( QKeyEvent *e ) 
		{ 
			emit keyPressed( e->ascii() ) ; 
		}
		
		virtual void mousePressEvent( QMouseEvent *e ) {
		}
		virtual void mouseReleaseEvent( QMouseEvent *e ) {}
		virtual QPopupMenu *createPopupMenu( const QPoint &pos ) 
		{ 
			QPopupMenu *menu = new QPopupMenu( this ) ;
			menu->insertItem( "clear view", this, SLOT( clearView() ) ) ;
			return menu ; 
		}
	public slots:
		void clearView() {
			clear() ;
		}

	signals:
		void keyPressed( int key ) ;
	
} ;

class KSerialView : public QWidget
{
	Q_OBJECT
	public:
    	KSerialView( CPicoBlaze *cpu, QWidget *parent );
	    ~KSerialView();
	
		KPort * rxPort, * txPort, * statusPort ;
		
	public slots:
		void transmit( unsigned char ) ;
		void receive() ;
		void keyPressed( int key ) ;
	
	protected:
		KSerialWindow *view ;
		
		unsigned char rxFifo[ 16 ] ;
		unsigned char fifoPtr ;		
		
		unsigned char getReceiveFlags() ;
		unsigned char getTransmitFlags() ;
		void setStatusBits( unsigned char ) ;
		
		CPicoBlaze * m_cpu ;

		KLineEdit *txPortID, *rxPortID, *statusPortID ;
		QCheckBox *m_statusBits[ 8 ] ;

		QColor m_backgroundColor ;
		QPushButton *m_clearButton ;

	public slots:
		void updateTxId( const QString & ) ; 
		void updateRxId( const QString & ) ;
		void updateStatusId( const QString & ) ;
		void txFlagsChanged( bool en ) ;
};

#endif
