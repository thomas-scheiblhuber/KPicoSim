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


/* Revision History
 * V0.1: - Initial Version
 * V0.2: 
 *     ADDED
 *		  - Improved debugging. Added Execution and Breakpoint icons in border.
 *        - Show more compiler messages.
 *        - Improved Syntax Highlighting (Now automatically installed).
 *		  - Improved the assembler. (Code is still a mess!)
 *        - Added icons.
 *        - Installation of the .desktop file is now in 'Development'
 * V0.3:
 *     ADDED
 *        - Export to HEX and MEM files
 * V0.4:
 *     ADDED
 *        - Expanded the debug toolbar.
 * V0.5:
 *     BUG FIX
 *        - Assembler: 'ORR sX, sY' assembled to 'ORR sX, kk' where kk was undefined
 *        - Simulator:  SUBCY sX, sY simulated wrongly (undefined behaviour)
 *                      SUBCY sX, kk simulated wrongly (undefined behaviour)
 *     IMPROVED
 *        - Change port ID per port and in serial window.
 *     ADDED
 *        - Save/Restore settings.
 *		  - Clear serial view popupmenu
 *
 * V0.6:
 *     ADDED
 *         - Debian packaging support by "Adrian Knoth"
 *         - Initial JTAG support
 *         - Initial Help
 *  
 *     IDEAS (Oct 9, 2005)
 *			- Multiple picoblaze support
 *			- IO ports (and irq) can be changed from other software. 
 *          - Download mem file with JTAG
 * V0.7:
 *     ADDED
 *          - internationalizion support (=i18n)
 *          - #include <iee1284.h> removed, gave compile error on some systems.
 *
 *     BUG FIX
 *			- Assembler: 'TEST' carry bit was not set properly because MSB bit was not evaluated.
 */


#include "kpicosim.h"

#include <qlabel.h>

#include <kmainwindow.h>
#include <klocale.h>

#include <kmenubar.h>
#include <qlayout.h>
#include <qkeysequence.h>
#include <knuminput.h>
#include <qnamespace.h>
#include <kportview.h>
#include <qdockwindow.h>
#include <kstatusbar.h>
#include <kiconloader.h>
#include "kexportdialog.h"
#include "kjtagdialog.h"
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <qlayout.h>
#include <ksimpleconfig.h>
#include <khelpmenu.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>

//const char version[] = "0.6" ;
extern const char *version ;

enum IDs {
	START_SIM_ID = 0,
	COMPILE_ID,
	RUN_ID,
	NEXT_ID,
	INTERRUPT_ID,
	RESET_ID,
	
	VIEW_SERIAL_ID,
	VIEW_SCRATCHPAD_ID
} ;


