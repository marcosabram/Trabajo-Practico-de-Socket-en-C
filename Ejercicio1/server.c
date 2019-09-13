#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)
int main(void)
{
  /*variables para el buffer utilizado para recibir datos*/
  int cant=40;
  char rcvBuffer[cant];  
  char *msg = "Hello World Client!";
  /*** socket() ***/
  /*genero el socket,
  ***socketfd= socket description file 
  ***AF_INET=voy a usar la red en ipv4, 
  ***= voy a usar TCP,
  ***0= especifica el protocolo a usar, en este caso con 0, lo elije socket() 
  genere un espacion en ipv4 con tcp y protocolo a eleccion*/
  int socket_fd=socket(AF_INET, SOCK_STREAM, 0); 
  if(socket_fd==-1){
    printf("Socket cannot be generated \n Ended process");
    handle_error("socket");
    };
  //memset(&serv_addr, '0', sizeof(serv_addr)); //nose que hace
  memset(rcvBuffer, ' ', sizeof(rcvBuffer));//limpia el buffer, si no anda cuando quiere
  /*esta estructura contiene informacion de nuestra direccion y puerto, etc*/
  struct sockaddr_in serv_addr,rcv;   
  serv_addr.sin_family = AF_INET;    
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
  serv_addr.sin_port = htons(5001);//especificamos el puerto, con sin_port=0, bind() lo elije   
/*** bind() ***/
 /* asigna la direccion ip a el socket
 *** primer argumento es el socket
 *** el segundo argumento es un puntero a la estructura con la infromacion de nuestra maquina,
    se puede modificar esta estructura.
 *** el tercer argumento es el tamaño de la estructura*/
 
 /* asigno al socket, los elementos necesarios para una conexion, ip, puerto y demas, 
 *** que se definieron es serv_addr*/
  if(bind(socket_fd, (struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1){
    printf("Connect witch socket cannot be generated \nEnded process\n");
    handle_error("bind");
    exit(-1);
    };
  /*** listen()***/
  /* pongo a escuchar el socket */ 
  if(listen(socket_fd, 10) == -1){//acepta hasta 10 conexiones
    printf("Failed to listen\n");
    handle_error("listen");
    };     
  int addr_len = sizeof(struct sockaddr_in);    
  /*** accept() ***/
  /* acepta la conexiones etrantes */
  int connfd = accept(socket_fd,(struct sockaddr*)&rcv,&addr_len); // accept awaiting request acepta la conexion
  if(connfd==-1){
    printf("Failed to accept\n");
    handle_error("accept");
    };
  /*** recv() ***/
  /* recibe los datos y los cargar en recvBuffer, esta funcion devuleve 
     la cantidad de bytes recibidos*/
  int numrcv = recv(connfd, rcvBuffer, strlen(rcvBuffer), 0); //calculo los cracteres recibidos
  if(numrcv==-1){
    printf("recv failed\n");
    handle_error("recv");
    };
  //imprimo en consola
  printf("Bytes recibidos: %d \nMensaje: %s\n****\n****\n", numrcv,rcvBuffer);
  int bytes_enviados = send(connfd, rcvBuffer, strlen(rcvBuffer), 0);
  printf("Bytes enviados: %d \nMensaje: %s\n****\n****\n", bytes_enviados,rcvBuffer);
  //cierro la concexion
  close(connfd); 
  return 0;
}
