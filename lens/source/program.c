#include <stdlib.h>
#include <stdio.h>
int main(){
 int size = 16;
 char *ptr = malloc(size * sizeof(char));
 printf("virtual addr of size variable: %p\n", &size);
 printf("virtual addr of malloc'd memory: %p\n", ptr);
 printf("virtual addr of main() function: %p\n", main);
 free(ptr);
 return 0;
 }