KPicoSim::KPicoSim() : KMainWindow( 0, "KPicoSim" )
{
//	set the shell's ui resource file
//	setXMLFile("kpicosimui.rc");

	m_splitter = new QSplitter( this ) ;
	m_tabWidget = new QTabWidget( m_splitter ) ;
	m_editor = new CodeEditor( m_tabWidget ) ;
	m_messages = new KListView( m_splitter, "messages" ) ;
	m_simulator = new KSimulator( this ) ;
	m_processorView = new KProcessorView( this ) ;
	
	m_tabWidget->addTab( m_editor, "Source" ) ;
	addDockWindow( m_processorView, DockLeft ) ;
	
	m_splitter->setOrientation( QSplitter::Vertical ) ;
	setCentralWidget( m_splitter ) ;
	m_messages->setAllColumnsShowFocus( true ) ;
	m_messages->setFullWidth( true ) ;
	m_messages->resize( m_messages->width(), m_splitter->height() / 5 ) ;	

	buildMenu() ;	
	
	KToolBar *toolbar = new KToolBar( this ) ;
	addDockWindow( toolbar ) ;
	
	toolbar->insertButton( "filenew", -1, SIGNAL( clicked() ), m_editor, SLOT( slotNewFile() ), true, i18n("New") ) ;
	toolbar->insertButton( "fileopen", -1, SIGNAL( clicked() ), m_editor, SLOT( slotOpen() ), true, i18n("Open") ) ;
	toolbar->insertButton( "filesave", -1, SIGNAL( clicked() ), m_editor, SLOT( slotSave() ), true, i18n("Save") ) ;
	toolbar->insertButton( "filesaveas", -1, SIGNAL( clicked() ), m_editor, SLOT( slotSaveAs() ), true, i18n("Save As") ) ;
	
	m_debugBar = new KToolBar( this ) ;
	addDockWindow( m_debugBar ) ;
	m_debugBar->insertButton( UserIcon( "rebuild" ), COMPILE_ID, SIGNAL( clicked() ), this, SLOT( compile() ), true, i18n ("Compile") ) ;
	m_debugBar->insertButton( "run", START_SIM_ID, SIGNAL( clicked() ), this, SLOT( startSim() ), true, i18n("Start/Stop Debug") ) ;
	m_debugBar->insertSeparator() ;
	m_debugBar->insertButton( UserIcon( "continue" ), RUN_ID, SIGNAL( clicked() ), this, SLOT( startStop() ), false, i18n ("Continue") ) ;
	
	m_debugBar->insertButton( UserIcon( "next" ), NEXT_ID, SIGNAL( clicked() ), m_simulator, SLOT( next() ), false, i18n ("Next") ) ;
	m_debugBar->insertButton( UserIcon( "interrupt" ), INTERRUPT_ID, SIGNAL( clicked() ), m_simulator, SLOT( interrupt() ), false, "Interrupt" ) ; 
	m_debugBar->insertButton( UserIcon( "reset" ), RESET_ID, SIGNAL( clicked() ), m_simulator, SLOT( reset() ), false, i18n ("Reset") ) ;
	
	connect( this, SIGNAL( run() ), m_simulator, SLOT( run() ) );
	connect( this, SIGNAL( stop()  ), m_simulator, SLOT( stop()  ) ) ;
	connect( m_simulator, SIGNAL( stepped( unsigned int ) ), this, SLOT( stepped( unsigned int ) ) ) ;
	connect( m_processorView, SIGNAL( processorRegsChanged() ), this, SLOT( updateProcessorRegs() ) ) ;
	connect( m_processorView, SIGNAL( processorFlagsChanged() ), this, SLOT( updateProcessorFlags() ) ) ;
	connect( m_messages, SIGNAL( clicked( QListViewItem * ) ), this, SLOT( messageListClicked( QListViewItem * ) ) ) ;
	
	m_messages->addColumn( i18n("Line") ) ;
	m_messages->addColumn( i18n("Description") ) ;
	m_messages->setSorting( -1, FALSE ) ;
	m_simulator->setMessageList( m_messages ) ;

	m_simulationMode = false ;	
	m_scratchpadView = NULL ;
	m_serialView = NULL ;

	statusBar()->insertItem( QString( i18n("Mode: Edit") ), 0 ) ;
	statusBar()->insertItem( QString( i18n("Status: Stopped") ), 1 ) ;
	statusBar()->insertItem( QString( i18n("Instructions: 0") ), 2 ) ;

	m_templateFile = "" ;
	m_outputDir = "" ;
	m_entityName = "mpu_rom" ;


	openGUI() ;
}


