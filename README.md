# Lens
Lens: Experiencing Multi-level Page Tables at Close Quarters

change directory into **lens** folder

Add your username to /etc/sudoers for file "insert\_module.sh" contained in **lens/source** folder with sudo permission

username ALL=(root) NOPASSWD: path to lens/source/insert\_module.sh

Lens uses Tkinter for user interface, install Tkinter for your Linux distribution.

For Ubuntu use **sudo apt install python3-tk**

Create a Python3 virtual environment, by following instruction for your Linux distribution

For Ubuntu use **python3 -m venv myenv**

Activate Python3 virtual environment and install required packages using **requirements.txt** file provided under **lens**

**pip install -r requirements.txt**

After activating Python3 virtual environment and installing all requirements you can start **Lens** by running **lens.py** in **lens/source/** directory

**python3 lens.py**

Python version : Python 3.8.10
Linux Kernel Version: 6.1.4
Tkinter version 8.6
