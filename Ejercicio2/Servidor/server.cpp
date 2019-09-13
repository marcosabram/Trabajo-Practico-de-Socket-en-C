#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
/////
#include<stdio.h>
#include<stdlib.h>
#include<string>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <dirent.h>
#include <unistd.h>
#include <utility>
#include <map>
#include <list>
#include <vector>
#include <iostream>
#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)

using namespace std; 

void error(const char *s);
/* Función que hace algo con un archivo */
long tamanioArchivo(char *archivo);

long buscar (char nombreArchivo[]);

void procesarArchivo(char *archivo, char *buffer);

void moveToCache(char *archivo);

int tamanioCache();



typedef pair<string,string> par;
map<string, string> m_cache;

std::list<string> mylist;
std::list<string>::iterator l_it;

int main(void)
{

  map<string,string>::iterator it = m_cache.begin();
  l_it = mylist.begin();

  /*variables para el buffer utilizado para recibir datos*/
  int cant=40;
  char rcvBuffer[cant]; 
  char sizeofBuffer[cant];
  
  	long sizeofFile;
 	char *file;
    //gido usa puntero
  /*** socket() ***/
  /*genero el socket,
  ***socketfd= socket description file 
  ***AF_INET=voy a usar la red en ipv4, 
  ***= voy a usar TCP,
  ***0= especifica el protocolo a usar, en este caso con 0, lo elije socket() 
  genere un espacion en ipv4 con tcp y protocolo a eleccion*/
  int socket_fd=socket(AF_INET, SOCK_STREAM, 0); 
  if(socket_fd==-1){
    handle_error("socket");
    };
  //memset(&serv_addr, '0', sizeof(serv_addr)); //nose que hace
  memset(rcvBuffer, ' ', sizeof(rcvBuffer));
  memset(sizeofBuffer, ' ', sizeof(sizeofBuffer));//limpia el buffer, si no anda cuando quiere
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
    handle_error("bind");
    exit(-1);
    };

  moveToCache("medium.txt");
  moveToCache("medium2.txt");
  moveToCache("medium3.txt");
  moveToCache("medium4.txt");




  /*** listen()***/
  /* pongo a escuchar el socket */ 

  do{
    if(listen(socket_fd, 10) == -1){//acepta hasta 10 conexiones
      handle_error("listen");
      };     
  socklen_t addr_len= sizeof(struct sockaddr_in);    
    /*** accept() ***/
    /* acepta la conexiones etrantes */
    int connfd = accept(socket_fd,(struct sockaddr*)&rcv,&addr_len); // accept awaiting request acepta la conexion
    if(connfd==-1){
      handle_error("accept");
      };
    /*** recv() ***/
    /* recibe los datos y los cargar en recvBuffer, esta funcion devuleve 
      la cantidad de bytes recibidos*/
    int numrcv = recv(connfd, rcvBuffer, strlen(rcvBuffer), 0); //calculo los cracteres recibidos
    if(numrcv==-1){
    handle_error("recv");
    };

   sizeofFile=buscar(rcvBuffer);
   file=(char *)malloc(sizeofFile);

    if (sizeofFile > 536870912){
      cout<<"Supera la cache, enviando desde disco..."<<endl<<endl;
      	procesarArchivo(rcvBuffer, file);
      	sprintf(sizeofBuffer,"%ld", sizeofFile);
      	send(connfd, sizeofBuffer, sizeof(sizeofBuffer),0);
        cout<<"Enviando archivo "<< rcvBuffer<<endl;
        send(connfd, file, sizeofFile,0);
        //system("@cls||clear");
        cout<<"Envio Exitoso\n"<<endl;

      }
      else{
        if(sizeofFile >= 0){
          sprintf(sizeofBuffer,"%ld", sizeofFile);
          send(connfd, sizeofBuffer, sizeof(sizeofBuffer),0);
          cout<<"Enviando archivo "<< rcvBuffer<<endl;
          it=m_cache.find(rcvBuffer);
          strcpy(file, it->second.c_str()); 
          send(connfd, file, sizeofFile,0);
          //system("@cls||clear");
          cout<<"Envio Exitoso\n"<<endl;
        }
        else
        {
            cout<<"El archivo no existe"<<endl;
            send(connfd, "-1",2,0);          //'-1' sera archivo inexistente, 2 *sizeof(char).
        }
      }

    free(file);
    }while(true);

  close(socket_fd); 
    cout<<"Terminó el servidor...\n"<<endl;
  return 0;
}