void KPicoSim::buildMenu()
{
	KIconLoader * ldr = KGlobal::iconLoader() ;

	KPopupMenu * exportMenu = new KPopupMenu( this ) ;

	exportMenu->insertItem( "VHDL", this, SLOT( fileExportVHDL() ) ) ;
//	exportMenu->insertItem( "COE", this, SLOT( fileExportCOE() ) ) ;
	exportMenu->insertItem( "MEM", this, SLOT( fileExportMEM() ) ) ;
	exportMenu->insertItem( "HEX", this, SLOT( fileExportHEX() ) ) ;

	m_fileMenu = new KPopupMenu( this ) ;
	m_fileMenu->insertItem( ldr->loadIcon( "filenew", KIcon::Small ), i18n("New"), this, SLOT( slotFileNew() ) ) ;
	m_fileMenu->insertItem( ldr->loadIcon( "fileopen", KIcon::Small ), i18n("Open"), m_editor, SLOT( slotOpen() ) ) ;
	m_fileMenu->insertSeparator() ;
	m_fileMenu->insertItem( ldr->loadIcon( "filesave", KIcon::Small ), i18n("Save"), m_editor, SLOT( slotSave() ), QKeySequence::QKeySequence( "CTRL+S" ) ) ;
	m_fileMenu->insertItem( ldr->loadIcon( "filesaveas", KIcon::Small ), i18n("Save As..."), m_editor, SLOT( slotSaveAs() ) ) ;
	m_fileMenu->insertSeparator() ;
	m_fileMenu->insertItem( ldr->loadIcon( "fileprint", KIcon::Small ), i18n("Print..."), m_editor, SLOT( slotPrint() ), QKeySequence::QKeySequence( "CTRL+P" ) ) ;
	m_fileMenu->insertSeparator() ;
	m_fileMenu->insertItem( i18n("Export"), exportMenu ) ;
	m_fileMenu->insertSeparator() ;
	m_fileMenu->insertItem( ldr->loadIcon( "fileclose", KIcon::Small ), i18n("Quit"), this, SLOT( slotClose() ) ) ;
	
	m_editMenu = new KPopupMenu( this ) ;
	m_editMenu->insertItem( ldr->loadIcon( "undo", KIcon::Small ), i18n("Undo"), m_editor, SLOT( slotUndo() ),QKeySequence::QKeySequence( "CTRL+Z" ) ) ;
	m_editMenu->insertItem( ldr->loadIcon( "redo", KIcon::Small ), i18n("Redo"), m_editor, SLOT( slotRedo() ),QKeySequence::QKeySequence( "CTRL+SHIFT+Z" ) ) ;
	m_editMenu->insertSeparator() ;
	m_editMenu->insertItem( i18n("Select All"), m_editor, SLOT( slotSelectAll() ),QKeySequence::QKeySequence( "CTRL+A" ) ) ;
	m_editMenu->insertSeparator() ;
	m_editMenu->insertItem( ldr->loadIcon( "editcut", KIcon::Small ), i18n("Cut"), m_editor, SLOT( slotCut() ),QKeySequence::QKeySequence( "CTRL+X" ) ) ;
	m_editMenu->insertItem( ldr->loadIcon( "editcopy", KIcon::Small ), i18n("Copy"), m_editor, SLOT( slotCopy() ),QKeySequence::QKeySequence( "CTRL+C" ) ) ;
	m_editMenu->insertItem( ldr->loadIcon( "editpaste", KIcon::Small ), i18n( "Paste" ), m_editor, SLOT( slotPaste() ),QKeySequence::QKeySequence( "CTRL+V" ) ) ;
	m_editMenu->insertSeparator() ;
	m_editMenu->insertItem( ldr->loadIcon( "find", KIcon::Small ), i18n("Find..."), m_editor, SLOT( slotFind() ), QKeySequence::QKeySequence( "CTRL+F" ) ) ;
	m_editMenu->insertItem( "Find Next", m_editor, SLOT( slotFindNext() ), QKeySequence::QKeySequence( "F3" ) ) ;


	m_debugMenu = new KPopupMenu( this ) ;
	m_debugMenu->insertSeparator() ;
	m_debugMenu->insertItem( ldr->loadIcon( "rebuild", KIcon::Small ), i18n("Compile"), this, SLOT( compile() ), QKeySequence::QKeySequence( "SHIFT+F9" ) ) ;
	m_debugMenu->insertItem( ldr->loadIcon( "run", KIcon::Small ), i18n("Start Debug"), this, SLOT( startSim() ), QKeySequence::QKeySequence( "F9" ) , START_SIM_ID ) ;

	m_debugMenu->insertSeparator() ;
	m_debugMenu->insertItem( i18n("Continue"), this, SLOT( startStop() ), QKeySequence::QKeySequence( "F10" ) , RUN_ID ) ;
	m_debugMenu->insertItem( i18n("Next"), m_simulator, SLOT( next() ), QKeySequence::QKeySequence( "F5" ) , NEXT_ID ) ;
	m_debugMenu->insertItem( i18n("Interrupt"), m_simulator, SLOT( interrupt() ), QKeySequence::QKeySequence( "F4" ) , INTERRUPT_ID ) ;
	m_debugMenu->insertItem( i18n("Reset"), m_simulator, SLOT( reset() ), QKeySequence::QKeySequence( "F11" ) , RESET_ID ) ;
	
	m_debugMenu->insertSeparator() ;
	m_debugMenu->insertItem( i18n("Toggle Breakpoint"), m_editor, SLOT( slotToggleBreakpoint() ), QKeySequence::QKeySequence( "F8" ) ) ;
	
	m_settingsMenu = new KPopupMenu( this ) ;
	m_settingsMenu->insertItem( i18n("Configure Editor..."), m_editor, SLOT( slotShowConfig() ) ) ;
	
	m_peripheralMenu = new KPopupMenu( this ) ;
	m_peripheralMenu->insertItem( i18n("I/O Port"), this, SLOT( newIOPort() ) ) ;
	m_peripheralMenu->insertItem( i18n("Scratchpad"), this, SLOT( showScratchpad() ), 0, VIEW_SCRATCHPAD_ID ) ;
	m_peripheralMenu->insertItem( i18n("Serial port"), this, SLOT( showSerialPort() ), 0, VIEW_SERIAL_ID ) ;
	
	m_jtagMenu = new KPopupMenu( this ) ;
	m_jtagMenu->insertItem( i18n("Download"), this, SLOT( jtagDownload() ) ) ;

	menuBar()->insertItem( i18n("File"), m_fileMenu ) ;
	menuBar()->insertItem( i18n("Edit"), m_editMenu ) ;
	menuBar()->insertItem( i18n("Debug"), m_debugMenu ) ;
	menuBar()->insertItem( i18n("Peripheral"), m_peripheralMenu ) ;
	menuBar()->insertItem( i18n("JTAG"), m_jtagMenu ) ;
	menuBar()->insertItem( i18n("Settings"), m_settingsMenu ) ;
	menuBar()->insertItem( i18n("Help"), helpMenu() ) ;
	
	m_debugMenu->setItemEnabled( RUN_ID, false ) ;
	m_debugMenu->setItemEnabled( NEXT_ID, false ) ;
	m_debugMenu->setItemEnabled( INTERRUPT_ID, false ) ;
	m_debugMenu->setItemEnabled( RESET_ID, false ) ;
	
	m_peripheralMenu->setCheckable( true );
	m_peripheralMenu->setItemChecked( VIEW_SERIAL_ID, false ) ;
	m_peripheralMenu->setItemChecked( VIEW_SCRATCHPAD_ID, false ) ;

	
	menuBar()->show() ;
}

