#include <iostream>
#include <vector>
#include <map>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

#define DISK_SIZE 256


void  decToBinary(int n , char &c)
{
    // array to store binary number
    int binaryNum[8];

    // counter for binary array
    int i = 0;
    while (n > 0) {
        // storing remainder in binary array
        binaryNum[i] = n % 2;
        n = n / 2;
        i++;
    }

    // printing binary array in reverse order
    for (int j = i - 1; j >= 0; j--) {
        if (binaryNum[j]==1)
            c = c | 1u << j;
    }
}


// ============================================================================

class FsFile {

    int file_size;

    int block_in_use;

    int index_block;

    int block_size;





public:

    FsFile(int _block_size) {

        file_size = 0;

        block_in_use = 0;

        block_size = _block_size;

        index_block = -1;

    }

    void stpnt(int blk) {   //set index block
        index_block = blk;
    }

    int gtpnt() {   //get index block
        return index_block;
    }

    void stblks(int num) {
        block_in_use = num;
    }

    int gtblks() {
        return block_in_use;
    }

    void stsiz(int siz){ // set size

        file_size = siz;

    }

    int getfile_size(){ // get size

        return file_size;

    }

};


// ============================================================================

class FileDescriptor {

    string file_name;

    FsFile* fs_file;

    bool inUse;



public:

    FileDescriptor(string FileName, FsFile* fsi) {

        file_name = FileName;

        fs_file = fsi;

        inUse = true;

    }



    string getFileName() {

        return file_name;

    }

    FsFile* getFilfs() {    //get file fs

        return fs_file;

    }



};

#define DISK_SIM_FILE "DISK_SIM_FILE.txt"

// ============================================================================

class fsDisk {



    FILE *sim_disk_fd;

    bool is_formated;



    // BitVector - "bit" (int) vector, indicate which block in the disk is free

    //              or not.  (i.e. if BitVector[0] == 1 , means that the

    //             first block is occupied.

    int BitVectorSize;

    int *BitVector;

    // filename and one fsFile.

    map <string , FsFile*> MainDir;


    // OpenFileDescriptors --  when you open a file,

    // the operating system creates an entry to represent that file

    // This entry number is the file descriptor.


    map <int , FileDescriptor*> OpenFileDescriptors;

    int direct_enteris;

    int block_size;

    int maxSize;
    
    int freeBlocks;



