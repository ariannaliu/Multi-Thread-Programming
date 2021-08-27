#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <curses.h>
#include <termios.h>
#include <fcntl.h>

#define ROW 20
#define COLUMN 50 
#define NUM_THREAD 20
pthread_mutex_t mutex;


int mode = 0;
int signal = 1;

struct Node{
	int x , y; 
	Node( int _x , int _y ) : x( _x ) , y( _y ) {}; 
	Node(){} ; 
} frog ; 


char map[ROW+10][COLUMN] ; 

// Determine a keyboard is hit or not. If yes, return 1. If not, return 0. 
int kbhit(void){
	struct termios oldt, newt;
	int ch;
	int oldf;

	tcgetattr(STDIN_FILENO, &oldt);

	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);

	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);

	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	ch = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	if(ch != EOF)
	{
		ungetc(ch, stdin);
		return 1;
	}
	return 0;
}


void *logs_move( void *t ){
	
	pthread_mutex_lock(&mutex);
	int tid;
	tid = (int)t;
	bool flag = true;
	int pos;
	int count = 0;
	char previous = map[tid][0];
	int j;
	int condition;
	while(signal==1){
		
		
		if(tid%2 == 1){
			//moving left
			char rem_pos_0 = map[tid][0];
			for (j=0;j<COLUMN-2;j++){
				map[tid][j] = map[tid][j+1];
			}
			map[tid][COLUMN-2] = rem_pos_0;
			
			//moving frog's position if frog is moving left
			if(frog.x == tid){
				frog.y -= 1;
			}
			if (mode == 1){
				usleep(200000);
			}else if(mode == 2){
				usleep(100000);
			}else if(mode == 3){
				usleep(70000);
			}
			
		}
		else if(tid%2 == 0 && tid!=NUM_THREAD){
			//moving right
			char rem_pos_last = map[tid][COLUMN-2];
			for (j=COLUMN-2;j>0;j--){
				map[tid][j] = map[tid][j-1];
			}
			map[tid][0] = rem_pos_last;
			 
			//moving frog's position if frog is moving right
			if(frog.x == tid){
				frog.y += 1;
			}
			if (mode == 1){
				usleep(200000);
			}else if(mode == 2){
				usleep(100000);
			}else if(mode == 3){
				usleep(70000);
			}
		}
		else{
			/*  Check keyboard hits, to change frog's position or quit the game. */
			if(kbhit()){
				char dir=getchar();
				if(dir == 'w'||dir == 'W'){
					//frog[up]
					if((map[frog.x][(frog.y)-1]=='|') || (map[frog.x][(frog.y)+1]=='|')){
						map[frog.x][frog.y]='|';
						frog.x = frog.x - 1;
						map[frog.x][frog.y] = '0';
						
					}else{
						map[frog.x][frog.y]='=';
						frog.x = frog.x - 1;
						map[frog.x][frog.y] = '0';
					}
				}else if(dir == 'd'||dir == 'D'){
					//frog right
					if(frog.x==ROW){
						//if frog is in the last column
						if(frog.y == COLUMN-2){
							continue;
						}else{
							map[frog.x][frog.y]='|';
							frog.y += 1;
							map[frog.x][frog.y] = '0';
						}
						
					}else{
						// if frog is on the log
						map[frog.x][frog.y]='=';
						frog.y += 1;
						map[frog.x][frog.y] = '0';
					}
						
				}else if(dir == 'a'||dir == 'A'){
					if(frog.x==ROW){
						//if frog is in the last column
						if(frog.y == 0){
							continue;
						}else{
							map[frog.x][frog.y]='|';
							frog.y -= 1;
							map[frog.x][frog.y] = '0';
						}
					}else{
						//if frog is moving left on the log
						map[frog.x][frog.y]='=';
						frog.y -= 1;
						map[frog.x][frog.y] = '0';
						
					}
				}else if(dir == 's'||dir == 'S'){
					if(frog.x==ROW){
						continue;
					}else{
						map[frog.x][frog.y]='=';
						frog.x += 1;
						map[frog.x][frog.y] = '0';
					}
				}else if(dir == 'q'||dir == 'Q'){
					signal = 0;
				}
			}
			
			int log_len;
			if (mode == 1){
				log_len = 15;
			}else if(mode == 2){
				log_len = 9;
			}else if(mode == 3){
				log_len = 9;
			}
			
			//checking frog's status
			if(frog.x == 0){
				// success
				signal = 3;
			}else if((frog.y==0 || frog.y==COLUMN-2)&&(frog.x!=ROW)){
				// touch the wall
				signal = 2;
			}else if( map[frog.x][frog.y-1]==' ' && map[frog.x][frog.y+1]==' ' ){
				signal = 2;
			}else if(map[frog.x][(frog.y+log_len)%(COLUMN-1)]=='='){
				signal = 2;
			}else if(map[frog.x][(frog.y+COLUMN-1-log_len)%(COLUMN-1)]=='='){
				signal = 2;
			}
				
			
		}
		
		
		
		/*  Print the map on the screen  */
		printf("\033c");
		for( int i = 0; i <= ROW; ++i){
			puts( map[i] );
			
		}	
		
		usleep(100);
		
	}
	if(signal == 0){
		printf("\033c");
		printf("You exit the game!!\n");
	}else if(signal == 2){
		printf("\033c");
		printf("You lose the game!!\n");
	}else if(signal == 3){
		printf("\033c");
		printf("You win the game!!\n");
	}
	
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}

