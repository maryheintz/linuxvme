#ifndef STD_FUNC_H_
#define STD_FUNC_H_

/* Definitions of standard operating system functions */

#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <proto.h>
#include <types.h>
#include <stat.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>

void usleep(time_t useconds);
/*
void bzero(char *dst, int len);
char *_mkdata_module();
int munload();
int creat();
int write();
long atol();
int readln();
int system(char *command);
int open();
int read();
int fflush(FILE *fp);
int fread(char * buffer, int size, int nobjs, FILE * fp);
int fclose(FILE *fp);
int fseek(FILE *fp, long offset, int place);
int fwrite(char *buffer, int size, int nobjs, FILE *fp);
int fputs(char * string, FILE *fp);
int fgetc(FILE *fp);
int _errmsg();
int printf();
int toupper();
int _get_process_desc();
int _get_process_table();

int kill();
int getpid();
int unlink();
int _ev_link();
int _ev_unlink();
int _ev_delete();
int close();
int tsleep();
double fabs(double x);

char * getenv(char * name);
int wait(unsigned *status);

void exit(int code);
int fprintf(); 
*/

#endif
