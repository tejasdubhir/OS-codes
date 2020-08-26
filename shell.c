#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include<sys/wait.h> 

int open(const char *pathname, int flags);
pid_t fork(void);
int creat(const char *pathname, int flags);
ssize_t read(int fd, void *buf, size_t count);
// ssize_t write(int fd, void *buf, size_t count);
void execute(char* instructions){
    printf("%s\n", instructions);


//  check for I/O  redirection
    char command[1024];
    int io =0;
    char* filename;
    for(int i=0 ;i<strlen(instructions); i++){
        if(instructions[i]=='1'&& instructions[i+1]== '>'){
            // printf("close ho hyA \n");
            close(1);
            // printf("close ho hyA \n");
            open(&instructions[i+2], 0666);
            return;
        }
        else if(instructions[i] == '2' && instructions[i+1] == '>'){
            close(2);
            open(&instructions[i+2], 0666);
        }
        else if(instructions[i]== '2'&& instructions[i+1]== '>'&& instructions[i+2]== '&' && instructions[i+3]== '1' ){
            close(1);
            dup(2);
        }
        else if(instructions[i]=='>' && instructions[i+1]=='>'){
            io = 3;
            // printf("%d\n", i);
            int j;
            for(j=0 ; j<i; j++){
                command[j] = instructions[j];
            }
            command[j]= '\0';
            filename = &instructions[i+3];
        }
        else if(instructions[i]=='>'){
            io = 1;
            // printf("%d\n", i);
            int j;
            for(j=0 ; j<i; j++){
                command[j] = instructions[j];
            }
            command[j]= '\0';
            filename = &instructions[i+2];
        }
        else if(instructions[i]=='<'){
            io = 2;
            // printf("%d\n", i);
            int j;
            for(j=0 ; j<i; j++){
                command[j] = instructions[j];
            }
            command[j]= '\0';
            filename = &instructions[i+2];
        }
    }
    if(io==1){
        // printf("%s\n", command);
        // printf("%s\n", filename);
        int pid = fork();
        if(pid == 0){
            // printf("close ho gya \n");
            close(1);
            // printf("exec ho gya\n");
            creat(filename, 0666);
            // printf("exec ho gya\n");
            execute(command);
            // printf("exec ho gya\n");
            exit(0);
            return;
        }
        else{
            wait(NULL);
            return;
        }
    }
    else if(io == 2){
        int pid = fork();
        if (pid == 0) {
            close(0);
            open(filename, 0222);
            execute(command);
            exit(0);
        } else {
        wait(NULL);
        return;
        }
    }
    else if(io ==3){
        int pid = fork();
        if(pid == 0){
            // printf("close ho gya \n");
            close(1);
            // printf("exec ho gya\n");
            open(filename, 0666);
            // printf("exec ho gya\n");
            execute(command);
            // printf("exec ho gya\n");
            exit(0);
            return;
        }
        else{
            wait(NULL);
            return;
        }
    }


    // Chech for PIPE
    char if_pipe[1024];
    int  ffdd;
    for(int i =0 ; i< strlen(instructions); i++){
        if(instructions[i] == '|'){
            // printf("found pipi\n");
            if_pipe[i] = '\0';
            int fd[2];
            pipe(fd);
            int pid = fork();
            if(pid==0){
                    // printf("child process\n");
                    // printf(" NOW we execute %s\n", if_pipe);
                close(fd[0]);
                    // write(1, "NO dikkat 1\n", 51);
                close(1);
                // ffdd = creat("aaaa", 0666);
                    // write(1, "NO dikkat 2\n", strlen("NO dikkat 1\n"));
                dup(fd[1]);
                    // write(1, "NO dikkat 3\n", strlen("NO dikkat 1\n"));
                close(fd[1]);
                    // write(1, "NO dikkat 4\n", strlen("NO dikkat 1\n"));
                execute(if_pipe);
                exit(0);
            }
            else{
                wait(NULL);
                    // printf("paremt process \n");
                    // printf("NOW we execute %s\n", &instructions[i+2]);
                close(fd[1]);
                    // printf("NO dikkat 5\n");
                close(0);
                // dup(ffdd);
                // creat("aaaa", 0222);
                    // printf("NO dikkat 6\n");
                dup(fd[0]);
                    // printf("NO dikkat 7\n");
                close(fd[0]);
                    // printf("NO dikkat 8\n");
                execute(&instructions[i+2]);
                // exit(0);
            }
            // printf("DOme with PIPI\n");
            return;
        }
        if_pipe[i] = instructions[i];
    }

    // Execute the command otherwise
    char tokens[1024][1024];
    printf("execute start\n");
    int next_tok=0, token_no = 0, tok_ind =0;
    char * cur_token = malloc(1024);
    for(int i =0; i<1024; i++){
        // printf("%d\n", i);
        if(instructions[i] == ' ' || instructions[i] == '\0' || instructions[i]=='\n'){
            // printf("%s\n\n\n\n", tokens[token_no]);
            // printf("space found \n");
            tokens[token_no][tok_ind]= '\0';
            // tokens[token_no]=cur_token;
            // printf("%s\n", tokens[token_no]);
            tok_ind = 0;
            token_no++;
            // printf("token added\n");
            if(instructions[i]== '\0')
            break;
        }
        else{
            // cur_token[tok_ind]= instructions[i];
            tokens[token_no][tok_ind] = instructions[i];
            tok_ind++;
        }
    }
    // for(int i =0 ;i< token_no; i++){
    //     printf("%s\n", tokens[i]);
    // }
    for(int i = token_no; i<1024; i++){

    }
    int pid = fork();
    // printf("fork ho gya\n");
    if(pid!= 0){
        // printf("child ka wait \n");
        wait(NULL);
        // printf("child khatam \n");
    }
    else{
        char* args[1024] = {tokens[0], tokens[1], tokens[2], NULL};
        if(*tokens[1]== '\0')
            args[1] =NULL;
        if(*tokens[2]== '\0')
            args[2] =NULL;
        // printf("1 %s\n", args[0]);
        // printf("2 %s\n", args[1]);
        // printf("3 %s\n", args[2]);
        // printf("exec hoga\n");
        if(execv(args[0], args)==-1){
            printf("ERRE\n");
        }
        exit(0);
    }
    free(cur_token);
    // printf("exec h pgya!!\n");
}


int main(){
    printf("start the shell\n");
    char* attr ;
    char* exit = "exit\n";
    int commands = 0;
    while(strcmp(attr, "exit\n")){
        char c = '\0';
        printf("\nENter command>>>>");
        fflush(stdout);    
        attr = malloc(1024);
        while(c != '\n'){
            read(1, &c, 1);
            strcat(attr, &c);
        }
        // char end = ' ';
        // strcat(attr, &end);
        execute(attr);
    }
    free(attr);
    printf("BYE BYE\n");
}