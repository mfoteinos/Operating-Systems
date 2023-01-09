#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#define N 30

int find_maximum(int *p) //Function για την εύρεση του μέγιστου για την επιλογή number/ticket.
{
    int i;
    int max = 0;

    for (i = 0; i < N; i++)
        if (*(p + i) >= max)
            max = *(p + i);
       
    return max;
}

int main()
{

int *sa_pointer; //Ο pointer που θα δείχνει στην αρχή του shared memory αλλα και του shared memory array
int j; //Μετρητές
int i;
int pid[N]; //Array για την αποθήκευση των pid των παιδιών
int child_status; //Αποθήκευση κατάστασης ενός παιδιού

//Αρχικοποιούμε τα array στο shared memory
int shmid = shmget (9090, 3 * N, 0644|IPC_CREAT); //Διαλέγουμε ένα κλείδι που πιστεύουμε οτι δεν χρησιμοποιείται

if(shmid < 0) //Έλεγχος σφάλματος 
{
    perror("shmget\n");
    exit(1);
}
//Κάνουμε attach του pointer με το shared memory
sa_pointer = (int *)shmat(shmid, (void*)0, 0);

/*Οι παρακάτω προσαρμογές γίνονται επειδή αποφασίσαμε να χωρίσουμε το 3N μέγεθους shared memory σε
τρία μέρη, με τις Ν πρώτες θέσεις να χρησιμοποιούνται για το shared array, οι επόμενες Ν για το
array του choosing και οι υπόλοιπες Ν για το number*/

int *choosing_pointer = sa_pointer + N; //Προσαρμογή του sa_pointer ωστε να δείχνει στις θέσεις που χρησιμοποιούνται για το choosing
int *number_pointer = sa_pointer + 2 * N; //Προσαρμογή του sa_pointer ωστε να δείχνει στις θέσεις που χρησιμοποιούνται για το number


for (i=0; i<(3 * N); i++) //Θέτουμε στο 0 όλες τις θέσεις του shared memory στο 0.
{
    *(sa_pointer + i) = 0;
}

for (i = 0; i < N; i++) //For loop για την διεργασία-γονεάς, ώστε να εκτελέσει N fork.
{
    pid[i] = fork(); //Εκτέλεση fork. Το pid χρησιμοποιείται από την διεργασία-γονέας για να αποθηκέυει το pid των παιδιών της.

    if (pid[i] < 0)
    {
        printf("Fork error.\n");
        return (-1);
    }

    if (pid[i] == 0) //Έλεγχος, ώστε μονο αν η διεργασία είναι διεργασία-παιδί θα εισέλθει στο if, αφου θα έχει pid=0.
    {

        *(choosing_pointer + i) = 1; //choosing = true
        *(number_pointer + i) = find_maximum + 1; //Η διεργασία παίρνει number/ticket.
        *(choosing_pointer + i) = 0; //choosing = false

        for (j = 0; j<N; j++)
        {
            while (*(choosing_pointer + j) == 1) //Όσο το choosing του j είναι true, μην κάνεις τίποτα
            {
                sleep(1);
            }

            /*Όσο το number του j δεν είναι μηδέν και είναι μικρότερο απο το number σου (number του i) ή
            όσο το number του j είναι ίσο με το δικό σου άλλα το j έιναι μικρότερο απο εσένα (το i), μην
            κάνεις τίποτα. Αλλιώς, η διεργασία μπαίνει στο κρίσιμο τμήμα.*/
            while (*(number_pointer + j) != 0 && *(number_pointer + j) < *(number_pointer + i) || (*(number_pointer + j) == *(number_pointer + i) && j < i) )
            { 
               sleep(1);
            }
        }
        /*Τα print έχουν τοποθετηθεί για έλεγχο. Όλες οι θέσεις του shared array πρέπει να έχουν την ίδια τιμή,
        για αυτό ελέγχουμε την πρώτη και την τελευταία ώστε να είμαστε σίγουροι ότι όλα δουλεύουν σωστά.*/
        printf("I am child %d and this is position 0 of the shared array before I added: %d \n", i, *(sa_pointer));
        for (j=0; j<N; j++) //Προσθήκη i σε κάθε θέση του shared array.
        {
           *(sa_pointer + j) += i;
        }
        printf("I am child %d and this is position 29 of the shared array after I added: %d \n", i, *(sa_pointer + 29));

        *(number_pointer + i) = 0; //Θέτει το number στο μηδέν αφού έχει βγει απο το κρίσιμο τμήμα.

        exit(i); //Τερματισμός διεργασίας-παιδού
    }
}

for (i=0; i<N; i++) //Η διεργασία-γονέας περιμένει να τερματιστούν όλες οι διεργασίες-παιδιά της και τυπώνει αν τερματίστηκαν σωστά.
{
    pid_t wpid = waitpid(pid[i] ,&child_status, 0);

    if (WIFEXITED(child_status))
    {
        printf("Child%d terminated with exit status %d %d\n", pid[i], WEXITSTATUS(child_status), wpid);
    }
    else
    {
      printf("Child%d terminated abnormally\n", wpid);
    }
}

//Shared memory detach
shmdt(sa_pointer);
shmctl(shmid, IPC_RMID, 0);

return (0);

}

