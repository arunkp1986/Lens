#from tkinter import *
import tkinter as tk
from tkinter.ttk import *
import subprocess
from ttkthemes import ThemedStyle
import pandas as pd
from pandastable import Table, TableModel, config
from tempfile import mkstemp
from shutil import move, copymode
from os import fdopen, remove
import os

root = tk.Tk()
root.title('Lens')
logo = tk.PhotoImage(file='../images/log.gif')
logo = logo.subsample(2)
root.tk.call('wm', 'iconphoto', root._w, logo)
style = ThemedStyle(root)
style.set_theme("clearlooks")

frame = tk.Frame(root)
explanation = """Lens is an educational tool for\n virtual memory study in Linux Intel x86-64 systems."""
w1 = tk.Label(frame,padx=10, text=explanation)
w1.grid(row=0, column=1)
w1.config(font=("Times", 18))
w1.config(fg="#2b0099")


w2 = tk.Label(frame, image=logo)
w2.grid(row=0, column=0)
w2.config(highlightthickness=1)
w2.config(highlightbackground="#3e1ca8")
#w2.config(highlightbackground="#ffba13")

frame.grid(row =0, column = 0)

tb = tk.Text(root,height=20,width=100,wrap='word')
tb.insert(1.0,r"""#include <stdlib.h>
#include <stdio.h>
int main(){
 int size = 16;
 char *ptr = malloc(size * sizeof(char));
 printf("virtual addr of size variable: %p\n", &size);
 printf("virtual addr of malloc'd memory: %p\n", ptr);
 printf("virtual addr of main() function: %p\n", main);
 free(ptr);
 return 0;
 }""")
#text_box.pack(side=tk.LEFT,expand=True)
tb.grid(row=1, column=0, sticky="nsew")


sb = tk.Scrollbar(root)
#sb.pack(side=tk.RIGHT, fill=tk.BOTH)
sb.grid(row=1, column=1, sticky="nsw")

tb.config(yscrollcommand=sb.set)
sb.config(command=tb.yview)


fm2 = tk.Frame(root)

l1 = tk.Label(fm2, text="line number")
l1.grid(row=0, column=0)

e1 = tk.Entry(fm2, width = 50)
e1.grid(row=0, column=1)
e1.insert(0,"enter line number containing variable,eg 1,2 etc.")

l2 = tk.Label(fm2, text="variable address")
l2.grid(row=1, column=0)

e2 = tk.Entry(fm2, width = 50)
e2.grid(row=1, column=1)
e2.insert(0,"enter address of the variables, eg: &a, ptr etc.")

l3 = tk.Label(fm2, text="Number of entries")
l3.grid(row=2, column=0)

e3 = tk.Entry(fm2, width = 50)
e3.grid(row=2, column=1)
e3.insert(0,"enter number of pages table entries to show, eg: 1, 4 etc.")

def edel():
    e1.delete(0,tk.END)
    e2.delete(0,tk.END)
    e3.delete(0,tk.END)
    #tb.delete('1.0',tk.END)

def eshow():
    l4 = tk.Label(root, text=e1.get())
    l4.grid(row=1, column=2)

fm3 = tk.LabelFrame(root, text="output:", bg='#f0f0f0',padx=50, borderwidth = 0,highlightthickness = 0, font=(20))

