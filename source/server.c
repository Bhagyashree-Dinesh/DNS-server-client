// Server side implementation of UDP client-server model

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>

#define PORT     8080
#define MAXLINE 1024

struct node{
char ip[100];
char dname[100];
struct node* next;
};


struct node* readnode(struct node* ,char [],char []);
void displaynode(struct node* );
char* searchnode(struct node*,char []);
void LinkedListToFile(struct node*);
struct node* head=NULL;
 

	
// Driver code
int main() {
 
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    char buffer[MAXLINE];
    char *dns_reply ;
	char tempname[100];
	char line[100];
	char doname[100];
	char clip[100];
	char* word;
	char tempip[100];
         if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) 
		 {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
          }   
    
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
     
    // Filling server information
    servaddr.sin_family    = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);
     
    // Bind the socket with the server address
    if ( bind(sockfd, (const struct sockaddr *)&servaddr, 
            sizeof(servaddr)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
     
    int len, n;
    while(1)
    {
    n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
                MSG_WAITALL, ( struct sockaddr *) &cliaddr,
                &len);
    buffer[n] = '\0';
    //printf("\n%d\n",len);
    printf("DNS Query : %s\n", buffer);
     
     FILE *fp = fopen("dns5.txt","r");
	
	if(fp==NULL)
	{printf("error");
	}
	
	else
	{
	while(fgets(line,100,fp))
	{
	word = strtok(line," ");
	strcpy(tempname,word);
	word = strtok(NULL,"\n");
	strcpy(tempip,word);	
	head=readnode(head,tempname,tempip);
	}
	}
	
	fclose(fp);
	
	dns_reply=searchnode(head,buffer);
	sendto(sockfd, (const char *)dns_reply, strlen(dns_reply), 
        MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
            len);
	printf("\nDNS reply sent!\n");
       }    
return 0;
}

struct node* readnode(struct node* head,char name[100],char ipaddr[100]) //To insert a new entry into the linked list
{
    struct node* cur=(struct node*)malloc(sizeof(struct node));
	if(head==NULL)
	{
	strcpy(cur->ip,ipaddr);
	strcpy(cur->dname,name);
	cur->next=NULL;	
	head=cur;
	}
	
	else
	{
	struct node* temp;
	temp=head;
	while(temp->next!=NULL)
	{
	temp=temp->next;
	}
	strcpy(cur->ip,ipaddr);
	strcpy(cur->dname,name);
	cur->next=NULL;	
	temp->next=cur;
	}
return head;
}

void displaynode(struct node* head) //To display the linked list
{	
	struct node* temp;
	temp=head;
	while(temp!=NULL)
	{
	printf("%s %s",temp->dname,temp->ip);
	printf("\n");
	temp=temp->next;
	}
	return;
}


char *searchnode(struct node* head,char name[100])
{
	struct node* temp;
	temp=head;
	int flag=0;
	while(temp!=NULL)
	{
	if(!strcmp(name,temp->dname)) // Incase of a match with existing list
	{
	printf("Host IP: %s",temp->ip);
	return temp->ip;
	}	
	temp=temp->next;	
	}
	

	{
	
	char* IPbuffer;
    struct hostent* host_entry;
	host_entry = gethostbyname(name);
	if(host_entry==NULL) //If user entry is invalid
	{
	IPbuffer="NO SUCH DOMAIN NAME!! ";
	fprintf(stderr,"\nNO DN\n");
	return IPbuffer;
	}
	
	IPbuffer = inet_ntoa(*((struct in_addr*)
        		       host_entry->h_addr_list[0]));
 	printf("Host IP: %s", IPbuffer);
	
	//printf("\n%d",cliaddr.sin_port);	
	struct node* first;
	first=readnode(head,name,IPbuffer);
	LinkedListToFile(first);
	return IPbuffer;
	}
	
	
}
	
void LinkedListToFile(struct node* first) //To copy new entries into file
{
    char line[100];  
    struct node* temp=first;	  
    FILE *fp1 = fopen("dns.txt","a");
	if(fp1==NULL)
	{printf("error");
	}
    else
	{
        while(temp->next!= NULL)
		{          
            
            temp = temp->next;        
        }
	        strcpy (line, temp->dname);
            strcat (line, " ");
            strcat (line, temp->ip);
            strcat (line,"\n");
            fprintf(fp1,"%s",line);
        
        fclose(fp1);                     // CLOSE FILE
    }
    
}





