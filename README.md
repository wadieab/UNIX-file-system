# UNIX-file-system
===================Description===================

a Disk management system that allows you to create ,open ,close ,read ,write ,delete ,and list files with a chosen block size to format the disk in

the three classes:

fsDisk: it represents the disk itself with the disk size , main directory, and opened files 


FileDescriptor: links between a given opened file and it name , FsFile object

FsFile: contains information about the file itself the file size , block size (size of the data segments) , index block (where we can find the data segments 



===================Functions===================

void listAll(): lists all the files in the directory  

void fsFormat(int blockSize): formats the disk and gets it ready with the given block size 

int CreateFile(string fileName): creates a file with a given name and opens it 

int OpenFile(string fileName): opens a closed file from main directory 

string CloseFile(int fd): closes an open file from opened files directory 

int WriteToFile(int fd, char *buf, int len ): writes to the given opened file fd from the buffer "buf" of a "len" length   

int DelFile( string FileName ): frees the allocated file blocks and gets them ready for overwriting 

int ReadFromFile(int fd, char *buf, int len ): reads from the opened file fd to the buffer "buf" with "len" length 
 

==========Program Files==============

memsim.cpp : the main of the project

=================compiling steps===================

compile : g++ memsim.cpp -o project

run : ./project

================= Output ===================

2
3 
3
v
CreateFile: v with File Descriptor #: 0
3
r
CreateFile: r with File Descriptor #: 1
6
1
fttf
6
2
ghj
6
0
60
5
1
CloseFile: r with File Descriptor #: 1
8
r
DeletedFile: r with File Descriptor #: 1
1
index: 0: FileName: v , isInUse: true
Disk content: 'fttf60'
1
index: 0: FileName: v , isInUse: true
Disk content: 'fttf60'
8
v
DeletedFile: v with File Descriptor #: 1
1
Disk content: 'fttf60'
0
