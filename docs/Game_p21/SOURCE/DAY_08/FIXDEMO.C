
/// I N C L U D E S ///////////////////////////////////////////////////////////

#include <math.h>
#include <stdio.h>

// D E F I N E S /////////////////////////////////////////////////////////////

#define FP_SHIFT          8 // number of binary decimal digits
#define FP_SHIFT_2N     256 // 2^FP_SHIFT, used during conversion of floats

// S T R U C T U R E S ///////////////////////////////////////////////////////

// define our new magical fixed point data type

typedef long fixed;

// F U N C T I O N S //////////////////////////////////////////////////////////

fixed Assign_Integer(long integer)
{
// this function assigns a integer to a fixed point type by shifting

return((fixed)integer << FP_SHIFT);

} // end Assign_Integer

///////////////////////////////////////////////////////////////////////////////

fixed Assign_Float(float number)
{
// this function assigns a floating point number to a fixed point type
// by multiplication since it makes no sense to shift a floating point data type

return((fixed)(number * FP_SHIFT_2N));

} // end Assign_Float

///////////////////////////////////////////////////////////////////////////////

fixed Mul_Fixed(fixed f1,fixed f2)
{
// this function mulitplies two fixed point numbers and returns the result
// notice how the final result is shifted back

return((f1*f2) >> FP_SHIFT);

} // end Mul_Fixed

///////////////////////////////////////////////////////////////////////////////

fixed Div_Fixed(fixed f1,fixed f2)
{
// this function divvides two fixed point numbers and returns the result
// notice how the divedend is pre-shifted before the division

return((f1<<FP_SHIFT)/f2);

} // end Div_Fixed

///////////////////////////////////////////////////////////////////////////////

fixed Add_Fixed(fixed f1,fixed f2)
{

// this function adds two fixed point numbers and returns the result
// notice how no shifting is necessary

return(f1+f2);

} // end Add_Fixed

///////////////////////////////////////////////////////////////////////////////

fixed Sub_Fixed(fixed f1,fixed f2)
{

// this function subtracts two fixed point numbers and returns the result
// notice how no shifting is necessary

return(f1-f2);

} // end Sub_Fixed

///////////////////////////////////////////////////////////////////////////////

void Print_Fixed(fixed f1)
{
// this function prints out a fixed point number, it does this by
// extracting the portion to the left of the imaginary decimal and
// extracting the portion to the right of the imaginary decimal point

printf("%ld.%ld",f1 >> FP_SHIFT, 100*(unsigned long)(f1 & 0x00ff)/FP_SHIFT_2N);

} // end Print_Fixed


//M A I N //////////////////////////////////////////////////////////////////////

void main(void)
{

fixed f1,f2,f3; // defines some fixed point numbers

f1 = Assign_Float((float)15.00);
f2 = Assign_Float((float)233.45);

printf("\nf1:=");
Print_Fixed(f1);

printf("\nf2:=");
Print_Fixed(f2);

printf("\nf1+f2:=");
f3 = Add_Fixed(f1,f2);
Print_Fixed(f3);

printf("\nf1-f2:=");
f3 = Sub_Fixed(f1,f2);
Print_Fixed(f3);

printf("\nf1*f2:=");
f3 = Mul_Fixed(f1,f2);
Print_Fixed(f3);

printf("\nf2/f1:=");
f3 = Div_Fixed(f2,f1);
Print_Fixed(f3);

} // end main

