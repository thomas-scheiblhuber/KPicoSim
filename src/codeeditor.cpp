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
#include "codeeditor.h"
#include <klibloader.h>

#include <qmessagebox.h>
#include <kmessagebox.h>
#include <qlayout.h>
#include <kfiledialog.h>

CodeEditor::CodeEditor(QWidget *parent, const char *name)
 : QWidget(parent, name)
{
	QVBoxLayout *layout = new QVBoxLayout(this) ;
		
	KLibFactory *factory = KLibLoader::self()->factory( "libkatepart" ) ;
	m_doc =  ( Kate::Document* ) factory->create( 0L, "kate", "KTextEditor::Document" ) ;
	m_view = ( Kate::View * ) m_doc->createView( this ) ;
	m_view->setIconBorder( true ) ;

    connect( m_view, SIGNAL(cursorPositionChanged()), this, SLOT(slotCursorPositionChanged()) );

	layout->addWidget( m_view ) ;
	
	m_exeLine = 0 ;
	m_filename = "" ;
	m_bFilename = false ;
	m_doc->setModified( false );
	
	m_doc->setDescription((KTextEditor::MarkInterface::MarkTypes)Breakpoint, "Breakpoint");
	m_doc->setPixmap((KTextEditor::MarkInterface::MarkTypes)Breakpoint, *inactiveBreakpointPixmap());
	m_doc->setPixmap((KTextEditor::MarkInterface::MarkTypes)ActiveBreakpoint, *activeBreakpointPixmap());
	m_doc->setPixmap((KTextEditor::MarkInterface::MarkTypes)ReachedBreakpoint, *reachedBreakpointPixmap());
	m_doc->setPixmap((KTextEditor::MarkInterface::MarkTypes)DisabledBreakpoint, *disabledBreakpointPixmap());
	m_doc->setPixmap((KTextEditor::MarkInterface::MarkTypes)ExecutionPoint, *executionPointPixmap());
	m_doc->setMarksUserChangable( Bookmark | Breakpoint );

	setHighlightMode() ;


	m_statusBar = new KStatusBar( this ) ;
	layout->addWidget( m_statusBar ) ;
	m_statusBar->insertItem( "", 0, 0, true ) ;		

	slotCursorPositionChanged() ;
}

CodeEditor::~CodeEditor()
{
	if ( m_view->document()->views().count() == 1) 
		delete m_view->document();

	delete m_statusBar ;
}

void CodeEditor::slotCursorPositionChanged()
{
	unsigned int line, column ;
	m_view->cursorPosition(  &line, &column ) ;
	m_statusBar->changeItem( QString( " Line: %1 Col: %2 " ).arg( QString::number( line+1 ) ).arg( QString::number( column+1 ) ), 0 ) ;
}

void CodeEditor::slotToggleBreakpoint()
{
	unsigned int line, col ;

	m_view->cursorPosition( &line, &col ) ;
	
	if ( !isBreakpoint( line ) )
		if ( m_doc->mark( m_exeLine ) & ExecutionPoint )
			m_doc->setMark( line, Breakpoint | ExecutionPoint) ; 
		else
			m_doc->setMark( line, Breakpoint ) ; 
	else
		m_doc->removeMark( line, Breakpoint ) ;
}

void CodeEditor::clearExecutionMarker()
{
	m_doc->removeMark( m_exeLine, ExecutionPoint ) ;
}

void CodeEditor::setCursor( unsigned int line )
{
	m_view->setCursorPosition( line, 0 ) ;
}

void CodeEditor::setExecutionMarker( unsigned int line )
{
	m_doc->removeMark( m_exeLine,ExecutionPoint ) ;
	m_exeLine = line ;
	if ( isBreakpoint( line ) ) {
		m_doc->setMark( m_exeLine, ExecutionPoint| Breakpoint ) ;
	} else
		m_doc->setMark( m_exeLine, ExecutionPoint ) ;

	setCursor( line ) ;	// KDevelop does this too!

}

bool CodeEditor::isBreakpoint( unsigned int line )
{
	return ( ( m_doc->mark( line ) &  Breakpoint ) != 0 ) ;
}

void CodeEditor::slotNewFile()
{
	m_doc->clear() ;
	m_doc->setModified( false ) ;
	m_filename = "" ;
	m_bFilename = false ;
}

void CodeEditor::slotSaveAs()
{
	if ( m_view->saveAs() == Kate::View::SAVE_OK ) {
		m_filename = m_doc->url().path() ;
		m_bFilename = true ;
	}
}

bool CodeEditor::save()
{
	slotSave() ;
	return m_bFilename ;
}

void CodeEditor::slotSave()
{
	if ( !m_bFilename )
		slotSaveAs() ;
	else if ( m_view->save() == Kate::View::SAVE_OK ) {
		m_filename = m_doc->url().path() ;
		m_bFilename = true ;
	}
}

void CodeEditor::slotPrint()
{
	m_doc->print() ;
}

void CodeEditor::slotFind()
{
	m_view->find() ;
}

void CodeEditor::slotFindNext()
{
	m_view->findAgain() ;
}

void CodeEditor::slotShowConfig()
{
	m_doc->configDialog() ;
}

void CodeEditor::slotCut()
{
	m_view->cut() ;
}

void CodeEditor::slotCopy()
{
	m_view->copy() ;
}

void CodeEditor::slotPaste()
{
	m_view->paste() ;
}

void CodeEditor::slotSelectAll()
{
	m_doc->selectAll() ;
}	

void CodeEditor::slotUndo()
{
	m_doc->undo() ;
}

void CodeEditor::slotRedo()
{
	m_doc->redo() ;
}

