#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h> 
#include <semaphore.h>
#include <fcntl.h>

typedef sem_t Semaphore; 

//Ορισμός σημαφόρων ως global
Semaphore *s0; 
Semaphore *s1;

int main()
{
pid_t pid_pq[2]; //Πίνακας αποθήκευσης pid διεργασιών P και Q
pid_t pid[10]; //Πίνακας αποθήκευσης pid διεργασιών παιδιών των P και Q
int child_status;

//Άνοιγμα σεμαφόρων
s0 = sem_open ("Sem1", O_CREAT | O_EXCL, 0644, 0); 
s1 = sem_open ("Sem2", O_CREAT | O_EXCL, 0644, 0);

//Δημιουργία διεργασίας P
pid_pq[0] = fork();


//Αν είσαι η διεργασία P
if (pid_pq[0] == 0)
{
    //Ξεκινά την διεργασία E1
    pid[1] = fork();
    //Αν είσαι η διεργασία E1
    if (pid[1] == 0)
    {
        system("ls -l"); //Εκτελεση εντολής συστήματος και έξοδος
        exit(1);
    }
    waitpid(pid[1], &child_status, 0); //Αναμονή μέχρι να τελείωσει η E1 ώστε να ξεκινήσει η επόμενη, η Ε5, σύμφωνα με την λειτουργία του begin;
    if (WIFEXITED(child_status)) //If για την εκτύπωση του πως τερματίστηκε η E1
    {
        printf("Process%d terminated with exit status %d %d\n", pid[1], WEXITSTATUS(child_status), pid[1]);
    }
    else
    {
        printf("Process%d terminated abnormally\n", pid[1]);
    }
    //Το πρόγραμμα συνεχίζει ακριβώς με τον ίδιο τρόπο όπως το παραπάνω κομμάτι με εξαίρεση τους σημαφόρους όπου υπάρχουν σχόλια.
    sem_post(s0); //s0 γίνεται 1 ώστε να μπορεί να ξεκινήσει η Ε2 αφού πλέον τελείωσε η Ε1
    pid[5] = fork();
    if (pid[5] == 0)
    {
        system("ls -l");
        exit(5);
    }
    waitpid(pid[5], &child_status, 0);
    if (WIFEXITED(child_status))
    {
        printf("Process%d terminated with exit status %d %d\n", pid[5], WEXITSTATUS(child_status), pid[5]);
    }
    else
    {
        printf("Process%d terminated abnormally\n", pid[5]);
    }
    sem_post(s1); //s1 γίνεται 1 ώστε να μπορεί να ξεκινήσει η Ε6 αφού πλέον τελείωσε η Ε5
    pid[8] = fork();
    if (pid[8] == 0)
    {
        system("ls -l");
        exit(8);
    }
    waitpid(pid[8], &child_status, 0);
    if (WIFEXITED(child_status))
    {
        printf("Process%d terminated with exit status %d %d\n", pid[8], WEXITSTATUS(child_status), pid[8]);
    }
    else
    {
        printf("Process%d terminated abnormally\n", pid[8]);
    }
    pid[9] = fork();
    if (pid[9] == 0)
    {
        system("ls -l");
        exit(9);
    }
    waitpid(pid[9], &child_status, 0);
    if (WIFEXITED(child_status))
    {
        printf("Process%d terminated with exit status %d %d\n", pid[9], WEXITSTATUS(child_status), pid[9]);
    }
    else
    {
        printf("Process%d terminated abnormally\n", pid[9]);
    }
    sem_wait(s0); //Η E7 απαίτει ο s0 να είναι 1 ώστε να ξεκινήσει, δηλαδή να τελείωσει η Ε6
    pid[7] = fork();
    if (pid[7] == 0)
    {
        system("ls -l");
        exit(7);
    }
    waitpid(pid[7], &child_status, 0);
    if (WIFEXITED(child_status))
    {
        printf("Process%d terminated with exit status %d %d\n", pid[7], WEXITSTATUS(child_status), pid[7]);
    }
    else
    {
        printf("Process%d terminated abnormally\n", pid[7]);
    }
    sem_post(s1); //s1 γίνεται 1 ώστε να μπορεί να ξεκινήσει η Ε4 αφού πλέον τελείωσε η Ε7
    exit(10);
}

//Δημιουργία της Q
pid_pq[1] = fork();

if (pid_pq[1] == 0)
{
    sem_wait(s0); //Η E2 απαίτει ο s0 να είναι 1 ώστε να ξεκινήσει, δηλαδή να τελείωσει η Ε1
    pid[2] = fork();
    if (pid[2] == 0)
    {
        system("ls -l");
        exit(2);
    }
    waitpid(pid[2], &child_status, 0);
    if (WIFEXITED(child_status))
    {
        printf("Process%d terminated with exit status %d %d\n", pid[2], WEXITSTATUS(child_status), pid[2]);
    }
    else
    {
        printf("Process%d terminated abnormally\n", pid[2]);
    }
    pid[3] = fork();
    if (pid[3] == 0)
    {
        system("ls -l");
        exit(3);
    }
    waitpid(pid[3], &child_status, 0);
    if (WIFEXITED(child_status))
    {
        printf("Process%d terminated with exit status %d %d\n", pid[3], WEXITSTATUS(child_status), pid[3]);
    }
    else
    {
        printf("Process%d terminated abnormally\n", pid[3]);
    }
    sem_wait(s1); //Η E6 απαίτει ο s1 να είναι 1 ώστε να ξεκινήσει, δηλαδή να τελείωσει η Ε5
    pid[6] = fork();
    if (pid[6] == 0)
    {
        system("ls -l");
        exit(6);
    }
    waitpid(pid[6], &child_status, 0);
    if (WIFEXITED(child_status))
    {
        printf("Process%d terminated with exit status %d %d\n", pid[6], WEXITSTATUS(child_status), pid[6]);
    }
    else
    {
        printf("Process%d terminated abnormally\n", pid[6]);
    }
    sem_post(s0); //s0 γίνεται 1 ώστε να μπορεί να ξεκινήσει η Ε7 αφού πλέον τελείωσε η Ε6
    sem_wait(s1); //Η E4 απαίτει ο s1 να είναι 1 ώστε να ξεκινήσει, δηλαδή να τελείωσει η Ε7
    pid[4] = fork();
    if (pid[4] == 0)
    {
        system("ls -l");
        exit(4);
    }
    waitpid(pid[4], &child_status, 0);
    if (WIFEXITED(child_status))
    {
        printf("Process%d terminated with exit status %d %d\n", pid[4], WEXITSTATUS(child_status), pid[4]);
    }
    else
    {
        printf("Process%d terminated abnormally\n", pid[4]);
    }
    exit(11);
}

//Τύπωση τερματισμού P και Q
waitpid(pid_pq[0], &child_status, 0);
    if (WIFEXITED(child_status))
    {
        printf("Process P terminated with exit status %d %d\n", WEXITSTATUS(child_status), pid_pq[0]);
    }
    else
    {
        printf("Process P terminated abnormally\n");
    }

waitpid(pid_pq[1], &child_status, 0);
    if (WIFEXITED(child_status))
    {
        printf("Process Q terminated with exit status %d %d\n", WEXITSTATUS(child_status), pid_pq[1]);
    }
    else
    {
        printf("Process Q terminated abnormally\n");
    }

//Κλείσιμο σεμαφόρων
sem_unlink("Sem1");
sem_close(s0);
sem_unlink("Sem2");
sem_close(s1);

printf("My job here is done.\n");
return (0);
}