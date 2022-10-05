//declaration of required libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//defining the port to bind
#define PORT 4444

//helpers
void doDisconnect();


//main function
int main(){

	//declaration of integer variables and structures
	int c_sock, R;
	struct sockaddr_in serverAddr;
	
	//defining buffer 
	char buff[1024];
	char userinp[1024];

	// defining client socket
	c_sock = socket(AF_INET, SOCK_STREAM, 0);
	
	//if loop to know wheather the connection is successfull or not 
	if(c_sock < 0){
		printf("[-]Connection Error.\n");
		exit(1);
	}
	printf("[+]Client Socket creation successfull.\n");

	//declaration of memset and assigning respective values to serverAddr.sin....
	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	//To know wheather the binding is successfull or not
	R = connect(c_sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(R < 0){
		printf("[-]Connection Error.\n");
		exit(1);
	}
	printf("[+]Connection to Server successfull.\n");
	
	//creation of infinite loop till client returns exit or 0
	while(1){//connected to server loop
		strcpy(buff,"#connected");
		send(c_sock, buff, strlen(buff), 0);
		
		//listen
		bzero(buff,1024);
		recv(c_sock,buff,1024,0);
		//then consume characters until we find another or the end //check to see if response from server was a special command..
		if ( strcmp(buff,"#disconnect") == 0 || strcmp(buff,"#cmd") == 0) {
			doDisconnect();	
		} 

		//print server output to client
		printf("%s",buff);
		bzero(buff,1024);
		//get user input
		printf("\n>>:");
		scanf("%s", buff);

		//check input 
		if ( strcmp(userinp,"q") == 0 || strcmp(userinp,"exit") == 0) 
			doDisconnect(); //disconnect from server cleanly

		//send input to server
		send(c_sock, buff, strlen(buff), 0);

		//continue back to listening
		continue;



















//		printf("Please select an option from below list :  ");
		// display options from options.txt
		
		//get the response from client and load it to buffer
/*		
		//send response to server
		
		//if loop to know client wants exit or not
		if(strcmp(buff, "exit") == 0){
			close(c_sock);
			printf("[-]Disconnected from server.\n");
			exit(1);
		}

		if(recv(c_sock, buff, 1024, 0) < 0){
			printf("[-]Error in receiving data.\n");
		}else{
		
			//the below print we just print the same response from client just for refference..we can remove after we complete the integration of project.
			printf("From Server: \t%s\n", buff);
			//display the list of columns in the selected option to client so that client can choose a desired column with which we are going to processes
		
		
		
		
		//send the selected coloumn response to server 
		//server is going to create respective number of processess needed.
		
		
		
		//We are going to display the options as below :
		//printf("1.Display the records\n2. save the records\n3.Display the records\n4.Exit");



		//after selecting the option from above.
		//for option 1 we have to display data from the respective process data recieved from server and for option 3 we have to display the count of list of columns from the respective process data recieved from server
		
		//for option 2 we have to display "saved" after server saves the record
		
		//for option 4 we have to disconnect from the sever.
		
		
		}
		*/

	}

	return 0;
}



void doDisconnect() {
	//fix this so its cleaner
	exit(0);

}
