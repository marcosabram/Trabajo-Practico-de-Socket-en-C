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
int main(void)
{
  /*variables para el buffer utilizado para recibir datos*/
  int cant=40;
  char rcvBuffer[cant];  
  char *msg = "Hello World Server!";
  /*** socket() ***/
  /*genero el socket,
  ***socketfd= socket description file 
  ***AF_INET=voy a usar la red en ipv4, 
  ***= voy a usar TCP,
  ***0= especifica el protocolo a usar, en este caso con 0, lo elije socket() 
  genere un espacion en ipv4 con tcp y protocolo a eleccion*/
  int socket_fd=socket(AF_INET, SOCK_STREAM, 5); 
  if(socket_fd==-1){
    printf("Socket cannot be generated \n Ended process");
    handle_error("socket");
    };
  memset(rcvBuffer, ' ', sizeof(rcvBuffer));//limpia el buffer, si no anda cuando quiere
 /*esta estructura contiene informacion de nuestra direccion y puerto, etc*/
  struct sockaddr_in serv_addr,rcv;   
  serv_addr.sin_family = AF_INET;    
  serv_addr.sin_addr.s_addr = inet_addr("192.168.1.10"); 
  serv_addr.sin_port = htons(5000);//especificamos el puerto, con sin_port=0, bind() lo elije 
  /*** connnect() ***/
  /*realizamos la peticion al servidor, esperando que acepte nuesta conexion*/
  if(connect(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))==-1)
    {
    printf("Connect cannot be generated \n Ended process");
    handle_error("connect");
    };
  /*** send() ***/
  /* enviamos un mensaje al servidor */
  int bytes_enviados = send(socket_fd, msg, strlen(msg), 0);
  //imprimo en consola
  printf("Mensaje enviado %s\nBytes enviados: %d \n****\n****\n", msg, bytes_enviados);
  /*** recv() ***/
  /* recibe los datos y los cargar en recvBuffer, esta funcion devuleve 
     la cantidad de bytes recibidos*/
  int numrcv = recv(socket_fd, rcvBuffer, strlen(rcvBuffer), 0); //calculo los cracteres recibidos
  if(numrcv==-1){
    printf("recv failed\n");
    handle_error("recv");
    };
  //imprimo en consola
  
  printf("Bytes recibidos: %d \nMensaje: %s\n****\n****\n", numrcv,rcvBuffer);
  
  //cierro conexion
  close(socket_fd);
  return 0;
}
