/* Filename: cmdline.h 
   Author: M.W. Mak
   Date: 12 Dec 95
   Version: 1.0
   Description: definition of CLINEPARA structure for getting command lin
                arguments 
*/


#ifndef __CMDLINE_INCLUDED__
#define __CMDLINE_INCLUDED__

/* command line parameters */
typedef struct {
        char *name;             /* full name of the parameter */
	char *sname;            /* short name of the parameter */
        char **parameter;       /* pointer to a string storing that parameter */
} CLINEPARA;

void usage(char *progname, int num_options, CLINEPARA *options); 
void get_cmdline(int argc, char *argv[],int num_options, CLINEPARA *option);
float string_to_float(char *str, int pos);

#endif
