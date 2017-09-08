#include "jtag.h"
#include <errno.h>
#include <string.h>
#include <iostream>
#include <linux/lp.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>

#include <linux/ppdev.h>
#include <linux/parport.h>

using namespace std ;


#define JTAG_TDO 	PARPORT_STATUS_SELECT 
#define JTAG_TDI	( 1 << 0 )
#define JTAG_CLK	( 1 << 1 )
#define JTAG_TMS	( 1 << 2 )
#define XILINX_PROG     ( 1 << 4 )

const char *jtag_states[] = {
	"UNDEFINED", "TEST_LOGIC_RESET", "RUN_TEST_IDLE", "SELECT_DR_SCAN", "CAPTURE_DR", "SHIFT_DR", "EXIT_DR", "PAUSE_DR",
	"EXIT2_DR", "UPDATE_DR", "SELECT_IR_SCAN", "CAPTURE_IR", "SHIFT_IR", "EXIT_IR", "PAUSE_IR", "EXIT2_IR", "UPDATE_IR"
} ;

CJTAG::CJTAG()
{
	m_state = ST_UNDEFINED ;
	m_fd = -1 ;
	m_open = false ;	
	m_verbose = false ;

}

CJTAG::~CJTAG()
{
	if ( m_open )
		close() ;
}

void CJTAG::error( char *str  )
{
	cout << "JTAG ERROR: " << str << "\r\n" ;
}

bool CJTAG::open( char *dev )
{
	if ( m_open )
		close() ;

//	m_fd = ::open( dev, O_WRONLY | O_NONBLOCK ) ;

	m_fd = ::open( dev, O_RDWR ) ;
	if ( m_fd < 0 ) {
		cerr << "Please check if you have read/write right to device '" << dev << "'" << endl ;
		error( strerror( errno ) ) ;
		return false ;
	}

//	cout << "Successfully opened \"" << dev << "\"\r\n"  ;

	if ( ioctl( m_fd, PPCLAIM ) )
		cout << "Unable to claim parallel port" << endl ;

//	if ( ioctl( m_fd, LPRESET, 0 ) )
//		cout << "reset returned non-zero" << endl ;

	int i ;
	for ( i = 0 ; i < 10 ; i++ )
		execute( 1, 0 ) ;
	
	m_state = ST_TEST_LOGIC_RESET ;

	execute( 0, 0 ) ;

	m_open = true ;

	return true ;
}

bool CJTAG::isOpen()
{
	return m_open ;
}

void CJTAG::close()
{
	if ( m_open && m_fd >= 0 ) {
		char c = 0 ;
		ioctl( m_fd, PPWDATA, &c ) ;
		ioctl( m_fd, PPRELEASE ) ;
		::close( m_fd ) ;
		m_open = false ;
	}
}

void CJTAG::selectIR()
{
	if ( m_state == ST_TEST_LOGIC_RESET ) 
		execute( 0, 0 ) ;

	while ( m_state != ST_SELECT_IR_SCAN )
		execute( 1, 0 ) ;

	// capture ir
	execute( 0, 0 ) ;

	// shift ir
	execute( 0, 0 ) ;
}

void CJTAG::setIR( bool *instruction, int len  )
{
	while ( len--  )
		execute( 0, *instruction++ ) ;
	
}

void CJTAG::exitIR( bool b )
{
	// exit ir
	execute( 1, b ) ;

	// update ir
	execute( 1, 0 ) ;
}

void CJTAG::selectRunTestIdle()
{
	if ( m_state != ST_RUN_TEST_IDLE ) {
		if ( m_state == ST_SELECT_IR_SCAN || m_state == ST_SELECT_DR_SCAN )
			execute( 0, 0 ) ;
		while ( m_state != ST_UPDATE_DR && m_state != ST_UPDATE_IR )
			execute( 1, 0 ) ;
		execute( 0, 0 ) ;
	}
}

void CJTAG::selectTestLogicReset()
{
	while ( m_state != ST_TEST_LOGIC_RESET )
		execute( 1, 0 ) ;
}

void CJTAG::selectDR()
{
	if ( m_state == ST_TEST_LOGIC_RESET ) 
		execute( 0, 0 ) ;

	while ( m_state != ST_SELECT_DR_SCAN )
		execute( 1, 0 ) ;

	// capture dr
	execute( 0, 0 ) ;

	// shift dr
	execute( 0, 0 ) ;
}

