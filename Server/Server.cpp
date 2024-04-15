#include "winsock2.h"
#include <stdio.h>
#include <stdlib.h>
int main(void)
{
	WSADATA ws;
	struct sockaddr_in local;
	int s;
	int s1;
	int rc;
	char buf[1];
	local.sin_family = AF_INET;
	local.sin_port = htons(7500);//порт, который прослушивает сервер
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	// Init
	if (WSAStartup(0x0202, &ws) != 0)
	{
		printf("error");
		exit(1);
	}
	// Создаѐм сокет
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0)
	{
		perror("socket call failed");
		exit(1);
	}
	//Привязываем адрес интерфейса и номер порта к прослушивающему сокету
	rc = bind(s, (struct sockaddr*)&local, sizeof(local));
	if (rc < 0)
	{
		perror("bind call failure");
		exit(1);
	}
	//прослушивание порта
	rc = listen(s, 5);
	if (rc)
	{
		perror("listen call failed");
		exit(1);
	}
	//прием соединения, ожидающего во входной очереди
	s1 = accept(s, NULL, NULL);
	if (s1 < 0)
	{
		perror("accept call failed");
		exit(1);
	}
	// Получаем сообщение от клиента
	rc = recv(s1, buf, 1, 0);
	if (rc <= 0)
	{
		perror("recv call failed");
		exit(1);
	}
	printf("Client message: %c\n", buf[0]);
	//запись принятого сообщения
	rc = send(s1, "2", 1, 0);
	if (rc <= 0)
		perror("send call failed");
	exit(0);
	return 0;
}
