/* Filename: cmdline.c
   Author: M.W.Mak
   Date: 12 Dec 95
   Last update: 18 March 96
   Version: 1.0
   Description: extracting parameters from the command line argument
*/

#include "cmdline.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void usage(char *progname, int num_options, CLINEPARA *options)
{
     int j;
     
     fprintf(stderr,"Usage: %s [PNAME PVALUE] [PNAME PVALUE] ... \n",progname);     
     fprintf(stderr,"\t%-8s%-42s%-s\n","Option","Parameter meaning","Default value");
     fprintf(stderr,"\t---------------------------------------------------------------\n");
     for (j=0;j<num_options;j++) {
         if (*options[j].parameter!=NULL) 
             fprintf(stderr,"\t%-8s%-42s%-s\n",options[j].sname,&options[j].name[1],
	 	                                 *options[j].parameter);
         else
             fprintf(stderr,"\t%-8s%-42s%-s\n",options[j].sname,&options[j].name[1],
	 	                                 "NULL");
     }
     fprintf(stderr,"\n");
     exit(EXIT_FAILURE);
}     

void get_cmdline(int argc, char *argv[],int num_options, CLINEPARA *options)
{
    int i,j;
    /*-------------------------------------------------------------
      Parse command line. Each of the options require a value. So
      we process the command line arguments in pairs
    -------------------------------------------------------------*/  
    for (i=1;i<argc;i+=2){
	if (strcmp("--help",argv[i])==0)
	    usage(argv[0],num_options,options);	
        for (j=0;j<num_options;j++){
	    if (strcmp(options[j].name,argv[i])==0 ||
		strcmp(options[j].sname,argv[i])==0){
	        *options[j].parameter=argv[i+1];
		break;
	    }
	    /* Search for --help */
	    if (strcmp("--help",argv[i])==0)
		usage(argv[0],num_options,options);	
	}
	if (j>=num_options)
	    usage(argv[0],num_options,options);
    }
}

/*
    Input
       str: string containing floating point number separated by ','
       pos: position of the floating point number needed to convert
    Ouput
       return the floating point number
*/
float string_to_float(char *str, int pos)
{
      char buf[50];
      int  i,j;

      /* Skip pos-1 floating point numbers */
      pos--;	
      i=0;
      while (pos) {
	 while (str[i++]!=',');
	 pos--;
      }	

      /* Store number in buffer */
      j=0;	
      while (str[i]!=',' && str[i]!='\0') {
	   buf[j++]=str[i++];
      }
      buf[j]='\0';	

      return((float)atof(buf));
}
