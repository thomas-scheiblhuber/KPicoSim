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
#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <qwidget.h>
#include <kate/view.h>
#include <kate/document.h>
#include <kstatusbar.h>

class CodeEditor : public QWidget
{
	Q_OBJECT
	public:
    	CodeEditor(QWidget *parent = 0, const char *name = 0);
    	~CodeEditor();

		void clearExecutionMarker() ;
		void setExecutionMarker( unsigned int line ) ;
		
		QString getFilename() ;
		bool isBreakpoint( unsigned int line ) ;
		void setCursor( unsigned int line ) ;

		void open( QString filename ) ;
		bool close() ;
		bool save() ;

	protected:
		Kate::View 		* m_view ;
		Kate::Document 	* m_doc ;

		KStatusBar *m_statusBar ;		

		unsigned int m_exeLine ;
		QString m_filename ;
		bool m_bFilename ;		
		void setHighlightMode() ;
		bool askSave() ;

		enum MarkType {
                Bookmark           = KTextEditor::MarkInterface::markType01,
                Breakpoint         = KTextEditor::MarkInterface::markType02,
                ActiveBreakpoint   = KTextEditor::MarkInterface::markType03,
                ReachedBreakpoint  = KTextEditor::MarkInterface::markType04,
                DisabledBreakpoint = KTextEditor::MarkInterface::markType05,
                ExecutionPoint     = KTextEditor::MarkInterface::markType06
        };
	

		static const QPixmap* inactiveBreakpointPixmap();
        static const QPixmap* activeBreakpointPixmap();
        static const QPixmap* reachedBreakpointPixmap();
        static const QPixmap* disabledBreakpointPixmap();
        static const QPixmap* executionPointPixmap();


	public slots:
		void slotToggleBreakpoint() ;					// set breakpoint at current position
		void slotSave() ;
		void slotSaveAs() ;
		void slotOpen() ;
		void slotShowConfig() ;
		void slotFind() ;
		void slotFindNext() ;
		void slotNewFile() ;
		void slotPrint() ;
		
		void slotCut() ;
		void slotCopy() ;
		void slotPaste() ;
		
		void slotUndo() ;
		void slotRedo() ;
		void slotSelectAll() ;

		void slotCursorPositionChanged() ;
		
};

#endif
