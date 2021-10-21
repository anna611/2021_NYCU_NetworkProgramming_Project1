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
#include <cstring>
using namespace std;
typedef struct{
	int fd[2];
	int index;
}pipe_data;
void handle_child(int signo) {
        /* Declare function as [static] to purge the hidden [this] pointer
 *          * C library does not know what the heck is this pointer.*/
        int status;
        while (waitpid(-1, &status, WNOHANG) > 0) {

        }
}
vector<string> spilt_input(const string& s){	//spilt commands with space
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
vector<string> parse(string& s){	//split commands with pipe
	vector<string> res;
	size_t pos = 0;
	string token;
	while ((pos = s.find(" | ")) != string::npos) {
		token = s.substr(0, pos);
		res.push_back(token);
		s.erase(0, pos + 3);
	}	
	res.push_back(s);
	return res;

}
int operation(vector<string> s){
	char *commands[256];
	int redirectout = 0;
	vector<pipe_data> record;
	pipe_data p;
	for(int i = 0; i < s.size();++i){
		if(s.size() > 1 && i != s.size()-1){		//create pipe
			if(pipe(p.fd) < 0)
				cout << "pipe create error"<<endl;
			p.index = i;
			record.push_back(p);
		}
		vector<string> tmp = spilt_input(s[i]);
		signal(SIGCHLD,handle_child);
		pid_t c_pid = fork();
		if (c_pid == -1) {
			perror("fork");
			exit(EXIT_FAILURE);
		} else if (c_pid > 0) {		//parent process
			if(i != 0){
				close(record[i-1].fd[0]);
				close(record[i-1].fd[1]);
			}
			waitpid(c_pid, nullptr, 0);
		} else {	//child process
			for(int i = 0;i < tmp.size();++i){
				if(tmp[i] == ">"){
					redirection(tmp[i+1]);
					redirectout = 1;
				}
				else{
					if(!redirectout)
						commands[i] = const_cast<char*>(tmp[i].c_str());
				}
			}
			if(redirectout)
				commands[tmp.size()-2] = NULL;
			else
				commands[tmp.size()] = NULL;
			if(s.size() > 1){		//need to pipe
				if(i != 0){
					dup2(record[i-1].fd[0], STDIN_FILENO);
				}
				if(i != s.size()-1){
                                        dup2(record[i].fd[1], STDOUT_FILENO);	
				}
				for(int j = 0;j < record.size(); ++j){
					close(record[j].fd[0]);
					close(record[j].fd[1]);
				}
			
			}
			execvp(commands[0],commands);
			cerr << "Unknown command: [" << commands[0] << "]." << endl;
			exit(EXIT_SUCCESS);
		}
	}
	return 0;	
}
int main(){
	string str;	
	cout << "% ";
	setenv("PATH","bin:.",1);
	while(getline(cin,str)){	
		vector<string> results = spilt_input(str);
		vector<string> cmds = parse(str);
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
			operation(cmds);			
		}		
		cout << "% ";
	}
	return 0;
}
