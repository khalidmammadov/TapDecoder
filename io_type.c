#include <stdio.h>
#include <math.h>
#include "io_lib.c"


// ***** GLOSSARY *****************
//  io  identifier octet
//  ib  input byte
//  pc  primitive constructed
// ***** END *****************






main () {

 BYTE n, f;

 if ( (n= get_pc(52)) == PRIMITIVE )
   printf("prim res. is %d\n", n);
 else 
   printf("constructed res. is%d\n", n);

 if ( (n = get_io_class(129)) == APPLICATION) 
   printf("APPLICATION res. is %d\n", n);
 else 
   printf("OTHER res. is%d\n", n);
   
 if ( (n = get_io_rest(95)) == 2) 
   printf("res. is %d\n", n);
 else 
   printf("res. is%d\n", n);

 if ( (n = get_length_type(129)) == _SHORT) 
   printf("Length is short%d\n", n);
 else 
   printf("Length is long%d\n", n);

 f = 5;

 if ( get_length_type(f) == _SHORT )
  printf("Length is: %d\n", get_lengths_length(f));
 else 
   printf("Length is long%d\n", n);

 f = 10;

 f = f>>1;

 printf("f:%d \n", f)

 printf("Debug:It works!!!\n");  

 return 0;

}