void KPicoSim::jtagDownload()
{
	/* JTAG is still in its infancy. This code works for me. I'm using the Xilinx Spartan-3
     * development board. If it works for you, great, if not too bad...
	 */

	KJTAGDialog dlg( this ) ;
	dlg.setFilename( m_bitfile ) ;
	dlg.exec() ;
	m_bitfile = dlg.getFilename() ;
}

void KPicoSim::fileExportCOE()
{
	KMessageBox::information( this, i18n("This function is not supported yet"), i18n("Export COE") ) ;

//	if ( compile() ) {
		//m_simulator->exportCOE() ;
//	}
}

void KPicoSim::fileExportHEX()
{

	QString filename = KFileDialog::getSaveFileName( QString::null, 
                                                     "*.hex|HEX files\n*|All files", 
		   										     this,
  												     i18n( "Export HEX" ) ) ;

	if ( filename != "" && compile() ) {
		m_simulator->exportHEX( filename, FALSE ) ;
	}
}

void KPicoSim::fileExportMEM()
{

	QString filename = KFileDialog::getSaveFileName( QString::null, 
                                                     "*.mem|MEM files\n*|All files", 
		   										     this,
  												     i18n( "Export MEM" ) ) ;
	if ( filename != "" && compile() ) {
		m_simulator->exportHEX( filename, TRUE ) ;
	}
}

