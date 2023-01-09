#include <unistd.h>  
#include <stdio.h>         
#include <stdlib.h>        
#include <sys/types.h>      
#include <sys/shm.h>        
#include <errno.h>          
#include <semaphore.h>      
#include <fcntl.h>  
#include <time.h>    

typedef sem_t Semaphore;

//Ορισμός σημαφόρων ως global, 1 για κάθε καπνιστή και 1 για τον πωλητή
Semaphore *smoker0;
Semaphore *smoker1;
Semaphore *smoker2;
Semaphore *seller;

//Τύπωση τραπεζιού και προιόντων που περιέχει επάνω σχηματικά.
void print_Table(int n, int m, int kapnos, int xarti, int spirta) 
{ 
    int i, j; 
    for (i = 1; i <= n; i++) 
    { 
        for (j = 1; j <= m; j++) 
        { 
            if ((i==1 || i==n || j==1 || j==m) && i != 2)             
                printf("*");             
            else if (i==2 && j==2)
                printf("KAPNOS");
            else if (i==2 && j==10)
                printf("XARTI");
            else if (i==2 && j==17)
                printf("SPIRTA");
            else if ((i==3 || j==11 || j==23) && i != 2)
                printf("*");
            else if (i==4 && j==4)
                printf("%d", kapnos);
            else if (i==4 && j==16)
                printf("%d", xarti);
            else if (i==4 && j==29)
                printf("%d", spirta);
            else
                printf(" ");             
        } 
        printf("\n"); 
    } 
  
} 

//Function όπου χρησιμοποιεί ο καπνιστής για να πάρει υλικά απο το τραπέζι ανάλογα με το τι υπάρχει, το οποίο καθορίζεται απο το r
int TakeMaterialsFromTable(int *p, int r)
{   //Θέση υλικού στο shared memory
    int *kapnos = p;
    int *xarti = (p+1);
    int *spirta = (p+2);
    //Τι υλίκο παίρνει
    if (r==0){
        *xarti = 0;
        *spirta = 0;
    }
    else if(r==1) {
        *kapnos = 0;
        *spirta = 0;
    }
    else {
        *kapnos = 0;
        *xarti =0;
    }
}

//Function όπου χρησιμοποιεί ο πωλητής για να βάλει υλικά στο τραπέζι, το οποίο καθορίζεται απο το r
int DecideWhichMaterialsToSell(int *p, int r)
{   //Θέση υλικού στο shared memory
    int *kapnos = p;
    int *xarti = (p + 1);
    int *spirta = (p + 2);
    //Τι υλίκο βάζει
    if (r==0){
        *kapnos = 0;
        *xarti = 1;
        *spirta = 1;
        print_Table(7, 38, *p, *(p+1), *(p+2));
    }
    else if(r==1) {
        *kapnos = 1;
        *xarti = 0;
        *spirta = 1;
        print_Table(7, 38, *p, *(p+1), *(p+2));
    }
    else {
        *kapnos = 1;
        *xarti = 1;
        *spirta = 0;
        print_Table(7, 38, *p, *(p+1), *(p+2));
    }
}

int main(){
    //Αρχικοποίηση μεταβλητών και shared memory
    key_t shmkey; 
	int pid[3];
	int *p;
	int shmid;  
	int i; 

    shmkey = 9090;       
    shmid = shmget (shmkey, 3, 0644 | IPC_CREAT);
    
      if (shmid < 0)                           
    {
        perror ("shmget\n");
        exit (1);
    }

    p  = (int *) shmat (shmid, NULL, 0);   

    printf(" Initial value of shared variable: %d\n\n", *p);
	
    //Άνοιγμα σεμαφόρων
    smoker0 = sem_open ("Sem0", O_CREAT | O_EXCL, 0644, 0); 
    smoker1 = sem_open ("Sem1", O_CREAT | O_EXCL, 0644, 0);  
    smoker2 = sem_open ("Sem2", O_CREAT | O_EXCL, 0644, 0);
    seller = sem_open ("Sem3", O_CREAT | O_EXCL, 0644, 0);

    //seller = 1 για να τοποθετήσει τα πρώτα υλικά o seller στο τραπέζι
    sem_post(seller);

    //Δημιουγία καπνιστών
    for (i=0; i<3; i++)
	{
		pid[i] = fork();
		

		if (pid[i] < 0) 
    	{
       		printf ("Fork error.\n");
       		return (-1);  
    	}
    }
    //Άν είσαι ο καπνιστής 1
	if (pid[0] == 0)
    {
    while(1 != 0){
        //Αναμονή για αφύπνιση από seller αν τοποθετηθούν τα υλικά που του λείπουν
        sem_wait (smoker0);      
        printf (" Eimai o kapnisths 1 kai xreiazomai xarti kai spirta \n\n");
        TakeMaterialsFromTable(p, 0);  //Παίρνει τα υλικά
        sem_post (seller); //Αφύπνιση seller
    }
    }
    else if (pid[0] != 0 && pid[1] == 0) //Άν είσαι ο καπνιστής 2
    {   
    while(1 != 0){
       sem_wait(smoker1);
       printf (" Eimai o kapnisths 2 kai xreiazomai kapno kai spirta \n\n");
       TakeMaterialsFromTable(p, 1);
       sem_post (seller);
    }
    }
    else if (pid[0] != 0 && pid[1] != 0 && pid[2] == 0) //Άν είσαι ο καπνιστής 3
    {
    while(1 != 0)
    {
       sem_wait (smoker2);
       printf (" Eimai o kapnisths 3 kai xreiazomai kapno kai xarti \n\n");
       TakeMaterialsFromTable(p, 2);  
       sem_post (seller);
    }
    }

    //Αρχικοποίηση r για χρήση στην τυχαία επιλογή
    int r = 0;
	for (i = 0; i < 10; i++)
	{   
        sem_wait(seller); //Αναμόνη για αφύπνιση απο smoker (με εξαίρεση την πρώτη φόρα που θα τρέξει)
        srand(time(NULL) + i); //Τυχαία επιλογή μέσω της τιμής του τρέχων χρόνου και του i
        r = rand() % 3; //mod 3 γιατί έχουμε 3 επιλογές
        printf("o pwlhths topothetei ta prointa ston pagko: \n\n");
        DecideWhichMaterialsToSell(p, r); //Τοποθέτηση προιόντων
        //Αφύπνιση κατάλληλου smoker
        if (r == 0)
            sem_post (smoker0); 
        else if (r == 1)
            sem_post (smoker1);
        else
            sem_post (smoker2);
        }
	//Κλείσιμο σεμαφόρων
    sem_unlink ("Sem0");   
    sem_close(smoker0);  
    sem_unlink ("Sem1");   
    sem_close(smoker1);  
    sem_unlink ("Sem2");   
    sem_close(smoker2); 
    sem_unlink ("Sem3");
    sem_close(seller);  

    shmdt(p); //Αποδέσμευση shared memory
	shmctl(shmid, IPC_RMID, 0);

return(0);
}

