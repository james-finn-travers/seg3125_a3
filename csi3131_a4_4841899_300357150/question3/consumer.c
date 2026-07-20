#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>


int main(int argc, char* argv[]){
    struct stat shm_stats;
    const char *name = "/UNIQUE_CATALAN_123";
    int shm_fd = shm_open(name, O_RDONLY,0666);
    if (fstat(shm_fd, &shm_stats) == -1){perror("fstat");return 1;}
    size_t n = shm_stats.st_size / sizeof(long);
    long *ptr = mmap(0,n*sizeof(long),PROT_READ,MAP_SHARED,shm_fd,0);
    printf("Consumer read: ");
    for (int i = 0; i<n; i++)printf("%ld ", ptr[i]);
    printf("\n");
    printf("\n");
    munmap(ptr, n*sizeof(long));
    shm_unlink(name);
    return 0;
}