def tsave():
    linenumber = 0
    code = tb.get('1.0', 'end')
    variableaddress = e2.get()
    variableentries = e3.get()
    #print("variableaddress", variableaddress) 
    if(not variableaddress.replace("&","") in code):
        l4 = tk.Label(fm3, text="Enter variable name not present", bg='#f0f0f0')
        l4.grid(row=0, column=0)
        fm3.grid(row=4, column=0, sticky=tk.NSEW)
        return 0

    with open("program.c","w") as f:
        f.write(code)
    #print("after write")
    try:
        linenumber = int(e1.get())
    except:
        #print("line number not int")
        l4 = tk.Label(fm3, text="Enter a valid line number", bg='#f0f0f0')
        l4.grid(row=0, column=0)
        fm3.grid(row=4, column=0, sticky=tk.NSEW)
        return 0

    #print("linenumber", linenumber)
    fd, abspath = mkstemp()
    foundline = 0
    with open("program.c","r") as f, fdopen(fd,"w") as g:
        count = 0
        gotmain = 0
        opencount = 0
        for line in f:
            #print(line)
            if(count == 0):
                g.write("""#include <sys/types.h> \n#include <unistd.h> \n#include <stdio.h> \n#include <time.h> \n#include <sys/stat.h> \n#include <fcntl.h> \n#include <stdlib.h> \n#include <string.h> \n#include <sys/wait.h> \n#include <signal.h>""")
                g.write("\n")
                g.write("""struct pt_data{
                unsigned long vaddr;
                unsigned long pgd_entry;
                unsigned pgd_flags;
                unsigned long pud_entry;
                unsigned pud_flags;
                unsigned long pmd_entry;
                unsigned pmd_flags;
                unsigned long pte_entry;
                unsigned pte_flags;};\nstruct metadata{pid_t pid;unsigned long vaddr;unsigned num_entries;};""")
                g.write("\n")
            count += 1
            #if("main" in line):
                #gotmain = 1
            #if(gotmain and ("{" in line)):
                #opencount += 1;
            #elif(gotmain and ("}" in line)):
                #opencount -= 1;
            #if(opencount == 0 and ("}" in line)):
                #g.write('sleep(2);')
                #g.write("\n")
                #gotmain = 0
            g.write(line)
            if(count == linenumber):
                foundline = 1
                #g.write(r'printf("onomatopoeia:%p\n",{0});'.format(variableaddress))
                g.write("""int chdev_fd = open("/dev/mychardev-0",O_RDWR);if(chdev_fd < 0){perror("open"); exit(-1);}\n""")
                g.write("""struct metadata *mt = (struct metadata*)malloc(sizeof(struct metadata));\nmt->pid = getpid();\nmt->vaddr=(unsigned long){0};\nmt->num_entries = {1};""".format(variableaddress,variableentries))
                g.write("\n")
                g.write(r"""int sysfs_task_fd = open("/sys/kernel/kobject_example/task",O_RDWR);
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
                }""")
                g.write("\n")
                g.write(r"""unsigned num_entr = strtoul(num_entries,0L,10);
                unsigned loop_iter_onomatopoeia = (((num_entr>>6)<<6)==num_entr)?(num_entr>>6):(num_entr>>6)+1;
                char *databuf = (char*)malloc(64*sizeof(struct pt_data));
                int i_onomatopoeia = 0, j_onomatopoeia = 0;
                struct pt_data *pagedata;
                unsigned elem = 0;
                fprintf(fPtr,"%s,%s,%s,%s,%s,%s,%s,%s,%s\n","virtual_addr", "pgd_entry","pgd_flags","pud_entry","pud_flags", 
                "pmd_entry","pmd_flags", "pte_entry","pte_flags");""")
                g.write("\n")
                g.write(r"""for(i_onomatopoeia = 0; i_onomatopoeia <loop_iter_onomatopoeia; i_onomatopoeia++){
            lseek(sysfs_task_fd,0,SEEK_SET);
            ret = read(sysfs_task_fd, databuf, 64*sizeof(struct pt_data));
            if(ret < 0){
                printf("read page data failed\n");
                exit(0);
            }
            elem = ret/sizeof(struct pt_data);
            char flags[5] ={'_','_','_','_','\0'};
            char pgd_flags[4] ={'_','_','_','\0'};
            char pmd_flags[6] ={'_','_','_','_','_','\0'};
            char pud_flags[6] ={'_','_','_','_','_','\0'};""")
                g.write("\n")
                g.write(r"""for(j_onomatopoeia = 0; j_onomatopoeia < elem; j_onomatopoeia++){
                flags[0] = '_';
                flags[1] = '_';
                flags[2] = '_';
                flags[3] = '_';
                pgd_flags[0] = '_';
                pgd_flags[1] = '_';
                pgd_flags[2] = '_';
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
                if(pagedata->pte_flags&(0x2)){
                    flags[3] = 'W';
                }else{
                    flags[3] = 'R';
                }
                if(pagedata->pte_flags&(0x4)){
                    flags[2] = 'U';
                }else{
                    flags[2] = 'S';
                }
                if(pagedata->pte_flags&(0x20)){
                    flags[1] = 'A';
                }
                if(pagedata->pte_flags&(0x40)){
                    flags[0] = 'D';
                }
                }
                if(pagedata->pmd_entry){
                if(pagedata->pmd_flags&(0x2)){
                    pmd_flags[4] = 'W';
                }else{
                    pmd_flags[4] = 'R';
                }
                if(pagedata->pmd_flags&(0x4)){
                    pmd_flags[3] = 'U';
                }else{
                   pmd_flags[3] = 'S';
                }
                if(pagedata->pmd_flags&(0x20)){
                    pmd_flags[2] = 'A';
                }
                if(pagedata->pmd_flags&(0x80) && pagedata->pmd_flags&(0x40)){
                    pmd_flags[1] = 'D';
                }
                if(pagedata->pmd_flags&(0x80)){
                    pmd_flags[0] = 'H';
                }else{
                    pmd_flags[0] = 'M';
                }
                }
                if(pagedata->pud_entry){
                if(pagedata->pud_flags&(0x2)){
                    pud_flags[4] = 'W';
                }else{
                    pud_flags[4] = 'R';
                }
                if(pagedata->pud_flags&(0x4)){
                    pud_flags[3] = 'U';
                }else{
                    pud_flags[3] = 'S';
                }
                if(pagedata->pud_flags&(0x20)){
                    pud_flags[2] = 'A';
                }
                if(pagedata->pud_flags&(0x80) && pagedata->pud_flags&(0x40)){
                    pud_flags[1] = 'D';
                }

                if(pagedata->pud_flags&(0x80)){
                    pud_flags[0] = 'G';
                }else{
                    pud_flags[0] = 'M';
                }
                }
                if(pagedata->pgd_entry){
                if(pagedata->pgd_flags&(0x2)){
                    pgd_flags[2] = 'W';
                }else{
                    pgd_flags[2] = 'R';
                }
                if(pagedata->pgd_flags&(0x4)){
                    pgd_flags[1] = 'U';
                }else{
                    pgd_flags[1] = 'S';
                }
                if(pagedata->pgd_flags&(0x20)){
                    pgd_flags[0] = 'A';
                }
                }
                fprintf(fPtr,"%lx,%lx,%s,%lx,%s,%lx,%s,%lx,%s\n",pagedata->vaddr,pagedata->pgd_entry,pgd_flags, pagedata->pud_entry,pud_flags,pagedata->pmd_entry,pmd_flags,pagedata->pte_entry,flags);}""")
                g.write("\n")
                g.write("""  } \nfclose(fPtr);\nclose(chdev_fd);\n""")
    if(foundline == 0):
        l4 = tk.Label(fm3, text="check line number", bg='#f0f0f0')
        l4.grid(row=0, column=0)
        fm3.grid(row=4, column=0, sticky=tk.NSEW)
        return 0

    copymode("program.c", abspath)
    move(abspath, "program_tmp.c")
    directory = os.getcwd()
    #print(directory+"/run.sh")
    rc = subprocess.call("./run.sh")
    
    with open("output.out","r") as f2:
        out = f2.read()
    l4 = tk.Label(fm3, text=out, bg='#f0f0f0')
    l4.grid(row=0, column=0)
    fm3.grid(row=4, column=0, sticky=tk.NSEW)

