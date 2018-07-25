
/* This is Client side implementation of UDP client-server model. A DNS request submitted at the client side 
will be searched in the local cache of the client, which is implemented as a linked list. If a match is found, the result
is outputted. Else, a request is sent to the server */

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
#include <time.h>


 
#define PORT     8080
#define MAXLINE 1024
#define MIN_COUNT 1
#define MAX_NODES 5 
/*The linked list is allowed to have only MAX_NODES number of entries */
/*If the number of entries exceed, the least frequently used ones are deleted*/


/*This structure specifies the fields of every entry allowed in the linked list
at the client side. ip and dname refer to IP address and Domain name. 'count' keeps 
track of number of times a particular domain name DNS query happens. 'seconds' acts as 
time stamp for each entry, which is refreshed every time a query pertaining to it happens. */

struct node 
{
	char ip[100];
	char dname[100];
	int count;
	time_t seconds;
	struct node* next;
};


struct node* readnode(struct node* ,char [],char []);
void displaynode(struct node* );
int searchnode(struct node*,char []);
struct node* deletenode(struct node*);
int node_count;


int main() 
{
    char dns_query[100];  /* String holding message from client to server*/
    int sockfd;
    struct sockaddr_in  servaddr;
    char buffer[MAXLINE];  /* String holding message from server to client*/
    struct node* head;
    int c;
    
/*In the while loop, 2 options are presented. First one is for DNS query
and option 2 is for exit */
	
while(1)
{
	printf("\n\t MAIN MENU\n1.Send DNS Query: \n2.Exit :\n  ");
    	printf("\nChoose an option : ");
	scanf("%d",&c);
		switch(c) 
	{
		case 1:{
   	 	printf("Enter Domain Name :");
   		 scanf("%s",dns_query);
		int flag;
    		flag=searchnode(head,dns_query);
    
   		 /* Creating socket file descriptor */
    		if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) 
			{
        	perror("socket creation failed");
        	exit(EXIT_FAILURE);
    			}
 
   		 memset(&servaddr, 0, sizeof(servaddr));
     
    		/*Filling server information */
   		servaddr.sin_family = AF_INET;
    		servaddr.sin_port = htons(PORT);
    		servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
     
    		int n, len;
    		if(!flag) /* server request in case of no mapping in client linked list */
		{	/*Send a request to server*/
   			 sendto(sockfd, (const char *)dns_query, strlen(dns_query),
     			 MSG_CONFIRM, (const struct sockaddr *) &servaddr, 
           	  	 sizeof(servaddr));
  			  printf("\n Dns request sent.\n");
  			  len=sizeof(struct sockaddr_in)  ;
			/*Receive the server reply,i.e IP address in the variable buffer*/
  			  n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
             		   MSG_WAITALL, (struct sockaddr *) &servaddr,
               			 &len);
			if(n<0)
			fprintf(stderr,"\n error in receiving");
   			 buffer[n] = '\0';
   			 printf("\nServer : %s\n", buffer);
   			 if(buffer[n-2]!='!')
   				 {	    
					if(node_count>=MAX_NODES)
	 				  { head=deletenode(head); }

	                 head=readnode(head,dns_query,buffer);
   			 displaynode(head);
   			 close(sockfd);
   				 }
  		  }

	}break;
	
case 2 : exit(0);
    } 
    } 
	return 0;

} 

/*Function to create a new node with domain name 
and IP address and add it to linked list*/
struct node* readnode(struct node* head,char name[100],char ipaddr[100])
{
    struct node* cur=(struct node*)malloc(sizeof(struct node));
	if(head==NULL) /*if linked list is empty*/
	{
	strcpy(cur->ip,ipaddr);
	strcpy(cur->dname,name);
        cur->seconds=time(NULL);
        cur->count=1;
	cur->next=NULL;	
	head=cur;
	}
	
	else  /*If linked list is empty, append new node at the end*/
	{
	struct node* temp;
	temp=head;
	while(temp->next!=NULL)
		 {
		temp=temp->next;
		}
	strcpy(cur->ip,ipaddr);
	strcpy(cur->dname,name);
   	 cur->seconds=time(NULL);
    	cur->count=1;
	cur->next=NULL;	
	temp->next=cur;
	}
node_count++;
return head;
}


/*Function to display the linked list*/
void displaynode(struct node* head)
{	
	struct node* temp;
	temp=head;
	while(temp!=NULL)
	{
	printf("%s %s",temp->dname,temp->ip);
	printf("\n");
	temp=temp->next;
	}
}


/*Function to serach for a DNS request in client cache*/
/*This function returns a flag which is equal to 1 incase a match is found,
else it returns 0 */
int  searchnode(struct node* head,char name[100])
{       
	
	struct node* temp;
	temp=head;
	int flag=0;
        int len;
	
	while(temp!=NULL)
	{
	if(!strcmp(name,temp->dname)) /*if match is found, increase count, refresh the timer and set the flag */
		{ 
         	temp->count++; //Number of times DNS query is done for a particular Domain name
          	temp->seconds=time(NULL);
   		printf("\n\nHost IP: %s\n",temp->ip);
		flag=1;
		break;
		}	
	temp=temp->next;	
	}
        
       return flag;
}

/*This function deletes a node that has been in the local cache 
for more than 50 seconds without being searched second time. It returns
the head node after deleting the least frequently used node*/
 struct node* deletenode(struct node* head)
{
	 
    
    if (head == NULL)
    {
        printf("Empty list at client cache");
        return NULL;
    }
    
    struct node *s, *ptr, *prev;
    s = head;
    time_t time_now;
    time_now=time(NULL);
    
  
	int flag=0;
	struct node *small;
	small=NULL;
        while (s->next != NULL)
        {
           
			if ((time_now-(s->next->seconds)>=50)&&(!flag))
			{
			 small=s;
            		 flag=1;
			}
			if ((time_now-(s->next->seconds)>=50)&&(flag))	
			{
			if(small->count>s->next->count)
			small=s;
			 
			}
		s=s->next;		
             
        }

         ptr=small->next;
	 prev=small;
	 prev->next=ptr->next;
	 free(ptr);   
	 return head;

}

 

