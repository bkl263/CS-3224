#include "types.h"
#include "user.h"
char buf[512];
char* prevbuf;
char* temp;
char* linesChanged;

void printline(int index) {
  int endInd;
  endInd = sizeof(prevbuf);

  while (!strchr("\n", buf[index]) && index >= 0) {
    --index;
  }
  if (index < 0) {
    while (!strchr("\n", prevbuf[endInd + index]) && (endInd + index) > 0) {
      --index;
    }
  }
  index++;
  while (!strchr("\n", buf[index])) {
    if (index < 0) { printf(1, "%c", prevbuf[endInd + index]); }
    else { printf(1, "%c", buf[index]); }
    ++index;
  }
  printf(1,"\n");
}

void findAndReplace(int fd, char find[], char replace[]) {

  int chrSize = sizeof(replace);

  if (find[0] != '-' || replace[0] != '-') {
    printf(1, "findAndReplace: syntax error needs '-' before find and replace arguements\n");
    exit();
  }

  int bytesREAD, i, j, findCount, inword, atInd, timesREAD;

  timesREAD = inword = findCount = bytesREAD = 0;
  atInd = 1;
  prevbuf = (char *) malloc(1);
  linesChanged = (char *) malloc(1);

  while((bytesREAD = read(fd, buf, sizeof(buf))) > 0) {
    timesREAD++;
    for (i = 0; i<bytesREAD; i++) {
      if (inword && buf[i] != find[atInd]) {
        atInd = 1;
        inword = 0;
      }
      if(atInd == (chrSize - 1) && inword) {
        findCount++;
        for(j = (chrSize - 1); j >= 1; j--) {
          buf[i] = replace[j];
          i--;
        }
        printline(i);
        atInd = 1;
        inword = 0;
      }
      if(buf[i] == find[atInd]) {
        atInd++;
        inword = 1;
      }
      if(strchr(" \r\t\n\v", buf[i])) {
        inword = 0;
        atInd = 1;
      }
    }
    if (bytesREAD < 0) {
      printf(1, "findAndReplace: read error\n");
      exit();
    }

    temp = prevbuf;
    prevbuf = (char *) malloc(512 * timesREAD);
    strcpy(prevbuf, temp);

    for (i = 0; i < 512; i++) {
      prevbuf[i * timesREAD] = buf[i];
    }
  }
  printf(1,"Found and Replaced %d occurances\n", findCount);
  free(prevbuf);
}

int main(int argc, char *argv[]) {
  int fd, i;

  if(argc <= 1) {
    findAndReplace(0, "-the", "-xyz");
  }

  else if (argc == 4) {
    if ((fd = open(argv[3], 0)) < 0) {
      printf(1, "sed: cannot open %s\n", argv[3]);
      exit();
    }
    findAndReplace(fd, argv[1], argv[2]);
  }
  else {
    for(i = 1; i < argc; i++) {
      if((fd = open(argv[i], 0)) < 0) {
        printf(1, "sed: cannot open %s\n", argv[i]);
        exit();
      }
      findAndReplace(fd, "-the", "-xyz");
    }
  }
  close(fd);
  exit();
}
