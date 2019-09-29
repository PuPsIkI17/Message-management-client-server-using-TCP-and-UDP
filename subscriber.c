// Pislari Vadim 323CB TEMA 2 PC

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <math.h>
#include <netinet/tcp.h>

#include "utils.h"


int main(int argc, char *argv[]){
    
    // se verifica numarul de parametri
    if (argc != 4) {
       error("Usage ./subscriber <ID_Client> <IP_Server> <Port_Server>");
    }

    //se verifica ca id-ul sa nu fie mai mare ca 10
    if(strlen(argv[1])>10){
        error("lungimea maxima a ID-ului este 10");
    }
    char id_client[ID_LEN];
    strcpy(id_client,argv[1]);

    // se deschide socketrul tcp
    int tcpsock = socket(AF_INET, SOCK_STREAM, 0);
    if (tcpsock < 0){
        error("Eroare la deschiderea socketului");
    }

    int yes = 0;
    int result = setsockopt(tcpsock, IPPROTO_TCP, TCP_NODELAY, (char *) &yes, sizeof(int));   
    if (result < 0){
        error("TCP_NODELAY eroare");
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[3]));
    inet_aton(argv[2], &serv_addr.sin_addr);

    // conectarea cu serverul
    if (connect(tcpsock,(struct sockaddr*) &serv_addr,sizeof(serv_addr)) < 0){
        error("Eroare la connect");
    }
    else{
        int res = send(tcpsock,id_client,strlen(id_client), 0);
        if (res < 0){ 
            error("Eroare la transmiterea id-ului");
        }
    }

    // multime de citere, si multime temporara utilizate pentru select
    fd_set read_fds, tmp_fds;
    FD_ZERO(&read_fds);
    FD_ZERO(&tmp_fds);
    FD_SET(0, &read_fds);
    FD_SET(tcpsock, &read_fds);

    int fdmax = tcpsock,j;
    char buffer[BUFLEN];

    while(1) {
        tmp_fds = read_fds;
		if (select(fdmax + 1, &tmp_fds, NULL, NULL, NULL) == -1)
			error("Eroare la select");
            
        for(int i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &tmp_fds)) {

                if (i == tcpsock) {

                    // daca a fost primit un mesaj de la server
                    Message msg;
                    int res = recv(tcpsock, (char*)&msg, sizeof(Message), 0);

                    if (res == 0){
                        printf("Fie ca s-a oprit serverul, sau ati incercat sa creati un client cu un nume existent\n");
                        exit(1);
                    }

                    if (res > 0) {

                        // prelucrarea mesajului primit
                        char topic[TPC_LEN];
                        strcpy(topic, msg.buffer);
                        uint8_t tip = (uint32_t) *(msg.buffer + 50);
                        
                        // in functie de tipul mesajului STRING INT... se determina mesaul 
                        if(tip == 0){

                            // daca e INT
                            uint8_t semn = (uint8_t) *(msg.buffer + 51);
                            uint8_t b0,b1,b2,b3;
                            b0 = msg.buffer[55];
                            b1 = msg.buffer[54];
                            b2 = msg.buffer[53];
                            b3 = msg.buffer[52];
                            uint8_t arr[4] ={b0,b1,b2,b3};
                            uint32_t res = *((uint32_t*) arr);
                            int fin;
                            
                            if(semn == 1) {
                                fin = -res;
                            }else {
                                fin = res;
                            }

                            printf("%s:%d - %s - INT - %d\n",msg.ip,msg.port,topic, fin);
                        
                        }else if(tip == 1){
                            
                            // daca e SHORT_REAL
                            uint8_t b0,b1;
                            b0 = msg.buffer[52];
                            b1 = msg.buffer[51];
                            uint8_t arr[2] ={b0,b1};
                            uint16_t res = *((uint16_t*) arr);
                            float fin = (float)res/100; 

                            printf("%s:%d - %s - SHORT_REAL - %.2f\n",msg.ip,msg.port,topic,fin);
                        
                        }else if (tip == 2){

                            //daca e FLOAT
                            uint8_t semn = (uint8_t) *(msg.buffer + 51);
                            uint8_t b0,b1,b2,b3;
                            b0 = msg.buffer[55];
                            b1 = msg.buffer[54];
                            b2 = msg.buffer[53];
                            b3 = msg.buffer[52];
                            uint8_t arr[4] ={b0,b1,b2,b3};
                            uint32_t res = *((uint32_t*) arr);
                            uint8_t puterea = msg.buffer[56]; 
                            int la_put = 1;
                            for(j = 0; j != puterea; j++){
                                la_put*=10;
                            }

                            float fin = (float)res / la_put;
                            if(semn == 1) {
                                fin = -fin;
                            }
                            fin = roundf(fin * la_put) / la_put;

                            printf("%s:%d - %s - FLOAT - %.4f\n",msg.ip,msg.port,topic,fin);
                        }else if (tip == 3){

                            //daca e STRING
                            printf("%s:%d - %s - STRING - %s\n",msg.ip,msg.port,topic, msg.buffer + 51);
                        }
                    }
                } else {

              		// citesc de la tastatura
                	memset(buffer, 0 , BUFLEN);
                	fgets(buffer, BUFLEN-1, stdin);
                    int len = strlen(buffer)-1;
                    
                    // nu merge cu strcmp read_buffer e alocat
                    if(len == 4 && buffer[0] =='e' && buffer[1] == 'x' && buffer[2] == 'i' && buffer[3] =='t') exit(0);
                    char *command = malloc(sizeof(BUFLEN));
                    j=0;
                    int flag = 0;

                    // prelucrarea instructiunii de la tastatura
                    // determinarea comenzii subscribe sau unsubscribe
                    while(buffer[j] != ' '){
                        if(j >= len) {
                            flag = 1;
                            break;
                        }
                        command[j] = buffer[j];
                        j++;
                    }

                    while(buffer[j] == ' '){
                        if(j >= len) {
                            flag = 1;
                            break;
                        }
                        j++;
                    }
                    if(j == len) flag = 1;

                    // daca instructiunea data nu e corecta
                    if (flag == 1 ){
                        printf("Singurele instructiuni admise sunt: \"subscribe topic SF\", \"unsubscribe topic\",\"exit\"\n");
                        continue;
                    }

                    Subscribe sub;
                    
                    // daca instructiunea e subscribe
                    if (strcmp(command,"subscribe") == 0){
                        
                        // se determina topicul
                        char *topic = malloc(sizeof(BUFLEN));
                        int k = 0;
                        while(buffer[j] != ' '){
                            if(j >= len) {
                                flag = 1;
                                break;
                            }
                            topic[k] = buffer[j];
                            j++; k++; 
                        }
             
                        while(buffer[j] == ' '){
                            if(j >= len) {
                                flag = 1;
                                break;
                            }
                            j++;
                        }
                        if(j == len) flag = 1;

                        // se determina sf-ul
                        char sf = buffer[j];
                        j++;
                        
                        while(j < len){
                            if(buffer[j] != ' ')flag = 1;
                            j++;
                        }

                        // daca instructiunea nu e corecta
                        if (flag == 1 ){
                            printf("Singurele instructiuni admise sunt: \"subscribe topic SF\", \"unsubscribe topic\",\"exit\"\n");
                            continue;
                        }

                        // daca sf-ul e diferit de 0 sau 1
                        if( sf != '0' && sf != '1'){
                            printf ("sf poate fi 1 sau 0\n");
                            continue;
                        }

                        sub.tip = 0;
                        strcpy(sub.topic, topic);
                        sub.sf = sf;

                        //trimit mesaj la server cun instructiunea subscrie
                        int res = send(tcpsock,(char *)&sub, sizeof(Subscribe), 0);
                        if (res < 0){
                            error("Eroare la transmitere");
                        }

                        // se asteapta mesaj de confirmare daca a fost realizat cu suces subscribe- ul
                        flag = 0;
                        int tran = recv(tcpsock, (char*)&flag, sizeof(flag), 0);
                        if(tran < 0) {
                            error("Eroare send");
                        }
                        if(flag == 0){
                            printf("subscribed topic\n");
                        }
                        else{
                            printf("sunteti deja abonat la acest topic\n");
                        }

                    }else if(strcmp(command,"unsubscribe") == 0){
                        
                        // daca s-a realizat instructiunea unsubscribe 
                        char *topic = malloc(sizeof(BUFLEN));
                        int k = 0;

                        // se determina topicul
                        while(buffer[j] != ' ' ){
                            if(j >= len) {
                                break;
                            }
                            
                            topic[k] = buffer[j];
                            j++; k++; 
                        }
                   
                        while(j < len){
                            if(buffer[j] != ' ')flag = 1;
                            j++;
                        }

                        if (flag == 1 ){
                            printf("Singurele instructiuni admise sunt: \"subscribe topic SF\", \"unsubscribe topic\",\"exit\"\n");
                            continue;
                        }
      
                        sub.tip = 1;
                        strcpy(sub.topic, topic);

                        //trimit mesaj la server
                        int res = send(tcpsock,(char *)&sub, sizeof(Subscribe), 0);
                        if (res < 0){
                            error("Eroare la transmitere");
                        }  
                            
                        // se asteapta mesaj de confirmare daca unsubscribe a fost realizat cu succes
                        int tran = recv(tcpsock, (char*)&flag, sizeof(flag), 0);
                        
                        if(tran < 0) {
                            error("Eroare send");
                        }
                        if (flag == 1){
                            printf("unsubscribed topic\n");
                        }
                        else{
                            printf("clientul nu este abonat la acest topic");   
                        }

                    }else{

                        //daca a fost introdusa o instructiune invalida
                        printf("Singurele instructiuni admise sunt: \"subscribe topic SF\", \"unsubscribe topic\",\"exit\"\n");
                        continue;
                    }
                }
            }
        }
    }
    return 0;
}
