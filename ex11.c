#include <stdio.h>

int main(int argc, char *argv[]){
  // go through each string in argv

  //let's make our own array of strings

  char *states[] = {
    "California", "Oregon",
    "Washington", "Texas"
  };
  int num_states = 4;

  int i=0;
  for(i=0; i < argc; i++){
    if(i < num_states){
      states[i] = argv[i];
    }
  }

  i=0;  //watch for this
  while(i < num_states){
    printf("state %d: %s\n", i, states[i]);
    i++;
  }

  return 0;
}