long buscar(char nombreArchivo[]){
  //cout<<"Entrando a buscar"<<endl;
  long ftam=-1; 

  map<string,string>::iterator indice;
  indice = m_cache.find(nombreArchivo);

  if(indice != m_cache.end()){
    ftam=(indice->second).size();
    mylist.remove(nombreArchivo);
    mylist.push_back(nombreArchivo);
    cout<<"Encontrado en la cache, pesa "<<ftam<<" bytes"<<endl;
  }else{

    /* Con un puntero a DIR abriremos el directorio */
    DIR *dir;
    /* en *ent habrá información sobre el archivo que se está "sacando" a cada momento */
    struct dirent *ent;
    /* Empezaremos a leer en el directorio actual */
    dir = opendir (".");


    /* Miramos que no haya error */
    if (dir == NULL)
      error("No puedo abrir el directorio");
   
    /* Una vez nos aseguramos de que no hay error, ¡vamos a jugar! */
    /* Leyendo uno a uno todos los archivos que hay */
    while (((ent = readdir (dir)) != NULL) && (ftam == -1)){    /* Nos devolverá el directorio actual (.) y el anterior (..), como hace ls */
        if ( strcmp(ent->d_name, nombreArchivo)==0){            /* Una vez tenemos el archivo, lo pasamos a una función para procesarlo. */
          ftam=tamanioArchivo(nombreArchivo);
      	}
      }
    if(ftam!=-1 && ftam<536870912){
      cout<<"Encontrado en disco, moviendo a cache"<<endl;
      if(tamanioCache()+ftam<536870913){
        moveToCache(nombreArchivo);
        }
      else{
        int borrar=tamanioCache()+ftam-536870913;
        l_it=mylist.begin();
        while(borrar>0){
          borrar=borrar-(m_cache.find(*l_it)->second).size();
          m_cache.erase (*l_it);   
          l_it = mylist.erase (l_it);
        }
        moveToCache(nombreArchivo);
      }

    }

    closedir (dir);
  }
  return ftam;
}

void error(const char *s){
  /* perror() devuelve la cadena S y el error (en cadena de caracteres) que tenga errno */
  perror (s);
  exit(EXIT_FAILURE);
}

long tamanioArchivo(char *archivo){
  //cout<<"Entrando a tamanioArchivo"<<endl;
  /* Para "procesar", o al menos, hacer algo con el archivo, vamos a decir su tamaño en bytes */
  /* para ello haremos lo que vemos aquí: https://poesiabinaria.net/2010/04/tamano-de-un-fichero-en-c/ */
  FILE *fich;
  long ftam=-1;

  fich=fopen(archivo, "r");
  if (fich)
    {
      fseek(fich, 0L, SEEK_END);
      ftam=ftell(fich);
      fclose(fich);
    }
  else
    /* Si ha pasado algo, sólo decimos el nombre */
    cout<<"No se pudo acceder a .)\n"<<archivo<<endl;
  
  return ftam;
}

void procesarArchivo(char *archivo, char *buffer){
  /* Para "procesar", o al menos, hacer algo con el archivo, vamos a decir su tamaño en bytes */
  /* para ello haremos lo que vemos aquí: https://poesiabinaria.net/2010/04/tamano-de-un-fichero-en-c/ */
  //cout<<"Entrando a procesarArchivo"<<endl;
  FILE *fich;
  long ftam=-1;
  char aux[50];

  fich=fopen(archivo, "r");
  if (fich)
    {
      fseek(fich, 0L, SEEK_END);
      ftam=ftell(fich);
     	fseek(fich,0,SEEK_SET);
      fread(buffer,ftam,1,fich);
    }
  else
    /* Si ha pasado algo, sólo decimos el nombre */
    cout<<"No se pudo acceder a .)\n"<<archivo<<endl;
  
fclose(fich);
}


void moveToCache(char *archivo){
    FILE *fich;
    fich=fopen(archivo, "r");
    long ftam=-1;
    char *buffer;
  if (fich)
    {
      fseek(fich, 0L, SEEK_END);
      ftam=ftell(fich);
      buffer=(char *)malloc(ftam);
      fseek(fich,0,SEEK_SET);
      fread(buffer,ftam,1,fich);
      m_cache[archivo]=buffer;
      mylist.push_back(archivo);
      cout<<"Archivos en cache"<<endl;
      for(l_it=mylist.begin(); l_it!=mylist.end(); ++l_it)
         std::cout << ' ' << *l_it<<endl;
      std::cout << '\n';
      free(buffer);
    }
  else
    /* Si ha pasado algo, sólo decimos el nombre */
    cout<<"No se pudo acceder a .)\n"<<archivo<<endl;
  fclose(fich);
}

int tamanioCache(){
  int acum=0;
  for (map<string,string>::iterator it=m_cache.begin(); it!=m_cache.end(); ++it){
    acum=acum + (it->second).size();
  }
  return acum;
}