check_dirty = tk.IntVar()
check_write = tk.IntVar()


def fmapping():
    top = tk.Toplevel()
    lbl = tk.Label(top, text="Virtual to Physical Mapping")
    lbl.pack()
    lbl.config(font=("Times", 18))
    lbl.config(fg="#2b0099")
    #lbl.config(bg="yellow")
    fm5 = tk.Frame(top)
    fm5.pack(fill='both', expand=True)
    #lbl1 = tk.Label(fm5, text=",")
    #lbl1.grid(row=0,column=1)
    #lbl1.config(font=("Times bold", 12))
    #lbl1.config(fg="#000000")
    lbl2 = tk.Label(fm5, text="Dirty(D),")
    lbl2.grid(row=0,column=0)
    lbl2.config(font=("Times bold", 12))
    lbl2.config(fg="#cc0000")
    lbl3 = tk.Label(fm5, text="Accessed(A),")
    lbl3.grid(row=0,column=1)
    lbl3.config(font=("Times bold", 12))
    lbl3.config(fg="#006400")
    lbl4 = tk.Label(fm5, text="User(U)/Supervisor(S),")
    lbl4.grid(row=0,column=2)
    lbl4.config(font=("Times bold", 12))
    lbl4.config(fg="#841ca8")
    lbl5 = tk.Label(fm5, text="Read(R)/Write(W),")
    lbl5.grid(row=0,column=3)
    lbl5.config(font=("Times bold", 12))
    lbl5.config(fg="#6e2626")
    lbl6 = tk.Label(fm5, text="Maps 4KB Page(M),2MB Page(H)[PMD],")
    lbl6.grid(row=0,column=4)
    lbl6.config(font=("Times bold", 12))
    lbl6.config(fg="#a81c40")
    lbl7 = tk.Label(fm5, text="1GB Page(G)[PUD]")
    lbl7.grid(row=0,column=5)
    lbl7.config(font=("Times bold", 12))
    lbl7.config(fg="#6e2626")


    #chb1 = tk.Checkbutton(top, text = "Dirty", variable = check_dirty, onvalue = 1,offvalue = 0)
    #chb1.deselect()
    #chb1.pack()

    fm4 = tk.Frame(top)
    fm4.pack(fill='both', expand=True)
    pd.set_option('colheader_justify', 'center')
    df = pd.read_csv("output.csv")
    #address = "7fffffffe000"
    #addressrange = []
    #for i in range(1):
     #   addr = int(address,16)+(i*4096)
      #  h = hex(addr)
       # addressrange.append(h[2:])
    #df_new = df[df["virtual_addr"].isin(addressrange)]
    #print(df_new)
    pt = Table(fm4, dataframe=df, showstatusbar=True)
    pt.columnformats['alignment']['virtual_addr'] = 'center'
    pt.columncolors['pud_flags'] = '#b0e0e6'
    pt.columncolors['pmd_flags'] = '#b0e0e6'
    pt.columncolors['pte_flags'] = '#b0e0e6'
    
    #mask_1 = ('DAUW'== df_new['pte_flags'])
    #mask_2 = ('_AUR'== df_new['pte_flags'])
    #print(mask_1)
    #pt.setColorByMask('pte_entry', mask_1, '#EC7063')
    #pt.setColorByMask('pte_entry', mask_2, '#58D68D')
    #options = config.load_options()
    #options['colheadercolor'] = 'green'
    #print(options)
    #config.apply_options(options, pt)
    #if(check_dirty.get())
    pt.show()

b1 = Button(fm2, text='Clear', width=10, command=edel)
b1.grid(row=3, column=0, padx=10, pady=10, sticky=tk.NSEW)

b2 = Button(fm2, text='Run',width=10, command=tsave)
b2.grid(row=3, column=1, padx=10, pady=10, sticky=tk.NSEW)

b3 = Button(fm2, text='Mapping', width=10, command=fmapping)
b3.grid(row=3, column=2, pady=10, sticky=tk.NSEW)

b4 = Button(fm2, text='Exit', width=10, command=root.destroy)
b4.grid(row=3, column=3, padx=10, pady=10, sticky=tk.NSEW)

#b3 = Button(root, text='Show', width=10, command=eshow)
#b3.grid(row=4, column=2)


fm2.grid(row =2, column = 0)

root.mainloop()
