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
#include "kscratchpadview.h"

KScratchpadView::KScratchpadView( QWidget *parent, const char *name)
 : KToolBar(parent, name)
{
	view = new KTextEdit( this )  ;
	view->setFont( QFont( "Courier", view->font().pointSize() ) ) ;
	view->setReadOnly( true ) ;
	
	setWidget( view ) ;
	setResizeEnabled( true ) ;
}

KScratchpadView::~KScratchpadView()
{
	delete view ;
}

void KScratchpadView::setContent( unsigned char *values, unsigned int len )
{
	unsigned int i, val ;

	QString text, str ;
	
	text = "Scratchpad" ;
	for ( i = 0 ; i < len ; i++ ) {
		if ( (i % 8) == 0 ) {					 // Show address
			str.sprintf( "\n%02X:", i ) ;
			text += str ;
		}
		val = values[ i ] ;
		str.sprintf( " %02X", val ) ;			// values
		text += str ;
	}
	
	view->setText( text ) ;
}


#include "kscratchpadview.moc"
