#define _SHORT  0
#define _LONG   1

#define PRIMITIVE    0
#define CONSTRUCTED  1

#define UNIVERSAL    0
#define APPLICATION  1
#define CONTEXT_SPEC 2
#define PRIVATE      3


//type "short" was changed from unsigned char due to TD57's tag definition is more than 255. 
typedef short BYTE;

//*******NOTE **************
//CHAR is used due to it can hold only one byte 
//*******END ***************

/*
  ***********CHECK BIT*********************
  this function checks the bit of some octet 
  For example: if 3 bit of 8 is 1 or 0

  int a = 8;
  int b = 3;

  printf("%d \n", check_bit(8,3));

  // 8 = 1000  
  // 3 = 0100
  //result is 0 

*/

BYTE check_bit( BYTE num, BYTE bit_num ) {
  
  BYTE a = pow(2, bit_num -1);
 // printf("Debug::%d  %d %d\n", num, bit_num, a);
  if ( ( num & a ) > 0 )  
    return 1; 
  else 
    return 0; 

}



//  get_pc function checks if octet is primitive of constracted
//  it checks bit 6 of the octet wich determine if it is prim. or constracted.
BYTE get_pc( BYTE ib) {

  BYTE b = check_bit(ib, 6);

  if ( b == 0 )
     return PRIMITIVE;
  else
     return CONSTRUCTED;

}


//checks class of the io 
BYTE get_io_class( BYTE ib ) {

  if (( ib & 128) == 128 ) return CONTEXT_SPEC; //10
  if (( ib & 192) == 192 ) return PRIVATE;      //11 
  if (( ib & 64 ) == 64  ) return APPLICATION;  //01
  if (( ib & 0  ) == 0   ) return UNIVERSAL;    //00

}



//get rest of io after excluding class and pc
//eg. if 01001001
//it returns last 5 bits
//res. is    1001
BYTE get_io_rest( BYTE ib ) {
 
  return (ib & 31);

}


BYTE get_length_type (BYTE ib) {

  return (check_bit(ib, 8)== _SHORT )? _SHORT:_LONG;

}


//is used to 
//  1) retrive value of Long io
//  2) retrive short length of octets
BYTE get_first_seven (BYTE ib) {

  return (ib & 127);

}



