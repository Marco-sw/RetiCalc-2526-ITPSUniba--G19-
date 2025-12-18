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
#include <stdarg.h>

#define MAX 255
#define PORT 5000

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
    printf("Nessun errore verificato \n");   // The WinSock DLL is acceptable. Proceed
    #endif


    // CREAZIONE DELLA SOCKET
    int Csocket;
    Csocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (Csocket < 0) {
        ErrorHandler("Creazione socket fallita.\n");
        closesocket(Csocket);
        ClearWinSock();
        return EXIT_FAILURE;
    }


    // COSTRUZIONE DELL’INDIRIZZO DEL SERVER
    struct sockaddr_in sad;
    memset(&sad, 0, sizeof(sad));
    sad.sin_family = AF_INET;
    sad.sin_addr.s_addr = inet_addr("127.0.0.1");
    sad.sin_port = htons(port);

    // CONNESSIONE AL SERVER
    if (connect(Csocket, (struct sockaddr *)&sad, sizeof(sad)) < 0) {
        ErrorHandler( "Connessione fallita.\n" );
        closesocket(Csocket);
        ClearWinSock();
        return EXIT_FAILURE;
    }


    char* firstMessage = "Hello";
    int stringLen;
    char string[MAX];
    char buffer[MAX];

    // INVIO STRINGA INIZIALE
    if (send(Csocket, firstMessage, 6, 0) != 6) {
        ErrorHandler("send() inviato numero di byte diverso da quello previsto");
        closesocket(Csocket);
        ClearWinSock();
        return -1;
    }


    // INVIO STRINGA DA MODIFICARE
    printf("Inserisci la stringa da inviare al server: ");
    scanf("%s", string);

    if ((stringLen = strlen(string)) > MAX){
        ErrorHandler("La stringa inserita supera la lunghezza massima di %d caratteri", MAX);
    }
    else{
        if (send(Csocket, string, stringLen, 0) != stringLen)
            ErrorHandler("sendto() inviato numero di byte diverso da quello previsto");
    }


    // RICEVERE I DATI
    int bytesRcvd;
    char buf[MAX];

    bytesRcvd = recv(Csocket, buf, MAX - 1, 0);
    if (bytesRcvd <= 0) {
        ErrorHandler("recv() fallito o connessione interrotta.\n");
        return EXIT_FAILURE;
    }

    buf[bytesRcvd] = '\0';
    printf("%s\n", buf);



    // CHIUSURA DELLA CONNESSIONE
    closesocket(Csocket);
    ClearWinSock();


    //  FERMA IL SISTEMA
    System_pause();
    return 0;



}