unit Mouse;

{ Mouse Support Routines, Version 2.2 (c) 1993 By George M. Tzoumas }

{ This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
Use this software AT YOUR OWN RISK. }

interface

procedure M_Reset;
procedure M_Show;
procedure M_Hide;
procedure M_PutAt(newX, newY: integer);
procedure M_SetXrange(Xmin, Xmax: integer);
procedure M_SetYrange(Ymin, Ymax: integer);
procedure M_SetXYrange(minX, minY, maxX, maxY: integer);
procedure M_SetMickeys(Xmick, Ymick: integer);

function M_ButtonPressed: byte;
function M_Xpos: integer;
function M_Ypos: integer;
function M_relXpos: integer;
function M_relYpos: integer;
function M_Type: byte;
function StrM_Type: string;

var
   MouseYes, M_CursorHidden: Boolean;
   HalfX: Boolean;

implementation

procedure M_Reset; assembler;
asm
        mov MouseYes, False
        xor ax, ax
        int 33h
        cmp ax, $FFFF
        jne @@2
        mov MouseYes, True
        mov M_CursorHidden, True
        mov HalfX, False
@@2:
end;

procedure M_Show; assembler;
asm
        cmp MouseYes, True
        jne @@2
        cmp M_CursorHidden, True
        jne @@2
        mov ax, 1
        int 33h
        mov M_CursorHidden, False
@@2:
end;

procedure M_Hide; assembler;
asm
        cmp MouseYes, True
        jne @@2
        cmp M_CursorHidden, True
        je @@2
        mov ax, 2
        int 33h
        mov M_CursorHidden, True
@@2:
end;

procedure M_PutAt(newX, newY: integer); assembler;
asm
        cmp MouseYes, True
        jne @@2
        mov ax, 4
        mov cx, newX
        mov dx, newY
        int 33h
@@2:
end;

procedure M_SetXrange(Xmin, Xmax: integer); assembler;
asm
        cmp MouseYes, True
        jne @@2
        mov ax, 7
        mov cx, Xmin
        mov dx, Xmax
        int 33h
@@2:
end;

procedure M_SetYrange(Ymin, Ymax: integer); assembler;
asm
        cmp MouseYes, True
        jne @@2
        mov ax, 8
        mov cx, Ymin
        mov dx, Ymax
        int 33h
@@2:
end;

procedure M_SetXYrange(minX, minY, maxX, maxY: integer); assembler;
asm
        cmp MouseYes, True
        jne @@2
        mov ax, 7
        mov cx, minX
        mov dx, maxX
        int 33h
        mov ax, 8
        mov cx, minY
        mov dx, maxY
        int 33h
@@2:
end;

procedure M_SetMickeys(Xmick, Ymick: integer); assembler;
asm
        cmp MouseYes, True
        jne @@2
        mov ax, 15
        mov cx, Xmick
        mov dx, Ymick
        int 33h
@@2:
end;

function M_ButtonPressed: byte; assembler;
asm
        xor ax, ax
        cmp MouseYes, True
        jne @@2
        mov ax, 3
        int 33h
        mov al, bl
@@2:
end;

function M_Xpos: integer; assembler;
asm
        cmp MouseYes, True
        jne @@2
        mov ax, 3
        int 33h
        mov ax, cx
        cmp HalfX, True
        jne @@2
        shr ax, 1
@@2:
end;

function M_Ypos: integer; assembler;
asm
        cmp MouseYes, True
        jne @@2
        mov ax, 3
        int 33h
        mov ax, dx
@@2:
end;

function M_relXpos: integer; assembler;
asm
        cmp MouseYes, True
        jne @@2
        mov ax, 11
        int 33h
        mov ax, cx
@@2:
end;

function M_relYpos: integer; assembler;
asm
        cmp MouseYes, True
        jne @@2
        mov ax, 11
        int 33h
        mov ax, dx
@@2:
end;

function M_Type: byte; assembler;
asm
        mov ax, 42     { 0: No Mouse, 1: Bus Mouse, 2: Serial Mouse }
        int 33h        { 3: InPort Mouse, 4: IBM Mouse,             }
        mov al, dl     { 5: Hewlett-Packard Mouse                   }
end;

function StrM_Type: string;
var i: byte;
    s: string;
begin
  s := '';
  i := M_Type;
  case i of
    0: s := 'No Mouse';
    1: s := 'Bus Mouse';
    2: s := 'Serial Mouse';
    3: s := 'InPort Mouse';
    4: s := 'IBM Mouse';
    5: s := 'Hewlett-Packard Mouse';
  end;
  StrM_Type := s;
end;

end.