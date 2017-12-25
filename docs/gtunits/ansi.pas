{$R-,S-}

{ This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
Use this software AT YOUR OWN RISK. }

{ Copyright 1998 by George M. Tzoumas }

{ 2 Nov 1998 }

unit Ansi;

interface

function Ansi2Normal(as: string): string;
function Normal2Ansi(ns: string): string;

implementation

function Ansi2Normal(as: string): string;
const OctDigit = ['0'..'7'];
var
  i: Byte;
  add, code: Integer;
  ns, tmp: string;
begin
  ns := '';
  i := 1;
  while i <= Length(as) do
  begin
    add := -1;
    if as[i] <> '\' then add := Ord(as[i])
    else begin
      Inc(i);
      if i<=Length(as) then case as[i] of
        'a': add := 7;
        'b': add := 8;
        'f': add := 12;
        'n': add := 10;
        'r': add := 13;
        't': add := 9;
        'v': add := 10;
        '\': add := Ord('\');
        '''': add := Ord('''');
        '"': add := Ord('"');
        'x': if i <= Length(as) - 3 then begin
               Inc(i);
               tmp := Copy(as, i, 3);
               Val('$'+tmp, add, code);
               if Code <> 0 then add := -1;
               Inc(i,2);
             end;
        '0'..'7': if i <= Length(as) - 2 then if (as[i+1] in OctDigit) and (as[i+2] in OctDigit) then
             begin
               add := (Ord(as[i])-48)*512+(Ord(as[i+1])-48)*64+Ord(as[i+2])-48;
               Inc(i,2);
             end;
      end;
    end;
    if add > -1 then ns := ns + Chr(add);
    Inc(i);
  end;
  Ansi2Normal := ns;
end;

function Normal2Ansi(ns: string): string;
const
  HexDigits: array[0..15] of Char = '0123456789ABCDEF';
var
  i: Byte;
  as: string;
  add: string;
  function HexByte(b: Byte): String;
  begin
    HexByte := '0'+HexDigits[(b and $F0) shr 4]+HexDigits[b and $0F]
  end;
begin
  i := 1;
  as := '';
  while i <= Length(ns) do
  begin
    if Ord(ns[i]) in ([32..127]-[39,92,34]) then add := ns[i] else
    case ns[i] of
      #07: add := '\a';
      #08: add := '\b';
      #09: add := '\t';
      #10: add := '\n';
      #12: add := '\f';
      #13: add := '\r';
      '\': add := '\\';
      '"': add := '\"';
      '''':add := '\''';
    else
      add := '\x'+HexByte(Ord(ns[i]));
    end;
    as := as + add;
    Inc(i);
  end;
  Normal2Ansi := as;
end;

end.