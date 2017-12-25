{ Mathematical Routines, Version 1.0, (c) 1996 By George M. Tzoumas }

unit Math;

interface

const
  pi_ = 3.1415926535897932385;
  pi_2= 1.5707963268;   { pi/2}
  e_  = 2.718281828459045;

function Tan(x: Real): Real;
function ArcSin(x: Real): Real;
function ArcCos(x: Real): Real;
function Log10(x: Real): Real;
function Log(x, Base: Real): Real;
function Pow(x, p: Real): Real;
function Radians(x: Real): Real;
function Degrees(x: Real): Real;
function Fact(n: Word): Longint;
function SFact(n: Word): Longint;  { Sterling's approximation of factorial }

implementation

function Tan(x: Real): Real;
begin
{  if x = pi_2 then Tan := 1.7e38 else}
  Tan := Sin(x) / Cos(x);
end;

function ArcSin(x: Real): Real;
begin
{  if x = 1 then ArcSin := pi_2 else}
  ArcSin := ArcTan (x/sqrt (1-sqr (x)));
end;

function ArcCos(x: Real): Real;
begin
{  if x = 0 then ArcCos := 0 else}
  ArcCos := ArcTan (sqrt (1-sqr (x)) /x)
end;

function Log10(x: Real): Real;
begin
  Log10 := Ln(x) / Ln(10);
end;

function Log(x, Base: Real): Real;
begin
  Log := Ln(x) / Ln(Base);
end;

function Pow(x, p: Real): Real;
begin
  Pow := Exp(p * Ln(x));
end;

function Radians(x: Real): Real;
begin
  Radians := (pi_ * x) / 180;
end;

function Degrees(x: Real): Real;
begin
  Degrees := (180 * x) / pi_
end;

function Fact(n: Word): LongInt;
begin
  if n = 0 then Fact := 1 else Fact := n*Fact(n - 1)
end;

function SFact(n: Word): Longint;
begin
  SFact := Round(Sqrt(2*pi_*n)*Pow(n/e_, n))
end;

{begin
  pi_ := Pi;
  e_ := Exp(1);}
end.
