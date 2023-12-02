# Lens
Lens: Experiencing Multi-level Page Tables at Close Quarters

Lens provides a simple, flexible and intuitive interface which can be used to develop holistic understanding of virtual to physical memory address translation using multi-level page tables by correlating execution of simple C programs with OS-level status of the multi-level page tables.

## Step to setup and start Lens application

change directory into **lens** folder
### Step 1
Add your username to /etc/sudoers for file **insert\_module.sh** contained in **lens/source** folder with sudo permission

in Ubuntu system type **sudo visudo** to edit  /etc/sudoers file and add **username ALL=(ALL:ALL) ALL**, here replace **username** with your username, in Linux check using **whoami**


### Step 2
Lens uses Python **Tkinter** for user interface, install Tkinter for your Linux distribution.

For Ubuntu use **sudo apt install python3-tk**

### Step 3
Create a Python3 virtual environment, by following instruction for your Linux distribution

For Ubuntu use **python3 -m venv myenv**

Activate Python3 virtual environment by running below command

**source myenv/bin/activate**

Install required packages in the Python virtual environment by using **requirements.txt** file provided under **lens** folder

**pip install -r requirements.txt**

### Step 4
After activating Python3 virtual environment and installing all requirements you can start **Lens** by running **lens.py** in **lens/source/** directory

**python3 lens.py**

## Using Lens application
Lens consists of an easy-to-use graphical interface at the front-end and a Linux kernel interface at the back-end (referred to as ATT) for collecting
address translation details from the operating system page table.

The user writes(copy-paste) a C program of interest in the **code area** and provides three Lens input parameters— **the line number**, **variable address** and **number of entries**. The line number represents the program location where the user wants to examine the page table contents. The variable address parameter allows the user to specify the program variable for inspection. Using the number of entries, the user can specify the number of pages starting with the variable address for which the address translation information to be collected. 

## System configuration we used
Python version : Python 3.8.10
Linux Kernel Version: 6.1.4
Tkinter version 8.6
