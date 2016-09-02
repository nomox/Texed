#pragma once

typedef enum TOKENTYPE
{
	ttNULL,
	ttWHITESPACE,
	ttEOL, // end of line
	ttEOF, // end of file

	ttVARIABLE,
	// value
	ttNUMBER, // 123
	ttSTRING, // "abc"

	ttDOT, // .
	ttCOMMA, // ,
	ttGHOST, // #
	ttASSIGN, // =
	ttEXDENT, // :
	ttOPENBLOCK, // {
	ttCLOSEBLOCK, // }
	ttBOPEN, // (
	ttBCLOSE, // )
	ttSQOPEN, // [
	ttSQCLOSE, // ]
	ttPUBLIC, // &
	ttPRIVATE, // $
	ttPACK, // %
	ttEVENT, // @
	//binary operation
	ttPLUS, // +
	ttMINUS, // -
	ttMULTIPLY, // *
	ttDIVIDE, // /
	// logic
	ttOR, // or
	ttAND, // and
	ttNOT, // !
	ttISTRUE, // ?
	// statement
	ttIF, // if
	ttELSE, // else
	ttSWITCH, // switch
	ttCASE, // case

	ttFOR, // for
	ttWHILE, // while
	ttTO, // to
	ttSTEP, // step
	ttTIMES, // times
	ttDO, // do
	// reserved words
	ttFUNCTION, // function
	ttTHIS, // this
	ttSELF, // self
	ttUSE, // use
	ttNIL, // nil
	ttTRUE, // true
	ttFALSE // false
} TokenType;