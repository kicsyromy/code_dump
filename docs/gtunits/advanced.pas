unit Advanced;

{ Advanced Routines Unit, Version 1.0, Copyright 1996 by George M. Tzoumas }

{ This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
Use this software AT YOUR OWN RISK. }

interface

var InDosFlag: Pointer;

function InWindows: Boolean;
procedure BSave(FN: String; P: Pointer; Length: Word);
procedure BLoad(FN: String; var P: Pointer);
{procedure FixPtr(var P: Pointer);}

implementation

function InWindows: boolean; assembler;
asm
        mov ax, $1600
        int $2f
        cmp al, 00
        jne @@1
        xor al, al
        jmp @@2
@@1:    mov al, 1
@@2:
end;

procedure GetInDosFlag; assembler;
asm
        mov ah, $34
        int 21h
        mov word ptr ds:[InDosFlag], bx
        mov word ptr ds:[InDosFlag+2], es
end;

procedure BSave(FN: String; P: Pointer; Length: Word);
var F: File;
begin
  Assign(F, FN);
  Rewrite(F, Length);
  BlockWrite(F, P^, 1);
  Close(F);
end;

procedure BLoad(FN: String; var P: Pointer);
var F: File;
begin
  Assign(F, FN);
  Reset(F, 1);
  GetMem(P, FileSize(F));
  BlockRead(F, P^, FileSize(F));
  Close(F);
end;

{procedure FixPtr(var P: Pointer);
var s, o, tw: Word;
begin
  s:=seg(p);
  o:=ofs(p);
  tw:=memw[s:o+2];
  memw[s:o]:=tw shl 4;
  memw[s:o+2]:=0;
  memw[s:o+2]:=Word(tw shr 12);
  memw[s:o+2]:=memw[s:o+2] shl 12;
end;}

begin
  GetInDosFlag
end.