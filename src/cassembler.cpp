/*	The assembler works, but it ain't a pretty sight.
 *  Some day, I'll rewrite this part. 
 *		-- M6
 */

#include "cassembler.h"
#include "cpicoblaze.h"

#include <stdio.h>

#define NO_LINE_NR 	0xFFFFFFFF

const char *instructions[] = { 
	"ADD", "ADDCY", "AND", "CALL", "COMPARE", "DISABLE", "ENABLE", "FETCH", "INPUT",
	"JUMP", "LOAD", "OR", "OUTPUT", "RETURN", "RETURNI", "ROTATE", "RL", "RR", "SL0",
	"SL1", "SLA", "SLX", "SR0", "SR1", "SRA", "SRX", "STORE", "SUB", "SUBCY", "TEST",
	"XOR" 
} ;

/* Helper function to make a string uppercase */
string toUpper( string str )
{
	string upperStr ;
	unsigned int i ; 
	
	upperStr = "" ;
	for ( i = 0 ; i < str.length() ; i++ )
		upperStr += toupper( str[ i ] ) ;

	return upperStr ;
}

CAssembler::CAssembler()
{
	m_messageList = 0 ;
}

CAssembler::~CAssembler()
{
}

void CAssembler::error( unsigned int line, const char *description )
{
	cout << line << ": " << description << "\r\n" ; 

	if ( m_messageList ) {
		char str[ 128 ] ;
		sprintf( str, "%u", line + 1 ) ;
		QListViewItem *item = new QListViewItem( m_messageList, m_messageList->lastChild() ) ;
		
		if ( line != NO_LINE_NR )
			item->setText( 0, str ) ;
		item->setText( 1, description ) ;
	}
}

int CAssembler::getRegister( string name ) 
{
	if ( name[ 0 ] != 's' || name.length() <= 1 )
		return -1 ;
	
	int reg ;
	
	if ( sscanf( name.c_str() + 1, "%X", &reg ) != 1 )
		return -1 ;
		
	if ( reg < 0 || reg > 15 )
		return -1 ;
		
	return reg ;
	
}

int CAssembler::getInstruction( string name )
{
	unsigned int i ;
	string str = toUpper( name ) ;
	for ( i = 0 ; i < sizeof( instructions ) / sizeof( char *); i++ )
		if ( str == instructions[ i ] )
			return i ;
	return -1 ;
}

