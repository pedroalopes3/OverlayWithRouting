#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define MAX_N_NET 999
#define MAX_N_ID 99


int main(int argc, char *argv[])
{

    // Meu pc
    char IP[] = "192.168.55.34";
    char TCP[] = "58000";  
    char regIP[] = "193.136.138.142";
    char regUDP[] = "59000";

    printf("|||||||||||||||||||||||||||||||||||||||||||||");  
    printf("\n||||        OverlayWithRouting        ||||");
    printf("\n|||||||||||||||||||||||||||||||||||||||||||||");                                 
    printf("\n\n       usage: OWR IP TCP regIP regUDP      \n\n");

    if(argc < 3){
        printf("\n Please provide all the necessary inputs");
        return 1;
    }else {}





}