void KPicoSim::fileExportVHDL()
{
	KExportDialog dlg( this  ) ;
	
	dlg.setTemplateFile( m_templateFile ) ;
	dlg.setOutputDir( m_outputDir ) ;
	dlg.setEntityName( m_entityName ) ;
	dlg.modal() ;
	
	if ( dlg.isCanceled() ) 
		return ;
	
	m_templateFile = dlg.getTemplateFile() ;
	m_outputDir = dlg.getOutputDir() ;
	m_entityName = dlg.getEntityName() ;
		
	if ( compile() && m_simulator->exportVHDL( m_templateFile, m_outputDir, m_entityName ) ) {
		appendMessage( QString( i18n("File '%1/%2.vhd' exported") ).arg( m_outputDir ).arg(m_entityName) ) ;
		appendMessage( QString( i18n("Template file '%1' used").arg( m_templateFile ) ) ) ;
		appendMessage( i18n("***Export Success***") ) ;
	} else {
		appendMessage( i18n("***Export failed***") ) ;
	}
}

void KPicoSim::slotFileNew()
{
	if ( m_editor->close() )
		m_editor->slotNewFile() ;
}

void KPicoSim::slotClose()
{
	close() ;
}

void KPicoSim::closeEvent( QCloseEvent * e )
{
	if ( m_editor->close() )
		e->accept() ;


	// Save filename last opened
	// Save windows IO Ports, peripherals et al.
	closeGUI() ;
}

void KPicoSim::newIOPort()
{	
	KPortView * ioport = new KPortView( m_simulator->getCpu(), 0 ) ;			/* port id is 0 */
//	m_ioList.append( ioport ) ;
	addDockWindow( ioport, DockRight ) ;
//	connect( ioport, SIGNAL( closing( KPortView* ) ), this, SLOT( removeIOPort( KPortView* ) ) ) ;
}

void KPicoSim::showSerialPort()
{
	if ( m_serialView == NULL ) {
		m_serialView = new KSerialView( m_simulator->getCpu(), m_tabWidget ) ;
		m_tabWidget->addTab( m_serialView, i18n( "Serial" ) ) ;
		m_peripheralMenu->setItemChecked( VIEW_SERIAL_ID, true ) ;
	} else {
		m_peripheralMenu->setItemChecked( VIEW_SERIAL_ID, false ) ;
		delete m_serialView ;
		m_serialView = NULL ;
	}
}

void KPicoSim::showScratchpad()
{
	if ( m_scratchpadView == NULL ) {
		m_scratchpadView = new KScratchpadView( this ) ;
		updateScratchpadView() ;
		addDockWindow( m_scratchpadView, DockRight ) ;
		m_peripheralMenu->setItemChecked( VIEW_SCRATCHPAD_ID, true ) ;
	} else {
		m_peripheralMenu->setItemChecked( VIEW_SCRATCHPAD_ID, false ) ;
		delete m_scratchpadView ; 
		m_scratchpadView = NULL ;
	}
}

KPicoSim::~KPicoSim()
{
	// Delete dockwindows 
	// These are the IO ports, scratchpad and the processorview
	dockWindows().setAutoDelete( true ) ;
	dockWindows().clear() ;
	
	if ( m_serialView )	
		delete m_serialView ;
			
	delete m_simulator ;
	
	delete m_debugMenu ;
	delete m_editMenu ;
 	delete m_peripheralMenu ;
	delete m_fileMenu ;
	delete m_settingsMenu ;
}