bool CAssembler::buildSymbolTable()
{
	list<CSourceLine*>::iterator it ;
	unsigned int address = 0 ;
	bool ret = TRUE ;

	for ( it = m_source.begin() ; it != m_source.end() ; it++ ) {
		string name = toUpper( (*it)->getColumn( 0 ) ) ; 				// case insensitive
		
		if ( name == "NAMEREG" ) {
			if ( !(*it)->isColumn( 3 ) ) {
				error( (*it)->m_lineNr, "'NAMEREG registername, newname' expected" ) ;
				ret = FALSE ;
			}

			if ( (*it)->isColumn( 4 ) ) {
				error( (*it)->m_lineNr, "Rubbish found at end of line" ) ;
				ret = FALSE ;
			}
			
			if ( (*it)->getColumn( 2 ) != "," ) {
				error( (*it)->m_lineNr, "Comma expected" ) ;
				ret = FALSE ;
			}


			CNamereg *nr = new CNamereg ;
			nr->reg = (*it)->getColumn( 1 ) ;
			nr->name = (*it)->getColumn( 3 ) ;
			m_registerTable.push_back( nr ) ;
			(*it)->m_type = CSourceLine::stNamereg ;
			
		} else if ( name == "CONSTANT" ) {
			if ( !(*it)->isColumn( 3 ) ) {
				error( (*it)->m_lineNr, "'CONSTANT name, valued' expected" ) ;
				ret = FALSE ;
			}

			if ( (*it)->isColumn( 4 ) ) {
				error( (*it)->m_lineNr, "Rubbish found at end of line" ) ;
				ret = FALSE ;
			}
			
			
		 	if ( (*it)->getColumn( 2 ) != "," ) {
				error( (*it)->m_lineNr, "Comma expected" ) ;
				ret = FALSE ;
			}
					
			CConstant *nr = new CConstant ;
			nr->value = (*it)->getColumn( 3 ) ;
			nr->name = (*it)->getColumn( 1 ) ; ;
			m_constantTable.push_back( nr ) ;
			(*it)->m_type = CSourceLine::stConstant ;
		} else if ( name == "ADDRESS" ) {
			if ( !(*it)->isColumn( 1 ) ) {
				error( (*it)->m_lineNr, "Value expected" ) ;
				ret = FALSE ;
			} 
			
			if ( (*it)->isColumn( 4 ) ) {
				error( (*it)->m_lineNr, "Rubbish found at end of line" ) ;
				ret = FALSE ;
			}

			if ( sscanf( (*it)->getColumn( 1 ).c_str(), "%X", &address ) != 1 ) {
				error( (*it)->m_lineNr, "Invalid address" ) ;
				ret = FALSE ;
			}
			(*it)->m_type = CSourceLine::stAddress ;
			(*it)->m_address = address ;
			
		} else if ( getInstruction( (*it)->getColumn( 0 ) ) < 0 ) {
			CLabel *label = new CLabel ;
			label->name = (*it)->getColumn( 0 ) ;
			char buf[ 32 ] ;
			sprintf( buf, "%d", address ) ;
			label->value = buf ;
			m_labelTable.push_back( label ) ;
			
			(*it)->m_type = CSourceLine::stLabel ;
			(*it)->m_address = address ;
			if ( (*it)->isColumn( 1 ) && (*it)->getColumn( 1 ) == ":" ) {
				if ( (*it)->isColumn( 2 ) ) {
					if ( getInstruction( (*it)->getColumn( 2 ) ) < 0 ) {
						error( (*it)->m_lineNr, "Instruction expected" ) ;
						ret = FALSE ;
					} else {
						address = address + 1 ;
					}
				}
			} else {
				error( (*it)->m_lineNr, "Label or Instruction expected" ) ;
				ret = FALSE ;
			}
		} else {
			(*it)->m_address = address ;
			address = address + 1 ;
		}
	}	

	cout << "Constants :\r\n" ;
	list<CConstant*>::iterator it0 ;
	for ( it0 = m_constantTable.begin() ; it0 != m_constantTable.end() ; it0++ ) {
		cout << (*it0)->name << " = " << (*it0)->value << "\r\n" ;
	}
	
	cout << "Namereg :\r\n" ;
	list<CNamereg*>::iterator it1 ;
	for ( it1 = m_registerTable.begin() ; it1 != m_registerTable.end() ; it1++ ) {
		cout << (*it1)->reg << " = " << (*it1)->name << "\r\n" ;
	}
	
	cout << "labels :\r\n" ;
	list<CLabel*>::iterator it2 ;
	for ( it2 = m_labelTable.begin() ; it2 != m_labelTable.end() ; it2++ ) {
		cout << (*it2)->name << " = " << (*it2)->value << "\r\n" ;
	}
	return ret ;
}

string CAssembler::translateRegister( string name )
{
	list<CNamereg*>::iterator it1 ;
	for ( it1 = m_registerTable.begin() ; it1 != m_registerTable.end() ; it1++ ) {
		if ( (*it1)->name == name )
			return (*it1)->reg ;
	}
	
	return name ;

}

string CAssembler::translateConstant( string name )
{
	list<CConstant*>::iterator it1 ;
	for ( it1 = m_constantTable.begin() ; it1 != m_constantTable.end() ; it1++ ) {
		if ( (*it1)->name == name )
			return (*it1)->value ;
	}
	
	return name ;
}

string CAssembler::translateLabel( string label )
{
	list<CLabel*>::iterator it1 ;
	for ( it1 = m_labelTable.begin() ; it1 != m_labelTable.end() ; it1++ ) {
		if ( (*it1)->name == label )
			return (*it1)->value ;
	}
	
	return label ;
}

