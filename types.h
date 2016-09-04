#pragma once

typedef enum TOKENTYPE
{
	ttNULL, // 0
	ttWHITESPACE,
	ttEOL, // end of line
	ttEOF, // end of file

	ttVARIABLE,
	// value
	ttNUMBER, // 123
	ttHEXNUM, // 0xf1
	ttFLOATNUM, // 2.0
	ttSTRING, // "abc"

	ttDOT, // .
	ttCOMMA, // , 10
	ttGHOST, // #
	ttASSIGN, // =
	ttEXDENT, // :
	ttOPENBLOCK, // {
	ttCLOSEBLOCK, // }
	ttPOPEN, // (
	ttPCLOSE, // )
	ttSQOPEN, // [
	ttSQCLOSE, // ]
	ttPUBLIC, // & 20
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
	ttISTRUE, // ? 30
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
