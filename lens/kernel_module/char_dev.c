/* This kernel module gives stack area details of child process of pid passed through character device
 * It hooks wake_up_new_task function to get task struct of process 
 * */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/moduleparam.h>
#include <linux/unistd.h>
#include <asm-generic/set_memory.h>
#include <linux/kallsyms.h>
#include <linux/ftrace.h>
#include <linux/linkage.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <linux/kprobes.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/ptrace.h>
#include <linux/list.h>
#include <linux/pid.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/kprobes.h>


static int mychardev_open(struct inode *inode, struct file *file);
static int mychardev_release(struct inode *inode, struct file *file);
static long mychardev_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static ssize_t mychardev_read(struct file *file, char __user *buf, size_t count, loff_t *offset);
static ssize_t mychardev_write(struct file *file, const char __user *buf, size_t count, loff_t *offset);
int ptextract_thread_fn(void * data);



static const struct file_operations mychardev_fops = {
    .owner      = THIS_MODULE,
    .open       = mychardev_open,
    .release    = mychardev_release,
    .unlocked_ioctl = mychardev_ioctl,
    .read       = mychardev_read,
    .write       = mychardev_write
};

struct mychar_device_data {
    struct cdev cdev;
};

struct list_head sysfs_list;

LIST_HEAD(sysfs_list);

struct pt_data{
    unsigned long vaddr;
    unsigned long pgd_entry;
    unsigned long pud_entry;
    unsigned long pmd_entry;
    unsigned long pte_entry;
};

struct pt_stats{
    struct pt_data pd;
    struct list_head list;
};

struct metadata{
    pid_t pid;
    unsigned long vaddr;
    unsigned entries;
};


static int dev_major = 0;

static struct class *mychardev_class = NULL;

static struct mychar_device_data mychardev_data;

static struct kobject *example_kobject;

unsigned num_entries = 0;

static ssize_t task_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    //struct list_head * temp_list_head;
    struct pt_stats *cursor,*temp_pt_data;
    static unsigned flag = 0;
    unsigned next_pos = 0;
    int ret;
    unsigned long dest_address = 0;
    //pr_info("task sysfs show called\n");
    //pr_info("number of entries:%u\n",num_entries);
    if(!flag){
        flag = num_entries;
	//pr_info("inside num entries\n");
	ret = sprintf(buf, "%u", num_entries);
	return ret;
    }
    else{
	//pr_info("inside page table data copy\n");
        //list_for_each(temp_list_head, &sysfs_list){
	list_for_each_entry_safe(cursor, temp_pt_data, &sysfs_list, list){
            //temp_pt_data = list_entry(temp_list_head, struct pt_stats, list);
	    //printk("vaddr:%lx,pte:%lx\n",cursor->pd.vaddr,cursor->pd.pte_entry);
	    dest_address = (unsigned long)((unsigned long)buf+(next_pos*sizeof(struct pt_data)));
	    //printk("dest addr:%lx\n",dest_address);
	    stac();
            memcpy((char*)(dest_address), &cursor->pd,sizeof(struct pt_data));
	    clac();
            next_pos += 1;
	    list_del(&cursor->list);
	    kfree(cursor);
	    if(next_pos == 64){
		flag -= next_pos;
	        return (next_pos*sizeof(struct pt_data));
	    }
	}
	flag -= next_pos;
    }
    return (next_pos*sizeof(struct pt_data));
}

static ssize_t task_store(struct kobject *kobj, struct kobj_attribute *attr,
                     const char *buf, size_t count)
{
    struct metadata* data = (struct metadata*)buf;
    //printk("pid:%d, vaddr:%lx, entries:%u\n",data->pid,data->vaddr, data->entries);
    ptextract_thread_fn(data);
    return count;
}