void KPicoSim::startStop()
{
	if ( m_simulationMode ) {
		
		if ( !m_simulator->isRunning() ) {
			m_debugMenu->changeItem( RUN_ID, i18n("Stop") ) ;
			m_editor->clearExecutionMarker() ;
			m_simulator->run() ;
			statusBar()->changeItem( QString( i18n("Status: Running" ) ), 1 ) ;
			m_debugBar->setButton( RUN_ID, true ) ;
		} else {
			m_simulator->stop() ;
			updateViews() ;
			m_debugMenu->changeItem( RUN_ID, i18n("Continue") ) ;
			m_editor->setExecutionMarker( m_simulator->getNextSourceLine() ) ;
			statusBar()->changeItem( QString( i18n("Status: Stopped") ), 1 ) ;
			QString str ;
			str = QString( i18n("Instructions: %1") ).arg( m_nrInstructions) ;
			statusBar()->changeItem( str, 2 ) ;
			m_debugBar->setButton( RUN_ID, false ) ;
		}
	}
}

void KPicoSim::messageListClicked( QListViewItem *item ) 
{
	if ( item ) {	
		bool ok ;
		int line = item->text(0).toInt( &ok, 10 ) ;
	
		if ( ok ) 
			m_editor->setCursor( line - 1 ) ;				// C-programmers do it from zero
	}
}

void KPicoSim::updateProcessorRegs()
{
	unsigned char regValues[ 16 ] ;
	m_processorView->getRegisterValues( regValues ) ;
	m_simulator->setRegisterValues( regValues ) ;
}

void KPicoSim::updateProcessorFlags()
{
	m_simulator->setFlags( m_processorView->getFlags() ) ;
}

void KPicoSim::updateViews() 
{
	unsigned char regValues[ 16 ] ;
	m_simulator->getRegisterValues( regValues ) ;
	m_processorView->setRegisterValues( regValues ) ;
	m_processorView->setFlags( m_simulator->getFlags() ) ;
	
	updateScratchpadView() ;

	QString str ;
	str = QString( i18n("Instructions: %1") ).arg(m_nrInstructions) ;
	statusBar()->changeItem( str, 2 ) ;
}

void KPicoSim::updateScratchpadView()
{
	if ( m_scratchpadView != NULL ) {
		unsigned char sp_ram[ 64 ] ;
		m_simulator->getScratchpad( sp_ram ) ;
		m_scratchpadView->setContent( sp_ram, sizeof( sp_ram ) ) ;
	}
}

void KPicoSim::stepped( unsigned int line )
{
	m_nrInstructions++ ;
	if ( m_simulator->isRunning() ) {
		if ( m_editor->isBreakpoint( line ) ) { ;
			startStop() ;
			m_editor->setExecutionMarker( line ) ;
		} else if ( (m_nrInstructions % 100 ) == 0 ) {
			updateViews() ;
		}
	} else {
		m_editor->setExecutionMarker( line ) ;
		updateViews() ;
	}
}

void KPicoSim::appendMessage( QString str )
{
	QListViewItem *item = new QListViewItem( m_messages, m_messages->lastChild() ) ;
	item->setText( 0, "" ) ;
	item->setText( 1, str ) ;

}

