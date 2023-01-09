#include <stdio.h>        
#include <stdlib.h>         
#include <sys/types.h>      
#include <sys/shm.h>        
#include <errno.h>          
#include <semaphore.h>      
#include <fcntl.h>  
#define N 10

//Δυαδική αναζήτησ Β tree
int binarySearch(int *p, int l, int r, int x) 
{ 	
    if (r >= l) { 
        int mid = l + (r - l) / 2; 
  
        if (p + mid == p + x) 
            return *(p + mid); 
  
        
        else if (p + x < p + mid) 
            return binarySearch(p, l, mid - 1, x); 
  
        else if (p + x > p + mid)
        	return binarySearch(p, mid + 1, r, x); 
    } 
    return -1; 
} 

int main()
{	
	//Αρχικοποίηση μεταβλητών και shared memory
	key_t shmkey; 
	int pid[N];
	int *p;
	int shmid;  
	int i; 
	int child_status;
	
    shmkey = 9009;       
    shmid = shmget (shmkey, N, 0644 | IPC_CREAT);
	

    if (shmid < 0)                           
    {
        perror ("shmget\n");
        exit (1);
    }
							       					
    p  = (int *) shmat (shmid, NULL, 0);   

	//Καθορισμός θέσης αθροίσματος στο shared memory και εξίσωση με το 0
	int *sum_pointer = (p + N + 1);
	*sum_pointer = 0; 
    printf(" Initial value of shared variable: %d\n\n", *p);

	for (i=0; i< N; i++)		//Γεμιζουμε τον πινακα του b+ tree με i*2 τιμες σε καθε κελι i στην shared memory
	{ 
    *(p + i) = i*2;
	printf("timh tou shared b+ tree %d osto keli: %d\n\n", i, *(p+i));
	}

	//Δημιουργία παιδιών
	for (i=0; i<N; i++)
	{
		pid[i] = fork();
		

		if (pid[i] < 0) 
    	{
       		printf ("Fork error.\n");
       		return (-1);  
    	}
	
		if (pid[i]==0)
		{	
			//Κάθε παιδί τυπώνει το pid του και προσθέτει το i του στο sum, βρίσκωντας την τιμή του μέσω της δυαδικής αναζήτησης
			printf("I am a child (Id=%i)\n\n", getpid() );
			*sum_pointer = *sum_pointer + binarySearch(p,0,N-1,i);
			printf("I made the sum = %d \n", *sum_pointer);
			exit(i);
		}
	}

	//Η διεργασία γονές περιμένει να τερματίσουν όλα τα παιδιά και τυπώνει τον τρόπο με τον οποίο τερματίστηκαν.
	for(i=0; i<N; i++)
	{
		pid_t wpid = waitpid(pid[i] ,&child_status, 0);
		if (WIFEXITED(child_status))
    	{
        	printf("Child%d terminated with exit status %d %d\n\n", pid[i], WEXITSTATUS(child_status), wpid);
    	}
    	else
    	{
      		printf("Child%d terminated abnormally\n\n", wpid);
    	}
	}
	//Τύπωση τελικού αθροίσματος
	printf("sum = %d \n\n",*sum_pointer);
	shmdt(p); //Αποδέσμευση shared memory
	shmctl(shmid, IPC_RMID, 0);

	return(0);
}