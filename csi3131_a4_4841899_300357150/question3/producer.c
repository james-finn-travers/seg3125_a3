#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>


int main(int argc, char *argv[]){
    if (argc != 2){printf("Usage: ./producer <number_of_catalan_numbers>\n"); return 1;}
    int n = atoi(argv[1]);
    const char *name = "/UNIQUE_CATALAN_123";
    int shm_fd = shm_open(name,O_CREAT|O_RDWR|O_TRUNC, 0666);
    ftruncate(shm_fd,n*sizeof(long));
    long *ptr = mmap(0,n*sizeof(long),PROT_WRITE,MAP_SHARED,shm_fd,0);
    long cat[n];
    cat[0] = 1;
    for (int i = 1;i<n;i++){
        cat[i] = 0;
        for (int j = 0; j<i;j++)cat[i]+=cat[j]*cat[i-1-j];
    }
    for (int i = 0; i<=n; i++)ptr[i]=cat[i];  
    printf("Producer wrote %d Catalan numbers.\n", n);
    munmap(ptr, n * sizeof(long));
    close(shm_fd);
    return 0;
}