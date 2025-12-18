#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#endif

#include <stdio.h>
#include <string.h>

#define MAX 255
#define PORT 5000

void ErrorHandler(char *errorMessage) {
    printf(errorMessage);
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
    int iResult = WSAStartup(MAKEWORD(2 ,2), &wsaData);
    if (iResult != 0) {
        ErrorHandler("Errore WSAStartup()\n");
        return EXIT_FAILURE;
    }
    //printf("Nessun errore verificato. \n");
    #endif

    int sock;
    struct sockaddr_in servAddr;
    struct sockaddr_in clntAddr;
    unsigned int cliAddrLen;
    char buffer[MAX];
    int recvMsgSize;

    // CREAZIONE DELLA SOCKET
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
        ErrorHandler("Creazione socket fallita");
        return EXIT_FAILURE;
    }

    // COSTRUZIONE DELL'INDIRIZZO DEL SERVER
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(port);
    servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // BIND DELLA SOCKET
    if ((bind(sock, (struct sockaddr *)&servAddr, sizeof(servAddr))) < 0){
        ErrorHandler("bind() fallito");
        return EXIT_FAILURE;
    }

    printf("In ascolto...\n");


    // RICEZIONE DELLA STRINGA DAL CLIENT
    while(1) {

        cliAddrLen = sizeof(clntAddr);
        recvMsgSize = recvfrom(sock, buffer, MAX, 0, (struct sockaddr*)&clntAddr, &cliAddrLen);

        buffer[recvMsgSize] = '\0';
        printf("%s\n", buffer);

        if (strcmp("Hello", buffer) == 0) {
            struct in_addr addr;
            addr.s_addr = inet_addr(inet_ntoa(clntAddr.sin_addr));
            struct hostent *host = gethostbyaddr((char *) &addr, 4, AF_INET);
            char* canonical_name = host->h_name;
            fprintf(stdout, "Ricevuti dati dal client nome: %s, indirizzo: %s\n", canonical_name, inet_ntoa(clntAddr.sin_addr));
        }
        else{

            rimuoviVocali(buffer);

            // RINVIA LA STRINGA AL CLIENT
            if (sendto(sock, buffer, recvMsgSize, 0, (struct sockaddr *)&clntAddr, sizeof(clntAddr)) != recvMsgSize)
                ErrorHandler("sendto() inviato numero di byte diverso da quello previsto");

        }

    }


    System_pause();
    return EXIT_SUCCESS;


}