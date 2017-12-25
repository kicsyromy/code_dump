
// D E F I N E S /////////////////////////////////////////////////////////////

#define FP_SHIFT          8 // number of binary decimal digits
#define FP_SHIFT_2N     256 // 2^FP_SHIFT, used during conversion of floats

// S T R U C T U R E S ///////////////////////////////////////////////////////

// define our new magical fixed point data type

typedef long fixed;

// P R O T O T Y P E S ///////////////////////////////////////////////////////

fixed Assign_Integer(long integer);

fixed Assign_Float(float number);

fixed Mul_Fixed(fixed f1,fixed f2);

fixed Div_Fixed(fixed f1,fixed f2);

fixed Add_Fixed(fixed f1,fixed f2);

fixed Sub_Fixed(fixed f1,fixed f2);

void Print_Fixed(fixed f1);




