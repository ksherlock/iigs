unit TCPIPX;
interface

{********************************************************
*
*  Marinetti TCP/IP internal interfaces for ORCA/Pascal
*
*  Other USES Files Needed: Common
*
*  By Andrew Roughan
*  This file is released to the public domain.
*
*********************************************************
* Note: this file documents interfaces described in the
* Marinetti Debugging Guide.
*********************************************************
* 2003-07-18 AJR Original release
* 2007-06-06 AJR Added userRecord details [1700658]
* 2015-06-14 KWS port to MPW PascalIIgs
*********************************************************}

uses
   TYPES;

const
  trgCount = 1;
  strgCount = 1;

type

   userRecord = record
       uwUserID: integer;
       uwDestIP: longint;
       uwDestPort: integer;
       uwIP_TOS: integer;
       uwIP_TTL: integer;

       uwSourcePort: integer;
       uwLogoutPending: integer;
       uwICMPQueue: longint;
       uwTCPQueue: longint;

       uwTCPMaxSendSeg: integer;
       uwTCPMaxReceiveSeg: integer;
       uwTCPDataInQ: longint;
       uwTCPDataIn: longint;
       uwTCPPushInFlag: integer;
       uwTCPPushInOffset: longint;
       uwTCPPushOutFlag: integer;
       uwTCPPushOutSEQ: longint;
       uwTCPDataOut: longint;
       uwSND_UNA: longint;
       uwSND_NXT: longint;
       uwSND_WND: integer;
       uwSND_UP: integer;
       uwSND_WL1: longint;
       uwSND_WL2: longint;
       uwISS: longint;
       uwRCV_NXT: longint;
       uwRCV_WND: integer;
       uwRCV_UP: integer;
       uwIRS: longint;
       uwTCP_State: integer;
       uwTCP_StateTick: longint;
       uwTCP_ErrCode: integer;
       uwTCP_ICMPError: integer;
       uwTCP_Server: integer;
       uwTCP_ChildList: longint;
       uwTCP_ACKPending: integer;
       uwTCP_ForceFIN: integer;
       uwTCP_FINSEQ: longint;
       uwTCP_MyFINACKed: integer;
       uwTCP_Timer: longint;
       uwTCP_TimerState: integer;
       uwTCP_rt_timer: integer;
       uwTCP_2MSL_timer: integer;
       uwTCP_SaveTTL: integer;
       uwTCP_SaveTOS: integer;
       uwTCP_TotalIN: longint;
       uwTCP_TotalOUT: longint;

       uwUDP_Server: integer;
       uwUDPQueue: longint;
       uwUDPError: integer;
       uwUDPErrorTick: longint;
       uwUDPCount: longint;

       uwTriggers: array [0..trgCount] of longint;
       uwSysTriggers: array [0..strgCount] of longint;
      end;
   userRecordPtr = ^userRecord;
   userRecordHandle = ^userRecordPtr;


procedure TCPIPSetMyIPAddress (ipaddress: longint); 
inline $a2, $3638, $22, $e10000, $8f, '_toolErr';

function TCPIPGetDP: integer; 
inline $a2, $3639, $22, $e10000, $8f, '_toolErr';

function TCPIPGetDebugHex: boolean;
inline $a2, $363A, $22, $e10000, $8f, '_toolErr';

procedure TCPIPSetDebugHex (debugFlag: boolean);
inline $a2, $363B, $22, $e10000, $8f, '_toolErr';

function TCPIPGetDebugTCP: boolean;
inline $a2, $363C, $22, $e10000, $8f, '_toolErr';

procedure TCPIPSetDebugTCP (debugFlag: boolean);
inline $a2, $363D, $22, $e10000, $8f, '_toolErr';

function  TCPIPGetUserRecord(ipid: integer): userRecordHandle;
inline $a2, $363E, $22, $e10000, $8f, '_toolErr';

procedure TCPIPRebuildModuleList;
inline $a2, $364D, $22, $e10000, $8f, '_toolErr';

implementation
end. 
