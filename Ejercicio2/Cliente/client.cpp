#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)


void write_file(char msg[], char *buffer, long tamanio);

int main(void)
{
  /*variables para el buffer utilizado para recibir datos*/
  int cant=100;
  char rcvBuffer[cant]; 
  char *buffer;
  char msg[50]; 
  /*** socket() ***/
  /*genero el socket,
  ***socketfd= socket description file 
  ***AF_INET=voy a usar la red en ipv4, 
  ***= voy a usar TCP,
  ***0= especifica el protocolo a usar, en este caso con 0, lo elije socket() 
  genere un espacion en ipv4 con tcp y protocolo a eleccion*/
  int socket_fd=socket(AF_INET, SOCK_STREAM, 0); 
  if(socket_fd==-1)
  {
    handle_error("socket");
  };
  memset(rcvBuffer, ' ', sizeof(rcvBuffer));
  //memset(msg, ' ', sizeof(msg));//limpia el buffer, si no anda cuando quiere
 /*esta estructura contiene informacion de nuestra direccion y puerto, etc*/
  struct sockaddr_in serv_addr,rcv;   
  serv_addr.sin_family = AF_INET;    
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
  serv_addr.sin_port = htons(5001);//especificamos el puerto, con sin_port=0, bind() lo elije 
  /*** connnect() ***/
  /*realizamos la peticion al servidor, esperando que acepte nuesta conexion*/
  if(connect(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))==-1)
  {
    handle_error("connect");
    exit(-1);
  };
  printf("Que archivo quiere recuperar?: ");
  scanf("%s", msg);
  long l_tamanio;
  /*** send() ***/
  /* enviamos un mensaje al servidor */
  if (send(socket_fd, &msg, sizeof(msg), 0) == -1)
  {
    handle_error("send");
  };
  /*** recv() ***/
  /* recibe los datos y los cargar en recvBuffer, esta funcion devuleve 
     la cantidad de bytes recibidos*/
  if(recv(socket_fd, rcvBuffer, sizeof(rcvBuffer), 0)==-1)
  {
    handle_error("recv");
  };
  if(strcmp(rcvBuffer,"-1")!=0)
  {
      l_tamanio=atol(rcvBuffer);
      buffer=(char *)malloc(l_tamanio);
      recv(socket_fd, buffer, l_tamanio,0);
      write_file(msg,buffer,l_tamanio);
      printf("Archivo %s guardado exitosamente.\n", msg);
      free(buffer);
  }else{
      printf("El archivo no existe\n");
  }  
  //cierro conexion
  close(socket_fd);
  return 0;
}

void write_file(char msg[], char *buffer, long tamanio)
{
  FILE *fp;
  fp = fopen(msg, "w+");
  fwrite(buffer, sizeof(char), tamanio, fp);
  fclose(fp);
}