    // ------------------------------------------------------------------------



public:
    fsDisk() {

        sim_disk_fd = fopen(DISK_SIM_FILE , "r+");

        assert(sim_disk_fd);

        for (int i=0; i < DISK_SIZE ; i++) {

            int ret_val = fseek ( sim_disk_fd , i , SEEK_SET );

            ret_val = fwrite( "\0" ,  1 , 1, sim_disk_fd);

            assert(ret_val == 1);

        }

        fflush(sim_disk_fd);

        direct_enteris = 0;

        block_size = 0;

        is_formated = false;

    }

// ------------------------------------------------------------------------
void listAll() {
    
    int i = 0;
    int bol = 0;
    for ( auto it = begin (MainDir); it != end (MainDir); ++it) {       // lists all file in directory
        for ( auto it2 = begin (OpenFileDescriptors); it2 != end (OpenFileDescriptors); ++it2) {
            if (it2->second != NULL && it->first == it2->second->getFileName()) {
                cout << "index: " << i << ": FileName: " << it->first <<  " , isInUse: true" << endl;
                bol = 1;
                break;
            }
        }
        if (bol == 0) {
            cout << "index: " << i << ": FileName: " << it->first <<  " , isInUse: false" << endl;
        }
            i++;
            bol = 0;
    }




    char bufy;

    cout << "Disk content: '" ;

    for (i=0; i < DISK_SIZE ; i++) {

        int ret_val = fseek ( sim_disk_fd , i , SEEK_SET );

        ret_val = fread(  &bufy , 1 , 1, sim_disk_fd );

        cout << bufy;

    }

    cout << "'" << endl;

}

// ------------------------------------------------------------------------
void fsFormat(int blockSize) {  //formats disk and gets the disk initialized

        block_size = blockSize;
        BitVectorSize = DISK_SIZE/block_size;
        BitVector = new int[DISK_SIZE/blockSize];
        for(int i = 0 ; i < DISK_SIZE/blockSize ; i++) {
            BitVector[i] = 0;
        }
        freeBlocks = 0;
        OpenFileDescriptors.clear();
        MainDir.clear();

        maxSize = block_size;
        freeBlocks = DISK_SIZE/block_size;

        is_formated = true;
}

// ------------------------------------------------------------------------
int CreateFile(string fileName) {
    if (is_formated) {      // create file and open
        
        MainDir[fileName] = new FsFile(block_size);
        
        return OpenFile(fileName);
    }
    return -1;
}

// ------------------------------------------------------------------------
int OpenFile(string fileName) {     // open file from main directory
    if (is_formated) {
        
        for (auto &elm: OpenFileDescriptors) {
            if (elm.second->getFileName() == fileName) {
                return -1;
            }
        }
        
        int val = 0;
        for (auto &elm: OpenFileDescriptors) {
            
            if (elm.first >= DISK_SIZE/block_size) {
    
            }
            if (val == elm.first) {
                val++;
            }
            else {
                break;
            }
            
        }
        OpenFileDescriptors[val] = new FileDescriptor(fileName , MainDir[fileName]);
        return val;
    }
    return -1;
}

// ------------------------------------------------------------------------
string CloseFile(int fd) {      //close opend file
    if (is_formated) {
        if (OpenFileDescriptors.find(fd)->second != NULL) {
            string nam = OpenFileDescriptors.find(fd)->second->getFileName();
            delete OpenFileDescriptors.find(fd)->second;
            OpenFileDescriptors.erase(fd);
            return nam;
        }
    }
    return "-1";
}

// ------------------------------------------------------------------------
int WriteToFile(int fd, char *buf, int len ) {      //write to file with "len" long data from a buffer "buf"

    if (is_formated) {

        if (is_formated && OpenFileDescriptors[fd] != NULL && len > 0 ) {

            int filsiz = OpenFileDescriptors[fd]->getFilfs()->getfile_size();
            if (filsiz == 0 && freeBlocks > 1) {
                for (int i = 0 ; i < BitVectorSize ; i++) {     //allocate index block if needed
                    if (BitVector[i] == 0) {
                        OpenFileDescriptors[fd]->getFilfs()->stpnt(i);
                        BitVector[i] = 1;
                        freeBlocks--;
                        break;
                    }
                }
            }
            else if (filsiz == 0 && freeBlocks <= 1) {
                return -1;
            }
            bool bol = true;
            int lenwt = 0;
            if (filsiz < block_size * block_size) {

                if (len + filsiz > block_size*block_size) {
                    len = (block_size*block_size) - filsiz;
                    bol = false;
                }
                
                if (filsiz%block_size > 0) {            // handle fragmentation
                    char indcr[1];
                    fseek(sim_disk_fd , (OpenFileDescriptors[fd]->getFilfs()->gtpnt() * block_size) + (int) filsiz/block_size , SEEK_SET);
                    fread(indcr , sizeof(char) , 1 , sim_disk_fd);
                    fseek(sim_disk_fd , ((int) *indcr * block_size) + filsiz%block_size , SEEK_SET);

                    if (filsiz%block_size  > len) {
                        lenwt = len;
                    }
                    else {
                        lenwt = filsiz%block_size;
                    }
                    
                    fwrite(buf , sizeof(char) , lenwt , sim_disk_fd);
                    len -= lenwt;
                    OpenFileDescriptors[fd]->getFilfs()->stsiz(filsiz+lenwt);
                    filsiz += lenwt;
                }

                char conv[1];
                for (int curpoin = filsiz/block_size ; len > 0 && freeBlocks > 0 && curpoin < block_size; curpoin++) {      //write to the blocks
                    int i = 0;
                    filsiz = OpenFileDescriptors[fd]->getFilfs()->getfile_size();
                    for ( ; i < BitVectorSize ; i++) {
                        if (BitVector[i] == 0) {
                            fseek(sim_disk_fd , (OpenFileDescriptors[fd]->getFilfs()->gtpnt() * block_size) + curpoin , SEEK_SET);
                            BitVector[i] = 1;
                            freeBlocks--;
                            
                            conv[0] = 0;
                            decToBinary(i , conv[0]);
                            fwrite(conv , sizeof(char) , 1 , sim_disk_fd);
                            fseek(sim_disk_fd , i * block_size , SEEK_SET);
                            if (len > block_size) {
                                OpenFileDescriptors[fd]->getFilfs()->stsiz(filsiz+block_size);
                                fwrite(lenwt + buf , sizeof(char) , block_size , sim_disk_fd);
                                lenwt += block_size;
                                len -= block_size;
                            }
                            else {
                                OpenFileDescriptors[fd]->getFilfs()->stsiz(filsiz+len);
                                fwrite( lenwt+ buf , sizeof(char) , len , sim_disk_fd);
                                lenwt += len;
                                len -= len;
                            }
                            break;
                        }
                    }
                }
                if (len == 0 && bol == true) {
                    return 0;
                }
                
            }
            
        }
        
    }
    return -1;
}

// ------------------------------------------------------------------------
int ReadFromFile(int fd, char *buf, int len ) { // reads from file to a buffer "buf"

    for (int i = 0 ; i < DISK_SIZE ; i ++) {
        buf[i] = '\0';
    }

    if (is_formated && (OpenFileDescriptors[fd] != NULL)) {
        int filsiz = OpenFileDescriptors[fd]->getFilfs()->getfile_size();
        if (len > filsiz) {
            len = filsiz;
        }
        
        char curblk[1];
        for (int i = 0 ; i < block_size && len > 0 ; i++) {             //run on all file blocks if needed
            
            fseek(sim_disk_fd ,(OpenFileDescriptors[fd]->getFilfs()->gtpnt() * block_size) + i, SEEK_SET);
            
            curblk[0] = '\0';
            fread(curblk , sizeof(char) , 1 , sim_disk_fd);
            fseek(sim_disk_fd ,((int) *curblk ) * block_size , SEEK_SET); // find block from index block
            

            if (len < block_size) {                 // read with approprate size
                fread((i * block_size) + buf , sizeof(char) , len , sim_disk_fd);
                len = 0;
            }
            else {
                fread((i * block_size) + buf , sizeof(char) , block_size , sim_disk_fd);
                len -= block_size;
            }
        

        }

        if (len == 0) {
            return 1;
        }

    }
    
    return -1;
}

// ------------------------------------------------------------------------
int DelFile( string FileName ) {    // deletes the file from the bitvector for its data to be over written
    
    if (is_formated) {
        /**/for (auto &elm: OpenFileDescriptors) {
            if (elm.second != NULL && elm.second->getFileName() == FileName) {
                delete OpenFileDescriptors[elm.first];
                OpenFileDescriptors.erase(elm.first);
                break;
            }
        }
         
        if (MainDir.find(FileName)->second == NULL) {
            return -1;
        }

        char indcr[1];
       for (int i = 0 ; i < block_size ; i++) {
            
            fseek(sim_disk_fd , (MainDir.find(FileName)->second->gtpnt() * block_size) + i, SEEK_SET);
            fread(indcr , sizeof(char) , 1 , sim_disk_fd);
            if (*indcr != '\0') {
                BitVector[(int) *indcr] = 0;
            }
            else {
                break;
            }
        }

        BitVector[MainDir.find(FileName)->second->gtpnt()] = 0;
        delete MainDir.find(FileName)->second;
        MainDir.erase(FileName);
        return 1;
        
    }
    return -1;
}
};

