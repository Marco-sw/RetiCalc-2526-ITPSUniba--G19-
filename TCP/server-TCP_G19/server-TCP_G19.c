#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#define MAX 255
#define PORT 5000
#define QLEN 6

void ErrorHandler(char *errorMessage) {
    printf ("%s", errorMessage);
}
void ClearWinSock() {
    #if defined WIN32
    WSACleanup();
    #endif
}
void System_pause(){
    #if defined WIN32
    system("pause");
    #endif
}
int confronto(char c)
{
    c = tolower((unsigned char)c);
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
}
void rimuoviVocali(char *str)
{
    int read = 0;
    int write = 0;

    while (str[read] != '\0') {
        if (!confronto(str[read])) {
            str[write++] = str[read];
        }
        read++;
    }

    str[write] = '\0';
}


int main(int argc, char *argv[]) {


    int port;
    if (argc > 1) {
        port = atoi(argv[1]);
    }
    else {
        port = PORT;
        if (port < 0) {
            printf("numero di porta non corretto %s \n", argv[1]);
            return 0;
        }
    }


    // INIZIALIZZAZIONE WINSOCK
    #if defined WIN32
    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(2,2);
    int iResult = WSAStartup(wVersionRequested, &wsaData);
    if (iResult != 0) {
        ErrorHandler("Errore WSAStartup()\n");
        return EXIT_FAILURE;
    }
    printf("Nessun errore verificato. \n");   // The WinSock DLL is acceptable. Proceed
    #endif


    // CREAZIONE DELLA SOCKET
    int MySocket;
    MySocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (MySocket < 0) {
        ErrorHandler("Creazione socket fallita.\n");
        ClearWinSock();
        return EXIT_FAILURE;
    }


        // BIND DELLA SOCKET
    struct sockaddr_in sad;
    memset(&sad, 0, sizeof(sad));

    sad.sin_family = AF_INET;
    sad.sin_addr.s_addr = inet_addr( "127.0.0.1" );
    sad.sin_port = htons(port);

    if (bind(MySocket, (struct sockaddr*) &sad, sizeof(sad)) <0)
    {
        ErrorHandler("bind() fallito.\n");
        closesocket(MySocket);
        ClearWinSock();
        return EXIT_FAILURE;
    }

    // SETTAGGIO DELLA SOCKET ALL'ASCOLTO
    if (listen (MySocket, QLEN) < 0) {
        ErrorHandler("listen() fallito.\n");
        closesocket(MySocket);
        ClearWinSock();
        return EXIT_FAILURE;
    }



    // ACCETTARE UNA NUOVA CONNESSIONE
    struct sockaddr_in cad;
    int clientSocket;
    int clientLen;

    printf( "In attesa di connessione del client...\n");

    while (1) {

        clientLen = sizeof(cad);
        if ( (clientSocket=accept(MySocket, (struct sockaddr *)&cad, &clientLen)) < 0) {
            ErrorHandler("accept() fallito.\n");
            continue;
        }
        //printf( "Gestione del client %s\n", inet_ntoa(cad.sin_addr) );

        // RICEVERE DATI DAL CLIENT
        while (1) {
            int bytesRcvd;
            char buf[MAX];

            bytesRcvd = recv(clientSocket, buf, MAX - 1, 0);
            if (bytesRcvd <= 0) {
                ErrorHandler("Connessione terminata.\n");
                break; // esce dal ciclo recv, torna ad accept
            }

            buf[bytesRcvd] = '\0';
            printf("%s\n", buf);

            if (strcmp(buf, "Hello") == 0) {
                printf( "Ricevuti dati dal client con indirizzo: %s\n", inet_ntoa(cad.sin_addr) );
            }
            else{

                rimuoviVocali(buf);

                // INVIARE DATI AL CLIENT
                if (send(clientSocket, buf, bytesRcvd, 0) != bytesRcvd) {
                    ErrorHandler("sendto() inviato numero di byte diverso da quello previsto");
                }
            }

        }

        //CHIUSURA DELLA CONNESSIONE
        closesocket(clientSocket);

    }


    //  FERMA IL SISTEMA
    System_pause();
    return 0;

}