//============================================================================
// Name        : PublicMethod.h
// Author      : DeckerCHAN@20115858
// Version     : 1.0.0.1
// Copyright   : DeckerCHAN
// Description : HttpServer Public Methods
//============================================================================
#include <sys/stat.h>
#include <unistd.h>
#define BUFFER_SIZE 256
#define REQUEST_MAX_SIZE 1000
#define SERVER_NAME "DeckerServer"
using namespace std;
static int SendHeaders(int client_sock, int status, char *title,
		char *extra_header, char *mime_type, off_t length, time_t mod);
void Log(const char * messege)
{
	time_t now;
	struct tm *timenow;
	time(&now);
	timenow = localtime(&now);
	std::cout << '\n' << "++++++" << asctime(timenow) << messege << std::endl;
}
void Log(std::string messege)
{
	time_t now;
	struct tm *timenow;
	time(&now);
	timenow = localtime(&now);
	cout << '\n' << "++++++" << asctime(timenow) << messege.c_str() << endl;
}
char *substr(const char *s, int start_pos, int length, char *ret)
{
	char buf[length + 1];
	int i, j, end_pos;
	int str_len = strlen(s);

	if (str_len <= 0 || length < 0)
	{
		return "";
	}
	if (length == 0)
	{
		length = str_len - start_pos;
	}
	start_pos = (
			start_pos < 0 ?
					(str_len + start_pos) :
					(start_pos == 0 ? start_pos : start_pos--));
	end_pos = start_pos + length;

	for (i = start_pos, j = 0; i < end_pos && j <= length; i++, j++)
	{
		buf[j] = s[i];
	}
	buf[length] = '\0';
	strcpy(ret, buf);

	return (ret);
}
int strpos(const char *s, char c)
{
	int i, len;
	if (!s || !c)
		return -1;
	len = strlen(s);
	for (i = 0; i < len; i++)
	{
		if (s[i] == c)
			return i;
	}
	return -1;
}
/**
 * strrpos - find char at string last position
 *
 */
int strrpos(const char *s, char c)
{
	int i, len;
	if (!s || !c)
		return -1;
	len = strlen(s);
	for (i = len; i >= 0; i--)
	{
		if (s[i] == c)
			return i;
	}
	return -1;
}

/**
 * trim - strip left&right space char
 *
 */
char *trim(char *s)
{
	int l;
	for (l = strlen(s); l > 0 && isspace((u_char) s[l - 1]); l--)
	{
		s[l - 1] = '\0';
	}
	return (s);
}

/**
 * ltrim - strip left space char
 *
 */
char *ltrim(char *s)
{
	char *p;
	for (p = s; isspace((u_char) *p); p++)
		;
	if (p != s)
		strcpy(s, p);
	return (s);
}

/**
 *  filesize - get file size
 */
long filesize(char *filename)
{
	filename++;
	struct stat buf;
	if (!stat(filename, &buf))
	{
		return buf.st_size;
	}
	return 0;
}

/**
 * file_exists - check file is exist
 */
