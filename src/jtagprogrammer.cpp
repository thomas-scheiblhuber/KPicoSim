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

/* 	FIXME
 *
 *	This source is still a mess. 
 *  I will clean it in the (near?) future.
 *
 */

#include "jtagprogrammer.h"
#include <iostream>
#include <qapplication.h>
#include <qeventloop.h>

bool IDCODE_PROM[] = { 0, 1, 1, 1, 1, 1, 1, 1 } ;
bool BYPASS_PROM[] = { 1, 1, 1, 1, 1, 1, 1, 1 } ;
bool IDCODE[] =      { 1, 0, 0, 1, 0, 0 } ;
bool CFG_IN[] =      { 1, 0, 1, 0, 0, 0 } ;
bool JSTART[] =      { 0, 0, 1, 1, 0, 0 } ;
bool JPROGRAM[] =    { 1, 1, 0, 1, 0, 0 } ;
bool JSHUTDOWN[] =   { 1, 0, 1, 1, 0, 0 } ;
bool USER1[]  =      { 0, 1, 0, 0, 0, 0 } ;
bool USER2[]  =      { 1, 1, 0, 0, 0, 0 } ;

#define XCF02S   0x05045093
#define XC3S50   0x0140C093
#define XC3S200  0x01414093
#define XC3S400  0x0141C093
#define XC3S1000 0x01428093
#define XC3S1500 0x01434093
#define XC3S2000 0x01440093
#define XC3S4000 0x01448093
#define XC3S5000 0x01450093

#define ID_LEN		32	

const char *id_string[] =
{
	"XS3S50", "XS3S200", "XS3S400", "XS3S1000", "XS3S1500", "XS3S2000", "XS3S4000", "XS3S5000",
	"XCF02S" 
} ;

const unsigned int id_code[] =
{
	XC3S50, XC3S200, XC3S400, XC3S1000, XC3S1500, XC3S2000, XC3S4000, XC3S5000,
	XCF02S
} ;

#define MAX_IDS ( sizeof( id_code ) / sizeof( int ) ) 

JTAGProgrammer::JTAGProgrammer( QObject *parent )
{
	m_parent = parent ;
	m_bitFilename = "" ;

	m_dev = new CJTAG ;
}


JTAGProgrammer::~JTAGProgrammer()
{
	m_dev->close() ;
	delete m_dev ;
}

int JTAGProgrammer::getDevice( bool *id ) {
	unsigned int j, dev_id ;

	for ( j = 0, dev_id = 0 ; j < ID_LEN ; j++ ) {
		dev_id |= id[ j ] << j ;
	}
		
	for ( j = 0 ; j < MAX_IDS ; j++ ) {
		if ( id_code[j] == dev_id ) {
			std::string s ;
			s = std::string( "Found Device : ") + id_string[j] + std::string( "\n" );
			emit message( s.c_str() ) ;
			return dev_id ;
		}
	}

	std::cout << "Unknown ID: " << std::hex << dev_id << "\n" ;

	return 0 ;
}

void JTAGProgrammer::program()
{
	if ( m_bitFilename == "" ) {
		emit message( "No filename given\n" ) ;
		return ;
	}

	bool id[ ID_LEN], ones[ ID_LEN ], dummy[ 32 ] ;
	int device0, device1 ;
	bool prom_present = true ;

	for ( int i = 0 ; i < ID_LEN ; i++ )
		ones[i] = 1 ;

	if ( m_dev->isOpen() )
		return ;

	if ( !m_dev->open( (char*) "/dev/parport0" ) ) {
		emit message( "/dev/parport0 could not be opened; check permissions\n" ) ;
		return ;
	}
/* ...Get device(s)... */
	m_dev->selectIR() ;
	m_dev->setIR( IDCODE_PROM, 8 ) ;
	m_dev->setIR( IDCODE, 5 ) ;
	m_dev->exitIR( IDCODE[5] ) ;

	m_dev->selectDR() ;
	m_dev->setDR( ones, id, ID_LEN ) ;

	device0 = getDevice( id ) ;

	if ( !device0 ) {
		emit message( "Unknown device in JTAG chain\n" ) ;
		goto exit ;
	}
	m_dev->setDR( ones, id, ID_LEN-1 ) ;
	id[ 31 ] = m_dev->exitDR( 1 ) ;

	device1 = getDevice( id ) ;
	if ( !device1 ) {
		emit  message( "Second device is unknown (I will try to continue)\n" ) ;
		prom_present = false ;
	}

/* ....Setup for configuration..... */
	m_dev->selectIR() ;
	m_dev->setIR( JPROGRAM, 5 ) ;				/* Clear configuration memory */
	m_dev->exitIR( JPROGRAM[5] ) ;				/* Although this is not documented?? */
	
	m_dev->selectIR();
	if ( prom_present ) m_dev->setIR( BYPASS_PROM, 8 ) ;
	m_dev->setIR( JSHUTDOWN, 5 ) ;
	m_dev->exitIR( JSHUTDOWN[5] ) ;

	m_dev->selectRunTestIdle() ;
	for ( int i = 0 ; i < 20 ; i++ )		/* Shutdown sequence */
		m_dev->execute( 0, 0 ) ;

	m_dev->selectIR() ;
	if ( prom_present ) m_dev->setIR( BYPASS_PROM, 8 ) ;
	m_dev->setIR( CFG_IN, 5 ) ;
	m_dev->exitIR( CFG_IN[5] ) ;

/* ....Send bit file.....*/
	FILE *bit_file ;
	bit_file = fopen( m_bitFilename.c_str(), "r" ) ;
	if ( bit_file == NULL ) {
		emit message( "Could not read bit file\n" ) ;
		goto exit ;
	}

	int size  ;
	bool frame[ 8 ] ;
	unsigned int cur, next ;
	size = 0 ;
	
	m_dev->selectDR() ;
	fread( &cur, 1, 1, bit_file ) ;

	int prog, prev_prog, total ;
	fseek( bit_file, 0, SEEK_END ) ;
	total = ftell( bit_file ) ;
	rewind( bit_file ) ;
	
	emit message( "Programming..." ) ;
	for (prog=prev_prog=0;;) {
		for ( int i = 0 ; i < 8; i++, cur <<= 1 )  
			frame[i] = (cur & 0x80 ) != 0 ;

		size += 1;
		
		emit progress( size*100/total ) ;	
		
 		if ( fread( &next, 1, 1, bit_file ) == 0 ) 
			break ;
		m_dev->setDR( frame, dummy, 8 ) ;
		cur = next ;

		QApplication::eventLoop()->processEvents( QEventLoop::AllEvents ) ;
	}
	std::cout << std::endl ;
	
	m_dev->setDR( frame, dummy, 7 ) ;
	m_dev->exitDR( frame[7] ) ;
	fclose( bit_file ) ;

/*....Start FPGA.....*/
	m_dev->selectTestLogicReset() ;

	m_dev->selectIR() ;
	if ( prom_present ) m_dev->setIR( BYPASS_PROM, 8 ) ;
	m_dev->setIR( JSTART, 5 ) ;
	m_dev->exitIR( JSTART[5] ) ;

	m_dev->selectRunTestIdle() ;
	for ( int i = 0 ; i < 20 ; i++ )		/* Start up sequence */
		m_dev->execute( 0, 0 ) ;

	emit message( "done.\n" ) ;
exit:
	m_dev->close() ;
}

void JTAGProgrammer::setBitFile( std::string filename )
{
	m_bitFilename = filename ;
}