bool CAssembler::addInstruction( instrNumber instr, CSourceLine sourceLine, int offset )
{
	char err_desc[ 256 ] ;
	unsigned int address = sourceLine.m_address ;
	int maxColumn = 0 ;
	string s1 = sourceLine.getColumn( offset + 1 ) ;
	string s2 = sourceLine.getColumn( offset + 2 ) ;
	string s3 = sourceLine.getColumn( offset + 3 ) ;
	int line = sourceLine.m_lineNr ;
	
	uint32_t code ;
	string s ;
	bool b ;
	switch( instr ) {
		
	case ENABLE:
	case DISABLE:
		if ( toUpper( s1 ) != "INTERRUPT" ) {
			error( line, "'INTERRUPT' expected" ) ;
			return FALSE ;
		}
		if ( instr == ENABLE )
			code = instrENABLE_INTERRUPT ;
		else
			code = instrDISABLE_INTERRUPT ;

		maxColumn = 2 ;

		break ;
	case RETURNI:
		if ( toUpper( s1 ) == "ENABLE" ) {
			code = instrRETURNI_ENABLE ;
		} else if ( toUpper( s1 ) == "DISABLE" ) {
			code = instrRETURNI_DISABLE ;
		} else {
			error( line, "'ENABLE' or 'DISABLE' expected" ) ;
		}
		maxColumn = 2 ;
			
		break ;

	// Almost the same instructions	
	case CALL:
	case JUMP:
	case RETURN:
		b = TRUE ;
		maxColumn=  2 ;
		if ( toUpper( s1 ) == "C" ) {
			switch( instr ) {
			case CALL    : code = instrCALLC ; break ;
			case JUMP    : code = instrJUMPC ; break ;
			case RETURN  : code = instrRETURNC ; break ;
			default: error( line, "'CALL', 'JUMP' or 'RETURN' expected" ) ; return FALSE ;
			}
		} else if ( toUpper( s1 ) == "NC" ) {
			switch( instr ) {
			case CALL    : code = instrCALLNC ; break ;
			case JUMP    : code = instrJUMPNC ; break ;
			case RETURN  : code = instrRETURNNC ; break ;
			default: error( line, "'CALL', 'JUMP' or 'RETURN' expected" ) ; return FALSE ;
			}
		} else if ( toUpper( s1 ) == "NZ" ) {
			switch( instr ) {
			case CALL    : code = instrCALLNZ ; break ;
			case JUMP    : code = instrJUMPNZ ; break ;
			case RETURN  : code = instrRETURNNZ ; break ;
			default: error( line, "'CALL', 'JUMP' or 'RETURN' expected" ) ; return FALSE ;
			}
		} else if ( toUpper( s1 ) == "Z" ) {
			switch( instr ) {
			case CALL    : code = instrCALLZ ; break ;
			case JUMP    : code = instrJUMPZ ; break ;
			case RETURN  : code = instrRETURNZ ; break ;
			default: error( line, "'CALL', 'JUMP' or 'RETURN' expected" ) ; return FALSE ;
			}
		} else {
			switch( instr ) {
			case CALL    : code = instrCALL ; break ;
			case JUMP    : code = instrJUMP ; break ;
			case RETURN  : code = instrRETURN ; break ;
			default: error( line, "'CALL', 'JUMP' or 'RETURN' expected" ) ; return FALSE ;
			}
			b = FALSE ;
			maxColumn = 1 ;
		}
		
		
		if ( instr != RETURN ) {
			if ( b ) {
				if ( s2 != "," ) {
					error( line, "Comma expected" ) ;
					return FALSE ;
				}
				s = s3 ;
			} else
				s = s1 ;
		
			maxColumn = b ? 4 : 2 ;

			s = translateLabel( s )  ;
			int labelVal ;
		
			if ( sscanf( s.c_str(), "%d", &labelVal ) != 1 )  {
				error( line, "Invalid label" ) ;
				return FALSE ;
			}
		
			code |= labelVal ;
		} 
		break ;
	
	// Instruction that expect first an registername
	default:
		int reg = getRegister( translateRegister( s1 ) ) ;
		if ( reg < 0 ) {
			error( line, "Registername expected" ) ;
			return FALSE ;
		}
		
		code = instrROTATE | (reg<<8) ;
		maxColumn = 2 ;
		switch ( instr ) {
		case RL:	code |= instrRL_SX  ; break ;
		case RR:	code |= instrRR_SX  ; break ;
		case SL0:	code |= instrSL0_SX ; break ;
		case SL1:	code |= instrSL1_SX ; break ;
		case SLA:	code |= instrSLA_SX ; break ;
		case SLX:	code |= instrSLX_SX ; break ;
		case SR0:	code |= instrSR0_SX ; break ;
		case SR1:	code |= instrSR1_SX ; break ;
		case SRA:	code |= instrSRA_SX ; break ;
		case SRX:	code |= instrSRX_SX ; break ;

		// Instructions that expect a registername and then a comma
		default:
			if ( s2 != "," ) {
				error( line, "Comma expected" ) ;
				return FALSE ;
			}
			
			switch( instr ) {
			// Instruction Register Comma '(' or value
			case STORE:
			case OUTPUT:
			case INPUT:
			case FETCH:
				if ( sourceLine.getColumn( offset + 3 ) == "(" ) {
					if ( !sourceLine.isColumn( offset + 5 ) || sourceLine.getColumn( offset + 5 ) != ")" ) {
						error( line, "')' expected" ) ;
						return FALSE ;
					}
				
					int reg2 = getRegister( translateRegister( sourceLine.getColumn( offset + 4 ) ) ) ;
					if ( reg2 < 0 ) {
						error( line, "Register expected" ) ;
						return FALSE ;
					}
					code = (reg << 8) | (reg2 << 4) ;
					switch( instr ) {
						case STORE : code |= instrSTORE_SX_SY  ; break ;
						case OUTPUT: code |= instrOUTPUT_SX_SY ; break ;
						case INPUT : code |= instrINPUT_SX_SY  ; break ;
						case FETCH : code |= instrFETCH_SX_SY  ; break ;
						default: error( line, "'STORE', 'OUTPUT', 'INPUT' or 'FETCH' expected" ) ; return FALSE ;
					}
					maxColumn = 6 ; 
				} else {
				
					unsigned int value ;
					if ( sscanf( translateConstant( s3 ).c_str(), "%X", &value ) != 1 ) {
						sprintf( err_desc, "Value or (regname) expected, but \"%s\" found.", s3.c_str() ) ;
						error( line, err_desc ) ;
						return FALSE ;
					}
					
					code = (reg << 8) | value ;
					switch( instr ) {
						case STORE : code |= instrSTORE_SX_SS  ; break ;
						case OUTPUT: code |= instrOUTPUT_SX_PP ; break ;
						case INPUT : code |= instrINPUT_SX_PP  ; break ;
						case FETCH : code |= instrFETCH_SX_SS  ; break ;
						default: error( line, "'STORE', 'OUTPUT', 'INPUT' or 'FETCH' expected" ) ; return FALSE ;
					}
					maxColumn = 4 ;
				}
				break ;
			default:
				// Instruction register comma register or value
				int reg2 = getRegister( translateRegister( s3 ) ) ;

				maxColumn = 4 ;		
				if ( reg2 < 0 ) {
					unsigned int value ;
					if ( sscanf( translateConstant( s3 ).c_str(), "%X", &value ) != 1 ) {
						sprintf( err_desc, "Value expected, but \"%s\" found.", s3.c_str() ) ;
						error( line, err_desc ) ;
						return FALSE ;
					}
					code = (reg << 8) | value ;
					switch( instr ) {
					case ADD     : code |= instrADD_SX_KK     ; break ;
					case ADDCY   : code |= instrADDCY_SX_KK   ; break ;
					case AND     : code |= instrAND_SX_KK     ; break ;
					case COMPARE : code |= instrCOMPARE_SX_KK ; break ;
					case LOAD    : code |= instrLOAD_SX_KK    ; break ;
					case OR      : code |= instrOR_SX_KK      ; break ;
					case SUB     : code |= instrSUB_SX_KK     ; break ;
					case SUBCY   : code |= instrSUBCY_SX_KK   ; break ;
					case TEST    : code |= instrTEST_SX_KK    ; break ;
					case XOR     : code |= instrXOR_SX_KK     ; break ;
					default      : error( line, "Unknown instruction" )  ; return FALSE ;
					}
				} else {
					code = ( reg << 8 ) | ( reg2  << 4 ) ;
					switch( instr ) {
					case ADD    : code |= instrADD_SX_SY     ; break ;
					case ADDCY  : code |= instrADDCY_SX_SY   ; break ;
					case AND    : code |= instrAND_SX_SY     ; break ;
					case COMPARE: code |= instrCOMPARE_SX_SY ; break ;
					case LOAD   : code |= instrLOAD_SX_SY    ; break ;
					case OR     : code |= instrOR_SX_SY      ; break ;
					case SUB    : code |= instrSUB_SX_SY     ; break ;
					case SUBCY  : code |= instrSUBCY_SX_SY   ; break ;
					case TEST   : code |= instrTEST_SX_SY    ; break ;
					case XOR    : code |= instrXOR_SX_SY     ; break ;
					default     : error( line, "Unknown instruction" ) ; return FALSE ;
					}	
				}
			}
		}
	}

	// Check if there's is rubbish at the end of the line
	if ( sourceLine.isColumn( maxColumn + offset ) ) {
		sprintf( err_desc, "'%s' found at end of instruction", sourceLine.getColumn( maxColumn + offset ).c_str() ) ;
		error( line, err_desc ) ;
		return FALSE ;
	}

	// Finally 
	m_code->setInstruction( address, code, line ) ;
	
	return TRUE ;
}


