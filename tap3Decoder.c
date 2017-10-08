#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "io_lib.c"
#include "berspeclib.c"

FILE *fp;
short buf, last_val=-1;
short cnt =0;

short get_tag(void);
short get_io_long(void);
int get_long_length(BYTE octet_num);
int get_next_val(void);
int printstringval (int );
int printintval (int );
void printBCDString (int );
int body(char *spec_file_name, int depth, int seq_length);


main (int argc, char *argv[]) {

//===============INPUT FILE CHECK============
//if there is not nothig then exit
  if ( argc > 1 ) 
    printf ("Input files is:  %s\n", argv[1]);
  else {
        printf ("There is not input file.\n");
        return 0;
  }
//===============END CHECK===================


//===========Open input file=================
  if ((fp = fopen(argv[1], "r")) == NULL) {

     printf("Error::Can't open a file.");
     return 0;
  }
//===========================================


  body(argv[2], 0, 0);


  fclose(fp);

  return 0;

}

void print_tab(short tab_count) {

  while ( tab_count-- > 0 ) {
    
    printf ("  ");
   
  }

}


int body(char *spec_file_name, int depth, int seq_length) {

  short tag_, value_length;
  BYTE  tag_type;
  char  tag_name[128];
  int   val_type, i, cn =0;

//============Read the file in loop==========  
  while ( cnt != (cn = get_next_val()) ) {
//  printf("Class check for:%d\n", buf);
    if ( get_io_class(buf) == APPLICATION ) {

        tag_type = get_pc(buf);          
        tag_ = get_tag();

        //=========Get value tag_name and it's type==============
        get_tag_spec(spec_file_name, tag_, tag_name, &val_type);

        //print_tab(depth);printf("%s %d %d =", tag_name, value_length, val_type);
        print_tab(depth);printf("%s ", tag_name);

    }

    //===Retrive length of value==============
    if ( cnt != (cn = get_next_val()) )
        if ( get_length_type(buf) == _SHORT ) 
          value_length = (short)get_first_seven(buf);
        else
          value_length = get_long_length(get_first_seven(buf));
    //printf("Length =%d\n", value_length);
    //======================================== 

    //print_tab(depth);printf("(%3d) %s %d %d %d %d=", cnt, tag_name, tag_, value_length, val_type, depth);



    switch (val_type) {
      case 1: printstringval (value_length);break;
      case 2: printintval(value_length);break;
      case 3: printf("{\n");
              body(spec_file_name, depth+1, cnt+value_length);
              break;
      case 4: printBCDString(value_length);
     // default : printf("unknown type");
    }

    //printf("seq_length=%d cnt=%d\n", seq_length, cnt);
   
    if ( seq_length == cnt ) {

        print_tab(depth-1);
        printf("}\n");
        break;
    
    }

    if (last_val == 0 && buf == 0) {

        print_tab(depth-1);
        printf("}\n");
        
    
    }

   tag_=0;
   val_type=0;
   tag_name[0] = '\0';


  }
//======================End reading=====================



}


void getBCDString(BYTE input_val, BYTE *first_val, BYTE *next_val) {

   *first_val = input_val>>4;
   *next_val  = input_val&15;

}

void printBCDString (int value_length) {

  BYTE first_octet, next_octet;
  int cn = 0;

  while (value_length-- > 0)
    if ( cnt != (cn = get_next_val())) {
           getBCDString(buf, &first_octet, &next_octet);
           
           printf("%d", first_octet);
           if (next_octet!= 0xf) 
               printf("%d", next_octet);
    } 
  printf("\n");

}


int printstringval (int value_length) {

  int i, cn = 0;
  char  *value;

  value = (char *)malloc(value_length);

  for (i=0; ( value_length > i ); ++i) {

       if ( cnt != (cn = get_next_val()))
           value[i] = buf;

  }; value[i] = '\0';

  printf("%s\n", value );

  free(value);
  

}

int printintval (int value_length) {

int val = 0 , cn = 0;

  while ( value_length-- > 0)    
    if ( cnt != (cn = get_next_val()) ) {  

       if ( val != 0  ) val=val<<8;
       val = val+buf;              
  }

  printf("%d\n", val);

}




//Determine the type of the tag
//and retrive it in INTEGER type
short get_tag(void) {

  char t;

  if ( (t = get_io_rest(buf)) != 31 ) {
    // printf("Tag is short::%d\n", t);
     return t;
  }
  else return get_io_long();

}


//If io is long then assemble it and retrive.
short get_io_long (void) {

  int  tval, val =0, cn = 0;

    do {
      
      if ( cnt == (cn = get_next_val())) { printf("There is some error::\n");exit(0);};

      if ( val != 0 ) val=val<<7;

      tval = get_first_seven(buf);     

      val = val+tval;

    } while ( check_bit(buf, 8) == 1 );

    //printf("Tag is long::%d\n", val);
    return val;

}

//Read next BYTE into buf from the file.
int get_next_val(void) {

  last_val = buf;

  buf = getc(fp);

//  printf("Input val is:%d\n", buf);

  if (buf != EOF) return ++cnt;
  else return cnt;

}


//If type is long then assemble it and retrive.
int get_long_length(BYTE octet_num) {

  int  i, tval, val =0, cn = 0;

  for (i = 1; octet_num>= i; ++i) {

      if (cnt == (cn = get_next_val()) ) { printf("There is some error::\n");exit(0);};

      if ( val != 0 ) val=val<<8;

      tval = buf;     

      val = val+tval;
         
     
  }

  return val;




}









