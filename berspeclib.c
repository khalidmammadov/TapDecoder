#define LINE_LENGTH 1024;

struct asn_spec { 
                 char name[128];
                 char id[128];
                 char type[128];
                };

struct asn_spec record;


char bufl[1024];

int find_tag_line(char *fname, char *tag_str, int tag_length, char *input_val);
void get_tag_name(char *line, char *name);
struct asn_spec parse_line(char *line);

int fetch_line(FILE *fp, char *output_val);
int get_type(FILE *fp, char *tag_str);
int check_type(char *str);


// it looks into specification file and looks for the tag
//and retrives only name eg. first word in the line
int get_tag_by_name(char *spec_file, int tag_id, char *tag_name ) {

  char line[1024];
  char TAG_STR[128];
  int len, res = 0;

  len = sprintf(TAG_STR, "[APPLICATION %d]", tag_id);

  //printf("%s %d\n",TAG_STR, len);

  res = find_tag_line(spec_file, TAG_STR, len-1, line );
  
  if (res) get_tag_name(line, tag_name);
  
  return res;
}

// it looks for tag and when it found then it retrives it's line.
// "fname"--look into this file
// "tag_str" -- search for this word
// "tag_length" -- word length (length of the tag)
// "input_val" -- found line (RESULT, out value)
// return = 1 if it found the tag
int find_tag_line(char *fname, char *tag_str, int tag_length, char *input_val) {

FILE *fp;
short buf;

int found_flag = 0;
int l = 0, l_id = 0;

   fp = fopen(fname, "r");

   while ((buf = getc(fp)) != EOF) { 

    input_val[l_id] = buf;
    l_id++;
    if ( buf == '\n' ) {
       input_val[l_id] ='\0';
       l_id = 0;
   
         if (found_flag == 1 ) 
            break;
    }
 
 
    if ( buf == tag_str[l] )
       ++l;
     else l = 0;

     if ( l == tag_length ) 
          found_flag = 1;  

   }
   
   close(fp);


   return found_flag;
}

///This function retrives first word in the line
///input line is "line", return value is "name"
void get_tag_name(char *line, char *name) {

   while (*line != '\0') {

     if ( *line != ' ' )
        *name = *line;
     else break;

     line++;
     name++;
 
   }
   *name = '\0';

}



/////////////////////////////////////////////
//  Separates below string into 3 pieces
//  PlmnId ::= [APPLICATION 169] AsciiString
//  
//  1) PlmnId
//  2) [APPLICATION169]  not !!! [APPLICATION 169]
//  3) AsciiString
/////////////////////////////////////////////
struct asn_spec parse_line(char *line) {

   struct asn_spec temp;
   int element = 1, i=0;
   //printf("%s\n", line);


   while (*line != '\0') {

//     printf("Char is:%c %d  \n", *line, element);
     switch (*line) {
         case   '[':
                    element=2;                                        
                    i=0;
                    break;
         case   ']':
                    element=3;        
                    temp.id[i] = ']';            
                    temp.id[i+1] = '\0';
                    i=0;
                    line++;
 		    continue;
         case   '-':element = 4;
         case   ' ':
         case   '=':
         case   ':':
         case   '\n':if (element==1) {
                                      element=3; 
                                      temp.name[i] = '\0';i=0;}
                     line++;
                     continue;
     }
	

     
     switch (element) {
        case 1: temp.name[i] = *line;
        case 2: temp.id[i] = *line;
        case 3: {temp.type[i] = *line; temp.type[i+1]='\0';}
     }


     if (element == 3 && check_type(temp.type) > 0 )  break;

     i++;
     line++;
   }
   //temp.type[i] = '\0'; 

//   printf("\n%s\n", temp.name);
//   printf("%s\n", temp.id);
//   printf("%s\n", temp.type);

  return temp;

}

//Read file and get one line
// 0 --there is not lines left
// 1 --line is found
int fetch_line(FILE *fp, char *output_val) {

   int   res = 0;
   short l_id = 0, buf;

   
   while ((buf = getc(fp)) != EOF) { 
 
        output_val[l_id] = buf;

        if ( buf == '\n') {
           output_val[l_id] ='\0';       
           res = 1;
           break;
        }
        l_id++;
   }
   
   return res;

}


//Checks type 
// if it is 
// INTEGER      = 2
// OCTETSTRING  = 1
// SEQUENCE     = 3
// BCDString    = 4
// or UNKNOWN   = 0
int get_type(FILE *fp, char *tag_str) {

   int res = 0;
   struct asn_spec rec_tmp;



   //Move to the beginnig of the file
   fseek(fp, 0L, 0 );

   //printf("::CHECK::Looking for::%s\n", tag_str);

   while (fetch_line(fp, bufl)) {

        //printf("::CHECK2::Looking for::%s\n", tag_str);
	rec_tmp = parse_line(bufl);
        //printf("I am looking for::%s\n", tag_str);
        //printf("Found line:(%s) (%s) %d %d\n", rec_tmp.name, tag_str, sizeof(rec_tmp.name), sizeof(tag_str) );
      if ( strcmp(rec_tmp.name, tag_str) == 0 ) {
        // printf("Found line:%s\n", bufl);
        // printf("Found type:%s\n", rec_tmp.type);
         
         if ( (res = get_type(fp, rec_tmp.type)) >0 
                || (res = check_type (rec_tmp.type)) > 0   ) 
            break;
        
      }      
   }

   return res;

}


int check_type(char *str) {

  int res = 0;

  if  (strcmp(str, "OCTETSTRING") == 0) 
        res = 1;
  else if  (strcmp(str, "INTEGER") == 0) 
        res = 2;
  else if  (strcmp(str, "SEQUENCE") == 0 || (strcmp(str, "SEQUENCEOF") == 0)) 
        res = 3;
  else if  (strcmp(str, "BCDString") == 0) 
        res = 4;

  return res;  

}


void get_tag_spec(char *fname, short tag_id, char *spec_name, int *val_type) {

   FILE *fp;
   char TAG_STR[128];

   fp = fopen(fname, "r");
   *val_type = 0;
   sprintf(TAG_STR, "[APPLICATION%d]", tag_id);
   

   while (fetch_line(fp, bufl)) {

      record = parse_line(bufl);

      if ( strcmp(record.id, TAG_STR) == 0 ) {
        /*  printf("Found line:%s\n", bufl);
	  printf("Tag name is::%s\n", record.name);
  	  printf("Tag id is::%s\n", record.id);
  	  printf("Tag type is::%s\n", record.type);
  	  printf("==============================\n");*/
          
          strcpy(spec_name, record.name);
          
          if ((*val_type = check_type(record.type))>0 )  
            break;
          if ((*val_type = get_type(fp, record.type)) > 0)
            break;
      }      

   }

//   printf("\"%d\"", *val_type);

   close(fp);



  

}

