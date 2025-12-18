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
#include <stdarg.h>

#define MAX 255

void ErrorHandler(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
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


int main(){

    #if defined WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2 ,2), &wsaData);
    if (iResult != 0) {
        printf ("Errore WSASturtup\n");
        return EXIT_FAILURE;
    }
    #endif


    int sock;
    struct sockaddr_in servAddr;
    struct sockaddr_in fromAddr;
    unsigned int fromSize;
    char string[MAX];
    char buffer[MAX];
    int stringLen;
    int respStringLen;

    const char *name;
    const int port;
    struct hostent *host;
    struct in_addr *ina;
    const char *firstMessage = "Hello";


    printf("Inserisci il nome host del server: ");
    scanf("%s", name);
    printf("Inserisci la porta del server: ");
    scanf("%d", &port);


    host = gethostbyname(name);
    if (host == NULL) {
        fprintf(stderr, "gethostbyname() fallito.\n");
        exit(EXIT_FAILURE);
    } else {
        ina = (struct in_addr*) host->h_addr_list[0];
        //printf("Risultato di gethostbyname(%s): %s\n", name, inet_ntoa(*ina));
    }


    // CREAZIONE DELLA SOCKET
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
        ErrorHandler("socket() fallita");
        exit(EXIT_FAILURE);
    }


    // COSTRUZIONE DELL'INDIRIZZO DEL SERVER
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = PF_INET;
    servAddr.sin_port = htons(port);
    servAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*ina));

    // INVIO STRINGA INIZIALE
    if (sendto(sock, firstMessage, 6, 0, (struct sockaddr*)&servAddr, sizeof(servAddr)) != 6)
        ErrorHandler("sendto() inviato numero di byte diverso da quello previsto");


    // INVIO STRINGA DA MODIFICARE
    printf("Inserisci la stringa da inviare al server: ");
    scanf("%s", string);

    if ((stringLen = strlen(string)) > MAX){
        ErrorHandler("La stringa inserita supera la lunghezza massima di %d caratteri", MAX);
    }
    else{
        if (sendto(sock, string, stringLen, 0, (struct sockaddr*)&servAddr, sizeof(servAddr)) != stringLen)
            ErrorHandler("sendto() inviato numero di byte diverso da quello previsto");
    }


    // RITORNO DELLA STRINGA MODIFICATA
    fromSize = sizeof(fromAddr);
    respStringLen = recvfrom(sock, buffer, MAX, 0, (struct sockaddr*)&fromAddr, &fromSize);

    if (servAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr) {
        fprintf(stderr, "Errore: ricevuto pacchetto da una fonte sconosciuta!\n");
        exit(EXIT_FAILURE);
    }

    buffer[respStringLen] = '\0';
    printf("Stringa \"%s\" ricevuta dal server nome: %s, inidirizzo: %s\n", buffer, name, inet_ntoa(*ina));



    // CHIUSURA SOCKET
    closesocket(sock);
    ClearWinSock();


    System_pause();
    return EXIT_SUCCESS;



}