int file_exists(const char *filename)
{
	filename++;
	struct stat buf;
	if (stat(filename, &buf) < 0)
	{
		if (errno == ENOENT)
		{
			return 0;
		}
	}
	return 1;
}
static int Send404(int client_sock)
{
	char * errorHTML ="<head></head><body><html><h1>Error 404</h1><p>Document not found.</html></body>\n";
	SendHeaders(client_sock, 404, "Not Find", (char*) 0, "text/html",
			strlen(errorHTML), 0);

	write(client_sock, errorHTML, strlen(errorHTML));
	close(client_sock);
}
static int SendHeaders(int client_sock, int status, char *title,
		char *extra_header, char *mime_type, off_t length, time_t mod)
{
	time_t now;
	char timebuf[100], buf[BUFFER_SIZE], buf_all[REQUEST_MAX_SIZE], log[8];

	/* Make http head information */
	memset(buf_all, 0, REQUEST_MAX_SIZE);
	memset(buf, 0, BUFFER_SIZE);
	sprintf(buf, "%s %d %s\r\n", "HTTP/1.0", status, title);
	strcpy(buf_all, buf);

	memset(buf, 0, BUFFER_SIZE);
	sprintf(buf, "Server: %s\r\n", SERVER_NAME);
	strcat(buf_all, buf);

	now = time((time_t*) 0);
	strftime(timebuf, sizeof(timebuf), "%a, %d %b %Y %H:%M:%S GMT",
			gmtime(&now));
	memset(buf, 0, BUFFER_SIZE);
	sprintf(buf, "Date: %s\r\n", timebuf);
	strcat(buf_all, buf);

	if (extra_header != (char*) 0)
	{
		memset(buf, 0, BUFFER_SIZE);
		sprintf(buf, "%s\r\n", extra_header);
		strcat(buf_all, buf);
	}
	if (mime_type != (char*) 0)
	{
		memset(buf, 0, BUFFER_SIZE);
		sprintf(buf, "Content-Type: %s\r\n", mime_type);
		strcat(buf_all, buf);
	}
	if (length >= 0)
	{
		memset(buf, 0, BUFFER_SIZE);
		sprintf(buf, "Content-Length: %lld\r\n", (int64_t) length);
		strcat(buf_all, buf);
	}
	if (mod != (time_t) -1)
	{
		memset(buf, 0, BUFFER_SIZE);
		strftime(timebuf, sizeof(timebuf), "%a, %d %b %Y %H:%M:%S GMT",
				gmtime(&mod));
		sprintf(buf, "Last-Modified: %s\r\n", timebuf);
		strcat(buf_all, buf);
	}
	memset(buf, 0, strlen(buf));
	sprintf(buf, "Connection: close\r\n\r\n");
	strcat(buf_all, buf);
	write(client_sock, buf_all, strlen(buf_all));

	return 0;
}
/**
 * file_get_contents - read file contents
 *
 */
int file_get_contents(const char *filename, size_t filesize, char *ret,
		off_t length)
{
	if (!file_exists(filename) || access(filename, R_OK) != 0)
		return -1;

	int fd;
	char buf[filesize];

	if ((fd = open(filename, O_RDONLY)) == -1)
		return -1;
	length = (length > 0 ? length : filesize);
	read(fd, buf, length);
	strcpy(ret, buf);
	close(fd);

	return 0;
}

/**
 * is_dir - check file is directory
 *
 */
int is_dir(const char *filename)
{
	struct stat buf;
	if (stat(filename, &buf) < 0)
	{
		return -1;
	}
	if (S_ISDIR(buf.st_mode))
	{
		return 1;
	}
	return 0;
}

/**
 * is_file - check file is regular file
 *
 */
int is_file(const char *filename)
{
	struct stat buf;
	if (stat(filename, &buf) < 0)
	{
		return -1;
	}
	if (S_ISREG(buf.st_mode))
	{
		return 1;
	}
	return 0;
}

/**
 * Fetch current date tme
 *
 */