static inline pte_t* get_pte(struct mm_struct *mm, unsigned long addr, struct pt_stats *pt){
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *ptep = NULL;
    pgd = pgd_offset(mm, addr);
    if (pgd_none(*pgd) || unlikely(pgd_bad(*pgd)))
        goto out;
    pt->pd.pgd_entry = pgd_val(*pgd);
    p4d = p4d_offset(pgd, addr);
    if (p4d_none(*p4d)|| unlikely(p4d_bad(*p4d)))
        goto out;
    pud = pud_offset(p4d, addr);
    if (pud_none(*pud) || unlikely(pud_bad(*pud)))
        goto out;
    pt->pd.pud_entry = pud_val(*pud);
    pmd = pmd_offset(pud, addr);
    if (pmd_none(*pmd))
       goto out;
    pt->pd.pmd_entry = pmd_val(*pmd);
    ptep = pte_offset_map(pmd, addr);
    pt->pd.pte_entry = pte_val(*ptep);
out:
        return ptep;
}
int ptextract_thread_fn(void * thread_data){
    struct metadata* dt = (struct metadata*)thread_data;
    //struct mm_struct *mm; 
    struct vm_area_struct *vma;
    pid_t temp_pid = dt->pid;
    unsigned long vaddr = 0;
    unsigned long start_vaddr = dt->vaddr;
    unsigned entries = dt->entries;
    unsigned long start = 0, end = 0;
    unsigned counter = 0;
    pte_t *ptep =  NULL;
    int vma_count = 0;
    struct pt_stats * pt = NULL;
    struct task_struct * tsk = NULL; 
    struct mm_struct * mm = NULL;
    MA_STATE(mas, 0, 0, 0);

    tsk = pid_task(find_get_pid(temp_pid),PIDTYPE_PID); 
    if(!tsk){
        printk("task ended\n");
	return -1;
    }
    mm = tsk->mm;
    mas.tree = &mm->mm_mt;
    //printk("start:%lx,entries:%u\n",start_vaddr,entries);
    mmap_read_lock(mm);
    vma = mas_find(&mas, ULONG_MAX);
    if(!vma){
        mmap_read_unlock(mm);
	return -1;
    }
    do{
        vma_count += 1;
        tsk = pid_task(find_get_pid(temp_pid),PIDTYPE_PID);
        if(!tsk){
            printk("task ended :%u\n",temp_pid);
            mmap_read_unlock(mm);
            return -1;
	}
        start = vma->vm_start;
        end = vma->vm_end;
	if(start<=start_vaddr && start_vaddr<end){
	    //printk("start:%lx, end:%lx\n",start,end);
            for(vaddr=(start_vaddr&~(0xfff)), counter=0; 
			    vaddr < end && counter<entries; 
			    vaddr += PAGE_SIZE, counter++){
		//printk("addr:%lx\n",vaddr);
	        pt = (struct pt_stats*)kmalloc(sizeof(struct pt_stats), GFP_KERNEL);
	        INIT_LIST_HEAD(&pt->list);
	        pt->pd.vaddr = vaddr;
                pt->pd.pgd_entry = 0;
                pt->pd.pud_entry = 0;
                pt->pd.pmd_entry = 0;
	        pt->pd.pte_entry = 0;
	        ptep = get_pte(mm, vaddr, pt);
	        if(pt->pd.pgd_entry != 0){
		    //printk("vaddr:%lx, pgd:%lx, pud:%lx, pmd:%lx, pte:%lx\n",pt->pd.vaddr,pt->pd.pgd_entry,pt->pd.pud_entry,pt->pd.pmd_entry,pt->pd.pte_entry);
	            list_add_tail(&pt->list, &sysfs_list);
	            num_entries += 1;
		}else{
                    kfree(pt);
		}
	    }
	}
    }while((vma = mas_find(&mas, ULONG_MAX)) != NULL);
    //printk("vma count:%u\n",vma_count);
    vma_count = 0;
    mmap_read_unlock(mm);
    return 0;
}
static int mychardev_uevent(struct device *dev, struct kobj_uevent_env *env)
{
    add_uevent_var(env, "DEVMODE=%#o", 0666);
    return 0;
}

struct kobj_attribute task_sysfs_attr;
static int __init mychardev_init(void)
{
    int err;
     
    int error = 0;
    dev_t dev;

    err = alloc_chrdev_region(&dev, 0, 1, "mychardev");
    if(err)
        return err;

    dev_major = MAJOR(dev);

    mychardev_class = class_create(THIS_MODULE, "mychardev");
    mychardev_class->dev_uevent = mychardev_uevent;

    cdev_init(&mychardev_data.cdev, &mychardev_fops);
    mychardev_data.cdev.owner = THIS_MODULE;
    cdev_add(&mychardev_data.cdev, MKDEV(dev_major, 0), 1);
    device_create(mychardev_class, NULL, MKDEV(dev_major, 0), NULL, "mychardev-0");
    example_kobject = kobject_create_and_add("kobject_example", kernel_kobj);
    if(!example_kobject){
	printk("kobject creation failed\n");
        return -ENOMEM;
    }
    task_sysfs_attr.attr.name = "task";
    task_sysfs_attr.attr.mode = 0666;
    task_sysfs_attr.show = task_show;
    task_sysfs_attr.store = task_store;
    error = sysfs_create_file(example_kobject, &task_sysfs_attr.attr);
    if(error){
        pr_debug("failed to create the pid file\n");
    }
     
    
   return 0;
}

static void __exit mychardev_exit(void)
{
    
    pr_info("module unloaded\n");
    kobject_put(example_kobject);
    device_destroy(mychardev_class, MKDEV(dev_major, 0));
    //class_unregister(mychardev_class);
    class_destroy(mychardev_class);
    unregister_chrdev_region(MKDEV(dev_major, 0),1);
    
}

static int mychardev_open(struct inode *inode, struct file *file)
{

    //LIST_HEAD(sysfs_list);
    num_entries = 0;
    pr_info("MYCHARDEV: Device open\n"); 
    return 0;
}

static int mychardev_release(struct inode *inode, struct file *file)
{
    struct list_head * temp_list_head;
    struct list_head * q;
    struct pt_stats * temp_pt_data;
    list_for_each_safe(temp_list_head,q, &sysfs_list){
        temp_pt_data = list_entry(temp_list_head, struct pt_stats, list);
	//printk("removing pt_data:%p\n",temp_pt_data);
	list_del_init(temp_list_head);
        kfree(temp_pt_data);
    }
    printk("MYCHARDEV: Device close\n"); 
    return 0;
}

static long mychardev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    printk("MYCHARDEV: Device ioctl\n");
    return 0;
}

static ssize_t mychardev_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
    printk(KERN_INFO "read call\n");
    return count;
}


static ssize_t mychardev_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{

    printk(KERN_INFO "write call\n");
    return count;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("arunkp1986@gmail.com>");

module_init(mychardev_init);
module_exit(mychardev_exit);

