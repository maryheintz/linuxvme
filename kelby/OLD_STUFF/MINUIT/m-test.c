/* Simple optimization example. See README for details. */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cfortran.h"
#include "minuitfcn.h"
#include "minuit.h"

#define MAXLINE 256

typedef struct {
  char *name;
  double value;
  double error;
  double min;
  double max;
} par_t;

par_t pars[] = {
  { "x", 0., 1., 0., 0. },
  { "y", 0., 1., 0., 0. },
  { "z", 0., 1., 0., 0. },
};

void fcn (int npar, double* grad, double* fcnval, double* xval, int iflag, void* futil)
{
  *fcnval = xval[0] * xval[0] + xval[1] * xval[1] + xval[2] * xval[2];
}

int main (int argc, char *argv[])
{
  int i, dummy;
  char *command = NULL;

  command = (char *) malloc (MAXLINE * sizeof (char));

  MNINIT (5,6,7);

  for (i = 0; i < 3; i++)
    MNPARM (i+1, pars[i].name, pars[i].value, pars[i].error, pars[i].min, pars[i].max, dummy);

  snprintf (command, MAXLINE, "MIGRAD");
  MNCOMD (minuitfcn, command, dummy, NULL);

  exit (0);
}
