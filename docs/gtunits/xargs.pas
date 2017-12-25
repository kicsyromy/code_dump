unit XArgs;

{ Copyright 1998 by George M. Tzoumas }

{ This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
Use this software AT YOUR OWN RISK. }

{ 4 Nov 1998 }

interface

var
  argstr: string;
  args: set of Char;
  argopts: array[1..26] of Integer;

function ArgIndex(c: Char): Byte;

implementation

function ArgIndex(c: Char): Byte; assembler;
asm
  mov al, c
  sub al, 96
end;

function DnCase(c: Char): Char; assembler;
asm
    mov al, c
    cmp al, 'A'
    jb @@1
    cmp al, 'Z'
    ja @@1
    or al, $20
@@1:
end;

procedure GetArgs;
var
  Code: Integer;
  i, j: Byte;
  s: string;
begin
  argstr := '';
  args := [];
  FillChar(argopts, SizeOf(argopts), 0);
  for i := 1 to ParamCount do
  begin
    s := ParamStr(i);
    for j := 1 to Length(s) do s[j] := DnCase(s[j]);
    if s[1]='-' then for j := 2 to Length(s) do
      if not (s[j] in ['a'..'z',':']) then Continue else
      if s[j] <> ':' then Include(args, s[j]) else
      if j > 2 then
      begin
        Val(Copy(s, Succ(j), Length(s)), argopts[ArgIndex(s[Pred(j)])], Code);
        if Code <> 0 then argopts[ArgIndex(s[Pred(j)])] := 0;
      end else else argstr := argstr + ParamStr(i) + ' ';
  end;
  if argstr <> '' then Dec(argstr[0]);
end;

begin
  GetArgs
end.