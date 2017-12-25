unit StrHan;

{ String Handling Routines, Version 2.3 by George M. Tzoumas (c) 1993-1997 }

{ This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
Use this software AT YOUR OWN RISK. }

interface

const Parser : Char = ' ';

type
    String2 = String[2];
    String4 = String[4];

function Words(s: String): Byte;
function XWord(s: String; WNo: Byte): String;
function LTrim(s: String): String;
function RTrim(s: String): String;
function Left(s: String; nc: Byte): String;
function Right(s: String; nc: Byte): String;
function Verified(Source, Dest: String): Boolean;
function DnCase(ch: Char): Char;
function UpStr(s: String): String;
function DnStr(s: String): String;
function FullTrim(S: String): String;
function SingleSpace(s: String): String;
function Pad(s: String; Len: Byte): String;
function TwoDigit(W: Word; Pad: Boolean): String2;
function FourDigit(W: Word): String4;
function IntToStr(I: Longint): String;
function FormatDateTime(DT: Longint; Opts: Word): String;
function FormatAttr(Attr: Word): String4;
function MStr(n: Byte; c: Char): String;
function Contains(S1, S2: String): Boolean;

implementation

uses Dos;

function Words(s: String): Byte;
var
  i, n: Byte;
begin
  i := 1;
  repeat
    n := Pos(Parser, s);
    if n > 0 then Inc(I);
    Delete(s, n, 1);
  until n = 0;
  Words := i;
end;

function XWord(s: String; WNo: Byte): String;
var i: Byte;
begin
     for i := 1 to WNo - 1 do Delete(s, 1, Pos(Parser, s));
     i := Pos(Parser, s);
     if i = 0 then i := Length(s) + 1;
     XWord := Copy(s, 1, i - 1);
end;

function LTrim(s: String): String;
var st: String;
begin
  st := s;
  while (st[0] > #0) and (st[1] = #32) do
  begin
    Move(st[2], st[1], Pred(Length(st)));
    Dec(st[0]);
  end;
  LTrim := st;
end;

function RTrim(s: String): String;
var st: String;
begin
  st := s;
  while (st[0] > #0) and (st[Length(st)] = #32) do Dec(st[0]);
  RTrim := st;
end;

function Left(s: String; nc: Byte): String;
begin
  Left:=Copy(s, 1, nc);
end;

function Right(s: String; nc: Byte): String;
begin
  Right := Copy(s, Length(s) - nc + 1, Length(s));
end;

function Verified(Source, Dest: String): Boolean;
var
  i: Byte;
  v: Boolean;
begin
  v := False;
  if Pos(Source, Dest) > 0 then
    for i := 1 to Words(Dest) do if Source = XWord(Dest, i) then
    begin
      v := True;
      Break;
    end;
  Verified := v;
end;

function DnCase(ch: Char): Char;
begin
  if ('A' <= ch) and ( ch <= 'Z') then DnCase := Char(Byte(ch)+32)
    else Dncase := ch
end;

function UpStr(s: String): String;
var i: Byte;
begin
  for i := 1 to Length(s) do s[i] := UpCase(s[i]);
  UpStr := s;
end;

function DnStr(s: String): String;
var i: Byte;
begin
  for i := 1 to Length(s) do s[i] := DnCase(s[i]);
  DnStr := s;
end;

function FullTrim(S: String): String;
begin
  S := LTrim(RTrim(S));
  FullTrim := S;
end;

function SingleSpace(s: String): String;
var i: Byte;
begin
  i := pos('  ', s);
  while (i <> 0) do
  begin
    Delete(s, i, 1);
    i := Pos('  ', s);
  end;
  SingleSpace := s;
end;

function Pad(s: String; Len: Byte): String;
begin
  if Length(s) < Len then
    FillChar(s[Succ(Length(s))], Len-Length(s), ' ');
  s[0] := Char(Len);
  Pad := s;
end;

function TwoDigit(W: Word; Pad: Boolean) : String2;
var
  s: String2;
begin
  Str(W:2, s);
  if Pad and (s[1] = ' ') then s[1] := '0';
  TwoDigit := s;
end;

function FourDigit(W: Word) : String4;
var
  s: String4;
begin
  Str(W:4, s);
  FourDigit := s;
end;

function IntToStr(I: Longint): String;
var
   S: string[11];
begin
  Str(I, S);
  IntToStr := S;
end;

function FormatDateTime(DT: Longint; Opts: Word): String;
var
   s: String;
   t: DateTime;
begin
  UnpackTime(DT, t);
  s := '';
  if (Opts and 1) <> 0 then  { add the date }
  begin
    s := s + TwoDigit(t.Month, False) + '-' + TwoDigit(t.Day, True);
    s := s + '-' + Copy(FourDigit(t.Year),3,2);
  end;
  if (Opts and 2) <> 0 then  { add the time }
  begin
    if s <> '' then s := s + ' ';
    s := s + TwoDigit(t.Hour, True) + ':' + TwoDigit(t.Min, True) + ':' +
      TwoDigit(t.Sec, True);
  end;
  FormatDateTime := s;
end;

function FormatAttr(Attr: Word): String4;
var
  s: String4;
begin
  s := 'תתתת';
  if Attr and Archive = Archive then s[1] := 'A';
  if Attr and ReadOnly = ReadOnly then s[2] := 'R';
  if Attr and SysFile = SysFile then s[3] := 'S';
  if Attr and Hidden = Hidden  then s[4] := 'H';
  FormatAttr := s;
end;

function MStr(n: Byte; c: Char): String;
var
  i: Byte;
  s: String;
begin
  FillChar(s[1], n, c);
  s[0] := Char(n);
  MStr:=s;
end;

{... i think I 've copied this routine from somewhere else... }
function Contains(S1, S2: String): Boolean; assembler;
asm
	PUSH	DS
        CLD
        LDS	SI,S1
        LES	DI,S2
        MOV	DX,DI
        XOR	AH,AH
        LODSB
        MOV	BX,AX
        OR      BX,BX
        JZ      @@2
        MOV	AL,ES:[DI]
        XCHG	AX,CX
@@1:	PUSH	CX
	MOV	DI,DX
	LODSB
        REPNE	SCASB
        POP	CX
        JE	@@3
	DEC	BX
        JNZ	@@1
@@2:	XOR	AL,AL
	JMP	@@4
@@3:	MOV	AL,1
@@4:	POP	DS
end;

end.
