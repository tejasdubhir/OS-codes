#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include<string.h>
 extern ssize_t write (int __fd, const void *__buf, size_t __n) __wur;
 extern ssize_t read (int __fd, void *__buf, size_t __nbytes) __wur;
int pid =0;
int car1posreal=0, car2posreal=0;
int car1statreal =0, car2statreal =0 ;
int cheat1real =0, cheat2real = 0;
// you can change the prototype of existing
// functions, add new routines, and global variables
// cheatmode, car1, car2, report are different processes
// they communicate with each other via pipes

// step-1
// ask user's if they want to cheat
// if yes, ask them if they want to relocate car1 or car2
// ask them the new location of car1/car2
// instruct car1/car2 to relocate to new position
// goto step-1
void cheatmode(char inp[1024], int ppcheatcar1[2], int ppcheatcar2[2], int ppcar1report[2],int ppcar2report[2],int ppcar1car2[2] ,int c)
{
	while(!strcmp(inp, "cheat")){
		printf("Which car do you want to move?\n");
		int carnum;
		int dist; 
		scanf("%d", &carnum);
		if(carnum == 1){
			printf("By how much do you want to move car 1\n");
			scanf("%d", &dist);
			cheat1real = dist;
			// int cheat1;
			// cheat1 = dist;
			if(c==1){
			close(ppcheatcar1[0]);
			write(ppcheatcar1[1], &dist, sizeof(int));
			}
			printf("Jump value written in pipe\n");
			// car1(dist);
		}
		else if(carnum == 2){
			printf("By how much do you want to move car 2\n");
			scanf("%d", &dist);
			// car2(dist);
			// int cheat2;
			// cheat2 = dist;
			cheat2real = dist;
			if(c==1){
			close(ppcheatcar2[0]);
			write(ppcheatcar2[1], &dist, sizeof(int));
			}
			printf("Jump value written in pipe\n");
		}
		printf("Type \"cheat\" for more cheating o.w. type anything\n ");
		scanf("%s", inp);
	}
	return;
}

// step-1
// check if report wants me to terminate
// if yes, terminate
// sleep for a second
// generate a random number r between 0-10
// advance the current position by r steps
// check if cheat mode relocated me
// if yes set the current position to the new position
// send the current postion to report
// make sure that car1 and car2 generates a different
// random number
// goto step-1
void car1(char inp[1024], int ppcheatcar1[2], int ppcheatcar2[2], int ppcar1report[2],int ppcar2report[2],int ppcar1car2[2], int c)
{
	if(c==0){
		if(car1statreal==0){
			exit(0);
		}
		sleep(1);
		int dist = rand()%10;
		car1posreal+= dist;
		if(cheat1real>0){
			car1posreal = cheat1real;
			cheat1real = 0;
		}
	}
	if(c==1){
		int status ;
		fcntl(ppcar1report[0], F_SETFL, O_NONBLOCK);
		read(ppcar1report[0], &status, sizeof(int));
		if(status ==1 ){
			sleep(1);
			int dist = (rand()%10)+1;
			printf("move car1 to %d steps \n", dist);
			int car1pos ;
			car1pos += dist;
			int cheat1;
			read(ppcheatcar1[0], &cheat1, sizeof(int));
			if(cheat1 != 0){
				car1pos = cheat1;
				cheat1 = 0;
				write(ppcheatcar1[1], &cheat1, sizeof(int));
			}
			write(ppcar1report[1], &car1pos, sizeof(int));
			printf("now car1 is at %d \n", car1pos);
		}
		else if(status ==0 ){
			return;
		}
	}
}

// step-1
// check if report wants me to terminate
// if yes, terminate
// sleep for a second
// generate a random number r between 0-10
// advance the current position by r steps
// check if cheat mode relocated me
// if yes set the current position to the new position
// send the current postion to report
// make sure that car1 and car2 generates a different
// random number
// goto step-1
void car2(char inp[1024], int ppcheatcar1[2], int ppcheatcar2[2], int ppcar1report[2],int ppcar2report[2],int ppcar1car2[2], int c)
{
	if(c==0){
		if(car2statreal==0){
			exit(0);
		}
		sleep(1);
		int dist = rand()%10;
		car2posreal+= dist;
		if(cheat2real>0){
			car2posreal = cheat2real;
			cheat2real = 0;
		}
	}
	if(c==1){
		int status ;
		read(ppcar2report[0], &status, sizeof(int));
		if(status ==1 ){
			sleep(1);
			int dist = (rand()%10)+1;
			printf("move car1 to %d steps \n", dist);
			int car2pos;
			car2pos += dist;
			int cheat2;
			read(ppcheatcar2[0], &cheat2, sizeof(int));
			if(cheat2 != 0){
				car2pos = cheat2;
				cheat2 = 0;
				write(ppcheatcar2[1], &cheat2, sizeof(int));
			}
			write(ppcar2report[1], &car2pos, sizeof(int));
			printf("now car1 is at %d \n", car2pos);
		}
		else if(status ==0 ){
			return;
		}
	}
}

