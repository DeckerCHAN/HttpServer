//============================================================================
// Name        : HttpServer.cpp
// Author      : DeckerCHAN@20115858
// Version     : 1.0.0.1
// Copyright   : DeckerCHAN
// Description : HttpServer
//============================================================================

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include "PublicMethod.h"

using namespace std;
#define BUFFER_SIZE 256
#define REQUEST_MAX_SIZE 1000
#define SERVER_NAME "DeckerServer"
int main(int argc, char * args[])
{
	struct sockaddr_in LocalAddress;
	struct sockaddr_in RemoteAddress;
	int GetFD;

	LocalAddress.sin_family = AF_INET;
	LocalAddress.sin_port = htons(5862);
	LocalAddress.sin_addr.s_addr = INADDR_ANY;
	bzero(&(LocalAddress.sin_zero), 8);

	if ((GetFD = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		Log("socket create failed!");
		return 0;
	}
	else
	{
		cout<<LocalAddress.sin_port<<endl;
	}
	if (bind(GetFD, (struct sockaddr*) &LocalAddress, sizeof(struct sockaddr))
			== -1)
	{
		Log("bind error!");
		return 0;
	}
	if (listen(GetFD, 10) == -1)
	{

		std::cout << "listen error" << std::endl;
		return 0;
	}

	while (true)
	{
		Log("Ready To Connect!");
		socklen_t sin_size = sizeof(struct sockaddr_in);
		int RemoteFD = accept(GetFD, (struct sockaddr*) &RemoteAddress,
				&sin_size);
		int length = 0;
		char buffer[1024];
		recv(RemoteFD, buffer, 1024, 0);

		char * headln = (char *) malloc(strpos(buffer, '\n'));

		bcopy(buffer, headln, strpos(buffer, '\n'));

		char cmd[16], path[64], vers[16];
		sscanf(headln, "%s %s %s", cmd, path, vers);
		cout << path << endl;
		if (strcmp(path, "\\"))
		{
			//path = "\\index.html";
			bzero(path,64);
			strcpy(path,"\\index.html");
		}

		if (!file_exists(path))
		{
			Send404(RemoteFD);
			continue;
		}
		//cout<<substr(buffer,0,strpos(buffer,'\n'),a)<<endl;
		char * stream = (char *) malloc(filesize(path));
		bzero(stream, filesize(path));
		char * fpath = path;
		fpath++;
		FILE * file;
		cout << (strcmp(mime_content_type(path), "text/html") == 0) << endl;
		cout << filesize(path) << endl;

		if (strcmp(mime_content_type(path), "text/html") == 0)
		{
			file = fopen(fpath, "r");
			fread(stream, filesize(path), 1, file);
			fclose(file);
		}
		else
		{
			if ((file = fopen(fpath, "rb"
					"")) == NULL)
			{
				exit(0);
			}
			char temp[256];
			while (feof(file) == 0)
			{
				bzero(temp, 256);
				fread(temp, sizeof(char), 256, file);
				strcat(stream, temp);

			}
			file = fopen(fpath, "rb");
			fread(stream, filesize(path), 1, file);
		}

		//write(ProcessFD, http_html_hdr, strlen(http_html_hdr));

		SendHeaders(RemoteFD, 200, "OK", (char*) 0, mime_content_type(path),
				strlen(stream), 0);
		write(RemoteFD, stream, strlen(stream));
		//close(RemoteFD);
	}
	return 0;
}