bool CAssembler::exportVHDL( string templateFile, string outputDir, string entityName )
{
	int addr, i, j, k, l, n ;
	unsigned char INIT[ 32 ][ 64 ] ;		/* 32 * 64 = 2048 bytes */
	unsigned char INITP[ 32 ][ 8 ] ;		/* 32 * 8 =   256 bytes (Parity Table)*/
	unsigned int d ;                        /*			 2304 Bytes Total = (18b * 1024 ) / 8 (1 instr = 18 bits )*/
	
	CInstruction *instr ;
	
	for ( i = 0 ; i < 32 ; i++ )
		for( j = 0 ; j < 32 ; j++ )
			INIT[ i ][ j ] = 0 ;
			
	for ( i = 0 ; i < 32 ; i++ )
		for ( j = 0 ; j < 8 ; j++ )
			INITP[ i ][ j ] = 0 ;
	
	/* Build up BRAM in memory */
	for ( addr = i = j = k = l = 0, n = 0 ; addr < 1024 ; addr++ ) {			
		instr = m_code->getInstruction( addr ) ;
		
		if ( instr == NULL ) d = 0 ;
		else d = instr->getHexCode() ;
		
						
		INIT[ i++ ][ j ] =  d ;		// instruction( 15 downto 0 )
		INIT[ i++ ][ j ] =  d >> 8;
		
		INITP[ k ][ l ] |= ( ( d >> 16 ) & 0x3 ) << n ;	// instruction( 17 downto 16 ) ;
		n += 2 ;
				
		if ( n >= 8 ) {
			n = 0 ;
			k++ ;
			if ( k >= 32 ) {
				l++ ;
				k = 0 ;
			}
		}
		
		if ( i >= 32 ) {
			i = 0 ;
			j++ ;
		}
	}
	
	FILE * infile = fopen( templateFile.c_str(), "r" ) ;
	if ( infile == NULL ) {
		error( NO_LINE_NR, string( "Unable to open VHDL template file '" + templateFile + "'" ).c_str() ) ;
		return FALSE ;
	}
	
	string exportFile = outputDir + "/" + entityName + ".vhd" ;
	FILE * outfile = fopen( exportFile.c_str(), "w" ) ;		
	if ( outfile == NULL ) {
		error( NO_LINE_NR , string( "Unable to open VHDL template file '%s'" + exportFile + ".vhd").c_str() ) ;
		return FALSE ;
	}
		
	bool store = false, copy = false;
	char varname[ 64 ] ;
	int p = 0 ;
	int line, c ;
	while ( ( c = fgetc( infile ) ) != EOF ) {
		if ( store && p < 64 )
			varname[ p++ ] = c ;
		
		if ( c == '{' ) {
			store = true ;		
			p = 0 ;
		}
		
		if ( !store && copy )
			fputc( c, outfile ) ;
		
		if ( c == '}' ) {
			store = false ;
			if ( p > 0 )
				varname[ p - 1 ] = '\0' ;
			else
				varname[ 0 ] = '\0' ;
			if ( strncmp( "INIT_", varname, 5 ) == 0 ) {
				sscanf( varname, "INIT_%02X", &line ) ;
				if ( line >= 0 && line < 64 ) {
					for( j = 31 ; j >= 0 ; j-- ) 
						fprintf( outfile, "%02X", INIT[ j ][ line ] ) ;
				}
			} else if ( strncmp( "INITP_", varname, 6 ) == 0 ) {
				sscanf( varname, "INITP_%02X", &line ) ;
				if ( line >= 0 && line < 8 ) 
					for( j = 31 ; j >= 0 ; j-- )
		 				fprintf( outfile, "%02X", INITP[ j ][ line ] ) ;
			} else if ( strcmp( "name", varname ) == 0 ) {
				fprintf( outfile, entityName.c_str() ) ;				
			} else if ( strcmp( "begin template", varname ) == 0 ) {
				copy = true ;
			}
		}
	}
		
	fclose( infile ) ;
	fclose( outfile ) ;
	
	return TRUE ;
}

