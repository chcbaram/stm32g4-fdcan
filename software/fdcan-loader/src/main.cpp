/*
 * main.c
 *
 *  Created on: Dec 6, 2020
 *      Author: baram
 */


#include "main.h"



void exitISR(int sig)
{
  apExit();
}

int main(int argc, char *argv[])
{
  signal(SIGINT, exitISR);
  setbuf(stdout, NULL);

  hwInit();
  apInit();

  apMain(argc, argv);

  return 0;
}

