#ifndef KPROCESSORVIEW_H
#define KPROCESSORVIEW_H


#include <ktoolbar.h>
#include <knuminput.h>
#include <qcheckbox.h>
#include <klistview.h>

class MyListView : public KListView
{
	Q_OBJECT
	public:
		MyListView(QWidget *parent) ;
		~MyListView() ;

	protected:
//		virtual void mousePressEvent( QMouseEvent *event ) ;


	public slots:
		void slotContextMenu( KListView *, QListViewItem *, const QPoint & ) ;

	signals:
		void showPopupMenu() ;
} ;


class KProcessorView : public KToolBar
{
	Q_OBJECT
	public:
    	KProcessorView(QWidget *parent = 0, const char *name = 0);
    	~KProcessorView();
	
		void setRegisterValues( unsigned char *values ) ;
		void getRegisterValues( unsigned char *values ) ;
		
		void setFlags( unsigned char flags ); 
		unsigned char getFlags() ;
	
	public slots:
		void slotItemRenamed( QListViewItem * item ) ;
		void slotHexMode() ;
		void showPopupMenu() ;

	signals:
		void processorRegsChanged() ;
		void processorFlagsChanged() ;

	private:
		virtual void resizeEvent( QResizeEvent *event ) ;
	
	protected:
		MyListView	*view ;
		QListViewItem	* regs[ 16 ] ;
		QListViewItem	* zeroFlag, * carryFlag, * ieFlag ;
		bool m_bHexMode ;
};

#endif