bool CAssembler::exportHEX( string filename, bool mem )
{
	FILE * file = fopen( filename.c_str(), "w" ) ;		
	if ( file == NULL ) {
		error( NO_LINE_NR , string( "Unable to write to file '" + filename + "'").c_str() ) ;
		return FALSE ;
	}
	
	CInstruction * instr ;
	uint32_t d ;
	uint32_t addr ;

	/* A mem file requires the @ sign */
	if ( mem )
		fprintf( file, "@0\r\n" ) ;

	for ( addr = 0 ; addr < 1024 ; addr++ ) {			
		instr = m_code->getInstruction( addr ) ;
		
		if ( instr == NULL ) d = 0 ;
		else d = instr->getHexCode() ;

		fprintf( file, "%05X\r\n", d ) ;
	}

	fclose( file ) ;

	return TRUE ;
}


bool CAssembler::createOpcodes()
{
	list<CSourceLine*>::iterator it ;
	int columnOffset ; 
	bool ret = TRUE ;

	for ( it = m_source.begin() ; it != m_source.end() ; it++ ) {
		if ( (*it)->m_type == CSourceLine::stNamereg || 
		     (*it)->m_type == CSourceLine::stConstant || 
			 (*it)->m_type == CSourceLine::stAddress )
			continue ;
			
		if ( (*it)->m_type == CSourceLine::stLabel )
			columnOffset = 2 ;
		else 
			columnOffset = 0 ;	

		if ( !(*it)->isColumn( columnOffset + 0 ) )						// just a label
			continue ;
								
		int instr = getInstruction( (*it)->getColumn( columnOffset + 0 ) ) ;
		
		if ( instr < 0  ) {
			error( (*it)->m_lineNr, "Unknown instruction" ) ;
			ret = FALSE ;
		}
		
		if ( addInstruction( (instrNumber) instr, **it, columnOffset )  == FALSE )
			ret = FALSE ;
			
	} 
	return ret ;
}

