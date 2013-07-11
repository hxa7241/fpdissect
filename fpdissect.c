/*------------------------------------------------------------------------------

   FPDissect - floating-point format viewer (C90)
   Harrison Ainsworth / HXA7241 : 2013        

   http://www.hxa.name/tools/    

   License: CC0 -- http://creativecommons.org/publicdomain/zero/1.0/

------------------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>




/* declarations ------------------------------------------------------------- */

/* primitives */

typedef signed   int   bool;

typedef unsigned int   intu;

typedef signed   char  int08;
typedef signed   short int16;
typedef signed   int   int32;
typedef signed   long  int64;

typedef unsigned char  int08u;
typedef unsigned short int16u;
typedef unsigned int   int32u;
typedef unsigned long  int64u;

typedef float          real32;
typedef double         real64;

#define false  0
#define true   1


/* domain */

union SingleBits
{
   real32 real;
   int32u bits;
};

typedef union SingleBits SingleBits;


union DoubleBits
{
   real64 real;
   int64u bits;
};

typedef union DoubleBits DoubleBits;


void printDigits
(
   int64u bits,
   intu   length,
   bool   isBytes,
   bool   isSpacers
);

void printBytesAndBits
(
   int64u bits,
   intu   length
);




/* implementation ----------------------------------------------------------- */

void printDigits
(
   int64u bits,
   intu   length,
   bool   isBytes,
   bool   isSpacers
)
{
   static const char spacers[] = " :,:";

   int y;
   for( y = (int)(length <= 8u ? length : 8u);  y-- > 0; )
   {
      const int64u byte = (bits >> (y * 8)) & 0xFFu;

      if( isBytes )
      {
         printf( "%02X", (intu)byte );
      }
      else
      {
         int i;
         for( i = 8;  i-- > 0;  printf( "%u", (int)((byte >> i) & 1u) ) );
      }

      if( isSpacers && y ) printf( "%c", spacers[y & 0x03] );
   }
}


void printBytesAndBits
(
   int64u bits,
   intu   length
)
{
   printf( "bytes:   " );
   printDigits( bits, length, true, true );
   printf( "\nbits:    " );
   printDigits( bits, length, false, true );
   printf( "\n" );
}




/* main --------------------------------------------------------------------- */

int main
(
   int   argc,
   char* argv[]
)
{
   /* print help message */
   if( (argc <= 1) || !strcmp(argv[1], "-?") || !strcmp(argv[1], "--help") )
   {
      printf( "\n  FPDissect - floating-point format viewer (C90)\n"
         "  Harrison Ainsworth / HXA7241 : 2013-03-06\n"
         "  http://www.hxa.name/tools/\n"
         "\n"
         "Displays the structure of IEEE-754 floating point single (binary32)\n"
         "and double (binary64) precision numbers.\n"
         "\n"
         "usage:\n"
         "  fpdissect [options] number [number ...]\n"
         "\n"
         "options:\n"
         "* -bh | --brief-hex  :  display hex only\n"
         "* -bb | --brief-bin  :  display binary only\n"
         "\n" );
   }
   /* print platform/language precondition failure message */
   else if( sizeof(int64u) < 8 )
   {
      printf( "\n*** PROBLEM: FPDissect requires 64-bit ints ***\n\n" );
      return 1;
   }
   /* execute */
   else
   {
      const bool isBytesOnly = !strcmp(argv[1], "-bh") ||
         !strcmp(argv[1], "--brief-hex");
      const bool isBitsOnly  = !strcmp(argv[1], "-bb") ||
         !strcmp(argv[1], "--brief-bin");
      const bool isAll       = !(isBytesOnly || isBitsOnly);
      int        argi        = isAll ? 1 : 2;

      for( ; argi < argc;  ++argi )
      {
         const char* string = argv[argi];
         real64      number = strtod( string, 0 );

         if( isAll )
         {
            printf( "-------- INPUT -------------------------"
               "----------------------------------------\n" );
            printf( "string:  %s\n", string );
            printf( "single:  %.30e\n", (real32)number );
            printf( "double:  %.60e\n", number );
         }

         /* single */
         {
            SingleBits si;
            int08u     sign;
            int08u     exponent[1];
            int08u     mantissa[3];

            si.real     = number;
            sign        = si.bits >> 31;
            exponent[0] = si.bits >> 23;
            mantissa[2] = 0x80u | (si.bits >> 16);
            mantissa[1] = si.bits >> 8;
            mantissa[0] = si.bits;

            if( isAll )
            {
               printf( "-------- SINGLE ------------------------"
                  "----------------------------------------\n" );
               printf( "parts:   s %X  e %02X(%+i)  m %X,%02X:%02X\n",
                  sign, exponent[0], (int)exponent[0] - 127,
                  mantissa[2], mantissa[1], mantissa[0] );
               printBytesAndBits( si.bits, sizeof(si.bits) );
            }
            else
            {
               printDigits( si.bits, sizeof(si.bits), isBytesOnly, false );
               printf( "  " );
            }
         }

         /* double */
         {
            DoubleBits db;
            int08u     sign;
            int08u     exponent[2];
            int08u     mantissa[7];

            db.real     = number;
            sign        = db.bits >> 63;
            exponent[1] = (db.bits >> 60) & 0x07u;
            exponent[0] = db.bits >> 52;
            mantissa[6] = ((db.bits >> 48) & 0x0Fu) | 0x10u;
            mantissa[5] = db.bits >> 40;
            mantissa[4] = db.bits >> 32;
            mantissa[3] = db.bits >> 24;
            mantissa[2] = db.bits >> 16;
            mantissa[1] = db.bits >>  8;
            mantissa[0] = db.bits;

            if( isAll )
            {
               printf( "-------- DOUBLE ------------------------"
                  "----------------------------------------\n" );
               printf( "parts:   s %X  e %02X:%02X(%+i)  "
                  "m %X;%02X:%02X,%02X:%02X,%02X:%02X\n",
                  sign, exponent[1], exponent[0],
                  (((int)exponent[1] << 8) | ((int)exponent[0])) - 1023,
                  mantissa[6], mantissa[5], mantissa[4], mantissa[3],
                  mantissa[2], mantissa[1], mantissa[0] );
               printBytesAndBits( db.bits, sizeof(db.bits) );
            }
            else
            {
               printDigits( db.bits, sizeof(db.bits), isBytesOnly, false );
            }
         }

         printf( "\n" );
      }
   }

   return 0;
}