void CJTAG::setDR( bool *data, bool *out, int len )
{	
	// shift dr in, and get data
	if ( m_verbose ) {
		while ( len-- ) {
			cout << (*data ? "1" : "0") ;
			*out++ = execute( 0, *data++ ) ;
		}
		cout << endl ;
	} else {
		while ( len-- ) 
			*out++ = execute( 0, *data++ ) ;
	}
}

void CJTAG::setVerbose( bool verbose ) 
{
	m_verbose = verbose ;
}

bool CJTAG::exitDR( bool data )
{
	// exit dr
	return execute( 1, data ) ;
}	


inline bool CJTAG::step( bool TMS, bool TDI )
{
	uint8_t c  ;
	int status ;

	c = XILINX_PROG ; 			// output enable
	if ( TMS ) c |= JTAG_TMS ;
	if ( TDI ) c |= JTAG_TDI ;

	c |= JTAG_CLK ;
	ioctl( m_fd, PPWDATA, &c ) ;	
	c &= ~JTAG_CLK ;
	ioctl( m_fd, PPWDATA, &c ) ;	

	// read TDO
	ioctl( m_fd, PPRSTATUS, &status ) ;

	c |= JTAG_CLK ;
	ioctl( m_fd, PPWDATA, &c ) ;	
	
	return (status & JTAG_TDO) != 0 ; 
}

bool CJTAG::execute( bool TMS, bool TDI )
{
	bool TDO ;

	TDO = step( TMS , TDI ) ;
		
	switch ( m_state ) {
	case ST_TEST_LOGIC_RESET:
		if ( TMS )
			m_state = ST_TEST_LOGIC_RESET ;
		else
			m_state = ST_RUN_TEST_IDLE ;
	break ;
	case ST_RUN_TEST_IDLE:
		if ( TMS )
			m_state = ST_SELECT_DR_SCAN ;
		else
			m_state = ST_RUN_TEST_IDLE ;
	break ;
	case ST_SELECT_DR_SCAN:
		if ( TMS )
			m_state = ST_SELECT_IR_SCAN ;
		else
			m_state = ST_CAPTURE_DR ;
	break ;
	case ST_CAPTURE_DR:
		if ( TMS )
			m_state = ST_EXIT_DR ;
		else
			m_state = ST_SHIFT_DR ;
	break ;
	case ST_SHIFT_DR:
		if ( TMS )
			m_state = ST_EXIT_DR ;
		else
			m_state = ST_SHIFT_DR ;
	break ;
	case ST_EXIT_DR:
		if ( TMS )
			m_state = ST_UPDATE_DR ; 
		else
			m_state = ST_PAUSE_DR ;
	break ;
	case ST_PAUSE_DR:
		if ( TMS )
			m_state = ST_EXIT2_DR ;
		else
			m_state = ST_PAUSE_DR ;
	break ;
	case ST_EXIT2_DR:
		if ( TMS )
			m_state = ST_UPDATE_DR ;
		else
			m_state = ST_SHIFT_DR ;
	break ;
	case ST_UPDATE_DR:
		if ( TMS )
			m_state = ST_SELECT_DR_SCAN ;
		else
			m_state = ST_RUN_TEST_IDLE ;
	break ;
	case ST_SELECT_IR_SCAN:
		if ( TMS )
			m_state = ST_TEST_LOGIC_RESET ;
		else
			m_state = ST_CAPTURE_IR ;
	break ;
	case ST_CAPTURE_IR:
		if ( TMS )
			m_state = ST_EXIT_IR ;
		else
			m_state = ST_SHIFT_IR ;
	break ;
	case ST_SHIFT_IR:
		if ( TMS )
			m_state = ST_EXIT_IR ;
		else
			m_state = ST_SHIFT_IR ;
		break ;
	case ST_EXIT_IR:
		if ( TMS )
			m_state = ST_UPDATE_IR ;
		else
			m_state = ST_PAUSE_IR ;
	break ;
	case ST_PAUSE_IR:
		if ( TMS )
			m_state = ST_EXIT2_IR ;
		else
			m_state = ST_PAUSE_IR ;
	break ;
	case ST_EXIT2_IR:
		if ( TMS )
			m_state = ST_UPDATE_IR ;
		else
			m_state = ST_SHIFT_IR ;
	break ;
	case ST_UPDATE_IR:
		if ( TMS )
			m_state = ST_SELECT_DR_SCAN ;
		else
			m_state = ST_RUN_TEST_IDLE ;
	break ;
	default:
	break ;
	}

//	cout << "state( " << (TMS != 0) << ", " << (TDI != 0) << "): " << jtag_states[ m_state ] << "\r\n" ; 

	return TDO ;
}

void CJTAG::printState()
{
	cout << "State = " << jtag_states[ m_state ] << endl ;
}
