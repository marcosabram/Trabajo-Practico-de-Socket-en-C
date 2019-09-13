#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<netdb.h>

void escribirArchivo(char msg[], char *buffer, long tamanio);

int main(int argc, char **argv){
   system("@cls||clear");
   int conexionCliente, status=0;
   struct sockaddr_in destino;
   char msg[50]= "";
   char *buffer;
   char c_tamanio[50];
   printf("Que archivo quiere recuperar?: ");
   scanf("%s", msg);
   long l_tamanio;
   conexionCliente= socket(AF_INET,SOCK_STREAM, 0);
   if(conexionCliente ==-1) {
      printf("No se puede crear el socket\n");
      exit(1);
   }

   destino.sin_family=AF_INET;
   destino.sin_port=htons(3499);                  
   destino.sin_addr.s_addr=inet_addr("127.0.0.1"); 
   //memset(&(destino.sin_zero),0,8);
   status=connect(conexionCliente, (struct sockaddr *)&destino, sizeof(destino));
   if(status==-1) {
      printf("No se pudo realizar la conexion\n");
      exit(-1);
   }
   send(conexionCliente, &msg, sizeof(msg),0);
   recv(conexionCliente, c_tamanio, sizeof(c_tamanio),0);
   if(strcmp(c_tamanio,"-1")!=0){
      l_tamanio=atol(c_tamanio);
      buffer=(char *)malloc(l_tamanio);
      system("@cls||clear");
      recv(conexionCliente, buffer, l_tamanio,0);
      escribirArchivo(msg,buffer,l_tamanio);
      printf("Archivo %s guardado exitosamente.\n", msg);
      free(buffer);
   }else{
      printf("El archivo no existe\n");
    }
   close(conexionCliente);
}

void escribirArchivo(char msg[], char *buffer, long tamanio){
   FILE *fp;
   fp = fopen (msg, "w+" );
   fwrite( buffer, sizeof(char), tamanio, fp ); //char cadena[]... cada posición es de tamaño 'char'
   fclose(fp);
}
