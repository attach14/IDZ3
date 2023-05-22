#include "data.h"

#define MAXPENDING 30 /* Maximum outstanding connection requests */

int clntSock;         /* Socket descriptor for client */

void my_handler(int nsig)
{
    close(clntSock);
    exit(0);
}

int main(int argc, char* argv[])
{
    int servSock;                    /* Socket descriptor for server */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned short echoServPort;     /* Server port */
    unsigned int clntLen;            /* Length of client address data structure */

    if (argc != 3) /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage:  %s <Server IP> <Server Port>\n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[2]); /* First arg:  local port */

    /* Create socket for incoming connections */
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        perror("socket() failed");
        exit(1);
    }
    (void)signal(SIGINT, my_handler);
    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = inet_addr(argv[1]); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */

    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr*) & echoServAddr, sizeof(echoServAddr)) < 0)
    {
        perror("bind() failed");
        exit(1);
    }

    printf("Server IP address = %s. Wait...\n", inet_ntoa(echoClntAddr.sin_addr));

    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, MAXPENDING) < 0)
    {
        perror("listen() failed");
        exit(1);
    }
    double coeff[4];
    double x1, x2, y1, y2, epsilon;
    printf("Write f(x) coefficients: ");
    scanf("%lf%lf%lf%lf", &coeff[0], &coeff[1], &coeff[2], &coeff[3]);
    printf("Write area borders x1, x2, y1, y2: ");
    scanf("%lf%lf%lf%lf", &x1, &x2, &y1, &y2);
    if (x2 <= x1 || y2 <= y1) {
        printf("Incorrect borders\n");
        return 0;
    }
    if (fabs(x1) > 10 || fabs(x2) > 10 || fabs(y1) > 10 || fabs(y2) > 10) {
        printf("Incorrect borders\n");
        return 0;
    }
    printf("Write epsilon: ");
    scanf("%lf", &epsilon);
    double ans = (x2 - x1) * (y2 - y1);
    double delta_x = x1;
    double delta_y = -y1;
    double A = 0;
    double B = x2 - x1;
    double len = (B - A) / 20;
    double curA = A - len;
    int cnt = 0;
    for (;;) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        clntLen = sizeof(echoClntAddr);

        /* Wait for a client to connect */
        if ((clntSock = accept(servSock, (struct sockaddr*) & echoClntAddr,
            &clntLen)) < 0)
        {
            perror("accept() failed");
            exit(1);
        }

        /* clntSock is connected to a client! */

        printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));

        //
        double request[2];
        double answer[9];
        int recvMsgSize; /* Size of received message */

        /* Receive message from client */
        if ((recvMsgSize = recv(clntSock, request, sizeof(request), 0)) < 0)
        {
            perror("recv() failed");
            exit(1);
        }

        double finish = request[0];
        curA += len;
        if (curA == B) {
            printf("Area = %lf\n", ans);
        }
        answer[0] = curA;
        answer[1] = curA + len;
        answer[2] = epsilon;
        answer[3] = delta_x;
        answer[4] = delta_y;
        answer[5] = coeff[0];
        answer[6] = coeff[1];
        answer[7] = coeff[2];
        answer[8] = coeff[3];
        recvMsgSize = sizeof(answer);
        if (send(clntSock, answer, recvMsgSize, 0) != recvMsgSize)
        {
            perror("send() failed");
            exit(1);
        }
        recvMsgSize = sizeof(request);
        if ((recvMsgSize = recv(clntSock, request, recvMsgSize, 0)) < 0)
        {
            perror("recv() failed");
            exit(1);
        }
        finish = request[0];
        if (finish > 0)
        {
            ans -= request[1];
            close(clntSock);
            cnt++;
            if (cnt == 20) {
                printf("Area = %lf\n", ans);
                printf("The end\n");
                return 0;
            }
            continue;
        }
    }
}
