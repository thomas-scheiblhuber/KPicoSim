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


#ifndef _KPICOSIM_H_
#define _KPICOSIM_H_

#include "codeeditor.h"
#include "ksimulator.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kmainwindow.h>
#include <klistview.h>
#include <qsplitter.h>
#include <kpopupmenu.h>
#include <ktoolbar.h>
#include <qtabwidget.h>
#include "kprocessorview.h"
#include "kserialview.h"
#include "kscratchpadview.h"
#include "kportview.h"

class KPicoSim : public KMainWindow
{
    Q_OBJECT
	public:
	    KPicoSim();
	    virtual ~KPicoSim();
		
	protected:
		CodeEditor * m_editor ;
		KListView * m_messages ;		
		QSplitter * m_splitter ;
		KProcessorView * m_processorView ;
		KSerialView *m_serialView ;
		KScratchpadView *m_scratchpadView ;
		QTabWidget *m_tabWidget ;
		
		KPopupMenu * m_fileMenu, * m_debugMenu, * m_settingsMenu, * m_editMenu, * m_peripheralMenu, *m_jtagMenu, *m_helpMenu ;
		
		KSimulator * m_simulator ;
		
		// Export to VHDL
		QString m_templateFile, m_outputDir, m_entityName ;
	
		bool m_simulationMode ;
		unsigned int m_nrInstructions ;
				
		void appendMessage( QString str ) ;
		void updateViews() ;
		void updateScratchpadView() ;
		
		virtual void closeEvent( QCloseEvent *e ) ;
		
		void buildMenu() ;

		void openGUI() ;
		void closeGUI() ;

//		QPtrList<KPortView> m_ioList ;
		QString m_bitfile ;

		QPixmap  m_runPxp, m_stopPxp ;
		KToolBar *m_debugBar ;
				
	signals:
		void run() ;
		void stop() ;
	
	public slots:
		void startSim() ;
		void startStop() ;
		void stepped( unsigned int line ) ;
		void messageListClicked( QListViewItem * item ) ;
		
		void updateProcessorRegs() ;
		void updateProcessorFlags() ;
		
		void newIOPort() ;
		void showSerialPort() ;
		void showScratchpad() ;
		void slotClose() ;

		/* The export functions */
		void fileExportVHDL() ;
		void fileExportCOE() ;
		void fileExportMEM() ;
		void fileExportHEX() ;

		bool compile() ;
		void slotFileNew() ;

//		void removeIOPort( KPortView* ioport ) ;

		/* JTAG */
		void jtagDownload() ;
};

#endif // _KPICOSIM_H_
