unit Keyb2;

{ Copyright 1999 by George M. Tzoumas }

{ This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
Use this software AT YOUR OWN RISK. }

interface

function Caps_Lock: Boolean;
function RShift: Boolean;
function LShift: Boolean;

implementation

function Caps_Lock: Boolean; assembler;
asm
   mov bx, $40
   mov es, bx
   mov di, $97
   mov al, es:[di]
   and al, 4
   shr al, 2
end;

function RShift: Boolean; assembler;
asm
   mov bx, $40
   mov es, bx
   mov di, $17
   mov al, es:[di]
   and al, 1
end;

function LShift: Boolean; assembler;
asm
   mov bx, $40
   mov es, bx
   mov di, $17
   mov al, es:[di]
   and al, 2
   shl al, 1
end;


end.