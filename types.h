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
	ttEXTEND, // :
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
	ttLT, // <
	ttGT, // >
	//binary operation
	ttPLUS, // +
	ttMINUS, // -
	ttMULTIPLY, // *
	ttDIVIDE, // /
	// logic
	ttOR, // or 30
	ttAND, // and
	ttNOT, // !
	ttISTRUE, // ?
	// statement
	ttIF, // if
	ttELSE, // else
	ttSWITCH, // switch
	ttCASE, // case
	// loops
	ttFOR, // for
	ttWHILE, // while
	ttDO, // do 40
	ttIN, // in
	ttBREAK, // break
	ttCONTINUE, // contunue
	// reserved words
	ttFUNCTION, // function
	ttFNDEF, // fn
	ttRETURN, // return
	ttTHIS, // this
	ttSELF, // self
	ttUSE, // use
	ttNIL, // nil 50
	ttTRUE, // true
	ttFALSE, // false
	ttPRINT, // print
	ttERROR, // error
	ttDELETE // delete
} TokenType;