int main() {
    int blockSize;
    int direct_entries;
    string fileName;
    char str_to_write[DISK_SIZE];
    char str_to_read[DISK_SIZE];
    int size_to_read;
    int _fd;

    fsDisk *fs = new fsDisk();
    int cmd_;
    while(1) {
        cin >> cmd_;
        switch (cmd_)
        {
            case 0:   // exit
            delete fs;
            exit(0);
            break;

            case 1:  // list-file
            fs->listAll();
            break;

            case 2:    // format
            cin >> blockSize;
            fs->fsFormat(blockSize);
            break;

            case 3:    // creat-file
            cin >> fileName;
            _fd = fs->CreateFile(fileName);
            cout << "CreateFile: " << fileName << " with File Descriptor #: " << _fd << endl;
            break;

            case 4:  // open-file
            cin >> fileName;
            _fd = fs->OpenFile(fileName);
            cout << "OpenFile: " << fileName << " with File Descriptor #: " << _fd << endl;
            break;

            case 5:  // close-file
            cin >> _fd;
            fileName = fs->CloseFile(_fd);
            cout << "CloseFile: " << fileName << " with File Descriptor #: " << _fd << endl;
            break;

            case 6:   // write-file
            cin >> _fd;
            cin >> str_to_write;
            fs->WriteToFile( _fd , str_to_write , strlen(str_to_write) );
            break;

            case 7:    // read-file
            cin >> _fd;
            cin >> size_to_read ;
            fs->ReadFromFile( _fd , str_to_read , size_to_read );
            cout << "ReadFromFile: " << str_to_read << endl;
            break;

            case 8:   // delete file
            cin >> fileName;
            _fd = fs->DelFile(fileName);
            cout << "DeletedFile: " << fileName << " with File Descriptor #: " << _fd << endl;
            break;
            default:
                break;
        }
    }
}
