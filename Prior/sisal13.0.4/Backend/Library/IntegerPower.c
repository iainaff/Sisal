/**************************************************************************/
/* GLOBAL **************        IntegerPower       ************************/
/**************************************************************************/
/* PURPOSE:  A safer and faster way to compute integer powers		  */
/**************************************************************************/
int
IntegerPower(a,b)
     int a;
     register int b;
{
  register int  r,p;

  if ( b < 0 ) {
    if ( a == 1 ) return 1;
    return 0;
  }

  r = 1;
  p = a;
  while ( b != 0 ) {
    if ( b & 0x1 ) r *= p;
    p *= p;
    b = b >> 1;
  }

  return r;
}
