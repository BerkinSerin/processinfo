#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/syscalls.h>
#include <linux/fcntl.h>
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/cred.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

SYSCALL_DEFINE4(processinfo, char *, allProcesses, char *, processId, char *, commandLine, int, argument)

// allProcesses  -> -all command, processId|commandLine -> -p command, argument is passed for separation

{

    // variable declaration

    char processes[4096];
    char cmdline[256];

    // represents -all command
    if (argument == 1)
    {

        struct task_struct *task;
        for_each_process(task)
        {
            char state[16];
            char pid[16];
            char uid_str[32];
            char ppid[16];
            char processInfo[128];

            int state_val = task->state;
            snprintf(state, 16, "%d", state_val);

            int pid_val = task->pid;
            snprintf(pid, 16, "%d", pid_val);

            uid_t uid_val = __kuid_val(task_uid(task));
            snprintf(uid_str, 32, "%d", uid_val);

            int ppid_val = task_ppid_nr(task);
            snprintf(ppid, 16, "%d", ppid_val);

            strcpy(processInfo, uid_str);
            strcat(processInfo, "\t");
            strcat(processInfo, ppid);
            strcat(processInfo, "\t");
            strcat(processInfo, pid);
            strcat(processInfo, "\t");
            strcat(processInfo, task->comm);
            strcat(processInfo, "\t");
            strcat(processInfo, state);
            strcat(processInfo, "\t");
            strcat(processInfo, "\n");
            strcat(processes, processInfo);
        }

        // send processes to userspace
        copy_to_user(allProcesses, processes, 4096);
        // clear variables
        memset(processes, 0, 4096);
    }

    // represents -p command
    else if (argument == 2)
    {

        char pathToCmdline[32];
        // create path

        strcpy(pathToCmdline, "/proc/");
        strcat(pathToCmdline, processId);
        strcat(pathToCmdline, "/cmdline");

        //Read cmdline

        struct file *f;
        mm_segment_t fs;

        // open file
        f = filp_open(pathToCmdline, O_RDONLY, 0);

        if (f == NULL)

            printk(KERN_ALERT "filp_open error!!.\n");

        else

        {

            fs = get_fs();
            set_fs(get_ds());
            f->f_op->read(f, cmdline, 256, &f->f_pos);
            set_fs(fs);
            // send to userspace
            copy_to_user(commandLine, cmdline, 256);
        }

        // clear variables and close file
        filp_close(f, NULL);
    }

    // represents -k command
    else if (argument == 3)
    {

        // send kill signal to the process
        kill_pid(find_get_pid(processId), SIGKILL, 1);
    }

    return 0;
}
