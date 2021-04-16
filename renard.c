#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define TAILLE_BUF 1000
#define TAILLE_MAX 1000

/*CODE REQUETE*/
#define INIT 		0
#define PROP 		1
#define FIN 		2

/*CODE REPONSE*/
#define INIT_ACK 	3
#define PROP_ACK 	4
#define GAGNE 		5
#define FIN_ACK 	6
#define Erreur 		7

/*Structure*/
typedef struct ParametreConnexion
{
	char ad_serveur[255];
	char port[255];
} ParametreConnexion;

typedef struct message
{
	unsigned char code;
	int data[3];
	char login[12];
} message;

/*Signature de fonctions*/
ParametreConnexion initConnexion(char* host, char* port);
int connexion(void *infoConnexion);
void jeu(int sockfd, int taille_grille, int nb_de_renard, char* login, int seed);

/*------------------------------------------------------------------------------------------*/
int main(int argc, char* argv[])
{
	if (argc != 7)
	{
		printf("USAGE: %s <host> <port> <taille_grille> <nb_de_renard> <login> <seed>\n", argv[0]);
	}
	else
	{
		char host[255], port[255], login[12];
		int taille_grille, nb_de_renard, seed;
		
		strcpy(host, argv[1]);
		strcpy(port, argv[2]);
		taille_grille			=		atoi(argv[3]);
		nb_de_renard			=		atoi(argv[4]);
		strcpy(login, argv[5]);
		seed					=		atoi(argv[6]);

		ParametreConnexion pc = initConnexion(host, port); /*Initialise les infos de connexion*/
		int sockfd = connexion((void *)&pc); /*Lance la procédure de connexion avec ces paramètres*/
		jeu(sockfd, taille_grille, nb_de_renard, login, seed); /*Lance le jeu avec le socket du client*/	
	}

	return EXIT_SUCCESS;
}

ParametreConnexion initConnexion(char* host, char* port)
{
	/*Etablissement et initialisation structure*/
	int nbconnexion 			= 1;
	ParametreConnexion *pc 		= malloc(nbconnexion * sizeof(ParametreConnexion));

	strcpy(pc[0].ad_serveur, host);
	strcpy(pc[0].port, port);
	
	/*Affichage des paramètres de connexion*/
	printf("\n\n\tHost: %s\tPort: %s\n\n", pc[0].ad_serveur, pc[0].port);

	return *pc;
}

int connexion(void *infoConnexion)
{
	/*On recupere les donnees de la structure qui correspondent aux infos de connexion.*/
	ParametreConnexion *pc = infoConnexion;
	char *currentServeur = pc -> ad_serveur;
	char *currentPort = pc -> port;

	printf("\tConnexion à l'host : %s \tDepuis le port : %s en cours...\n\n", currentServeur, currentPort);

	/*On lance la procédure de connexion*/
		/*Initialisation de la socket et de la structure*/
		int sockfd, status, yes = 1, v6 = 0;
		struct addrinfo hints, *servinfo, *p;

		/*Initialisation des hints*/
		memset (&hints, 0, sizeof (hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = 0;
		

		/*On vérifie le status de getaddrinfo*/
		status = getaddrinfo (currentServeur, currentPort, &hints, &servinfo);
		if (status != 0)
		{
			perror ("\t(!) Erreur: client: getaddrinfo");
			exit(1);
		}

		/*On itère pour trouver la bonne socket du servinfo et s'y lier*/
		for(p = servinfo; p != NULL; p = p->ai_next)
		{
			/*Si la socket n'est pas bonne on continue*/
			if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
			{
				perror("\t(!) Erreur: server: socket");
				continue;
			}

			/*On passe les paramètres au socket*/
			if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
			{
				perror("\t(!) Erreur: server: setsockopt");
				exit(1);
			}
			setsockopt(sockfd, IPPROTO_IPV6, IPV6_V6ONLY,&v6, sizeof(int));

			/*On se connecte au bon socket*/
			if ((connect(sockfd, (const struct sockaddr*) &hints, sizeof(hints))) == -1)
			{
				perror("\t(!) Erreur: server: connect");
				exit(1);
			}
			else
			{
				printf("\tVous êtes bien connecté\n\n");
				break;
			}
		}
		return sockfd;
}

void jeu(int sockfd, int taille_grille, int nb_de_renard, char* login, int seed)
{
	int tour = 0, x = -1, y = -1;
	printf("\t||Jeu du renard||\n");

	/*Envoie des données sous forme de message au serveur*/
	message client;
	client.code = INIT;
	client.data[0] = htonl(taille_grille);
	client.data[1] = htonl(nb_de_renard);
	client.data[2] = htonl(seed);
	strcpy(client.login, login);
	
	if(send(sockfd, &client, sizeof(client), 0) == -1)
	{
		perror("(!) Erreur: client: send: \n");
		exit(-1);
	}

	printf("\tcoucou\n");

	// if(write(sockfd, &client, sizeof(client)))
	// {
	// 	perror("(!) Erreur: client: write: \n");
	// }
	// printf("\tcoucou\n");
	

	// if ((send(sockfd, &client, sizeof(client), 0)) < 0)
	// {
	// 	perror("(!) Erreur: client: send: \n");
	// }

	/*Jeu*/
	while(1)
	{
		tour ++;
		
		printf("\tChasseur donnez les coordonnées :\n");
		printf("\tx : ");
		scanf("%d", &x);
		printf("\ty : ");
		scanf("%d", &y);
	}
	
	
	// if (tour = 1)
	// {
	// 	printf("\tChasseur donnez les coordonnées :\n");
	// 	printf("\tOrdi: x : %d", x);
	// 	printf("\tOrdi: y : %d", y);
	// }
	// else if (luckyShot > 3)
	// {
	// }
	// else
	// {
	// 	/*Déplacement cavalier en partant en bas a gauche ou dernière position*/
	// }

}