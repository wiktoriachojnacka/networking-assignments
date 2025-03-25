# Networking Operations for Exam Preparation

This repository contains various networking assignments from my studies, designed to help prepare for exams and practical client-server communication exercises. Each folder corresponds to a specific networking concept and contains two files: `client.c` and `server.c`. These examples demonstrate different networking techniques such as FIFO, datagram sockets, stream sockets, etc.

---

## 1. FIFO (First In, First Out)

### Overview
The `fifo` folder contains a simple client-server application built using a FIFO (named pipe) for inter-process communication. The client sends a message to the FIFO, and the server receives and prints the message.

### Requirements
- A Linux-based environment with a compiler (e.g., `gcc`)
- The code has been tested in a WSL (Windows Subsystem for Linux) environment but can be used in any Linux system.

> **Note:** It is recommended to run these examples in the Linux file system (e.g., your home directory in WSL) rather than directly from a Windows file system (`/mnt/c/...`) because FIFO operations may not be fully supported on Windows file systems.

### How to Run
```bash
# Clone the Repository
git clone https://github.com/wiktoriachojnacka/networking-assignments.git
cd networking-assignments/fifo

# Compile the Code
gcc server.c -o server
gcc client.c -o client

# Create the FIFO (Named Pipe)
mkfifo fifo

# Verify FIFO creation
ls -l fifo

# Run the Server (in one terminal window)
./server

# Run the Client (in another terminal window, passing argumentS)
./client 

# Cleanup (If Needed)
# The FIFO file should be removed automatically when the program stops.
# If it persists, manually remove it:
rm fifo
