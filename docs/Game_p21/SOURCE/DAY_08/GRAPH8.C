
/// I N C L U D E S ///////////////////////////////////////////////////////////

#include <math.h>
#include <stdio.h>
#include "graph8.h"

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