bool CAssembler::assemble( )
{
	bool r1, r2 ;
	if ( loadFile() == FALSE )
		return FALSE ;
	
	r1 = buildSymbolTable() ;		// Even continue if symbol table failed..
	r2 = createOpcodes() ;			// .. this way we get the most errors/warnings in 1 compile cycle.
	return ( r1 && r2 ) ;

}

char * CAssembler::getWord( char *s, char *word ) {
	char *start, *end ;
	
	*word = '\0' ;
	
	while ( *s == ' ' || *s == '\t' )							// skip whitespaces
		s++ ;
	
	start = s ;
	
	if ( *start == '\0' || *start == '\r' || *start == '\n' || *start == ';' )	// end of line
		return NULL ;
		
	while ( *s != ' ' && *s != '\t' && *s != '\0' && *s != '\r' && *s != '\n' && 
	        *s != ';'  && *s != ',' && *s != ':' && *s != '(' && *s != ')' )
		s++ ;
	
	end = s ;
	
	if ( start != end ) {
		while ( start < end )
			*word++ = *start++ ;
		*word = '\0' ;
		return end ;
	} else if ( *s == ',' || *s == ':' || *s == '(' || *s == ')' ) {
		*word++ = *s ;
		*word = '\0' ;
		return end + 1 ;
	} else
		return NULL ;
}

CSourceLine * CAssembler::formatLine( int lineNr, char *s )
{
	CSourceLine *sourceLine = new CSourceLine( lineNr ) ;
	char *next, word[ 256 ] ;
	
	next = getWord( s, word ) ; 
	if ( word[ 0 ] == '\0' ) {			// empty line
		delete sourceLine ;
		return NULL ;
	}
	
	do {
		sourceLine->addColumn( word ) ;
		next = getWord( next, word ) ;	
		if ( word[ 0 ] == '\0' )
			break ;
	} while ( next != NULL ) ;
	
	return sourceLine ;
}

bool CAssembler::loadFile()
{
	FILE *f ;

	f = fopen( m_filename.c_str(), "r" ) ;

	if ( f == NULL ) {
		string str =  "Unable to load file '" + m_filename + "'";
		error( NO_LINE_NR, str.c_str() ) ;							// No linenumber information
		return FALSE ;
	}
	char buf[ 256 ] ;
	int linenr = 0 ;
	while( fgets( buf, sizeof( buf ), f ) ) {
		CSourceLine *sourceLine = formatLine( linenr++, buf ) ;
		if ( sourceLine != NULL )
			m_source.push_back( sourceLine ) ;
	}

	list<CSourceLine*>::iterator it ;
	
	for ( it = m_source.begin() ; it != m_source.end() ; it++ ) {
		cout << "(" << (*it)->m_lineNr << ")" ;
		int j = 0 ;
		while ( (*it)->isColumn( j ) )
			 cout << "[" << (*it)->getColumn( j++ ) << "]";
		cout << "\r\n" ;
	}
		
	cout << "File " << m_filename << " succesfully loaded\r\n" ;

	return TRUE ;
} 