void init_logs(char map[ROW+10][COLUMN]){
	int i,j;
	int log_init_pos;
	int log_len;
	if (mode == 1){
		log_len = 15;
	}else if(mode == 2){
		log_len = 9;
	}else if(mode == 3){
		log_len = 9;
	}
	srand((unsigned int)time(NULL));
	for(i=1;i<ROW;++i){
		log_init_pos= (rand() % (COLUMN-2+1));
		//check whether log needs to go to the other side
		int left_log = COLUMN-1-log_init_pos;
		if (left_log>=log_len){
			//no need 
			for(j=0;j<log_len;j++){
				map[i][log_init_pos+j]='=';
			}
		}
		else{
			for(j=0;j<left_log;j++){
				map[i][log_init_pos+j]='=';
			}
			for(j=0;j<(log_len-left_log);j++){
				map[i][j]='=';
			}
		}
		
	}
}

int main( int argc, char *argv[] ){

	// Initialize the river map and frog's starting position
	memset( map , 0, sizeof( map ) ) ;
	int i , j ; 
	for( i = 1; i < ROW; ++i ){	
		for( j = 0; j < COLUMN - 1; ++j )	
			map[i][j] = ' ' ;  
	}


	printf("Enter the mode (easy, normal, hard): ");
	char mode_str[4];
	
	while(mode == 0){
		scanf("%s",mode_str);
		if (strcmp(mode_str, "easy") == 0){
			mode = 1;
		}else if(strcmp(mode_str, "normal")==0){
			mode = 2;
		}else if(strcmp(mode_str, "hard")==0){
			mode = 3;
		}else{
			printf("Invalid mode input, please choose again: ");
		}
		
	}
	init_logs(map);

	for( j = 0; j < COLUMN - 1; ++j )	
		map[ROW][j] = map[0][j] = '|' ;

	for( j = 0; j < COLUMN - 1; ++j )	
		map[0][j] = map[0][j] = '|' ;

	frog = Node( ROW, (COLUMN-1) / 2 ) ; 
	map[frog.x][frog.y] = '0' ; 

	//Print the map into screen
	for( i = 0; i <= ROW; ++i)	
		puts( map[i] );


	/*  Create pthreads for wood move and frog control.  */
	pthread_mutex_init(&mutex,NULL);
	pthread_t threads[NUM_THREAD];
	int rc;
	long k;
	for(k=1; k<=NUM_THREAD; k++){
		rc = pthread_create(&threads[k],NULL,logs_move,(void*)k);
		if(rc){
		printf("ERROR: return code from pthread_create() is %d",rc);
		exit(1);
		}
	}
	
	
	/*  Display the output for user: win, lose or quit.  */
	
	pthread_mutex_destroy(&mutex);
	pthread_exit(NULL);
	
	return 0;

}