// step-1
// sleep for a second
// read the status of car1
// read the status of car2
// whoever completes a distance of 100 steps is decalared as winner
// if both cars complete 100 steps together then the match is tied
// if (any of the cars have completed 100 steps)
//    print the result of the match
//    ask cars to terminate themselves
//    kill cheatmode using kill system call
//    return to main if report is the main process
// else
// 	  print the status of car1 and car2
// goto step-1
void report(char inp[1024], int ppcheatcar1[2], int ppcheatcar2[2], int ppcar1report[2],int ppcar2report[2],int ppcar1car2[2], int c)
{
	if(c == 0){
		sleep(1);
		if(car1posreal>100 && car2posreal>100){
			printf("positions %d, %d \n", car1posreal, car2posreal);
			printf("TIE\n");
			exit(0);
		}
		else if(car1posreal>100){
			printf("positions %d, %d \n", car1posreal, car2posreal);
			printf("car1 wins \n");
			exit(0);
		}
		else if(car2posreal>100){
			printf("positions %d, %d \n", car1posreal, car2posreal);
			printf("car2 wins \n");
			exit(0);
		}
		else{
			printf("statuses %d, %d \n", car1statreal, car2statreal);
			printf("positions %d, %d \n", car1posreal, car2posreal);
		}

	}
	if(c==1){		
		int car1pos , car2pos;
		int car1stat, car2stat;
		read(ppcar1report[0], &car1pos, sizeof(int));
		read(ppcar2report[0], &car2pos, sizeof(int));
		if(car1pos>= 100 && car2pos>= 100){
			printf("positions %d, %d \n", car1posreal, car2posreal);
			kill(pid, 0);
			printf("TIE \n");
			exit(0);
		}
		else if( car1pos >=100 ){
			printf("positions %d, %d \n", car1posreal, car2posreal);
			kill(pid, 0);
			printf("Car1 wins \n");
			exit(0);
		}
		else if(car2pos >=100){
			printf("positions %d, %d \n", car1posreal, car2posreal);
			kill(pid, 0);
			printf("Car2 wins \n");
			exit(0);
		}
		else{
			car1stat =1 ;
			car2stat =1;
			int pid = fork();
			if(pid == 0)
			car1(inp, ppcheatcar1, ppcheatcar2, ppcar1report, ppcar2report, ppcar1car2, c);
			else
			car2(inp, ppcheatcar1, ppcheatcar2, ppcar1report, ppcar2report, ppcar1car2 ,c);
			printf("statuses %d , %d \n", car1stat, car2stat);
			printf("positions  %d , %d \n", car1pos, car2pos);
			return;
		}
	}
}

// create pipes
// create all processes
// wait for all processes to terminate
int main()
{
	car1statreal = 1;
	car2statreal = 1;
	int c = 0;
	char inp[1024];
	scanf( "%s",inp);
	int ppcheatcar1[2], ppcheatcar2[2], ppcar1report[2], ppcar2report[2], ppcar1car2[2];
	pipe(ppcar1car2);
	pipe(ppcar1report);
	pipe(ppcar2report);
	pipe(ppcheatcar1);
	pipe(ppcheatcar2);
	while(strcmp(inp , "exit")){
		printf("start exec\n");
		// printf("%s\n", inp);
		// int pid1 = fork();
		// int pid2 = fork();
		// if(pid1 ==0 && pid2 == 0){
		// 	printf("%d %d\n", pid1, pid2);
		// 	cheatmode(inp, ppcheatcar1, ppcheatcar2);
		// }
		// else if(pid1 == 0 && pid2!=0){
		// 	printf("%d %d\n", pid1, pid2);
		// 	car1(ppcar1car2, ppcar1report, ppcheatcar1);
		// }
		// else if(pid2 ==0 && pid1 != 0){
		// 	printf("%d %d\n", pid1, pid2);
		// 	car2(ppcar1car2, ppcar2report, ppcheatcar2);
		// }
		// else{
		// 	printf("%d %d\n", pid1, pid2);
		// 	report(ppcar1report, ppcar2report);
		// }
		cheatmode(inp, ppcheatcar1, ppcheatcar2, ppcar1report, ppcar2report, ppcar1car2, c);
		// printf("cheat done \n");
		car1(inp, ppcheatcar1, ppcheatcar2, ppcar1report, ppcar2report, ppcar1car2, c);
		car2(inp, ppcheatcar1, ppcheatcar2, ppcar1report, ppcar2report, ppcar1car2,c );
		report(inp, ppcheatcar1, ppcheatcar2, ppcar1report, ppcar2report, ppcar1car2, c);
		scanf( "%s",inp);
	}


		c =1;
		printf("returned\n");
		int pid = fork();
		if(pid == 0 ){
			car1(inp, ppcheatcar1, ppcheatcar2, ppcar1report, ppcar2report, ppcar1car2, c);
		}
		else{
			car2(inp, ppcheatcar1, ppcheatcar2, ppcar1report, ppcar2report, ppcar1car2, c);
		}
		wait(0);
		report(inp, ppcheatcar1, ppcheatcar2, ppcar1report, ppcar2report, ppcar1car2, c);
		inp[0] = '\0';
	// }
	return 0;
}
