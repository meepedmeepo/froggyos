#include "atoi.h"

int atoi(char *str) {
  int sign = 1, base = 0, i= 0;

  //Ignore whitespace.
  while(str[i] == ' ') i++;

  if (str[i] == '-' || str[i] == '+') sign = 1 - 2 * (str[i++] == '-');

  

  return 0;
}