bool KPicoSim::compile()
{
	m_simulator->clear() ;
	m_messages->clear() ;
	
	if ( !m_editor->save() ) 
		return FALSE;

	appendMessage( QString( i18n("File '%1' saved") ).arg( m_editor->getFilename() ) ) ;
	m_simulator->setFilename( m_editor->getFilename() ) ;
		
	if ( m_simulator->compile() == TRUE ) {
		appendMessage( i18n("***Compile Success*** ") ) ;
		return TRUE ;
	} else {
		appendMessage( i18n("***Compile Failed*** ") ) ;
		return FALSE ;
	}
}
void KPicoSim::startSim()
{
	KIconLoader * ldr = KGlobal::iconLoader() ;

	if ( !m_simulationMode ) {
		if ( compile() ) {
			setCaption( m_editor->getFilename() + i18n(" [Debugging]") ) ;
			m_debugMenu->changeItem( START_SIM_ID, ldr->loadIcon( "stop", KIcon::Small ), i18n("Stop Debug") ) ;
			m_debugBar->setButton( START_SIM_ID, true ) ;

			m_nrInstructions = -1 ;
			m_simulator->reset() ;
			
			m_simulationMode = TRUE ;
		}
	} else {
		if ( m_simulator->isRunning() )
			startStop() ;
			
		setCaption( m_editor->getFilename() ) ;
		m_debugMenu->changeItem( START_SIM_ID, ldr->loadIcon( "run", KIcon::Small ), i18n("Start Debug") ) ;
		m_debugBar->setButton( START_SIM_ID, false ) ;
		m_editor->clearExecutionMarker() ;
		m_simulationMode = FALSE ;
	}
	
	if ( m_simulationMode ) {
		statusBar()->changeItem( i18n("Mode: Debug"), 0 ) ;
	} else {
		statusBar()->changeItem( i18n("Mode: Edit"), 0 ) ;
	}
	
	m_debugMenu->setItemEnabled( RUN_ID, m_simulationMode ) ;
	m_debugMenu->setItemEnabled( NEXT_ID, m_simulationMode ) ;
	m_debugMenu->setItemEnabled( INTERRUPT_ID, m_simulationMode ) ;
	m_debugMenu->setItemEnabled( RESET_ID, m_simulationMode ) ;

	m_debugBar->setItemEnabled( RUN_ID, m_simulationMode ) ;
	m_debugBar->setItemEnabled( NEXT_ID, m_simulationMode ) ;
	m_debugBar->setItemEnabled( INTERRUPT_ID, m_simulationMode ) ;
	m_debugBar->setItemEnabled( RESET_ID, m_simulationMode ) ;

}
/*
void KPicoSim::removeIOPort( KPortView* ioport )
{
	m_ioList.removeRef( ioport ) ;
}
*/
void KPicoSim::closeGUI()
{
	KSimpleConfig config( "kpicosim" ) ;
	config.setGroup( "Peripherals" ) ;
	
	config.writeEntry( "serial", m_serialView != NULL ) ;
	config.writeEntry( "scratchpad", m_scratchpadView != NULL ) ; 
	config.writeEntry( "filename", m_editor->getFilename() ) ;
	config.writeEntry( "bitfile", m_bitfile ) ;
	
	saveWindowSize( &config ) ;
/*
	config.writeEntry( "numIOPorts", m_ioList.count() ) ;
	for ( int i = 0 ; i < m_ioList.count() ; i++ ) {
		QString group ;	
		group.sprintf( "IO Port %d", i ) ;
		m_ioList.at(i)->writeConfig( config, group ) ;
	}
*/
}

void KPicoSim::openGUI()
{
	KSimpleConfig config( "kpicosim" ) ;

	config.setGroup( "Peripherals" ) ;

	if ( config.readPropertyEntry( "serial", QVariant::Bool ).toBool() )     
		showSerialPort() ;
	if ( config.readPropertyEntry( "scratchpad", QVariant::Bool ).toBool() )	
		showScratchpad() ;
	m_editor->open( config.readEntry( "filename" ) ) ;
	m_bitfile = config.readEntry( "bitfile" ) ;

	restoreWindowSize( &config ) ;
	

/*
	int nports = config.readPropertyEntry( "numIOPorts", QVariant::Int ).toInt() ;

	for ( int i = 0 ; i < nports ; i++ ) {
		QString group ;
		group.sprintf( "IO Port %d", i ) ;
		KPortView * ioport = new KPortView( m_simulator->getCpu(), 0 ) ;
		ioport->readConfig( config, group ) ;
		m_ioList.append( ioport ) ;
		addDockWindow( ioport, DockRight ) ;
		connect( ioport, SIGNAL( closing( KPortView* ) ), this, SLOT( removeIOPort( KPortView* ) ) ) ;
	}
*/
}

#include "kpicosim.moc"
