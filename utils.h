// Pislari Vadim 323CB TEMA 2 PC

#define MAX_CLIENTS 30  // numarul clientilor pentru care se face listen la socketul tcp
#define BUFLEN 1560     // dimensiunea buffer-ului prmit de la udp 50 + 1500 + 10
#define IP_LEN 70       // dimensiunea IP-ului, 
#define ID_LEN 10    	// dimensiunea id-ului fiecarui client   
#define ST_DIM 40		// dimensiunea la store 
#define TPC_LEN 50		// dimensiunea topic-ului

// in cazul in care se determina o eroare in program
void error(char *msg){
    perror(msg);
    exit(0);
}

// mesaj transmis de la server catre client tcp despre informatia primita de la clientul udp
typedef struct {
	char buffer[BUFLEN];
	char ip[IP_LEN];
	int port;
}Message;

// mesaj transmis de la client tcp la server despre instructiunea primita
typedef struct {
	int tip; // daca e 0 atunci e subscribe, daca e 1 atunci e unsubscribe
	char topic[BUFLEN];
	char sf;	
}Subscribe;

// structura care pastreaza informatia despre client si face legatura intre socket si client
// adica arr[i] va pastra informatia clientului cu socketul i
typedef struct {
	char *id;
	char abonat[MAX_CLIENTS][TPC_LEN];
	char sf[MAX_CLIENTS];
	int nr_abonari;
}Client;

// informatia unui client atunci cand else este dezactivat (exit) si pastreaza toate mesajele
// de la udep in perioada cat el este dezactivt
typedef struct{
	char topic[TPC_LEN];
	Message message[10];
	int nr_buff;
	char sf;
	int sock;
}Lost;

// store este structura care pastreaza informatia tuturor clientilor dezactivati
// store are nr clienti cu un id si cu mesaje
typedef struct {
	char id[ID_LEN]; 
	Lost message[10];
	int nr;
}Store;
