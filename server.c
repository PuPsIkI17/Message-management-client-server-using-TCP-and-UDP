// Pislari Vadim 323CB TEMA 2 PC

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h> 
#include <netinet/tcp.h>

#include "utils.h"


void sig_action(int sig_number){
  if (sig_number == SIGINT) {
    exit(1);
  }
}

// functie care depisteaza ca a fost aplicat ctrl + c
int ver_signals(){
  struct sigaction sig;
  sig.sa_handler = sig_action;

  if (sigaction(SIGINT, &sig, 0) != 0) {
    return -1;
  }
  return 0;
}

// citirea in cazul in care se da exit
int read_inst(){
  char *read_buffer = malloc(BUFLEN * sizeof(char));   
  ssize_t len = read(STDIN_FILENO, read_buffer, BUFLEN);

  // nu merge cu strcmp, read_buffer e alocat
  if(len == 5 && read_buffer[0] =='e' && read_buffer[1] == 'x' && read_buffer[2] == 'i' && read_buffer[3] =='t') return 1;
  printf("Singura instructiune admisa este exit\n");
  return 0;
}


int main(int argc, char *argv[]){

    // verifiarea numaruilui de parametri
    if (argc != 2) {
        error("Usage ./server <PORT_DORIT>");
    }

    // in cazul in care se da ctrl+c la server
    if (ver_signals() != 0){
        exit(EXIT_FAILURE);
    }


    // deschiderea socketului tcp
    int tcpsock = socket(AF_INET, SOCK_STREAM, 0);
    if (tcpsock < 0){
       error("Eroare la deschiderea socketului");
    }
    int yes = 0;
    int result = setsockopt(tcpsock, IPPROTO_TCP, TCP_NODELAY, (char *) &yes, sizeof(int));   
    if (result < 0){
        error("TCP_NODELAY eroare");
    }
    
    // deschiderea socketului udp
    int udpsock = socket(AF_INET, SOCK_DGRAM, 0);
    if (tcpsock < 0){
       error("Eroare la deschiderea socketului");
    }

    struct sockaddr_in server_ad, cl_addr;
    memset((char *) &server_ad, 0, sizeof(server_ad));
    server_ad.sin_family = AF_INET;

    // se foloseste adresa IP caracteristica masinii
    server_ad.sin_addr.s_addr = INADDR_ANY;	
    server_ad.sin_port = htons(atoi(argv[1]));

    if (bind(tcpsock, (struct sockaddr *) &server_ad, sizeof(struct sockaddr)) < 0){
        error("Eroare bind");
    }

    if (bind(udpsock, (struct sockaddr *)&server_ad, sizeof(struct sockaddr)) < 0) {
        error("Eroare bind");
    }

    listen(tcpsock, MAX_CLIENTS);

    //multimea de citire utilizata pentru select
    fd_set read_fds;    

    //multime care este utlizata temporar
    fd_set tmp_fds;     

    //se golesc multimile se de descriptori
    FD_ZERO(&read_fds);
    FD_ZERO(&tmp_fds);

    //se adauga tcpsock si STDIN_FILENO in multimea de descriptori
    FD_SET(tcpsock, &read_fds);
    FD_SET(udpsock, &read_fds);
    FD_SET(STDIN_FILENO, &read_fds);
    
    // valoarea maxima din read_fds
    int fmax = 0;
    if(tcpsock > fmax) fmax = tcpsock;
    if(udpsock > fmax) fmax = udpsock;
    if (STDIN_FILENO > fmax) fmax = STDIN_FILENO;
	char buffer[BUFLEN];

    int i,j;
    // un vector care pentru fiecare socket va pastra id-ul corespunzator
    int dim = MAX_CLIENTS;
    Client *arr = malloc(sizeof(Client) * MAX_CLIENTS);
    for(i = 0; i!=dim; i++){
        arr[i].id = malloc(sizeof(char) * ID_LEN);
        arr[i].nr_abonari = 0;
    }

    // store utilizat pentru memorizarea mesajelor in momentul cand clientul este dezactivat 
    Store store[ST_DIM];
    int stdim = 0;

    while (1) {
        tmp_fds = read_fds;
        int sel = select(fmax + 1, &tmp_fds, NULL, NULL, NULL); 
		
        // verificarea de eroare
        if (sel == -1 || sel == 0){
			error("Eroare select");
        }

        // se verifica daca este in multimea temporara valoare stdin_fileno
        if (FD_ISSET(STDIN_FILENO, &tmp_fds)) {

            //se verifica daca nu a fost introdus la tastatura "exit" 
            int al = read_inst();
            if (al == 1){
                exit(1);
            }
            else {
                FD_CLR(STDIN_FILENO, &tmp_fds);
            }
        }
        
		for(i = 0; i <= fmax; i++) {
			if (FD_ISSET(i, &tmp_fds)) {
                    if (i == tcpsock) {
					// a venit ceva pe socketul inactiv(cel cu listen), o noua conexiune
					
                    unsigned int cllen = sizeof(cl_addr);
					int newtcpsock = accept(tcpsock, (struct sockaddr *)&cl_addr, &cllen);

                    if (newtcpsock == -1) {
						error("Eroare accept");
					}else {

						//adaug noul socket intors de accept() la multimea descriptorilor de citire
						FD_SET(newtcpsock, &read_fds);
						if (newtcpsock > fmax) {
							fmax = newtcpsock;
						}
                        
                        char *id_client = malloc(sizeof(char) * ID_LEN);
                        int n = recv(newtcpsock, id_client, sizeof(id_client), 0);
                        
                        // se verifica daca nu mai exista un client deschis cu un astfel de id
                        int flag = 0;
                        if (id_client == NULL) flag = 1;
                        for (j = 0; j != dim ; j++){
                            if(arr[j].id == NULL) continue;
                            else if( strcmp(id_client,arr[j].id)== 0){
                                flag = 1;
                                break;
                            }
                        }

                        // daca exist un cline deschis cu acelasi id
                        if(flag == 1){ 
                            close(newtcpsock);
                            FD_CLR(newtcpsock, &read_fds);
                        }
                        else if (n <= 0) {
                            printf("Eroare la primirea id-ului");
                            close(newtcpsock);
                            FD_CLR(newtcpsock, &read_fds);
                        } else { 

                            // in caz ca s-a depasit dimensiune vectorului, adica sunt mai multi
                            // clienti decat s-a alocat la inceput se realoca vectorul
                            if(newtcpsock > dim){
                                int aux_dim = dim;
                                dim = newtcpsock +1;
                                Client *aux = realloc(arr,dim * sizeof(Client));                                
                                for(j = 0; j!= dim; j++){
                                    aux[j].id = malloc(sizeof(char) * ID_LEN);
                                    aux[i].nr_abonari = 0;
                                } 
                                for(j = 0; j != aux_dim ; j++){
                                    aux[j].id = arr[j].id;
                                    aux[j].nr_abonari = arr[j].nr_abonari;
                                }
                                arr = aux;
                            }
                            arr[newtcpsock].id = id_client;
                            
                            //daca clientul s-a deschis cu succes
                            printf("New client %s connected from %s:%d.\n", id_client, inet_ntoa(cl_addr.sin_addr), atoi(argv[1]));      
                            
                            // se trimit toate mesajele care s-au pastrat in store la clientul corespunzator
                            int k,l;
                            for(j = 0; j!=stdim; j++){
                                
                                //se verifica egalitatea intre id-ul clientului si id-ul din store
                                if(strcmp(store[j].id,arr[newtcpsock].id) == 0){
                                    for(k =0; k!= store[j].nr;k++){

                                        //se verifica daca clientul are mesaje de primit
                                        if(store[j].message[k].nr_buff > 0){

                                            //se transmit toate mesajele
                                            for(l = 0; l != store[j].message[k].nr_buff; l++){ 
                                                int res = send(newtcpsock, (char*)&store[j].message[k].message[l], sizeof(Message), 0);
                                                if(res < 0) {
                                                    error("Eroare send");
                                                }
                                            }

                                            store[j].message[k].nr_buff = 0;
                                        }
                                    }
                                }
                            }

                            // se reactualizeaza arr astfel incat fiecarui socket sai corespunda id-ul corect
                            // un cliet se poate deconecta dintr-o parte si sa se conecteze in alta pare
                            // se modifica socketul insa clientul e acelasi    
                            for(j = 0; j != stdim ;j++){
                                if(strcmp(store[j].id,arr[newtcpsock].id) == 0){
                                    for(k = 0; k!= store[j].nr;k++){
                                        strcpy(arr[newtcpsock].abonat[k],store[j].message[k].topic);
                                        arr[newtcpsock].nr_abonari++;
                                    }
                                    break;
                                }
                            }
                        
                        }
					}
			    } else if(i == udpsock){

                    // daca s-a primit un mesaj de la clientul udp
                    struct sockaddr_in from_sock;
                    socklen_t sock_len = sizeof(from_sock);
                    memset(buffer, 0 , BUFLEN);
                    
                    // se foloseste recvfrom pentru a cunoaste si ip-ul si portul
                    int res = recvfrom(udpsock, buffer, BUFLEN, 0, (struct sockaddr *)&from_sock, &sock_len);
                    if (res <= 0){
                        continue;
                    }
 
                    // se copie toata informatia necesara in structura msg care se transmite la client    
                    Message msg;
                    strcpy(msg.buffer,buffer);
                    for(j = 50; j!=60; j++){
                        strcpy(msg.buffer+j,buffer+j);
                    }
                    strcpy(msg.ip,inet_ntoa(from_sock.sin_addr));
                    msg.port = ntohs(from_sock.sin_port);

                    char topic[TPC_LEN];
                    strcpy(topic, buffer);
                    int k,l;

                    //adaugarea mesajelor primite in store daca unii clienti nu sunt activi momentan
                    for (l = 0; l != stdim; l++){
                        int flag = 0; 

                        // verificarea daca clientul este activ
                        // daca clientul este activ nu se adauga mesajul in store 
                        for(k = 0; k != dim; k++){
                            if(arr[k].id == NULL)continue;
                            if(strcmp(store[l].id, arr[k].id)==0){
                                flag = 1;
                                break;
                            }
                        }
                        
                        if(flag == 1){
                            continue;
                        }

                        // se verifica pentru fiecare client din store daca contine un topic identic cu cel necesar
                        // si se verifica daca sf-ul corespunzator este 1, daca da se adaug un Message cu toata informatia necesara
                        for(k = 0; k != store[l].nr; k++){
                            if(strcmp(store[l].message[k].topic, topic) == 0 && store[l].message[k].sf == '1'){
                                int nr_buff = store[l].message[k].nr_buff;
                                memset(store[l].message[k].message[nr_buff].buffer, 0, BUFLEN);
                                int s;
                                for( s = 0 ; s!= BUFLEN ; s++){
                                    store[l].message[k].message[nr_buff].buffer[s] = buffer[s];
                                }
                                memset(store[l].message[k].message[nr_buff].ip, 0, IP_LEN);
                                strcpy(store[l].message[k].message[nr_buff].ip,msg.ip);
                                store[l].message[k].message[nr_buff].port=msg.port;

                                store[l].message[k].nr_buff++;
                            }
                        }
                    }

                    // se transmit mesajele primite de la udp catre clientii tcp care 
                    // sunt abonati la aceleasi topicuri
                    for(j = 1; j<= fmax ; j++){

                        if (j == tcpsock || j == udpsock) {
                            continue;
                        }

                        // se verifica daca este in multimea read_fds 
                        if(FD_ISSET(j, &read_fds)){
                            int flag = 0;

                            // se verifica daca este abonat la topicul respectiv
                            for(k = 0; k != arr[j].nr_abonari; k++){
                                if(strcmp(arr[j].abonat[k], topic) == 0){
                                    flag = 1;
                                    break;
                                }
                            }
                            if(flag == 0) {
                                continue;
                            }

                            // se transmite mesajul 
                            res = send(j, (char*)&msg, sizeof(Message), 0);
                            if(res < 0) {
                                error("Eroare send");
                            }
                        }   
                    }
                } else {
					
                    // s-a primit mesaj de la clientul cu care deja comunica serverul 
					
                    memset(buffer, 0, BUFLEN);
                    Subscribe sub;
                    int res = recv(i, (char *)&sub, sizeof(Subscribe), 0 );

					if (res == 0) {
                    
                        // daca s-a inchis un client se afiseaza mesajul dorit si se elimina din vector
						printf("Client %s disconnected.\n", arr[i].id);
				    
                        // se adauga clientul in store (id-ul) asa cum el deja s-a deconectat
                        // se cauta pozitia s-a in vectorul arr
                        int poz = stdim;
                        for(j = 0; j != stdim; j++){
                            if(strcmp(store[j].id, arr[i].id) == 0){
                                poz = j;
                                break;
                            }
                        }

                        // se adauga informatia necesara in structura Store
                        strcpy(store[poz].id,arr[i].id);
                        store[poz].nr = 0;
                        for(j = 0; j != arr[i].nr_abonari; j++){
                            strcpy(store[poz].message[store[poz].nr].topic,arr[i].abonat[j]);
                            store[poz].message[store[poz].nr].sf = arr[i].sf[j];
                            store[poz].message[store[poz].nr].nr_buff = 0;
                            store[poz].nr++;
                        }
                        
                        if(stdim == poz){
                            stdim++;
                        }

                        arr[i].id = NULL;
                        arr[i].nr_abonari = 0;
                        
                        close(i);
                        FD_CLR(i, &read_fds);
                	} else if(res < 0) {

						error("Eroare la primire");
					    close(i);
                        FD_CLR(i, &read_fds);
                    } else { 

                        // a fost receptionat un mesaj de la clientul tcp (subscribe/unsubscribe)
                         int flag = 0,k;

                        // verificarea daca clientrul este abonat la topic 
                        for(j = 0; j != arr[i].nr_abonari; j++){
                            if(strcmp(arr[i].abonat[j],sub.topic) == 0){
                                flag = 1;
                                break;
                            }
                        }
    
                        // daca a fost apelat subscribe
                        if(sub.tip == 0){

                            // daca clientul nu era abonat anterior la topicul asta se aboneaza
                            if (flag == 0){
                                strcpy(arr[i].abonat[arr[i].nr_abonari],sub.topic);
                                arr[i].sf[arr[i].nr_abonari] = sub.sf;
                                arr[i].nr_abonari++;
                            }

                            // se transmite catre client daca abonarea s-a realizat
                            // daca inainte de a fi apelat subscrie cllientul era deja abonat 
                            // la topic se transmite un mesaj de eroare
                            int tran = send(i, (char*)&flag, sizeof(flag), 0);
                            if(tran < 0) {
                                error("Eroare send");
                            }
                        }
                        else if(sub.tip == 1){

                            // daca a fost apelat unsubscribe
                            // daca clientul era abonat se realizeaza unsubscribe
                            if(flag == 1){
                                for(k = j + 1; k < arr[i].nr_abonari; k++){
                                    strcpy(arr[i].abonat[k-1],arr[i].abonat[k]);
                                    arr[i].sf[k-1] = arr[i].sf[k];
                                }
                                arr[i].nr_abonari--;
                            }

                            // daca clientul nu era abonat inainte de apelare unsubscribe
                            // se transmite mesaj de eroare 
                            int tran = send(i, (char*)&flag, sizeof(flag), 0);
                            if(tran < 0) {
                                error("Eroare send");
                            }
                        }
					}
				}
			}
		}
    }

    close(tcpsock);

    return 0;
}
