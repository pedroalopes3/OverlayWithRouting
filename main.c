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
    char* IP;  
    // 192.168.55.34
    char* TCP;   
    char* regIP;  
    char* regUDP;  
    

    printf("\n|||||||||||||||||||||||||||||||||||||||||||||");  
    printf("\n||||        OverlayWithRouting           ||||");
    printf("\n|||||||||||||||||||||||||||||||||||||||||||||");                                 
    printf("\n\n       usage: OWR IP TCP regIP regUDP      \n\n");

    if(argc < 3){
        printf("\n Please provide all the necessary inputs");
        return 1;
    }else {
        if(argc <= 4){
        
            IP = (char *) malloc((strlen(argv[1])+1)*sizeof(char));
            strcpy(IP, argv[1]);
            TCP = (char *) malloc((strlen(argv[2])+1)*sizeof(char));
            strcpy(TCP, argv[2]);
            regIP = (char *) malloc((strlen("193.136.138.142")+1)*sizeof(char));
            strcpy(regIP,"193.136.138.142");
            regUDP = (char *) malloc((strlen("59000")+1)*sizeof(char));
            strcpy(regUDP,"59000");

        }else{

            IP = (char *) malloc((strlen(argv[1])+1)*sizeof(char));
            strcpy(IP, argv[1]);
            TCP = (char *) malloc((strlen(argv[2])+1)*sizeof(char));
            strcpy(TCP, argv[2]);
            regIP = (char *) malloc((strlen(argv[3])+1)*sizeof(char));
            strcpy(regIP,argv[3]);
            regUDP = (char *) malloc((strlen(argv[4])+1)*sizeof(char));
            strcpy(regUDP,argv[4]);
        } 

    }
    
    printf("\nlen: %d, IP: %s, TCP: %s, regIP: %s, regUDP: %s\n",argc, IP, TCP, regIP, regUDP);
    
    free(IP);
    free(TCP);
    free(regIP);
    free(regUDP);

    return 0;
}