void getdate(char *s)
{
	char *wday[] =
	{ "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
	time_t timep;
	struct tm *p;

	time(&timep);
	p = localtime(&timep);
	sprintf(s, "%d-%d-%d %d:%d:%d", (1900 + p->tm_year), (1 + p->tm_mon),
			p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
}

/**
 * Get MIME type header
 *
 */
char * mime_content_type(const char *name)
{
	char *dot, *buf;

	dot = (char *) strrchr(name, '.');

	/* Not exist extension name */
	if (!dot)
	{
		buf = "application/octet-stream";
		return buf;
	}
	/* Exist extension name */
	else
	{
		/* Text */
		if (strcmp(dot, ".txt") == 0)
		{
			buf = "text/plain";
		}
		else if (strcmp(dot, ".css") == 0)
		{
			buf = "text/css";
		}
		else if (strcmp(dot, ".js") == 0)
		{
			buf = "text/javascript";
		}
		else if (strcmp(dot, ".xml") == 0 || strcmp(dot, ".xsl") == 0)
		{
			buf = "text/xml";
		}
		else if (strcmp(dot, ".xhtm") == 0 || strcmp(dot, ".xhtml") == 0
				|| strcmp(dot, ".xht") == 0)
		{
			buf = "application/xhtml+xml";
		}
		else if (strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0
				|| strcmp(dot, ".shtml") == 0 || strcmp(dot, ".hts") == 0)
		{
			buf = "text/html";

			/* Images */
		}
		else if (strcmp(dot, ".gif") == 0)
		{
			buf = "image/gif";
		}
		else if (strcmp(dot, ".png") == 0)
		{
			buf = "image/png";
		}
		else if (strcmp(dot, ".bmp") == 0)
		{
			buf = "application/x-MS-bmp";
		}
		else if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0
				|| strcmp(dot, ".jpe") == 0 || strcmp(dot, ".jpz") == 0)
		{
			buf = "image/jpeg";

			/* Audio & Video */
		}
		else if (strcmp(dot, ".wav") == 0)
		{
			buf = "audio/wav";
		}
		else if (strcmp(dot, ".wma") == 0)
		{
			buf = "audio/x-ms-wma";
		}
		else if (strcmp(dot, ".wmv") == 0)
		{
			buf = "audio/x-ms-wmv";
		}
		else if (strcmp(dot, ".au") == 0 || strcmp(dot, ".snd") == 0)
		{
			buf = "audio/basic";
		}
		else if (strcmp(dot, ".midi") == 0 || strcmp(dot, ".mid") == 0)
		{
			buf = "audio/midi";
		}
		else if (strcmp(dot, ".mp3") == 0 || strcmp(dot, ".mp2") == 0)
		{
			buf = "audio/x-mpeg";
		}
		else if (strcmp(dot, ".rm") == 0 || strcmp(dot, ".rmvb") == 0
				|| strcmp(dot, ".rmm") == 0)
		{
			buf = "audio/x-pn-realaudio";
		}
		else if (strcmp(dot, ".avi") == 0)
		{
			buf = "video/x-msvideo";
		}
		else if (strcmp(dot, ".3gp") == 0)
		{
			buf = "video/3gpp";
		}
		else if (strcmp(dot, ".mov") == 0)
		{
			buf = "video/quicktime";
		}
		else if (strcmp(dot, ".wmx") == 0)
		{
			buf = "video/x-ms-wmx";
		}
		else if (strcmp(dot, ".asf") == 0 || strcmp(dot, ".asx") == 0)
		{
			buf = "video/x-ms-asf";
		}
		else if (strcmp(dot, ".mp4") == 0 || strcmp(dot, ".mpg4") == 0)
		{
			buf = "video/mp4";
		}
		else if (strcmp(dot, ".mpe") == 0 || strcmp(dot, ".mpeg") == 0
				|| strcmp(dot, ".mpg") == 0 || strcmp(dot, ".mpga") == 0)
		{
			buf = "video/mpeg";

			/* Documents */
		}
		else if (strcmp(dot, ".pdf") == 0)
		{
			buf = "application/pdf";
		}
		else if (strcmp(dot, ".rtf") == 0)
		{
			buf = "application/rtf";
		}
		else if (strcmp(dot, ".doc") == 0 || strcmp(dot, ".dot") == 0)
		{
			buf = "application/msword";
		}
		else if (strcmp(dot, ".xls") == 0 || strcmp(dot, ".xla") == 0)
		{
			buf = "application/msexcel";
		}
		else if (strcmp(dot, ".hlp") == 0 || strcmp(dot, ".chm") == 0)
		{
			buf = "application/mshelp";
		}
		else if (strcmp(dot, ".swf") == 0 || strcmp(dot, ".swfl") == 0
				|| strcmp(dot, ".cab") == 0)
		{
			buf = "application/x-shockwave-flash";
		}
		else if (strcmp(dot, ".ppt") == 0 || strcmp(dot, ".ppz") == 0
				|| strcmp(dot, ".pps") == 0 || strcmp(dot, ".pot") == 0)
		{
			buf = "application/mspowerpoint";

			/* Binary & Packages */
		}
		else if (strcmp(dot, ".zip") == 0)
		{
			buf = "application/zip";
		}
		else if (strcmp(dot, ".rar") == 0)
		{
			buf = "application/x-rar-compressed";
		}
		else if (strcmp(dot, ".gz") == 0)
		{
			buf = "application/x-gzip";
		}
		else if (strcmp(dot, ".jar") == 0)
		{
			buf = "application/java-archive";
		}
		else if (strcmp(dot, ".tgz") == 0 || strcmp(dot, ".tar") == 0)
		{
			buf = "application/x-tar";
		}
		else
		{
			buf = "application/octet-stream";
		}
		return buf;
	}
}

