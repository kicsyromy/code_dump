unit MOVSB;

interface

procedure MoveW(var Source, Dest; Count: Word); { movsw }
procedure MoveD(var Source, Dest; Count: Word); { movsd }

procedure FillCharW(var X; Count: Word; value: Word); { stosw }
procedure FillCharD(var X; Count: Word; value: Longint); { stosb }

function CompareB(var Source, Dest; Count: Word): Word; { cmpsb }
function CompareW(var Source, Dest; Count: Word): Word; { cmpsw }
function CompareD(var Source, Dest; Count: Word): Word; { cmpsd }

function ScanB(var Dest; value: Byte; Count: Word): Word; { scasb }
function ScanW(var Dest; value, Count: Word): Word; { scasw }
function ScanD(var Dest; value: Longint; Count: Word): Word; { scasd }

implementation

procedure MoveW(var Source, Dest; Count: Word); assembler;
asm
   push ds
   mov cx, Count
   les di, Dest
   lds si, Source
   cld
   rep stosw
   pop ds
end;

procedure MoveD(var Source, Dest; Count: Word); assembler;
asm
   push ds
   mov cx, Count
   les di, Dest
   lds si, Source
   cld
{  rep movsd } rep; db $66; stosw
   pop ds
end;

procedure FillCharW(var X; Count: Word; value: Word); assembler;
asm
   mov cx, Count
   mov ax, value
   les di, X
   cld
   rep stosw
end;

procedure FillCharD(var X; Count: Word; value: Longint); assembler;
asm
   mov cx, Count
   db $66; mov ax, word ptr value
   les di, X
   cld
{  rep stosd } rep; db $66; stosw
end;

function CompareB(var Source, Dest; Count: Word): Word;
function CompareW(var Source, Dest; Count: Word): Word;
function CompareD(var Source, Dest; Count: Word): Word;

function ScanB(var Dest; value: Byte; Count: Word): Word;
function ScanW(var Dest; value, Count: Word): Word;
function ScanD(var Dest; value: Longint; Count: Word): Word;

end.