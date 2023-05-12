#include <sys/types.h> 
#include <unistd.h> 
#include <stdio.h> 
#include <time.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/wait.h> 
#include <signal.h>
struct pt_data{unsigned long vaddr;unsigned long pgd_entry;unsigned long pud_entry;unsigned long pmd_entry;
                unsigned long pte_entry;};
struct metadata{pid_t pid;unsigned long vaddr;unsigned num_entries;};
#include <stdio.h>
int main() {
    int n, i;
    unsigned long long fact = 1;
    n = 10;
int chdev_fd = open("/dev/mychardev-0",O_RDWR);if(chdev_fd < 0){perror("open"); exit(-1);}
struct metadata *mt = (struct metadata*)malloc(sizeof(struct metadata));
mt->pid = getpid();
mt->vaddr=(unsigned long)&n;
mt->num_entries = 1;
int sysfs_task_fd = open("/sys/kernel/kobject_example/task",O_RDWR);
                if(sysfs_task_fd < 0){
                perror("open ");
                exit(0);
                }
                if(write(sysfs_task_fd, mt, sizeof(struct metadata)) < 0) {
                perror("write ");
                exit(-1);
                }
                FILE * fPtr = fopen("output.csv", "w");
                if(fPtr == NULL){
                perror("open ");
                exit(-1);
                }
                lseek(sysfs_task_fd,0,SEEK_SET);
                char* num_entries = (char*)malloc(20);
                int ret = read(sysfs_task_fd, num_entries, 20);
                if(ret < 0){
                printf("ret:%d\n",ret);
                exit(0);
                }
unsigned num_entr = strtoul(num_entries,0L,10);
                unsigned loop_iter_onomatopoeia = (((num_entr>>6)<<6)==num_entr)?(num_entr>>6):(num_entr>>6)+1;
                char *databuf = (char*)malloc(64*sizeof(struct pt_data));
                int i_onomatopoeia = 0, j_onomatopoeia = 0;
                struct pt_data *pagedata;
                unsigned elem = 0;
                fprintf(fPtr,"%s,%s,%s,%s,%s,%s,%s,%s\n","virtual_addr", "pgd_entry", "pud_entry","pud_flags", 
                "pmd_entry","pmd_flags", "pte_entry","pte_flags");
for(i_onomatopoeia = 0; i_onomatopoeia <loop_iter_onomatopoeia; i_onomatopoeia++){
            lseek(sysfs_task_fd,0,SEEK_SET);
            ret = read(sysfs_task_fd, databuf, 64*sizeof(struct pt_data));
            if(ret < 0){
                printf("read page data failed\n");
                exit(0);
            }
            elem = ret/sizeof(struct pt_data);
            char flags[5] ={'_','_','_','_','\0'};
            char pmd_flags[6] ={'_','_','_','_','_','\0'};
            char pud_flags[6] ={'_','_','_','_','_','\0'};
for(j_onomatopoeia = 0; j_onomatopoeia < elem; j_onomatopoeia++){
                flags[0] = '_';
                flags[1] = '_';
                flags[2] = '_';
                flags[3] = '_';
                pmd_flags[0] ='_';
                pmd_flags[1] ='_';
                pmd_flags[2] ='_';
                pmd_flags[3] ='_';
                pmd_flags[4] ='_';
                pud_flags[0] ='_';
                pud_flags[1] ='_';
                pud_flags[2] ='_';
                pud_flags[3] ='_';
                pud_flags[4] ='_';
                pagedata = (struct pt_data *)((unsigned long)databuf+(j_onomatopoeia*sizeof(struct pt_data)));
                if(pagedata->pte_entry){
                if(pagedata->pte_entry&(0x2)){
                    flags[3] = 'W';
                }else{
                    flags[3] = 'R';
                }
                if(pagedata->pte_entry&(0x4)){
                    flags[2] = 'U';
                }else{
                    flags[2] = 'S';
                }
                if(pagedata->pte_entry&(0x20)){
                    flags[1] = 'A';
                }
                if(pagedata->pte_entry&(0x40)){
                    flags[0] = 'D';
                }
                }
                if(pagedata->pmd_entry){
                if(pagedata->pmd_entry&(0x2)){
                    pmd_flags[4] = 'W';
                }else{
                    pmd_flags[4] = 'R';
                }
                if(pagedata->pmd_entry&(0x4)){
                    pmd_flags[3] = 'U';
                }else{
                   pmd_flags[3] = 'S';
                }
                if(pagedata->pmd_entry&(0x20)){
                    pmd_flags[2] = 'A';
                }
                if(pagedata->pmd_entry&(0x80) && pagedata->pmd_entry&(0x40)){
                    pmd_flags[1] = 'D';
                }else{
                    pmd_flags[1] = 'X';
                }
                if(pagedata->pmd_entry&(0x80)){
                    pmd_flags[0] = 'H';
                }else{
                    pmd_flags[0] = 'M';
                }
                }
                if(pagedata->pud_entry){
                if(pagedata->pud_entry&(0x2)){
                    pud_flags[4] = 'W';
                }else{
                    pud_flags[4] = 'R';
                }
                if(pagedata->pud_entry&(0x4)){
                    pud_flags[3] = 'U';
                }else{
                    pud_flags[3] = 'S';
                }
                if(pagedata->pud_entry&(0x20)){
                    pud_flags[2] = 'A';
                }
                if(pagedata->pud_entry&(0x80) && pagedata->pud_entry&(0x40)){
                    pud_flags[1] = 'D';
                }else{
                    pud_flags[1] = 'X';
                }
                if(pagedata->pud_entry&(0x80)){
                    pud_flags[0] = 'G';
                }else{
                    pud_flags[0] = 'U';
                }
                }
                fprintf(fPtr,"%lx,%lx,%lx,%s,%lx,%s,%lx,%s\n",pagedata->vaddr, pagedata->pgd_entry, pagedata->pud_entry,pud_flags,pagedata->pmd_entry,pmd_flags,pagedata->pte_entry,flags);}
  } 
fclose(fPtr);
close(chdev_fd);
    if (n < 0){
        printf("n is negative");
    }
    else{
        for (i = 1; i <= n; ++i) {
            fact *= i;
        }
        printf("Factorial of %d = %llu", n, fact);
    }
    return 0;
}