void CodeEditor::slotOpen()
{
	QString filename = KFileDialog::getOpenFileName( QString::null, QString( "*.psm|PicoBlaze assembly files" ) ) ;
	if ( filename != "" ) {
		m_filename = filename ;
		m_bFilename = true ;
		m_doc->openURL( filename ) ;
		setHighlightMode() ;
	}
}

void CodeEditor::open( QString filename )
{
	if ( filename != "" ) {
		m_filename = filename ;
		m_bFilename = true ;
		m_doc->openURL( filename ) ;
		setHighlightMode() ;
	}
}

QString CodeEditor::getFilename()
{
	return m_filename ;
}

	
void CodeEditor::setHighlightMode()
{		
	int i = 0;
	int hlModeCount = m_doc->hlModeCount();
	while ( i < hlModeCount ) {
		if ( m_doc->hlModeName( i ) == "pblazeASM" ) {
			m_doc->setHlMode( i ) ;
			break ;
		}
		i++;
	}
}

bool CodeEditor::askSave()
{
	QString filename ;
	if ( m_bFilename )
		filename = m_filename ;
	else
		filename = "Untitled" ;

	int choice = KMessageBox::warningYesNoCancel( this, QString( "The document \'%1\' has been modified.\nDo you want to save it?").arg( filename ), "Save Document?", KGuiItem( "Save" ), KGuiItem( "Discard" ) );

    if ( choice == KMessageBox::Yes ) 
		save() ;

	return ( choice != KMessageBox::Cancel ) ;
}

bool CodeEditor::close()
{
	if ( m_doc->isModified() ) 
		return askSave() ;
	return true ;
}

const QPixmap* CodeEditor::inactiveBreakpointPixmap()
{
	const char*breakpoint_gr_xpm[]={
		"11 16 6 1",
		"c c #c6c6c6",
		"d c #2c2c2c",
		"# c #000000",
		". c None",
		"a c #ffffff",
		"b c #555555",
		"...........",
		"...........",
		"...#####...",
		"..#aaaaa#..",
		".#abbbbbb#.",
		"#abbbbbbbb#",
		"#abcacacbd#",
		"#abbbbbbbb#",
		"#abcacacbd#",
		"#abbbbbbbb#",
		".#bbbbbbb#.",
		"..#bdbdb#..",
		"...#####...",
		"...........",
		"...........",
		"..........."};
		static QPixmap pixmap( breakpoint_gr_xpm );
		return &pixmap;
}


const QPixmap* CodeEditor::activeBreakpointPixmap()
{
	const char* breakpoint_xpm[]={
		"11 16 6 1",
		"c c #c6c6c6",
		". c None",
		"# c #000000",
		"d c #840000",
		"a c #ffffff",
		"b c #ff0000",
		"...........",
		"...........",
		"...#####...",
		"..#aaaaa#..",
		".#abbbbbb#.",
		"#abbbbbbbb#",
		"#abcacacbd#",
		"#abbbbbbbb#",
		"#abcacacbd#",
		"#abbbbbbbb#",
		".#bbbbbbb#.",
		"..#bdbdb#..",
		"...#####...",
		"...........",
		"...........",
		"..........."};
		static QPixmap pixmap( breakpoint_xpm );
		return &pixmap;
}



const QPixmap* CodeEditor::reachedBreakpointPixmap()
{
	const char*breakpoint_bl_xpm[]={
		"11 16 7 1",
		"a c #c0c0ff",
		"# c #000000",
		"c c #0000c0",
		"e c #0000ff",
		"b c #dcdcdc",
		"d c #ffffff",
		". c None",
		"...........",
		"...........",
		"...#####...",
		"..#ababa#..",
		".#bcccccc#.",
		"#acccccccc#",
		"#bcadadace#",
		"#acccccccc#",
		"#bcadadace#",
		"#acccccccc#",
		".#ccccccc#.",
		"..#cecec#..",
		"...#####...",
		"...........",
		"...........",
		"..........."};
		static QPixmap pixmap( breakpoint_bl_xpm );
		return &pixmap;
}


const QPixmap* CodeEditor::disabledBreakpointPixmap()
{
	const char*breakpoint_wh_xpm[]={
		"11 16 7 1",
		"a c #c0c0ff",
		"# c #000000",
		"c c #0000c0",
		"e c #0000ff",
		"b c #dcdcdc",
		"d c #ffffff",
		". c None",
		"...........",
		"...........",
		"...#####...",
		"..#ddddd#..",
		".#ddddddd#.",
		"#ddddddddd#",
		"#ddddddddd#",
		"#ddddddddd#",
		"#ddddddddd#",
		"#ddddddddd#",
		".#ddddddd#.",
		"..#ddddd#..",
		"...#####...",
		"...........",
		"...........",
		"..........."};
		static QPixmap pixmap( breakpoint_wh_xpm );
		return &pixmap;
}


const QPixmap* CodeEditor::executionPointPixmap()
{
	const char*exec_xpm[]={
		"11 16 4 1",
		"a c #00ff00",
		"b c #000000",
		". c None",
		"# c #00c000",
		"...........",
		"...........",
		"...........",
		"#a.........",
		"#aaa.......",
		"#aaaaa.....",
		"#aaaaaaa...",
		"#aaaaaaaaa.",
		"#aaaaaaa#b.",
		"#aaaaa#b...",
		"#aaa#b.....",
		"#a#b.......",
		"#b.........",
		"...........",
		"...........",
		"..........."};
		static QPixmap pixmap( exec_xpm );
		return &pixmap;
}




#include "codeeditor.moc"
