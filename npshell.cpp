#include <iostream>
#include <string>
#include<sstream>
#include <vector>
#include <cstdlib>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
using namespace std;
typedef struct{
	int fd[2];
}pipefd;
int pipe_sig = 0;
vector<string> spilt_input(const string& s){
	istringstream stream(s);
	vector<string> inputs;
	string input;
	while(stream>>input){
		inputs.push_back(input);
	}
	
	return inputs;	
}
int printenv(string& s){
	char* char_arr;
	char_arr = &s[0];
	if(const char* env_p = getenv(char_arr))
        	cout  << env_p <<endl;
	return 0;	
}
void redirection(string& s){
	int out = open(s.c_str(),O_WRONLY|O_CREAT| O_TRUNC, S_IRUSR | S_IWUSR);
	if (out == -1){
                cout << "File open error." << endl;
                return;
            }
	
        dup2(out, STDOUT_FILENO);
	return;
}
int operation(vector<string> s){
	char *commands[256];
	int length = s.size();
	int redirectout = 0;
	pid_t c_pid = fork();
	if (c_pid == -1) {
        	perror("fork");
       		exit(EXIT_FAILURE);
    	} else if (c_pid > 0) {
		if(pipe_sig){
			pipefd p; 
			if(pipe(p.fd) == -1){
				perror("Pipe failed");
				exit(1);
			}
		}	
        	//cout << "printed from parent process " << getpid() << endl;
        	wait(nullptr);
    	} else {
		for(int i = 0;i < length;++i){
			if(s[i] == ">"){
				redirection(s[i+1]);
				redirectout = 1;
			}
			else if(s[i] == "|"){
				pipe_sig = 1;				
			}	
			else{
				if(!redirectout)
					commands[i] = const_cast<char*>(s[i].c_str());
			}
			
		}
		if(redirectout)
			commands[length-2] = NULL;
		else
			commands[length] = NULL;
		execvp(commands[0],commands);
		perror("execve");
        	//cout << "printed from child process " << getpid() << endl;
        	exit(EXIT_SUCCESS);
    	}
	return 0;	
}
int main(){
	string str;
	
	cout << "% ";
	while(getline(cin,str)){
		vector<string> results = spilt_input(str);
		if(results[0] == "printenv"){
			printenv(results[1]);
		}
		else if(results[0] == "setenv"){
			setenv(results[1].c_str(),results[2].c_str(),1);	
		}
		else if(results[0] == "exit" || results[0] == "EOF" ){
			exit(0);	
		}
		else{
			operation(results);			
		}		
		cout << "% ";
	}
	return 0;
}
