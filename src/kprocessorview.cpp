#include "kprocessorview.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qscrollview.h>
#include <qsize.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qcursor.h>
#include <klocale.h>

MyListView::MyListView(QWidget *parent) : KListView(parent)
{
	connect( this, SIGNAL( contextMenu( KListView *, QListViewItem *, const QPoint & ) ),
             SLOT( slotContextMenu( KListView *, QListViewItem *, const QPoint & ) ) );
}

MyListView::~MyListView()
{
}

void MyListView::slotContextMenu( KListView *, QListViewItem *, const QPoint & ) 
{
//	mousePressEvent( NULL ) ;
	emit showPopupMenu() ;
}
/*
void MyListView::mousePressEvent( QMouseEvent * event )
{
	//QMessageBox::information( this, "information", "mouse pressed" ) ;
	
//	if ( event->button() == RightButton ) 
		emit showPopupMenu() ;
//	else
//		KListView::mousePressEvent( event ) ;
}
*/

KProcessorView::KProcessorView(QWidget *parent, const char *name)
 : KToolBar(parent)
{
	name = name ; // avoid compiler warning
	
	view = new MyListView( this ) ;
	
	connect( view, SIGNAL( itemRenamed( QListViewItem*) ), this, SLOT( slotItemRenamed(QListViewItem*) ) ) ;
	connect( view, SIGNAL( showPopupMenu() ), this, SLOT( showPopupMenu() ) ) ;

	view->addColumn( i18n( "Register" ) ) ;
	view->addColumn( i18n( "Value" ) ) ;
	view->setRootIsDecorated( true ) ;
	view->setRenameable( 1, true ) ;	// Column 1 is renameable
	view->setRenameable( 0, false ) ;	// Column 1 is renameable
	view->setItemsRenameable( true ) ;
	setWidget( view ) ;
	setResizeEnabled( true ) ;
	
	int i ;
	QListViewItem *cpu = new QListViewItem( view ) ;
	cpu->setText( 0, i18n("CPU" ) ) ;

	QListViewItem *flags = new QListViewItem( cpu ) ;
	flags->setText( 0, i18n("Flags") ) ;

	zeroFlag = new QListViewItem( flags ) ;
	zeroFlag->setText( 0, i18n("Zero") ) ;
	zeroFlag->setText( 1, "0" ) ;
	
	carryFlag = new QListViewItem( flags ) ;
	carryFlag->setText( 0, i18n("Carry") ) ;
	carryFlag->setText( 1, "0" ) ;
	
	ieFlag = new QListViewItem( flags ) ;
	ieFlag->setText( 0, i18n("IE") ) ;
	ieFlag->setText( 1, "0" ) ;
	
	QListViewItem *reg = new QListViewItem( cpu ) ;
	reg->setText( 0, i18n("Registers") ) ;

	for ( i = 0 ; i < 16 ; i++ ) {
		regs[ i ] = new QListViewItem( reg ) ;
		regs[ i ]->setText( 0, QString( "s") + QString::number( i, 16 ).upper() ) ;
		regs[ i ]->setText( 1, "0" ) ;
	}
	
	view->setOpen( cpu, true ) ;
	view->setOpen( flags, true ) ;
	view->setOpen( reg, true ) ;

	m_bHexMode = false ;
}

void KProcessorView::slotHexMode()
{
	unsigned char regs[ 16 ] ;

	getRegisterValues(regs) ;
	m_bHexMode = !m_bHexMode ;
	setRegisterValues(regs) ;
}

void KProcessorView::showPopupMenu() 
{
	QPopupMenu *menu = new QPopupMenu( this ) ;
	menu->insertItem( i18n( "Hexadecimal" ), this, SLOT( slotHexMode() ), 0, 1 ) ;
	menu->setItemChecked( 1, m_bHexMode ) ;	
	menu->exec( QCursor::pos() );
}

void KProcessorView::slotItemRenamed( QListViewItem * item )
{
	int i, value, base ;
	bool ok ;
	
	if ( m_bHexMode ) base = 16 ;
	else              base = 10 ;

	if ( !item )
		return ;
	
	for ( i = 0 ; i < 16 ; i++ ) {
		if ( item == regs[ i ] ) {
			value = item->text(1).toInt( &ok, base ) ;
		
			if ( !ok || value < 0 || value > 255 ) {
				QMessageBox::warning( parentWidget(), i18n( "Modify register" ), i18n( "Value should be between 0-255" ) ) ;
				view->rename( item, 1 ) ;
			} else {
				emit processorRegsChanged() ;
			}
			return ;
		}
	}
	
	if ( zeroFlag == item || carryFlag == item || ieFlag == item ) {
		value = item->text(1).toInt( &ok ) ;
		if ( !ok || value < 0 || value > 1 ) {
			QMessageBox::warning( parentWidget(), i18n( "Modify flag" ), i18n( "Value should be between 0-1") ) ;
		} else
			emit processorFlagsChanged() ;
	}
}

void KProcessorView::resizeEvent( QResizeEvent *event )
{
	event = event ;
//	view->resize( width() - 10, height() - 10 ) ;
}

void KProcessorView::getRegisterValues( unsigned char *values )
{
	int i, base ;
	bool ok ;

	if ( m_bHexMode ) base = 16 ;
	else              base = 10 ;

	for ( i = 0 ; i < 16 ; i++ ) {
		values[ i ] = regs[ i ]->text(1).toInt( &ok, base ) ;
	}
}

void KProcessorView::setRegisterValues( unsigned char *values )
{
	int i, base ;
		
	if ( m_bHexMode ) base = 16 ;
	else              base = 10 ;

	QString str; 
	for ( i = 0 ; i < 16 ; i++ ) {
		str.setNum( values[ i ], base ) ;
		regs[ i ]->setText( 1, str ) ;
	}
}
		
void KProcessorView::setFlags( unsigned char flags )
{
	if ( flags & 0x01 ) zeroFlag->setText( 1, "1" ) ;
	else                zeroFlag->setText( 1, "0" ) ;
	
	if ( flags & 0x02 ) carryFlag->setText( 1, "1" ) ;
	else                carryFlag->setText( 1, "0" ) ;	
		
	if ( flags & 0x04 ) ieFlag->setText( 1, "1" ) ;
	else                ieFlag->setText( 1, "0" ) ;
} 

unsigned char KProcessorView::getFlags() 
{
	unsigned char flags = 0 ;
		
	if ( zeroFlag->text(1).toInt() == 1 )  flags |= 0x01 ;
	if ( carryFlag->text(1).toInt() == 1 ) flags |= 0x02 ;
	if ( ieFlag->text(1).toInt() == 1 )    flags |= 0x04 ;
	
	return flags ;
}

KProcessorView::~KProcessorView()
{
	delete view ;
}



