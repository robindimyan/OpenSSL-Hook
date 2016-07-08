#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include <openssl/ssl.h>

/*
	SSL I/O işlemlerinin hook fonksiyonları
*/
int SSL_write(SSL *s, const void *buf, int num)
{
//	İçeriği logla
	FILE *logfile = fopen("logfile", "a+");
	fprintf(logfile, "[WRITE]\n\n%s\n\n*-*-*-*-*-*-*-*-*\n", (char *)buf);
	fclose(logfile);

//	Asıl SSL_write fonksiyonuna SSL_write2 adlı bir pointer oluşturduk
	int (*SSL_write2)(SSL *s, const void *buf, int num);
	SSL_write2 = dlsym(RTLD_NEXT, "SSL_write");

//	İşi asıl fonksiyonumuza devredelim
	return SSL_write2(s, buf, num);
}

int SSL_read(SSL *s, void *buf, int num)
{
//	İçeriği logla
	FILE *logfile = fopen("logfile", "a+");

//	Asıl SSL_read fonksiyonuna SSL_read2 adlı bir pointer oluşturduk
	int (*SSL_read2)(SSL *s, const void *buf, int num);
	SSL_read2 = dlsym(RTLD_NEXT, "SSL_read");
	
//	Gelen veriyi önce asıl fonksiyona gönderip deşifre işlemini yaptırıyoruz
	int r = SSL_read2(s, buf, num);

//	Ardından bu verimizi de loglayalım
	fprintf(logfile, "[READ]\n\n%s\n\n*-*-*-*-*-*-*-*-*\n", (char *)buf);
	fclose(logfile);
	return r;
}
