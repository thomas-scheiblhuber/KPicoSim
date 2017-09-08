#include <stdint.h>
#include <fcntl.h>


class CJTAG {
	public:
		enum JTAG_STATE {
			ST_UNDEFINED,
			ST_TEST_LOGIC_RESET,
			ST_RUN_TEST_IDLE,
			ST_SELECT_DR_SCAN,
			ST_CAPTURE_DR,
			ST_SHIFT_DR,
			ST_EXIT_DR,
			ST_PAUSE_DR,
			ST_EXIT2_DR,
			ST_UPDATE_DR,
			ST_SELECT_IR_SCAN,
			ST_CAPTURE_IR,
			ST_SHIFT_IR,
			ST_EXIT_IR,
			ST_PAUSE_IR,
			ST_EXIT2_IR,
			ST_UPDATE_IR
		} ;
		CJTAG() ;
		~CJTAG() ;

		bool open( char *dev ) ;
		void close() ;

		void selectIR() ;
		void setIR( bool *instruction, int len ) ;
		void exitIR( bool b ) ;

		void selectDR() ;
		void setDR( bool *data, bool *out, int len ) ;			// 'data' is replaced by data that is read.
		bool exitDR( bool data ) ;


		void selectRunTestIdle() ;
		void selectTestLogicReset() ;

		bool execute( bool TMS, bool TDI ) ;

		void setVerbose( bool verbose ) ;

		void printState() ;

		bool isOpen() ;
	private:
		void error( char *str ) ;
		bool step( bool TMS, bool TDI ) ;

		int m_fd ;
		bool m_open ;
		enum JTAG_STATE m_state ;

		bool m_verbose ;

} ;


