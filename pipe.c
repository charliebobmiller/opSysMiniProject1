// pipe

int piping()
{
	// areguements to send through the pipe
	char msg[] = "harry potter";
	char input_str[100];
	
	int pipe1[2]; // storing in pipe
	
	// maybe create more pipe to handle more user simultaneously
	
	// pipe fail
	if(pipe() == -1)
	{
		prinft("Pipe fail");
		return 1;
	}
	
	// fork
	int pid = fork();
	
	if(pid == 0)//child
	{
		close(pipe1[1]);
		
		// read strign from pipe
		char pipemsg[100];
		read(pipe1[0], pipemsg, 100);
		
		int msgcount = strlen(pipemsg);
		
		for(int i = 0; i<strlen(msg); i++)
		{
			pipemsg[msgcount++] == msg[i];
		}
		
		// close reading 
		close(pipe1[0]);
		
	}
	else // parent, make sure to wait for the child
	{
		char pipemsg[100];
		
		close(pipe1[0]);
		// write string into pipe and close
		write(pipe1[1], input_str[100], strlen(input_str)+1);
		close(pipe1[1]);
		
		// wait for child
		wait();	
		
		// read string from child
		read(pipemsg1[0], input_str, strlen(input_str)+1);
		close(pipemsg1[0]);
	}
	
	return 0;
}