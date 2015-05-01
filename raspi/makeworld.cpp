#include <iostream>
#include <cstdlib> 
#include <stdint.h>
#include "makeworld.h"
#include <string.h>

using namespace std;

int main(int argc, char *argv[]){
  if(argc < 2){
	cout << "Usage: ./makeworld <OUTPUT> \n";
	return 0;
  }
  FILE * pFile;
  pFile = fopen (strcat(argv[1],".world"), "wb");
  fprintf(pFile, "%7lu", sizeof(WORLD_MAP));
  fprintf(pFile, "%7lu", WORLD_W); //W
  fprintf(pFile, "%7lu", WORLD_H); //H
  fwrite (WORLD_MAP , 1, sizeof(WORLD_MAP), pFile);
  fclose (pFile);
  return 0;
}


