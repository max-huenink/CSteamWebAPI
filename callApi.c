#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void errorAndExit(const char *msg) {
  perror(msg);
  exit(0);
}

char* CallWebApi(char *path, char* params_fmt, int useKey)
{
    int portno = 80;
    char *host = "api.steampowered.com";
    char *message_fmt = "GET /%s?%s&format=xml HTTP/1.0\r\nHost: %s\r\n\r\n";

    char *key_fmt = "key=%s&%s";
    char *apiKey = "D9D7D2179AD01BA70E5D71D012A33728";

    char *params;
    int paramLen = strlen(key_fmt)+strlen(apiKey)+strlen(params_fmt);
    params = malloc(paramLen);

    if (useKey == 1)
        sprintf(params, key_fmt, apiKey, params_fmt);
    else
        memcpy(params, params_fmt, strlen(params_fmt));;

    struct hostent *server;
    struct sockaddr_in serv_addr;
    int sockfd, bytes, sent, received, total;
    char *message, responseBuffer[1024];

    int messageLen = strlen(message_fmt) + strlen(path) + strlen(params) + strlen(host);
    message = malloc(messageLen);
    sprintf(message, message_fmt, path, params, host);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
      errorAndExit("ERROR opening socket");

    server = gethostbyname(host);
    if (server == NULL)
      errorAndExit("ERROR, no such host");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);

    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
      errorAndExit("ERROR connecting");

    total = strlen(message);
    sent = 0;
    do {
        bytes = write(sockfd,message+sent,total-sent);
        if (bytes < 0)
          errorAndExit("ERROR writing message to socket");
        if (bytes == 0)
            break;
        sent+=bytes;
    } while (sent < total);

    const char *bufferTmpPath = "tmp.OLD.xml";
    FILE *tmpFile = fopen(bufferTmpPath, "w");

    received = 0;
    do {
      memset(responseBuffer,0,1024);
      bytes = read(sockfd, responseBuffer, 1024);
      fwrite(responseBuffer, sizeof(char), strlen(responseBuffer), tmpFile);
      if (bytes < 0)
        errorAndExit("ERROR reading response from socket");
      if (bytes == 0)
        break;
      received += bytes;
    } while (1);

    if (received == total)
      errorAndExit("ERROR storing complete response from socket");

    close(sockfd);
    fclose(tmpFile);
    tmpFile = fopen(bufferTmpPath, "r");
    char *readBuffer = malloc(1024);
    memset(readBuffer,0,1024);
    fread(readBuffer,sizeof(char),1024,tmpFile);

    int firstNode = 0;
    for (int i = 0; i < 1024; i++)
    {
        if (*(readBuffer + i) == '<'
                && *(readBuffer + i + 1) != '?'
                && *(readBuffer + i + 1) != '!')
        {
            firstNode = i;
            break;
        }
    }
    char *retFilePath = "tmp.xml";
    FILE *retFile = fopen(retFilePath,"w");
    fwrite(readBuffer + firstNode, sizeof(char), 1024 - firstNode, retFile);
    do
    {
        memset(readBuffer,0,1024);
        fread(readBuffer,sizeof(char),1024,tmpFile);
        fwrite(readBuffer, sizeof(char), 1024, retFile);
    } while(!feof(tmpFile));

    fclose(tmpFile);
    fclose(retFile);

    //printf("First brace location: %d\n", firstNode);
    //printf("Original response: \n%s\n\n\n\n", responseBuffer);
    //printf("Cut response: \n%s\n\n\n\n", responseBuffer + firstNode);
    //int size = received - (firstNode * sizeof(char));
    //response = malloc(size);
    //memcpy(response, responseBuffer + firstNode, size);

    free(params);
    free(message);
    free(readBuffer);

    return retFilePath;
}
