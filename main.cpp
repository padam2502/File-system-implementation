//#include <bits/stdc++.h>
#include <iostream>
#include <string.h>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <unordered_map>
#include <sys/stat.h>
using namespace std;
#define Max 500000
typedef class diskblock
{
public:
    char info[Max];
    long long blockpointer;
    diskblock()
    {
        blockpointer = -100;
    }
} diskblock;
typedef class superblock
{
public:
    long long blocksize;
    long long total_blocks;
    long long total_inodes;
    superblock()
    {
        blocksize = sizeof(diskblock);
        total_blocks = 1000;
        total_inodes = 100;
    }
} superblock;

typedef class inodeinfo
{
public:
    long long firstblock;
    char name[32];
    inodeinfo()
    {
        firstblock = -1;
        strcpy(name, "");
    }
} inodeinfo;
void write_back(superblock *&sb, vector<inodeinfo *> &inode, vector<diskblock *> &dskblk, string disk_name)
{
    ofstream opfile;
    opfile.open(disk_name, ios::out | ios::trunc);
    opfile.write((char *)&(*sb), sizeof(superblock));
    for (long long i = 0; i < sb->total_inodes; i++)
    {
        opfile.write((char *)&(*inode[i]), sizeof(inodeinfo));
    }
    for (long long i = 0; i < sb->total_blocks; i++)
    {
        opfile.write((char *)&(*dskblk[i]), sizeof(diskblock));
    }
    opfile.close();
}
void make_disk(string str)
{
    superblock *sb = new superblock();
    vector<diskblock *> dskblk(sb->total_blocks);
    vector<inodeinfo *> inode(sb->total_inodes);
    for (int i = 0; i < sb->total_blocks; i++)
    {
        dskblk[i] = new diskblock();
    }
    
    for (int i = 0; i < sb->total_inodes; i++)
    {
        inode[i] = new inodeinfo();
    }
    write_back(sb, inode, dskblk, str);
}
long long search_inode(vector<inodeinfo *> &inode)
{
    long long ret = -1;
    for (auto u : inode)
    {
        ++ret;
        if (u->firstblock == -1)
        {
            return ret;
        }
    }
    return -1;
}
long long search_diskblock(vector<diskblock *> &dskblk)
{
    long long ret = -1;
    for (auto u : dskblk)
    {
        ++ret;
        if (u->blockpointer == -100)
        {
            return ret;
        }
    }
    return -1;
}
long long allocate_file(vector<inodeinfo *> &inode, vector<diskblock *> &dskblk, string fname)
{
    long long ind = search_inode(inode);
    long long blk = search_diskblock(dskblk);
    if(blk==-1 || ind==-1)
    {
        cout<<"Memory of disk full\n";
        return -1;
    }
    inode[ind]->firstblock = blk;
    strcpy(inode[ind]->name, &fname[0]);
    dskblk[blk]->blockpointer = -1;
    dskblk[blk]->info[0] = '\0';
    return ind;
}
long long fileexist(vector<inodeinfo *> &inode, string fname)
{
    for (int i = 0; i < inode.size(); i++)
    {
        if (inode[i]->name == fname)
        {
            return i;
        }
    }
    return -1;
}
void section_2(superblock *&sb, vector<inodeinfo *> &inode, vector<diskblock *> &dskblk, string disk_name)
{
    unordered_map<long long, long long> fmode; //inode number and mode
    long long x;
    string fname;
    string dif_modes[3] = {"Read mode", "Write mode", "Append mode"};
    while (1)
    {
        cout << endl;
        cout << "************************************\n";
        cout << " Press 1 for Create file\n";
        cout << " Press 2 for Open file\n";
        cout << " Press 3 for Read file\n";
        cout << " Press 4 for Write file\n";
        cout << " Press 5 for Append file\n";
        cout << " Press 6 for Close file\n";
        cout << " Press 7 for Delete file\n";
        cout << " Press 8 for list of files\n";
        cout << " Press 9 for list of Opened files\n";
        cout << " Press 10 for Unmount\n";
        cout << "************************************\n";
        cin >> x;
        long long check = -1;
        if (x == 1)
        {
            cout << "Enter unique file name\n";
            cin >> fname;
            check = fileexist(inode, fname);
            if (check != -1)
            {
                cout << "File name already exist\n";
            }
            else
            {
                long long err=allocate_file(inode, dskblk, fname);
                if(err==-1)
                {
                    return;
                }

                write_back(sb, inode, dskblk, disk_name);
                cout << "File created successfully\n";
            }
        }
        else if (x == 2)
        {
            long long m = -1;
            string mode = "";
            cout << "Enter unique file name\n";
            cin >> fname;
            cout << "0: read mode\n1: write mode\n2: append mode\n";
            cin >> m;
            if (m == 0)
            {
                mode = "Read mode\n";
            }
            else if (m == 1)
            {
                mode = "Write mode\n";
            }
            else if (m == 2)
            {
                mode = "Append mode\n";
            }
            else
            {
                cout << "Wrong choice\n";
                break;
            }
            check = fileexist(inode, fname);
            if (check == -1)
            {
                cout << "File name does not exist\n";
            }
            else
            {
                if (fmode.find(check) != fmode.end())
                {
                    if (fmode[check] != m)
                    {
                        cout << "File is opened in another mode\n";
                    }
                    else
                    {
                        fmode[check] = m;
                        cout << "File is now opened in " << mode << "with file descripter " << check << " allocated\n";
                    }
                }
                else
                {
                    fmode[check] = m;
                    cout << "File is now opened in " << mode << "with file descripter " << check << " allocated\n";
                }
            }
        }
        else if (x == 3)
        {
            cout << "Enter file descriptor of file you want to read\n";
            cin >> check;
            if (fmode.find(check) != fmode.end())
            {
                if (fmode[check] == 0)
                {
                    long long blockno = inode[check]->firstblock;
                    if (blockno == -1)
                    {
                        cout << "Inode is not pointing to any block\n";
                    }
                    int cnt = 0;

                    do
                    {
                        char *currblock = &(dskblk[blockno]->info[0]);

                        for (int i = 0; i < Max && currblock[i] != '\0'; i++)
                        {
                            cout << currblock[i];
                            cnt++;
                        }
                        blockno = dskblk[blockno]->blockpointer;
                        cnt = 0;
                    } while (blockno >= 0);
                    cout << "\nFile read succesfull\n";
                }
                else
                {
                    cout << "File is not opened in read mode\n";
                }
            }
            else
            {
                cout << "Invalid File Descriptor\n";
            }
        }
        else if (x == 4)
        {
            cout << "Enter file descriptor of file you want to write\n";
            cin >> check;
            if (fmode.find(check) != fmode.end())
            {
                if (fmode[check] == 1)
                {
                    int temp;
                    int tempblk = inode[check]->firstblock;
                    if (tempblk != -1)
                    {
                        do
                        {
                            temp = dskblk[tempblk]->blockpointer;
                            dskblk[tempblk] = new diskblock();
                            tempblk = temp;
                        } while (temp != -100 && temp != -1);
                        dskblk[inode[check]->firstblock]->info[0] = '\0';
                    }
                    int cnt = 0;
                    long long fb = inode[check]->firstblock;
                    cout << "Enter content of file\n";
                    string inpstr = "";
                    string inptemp = "";
                    cin.ignore();
                    while (1)
                    {
                        getline(cin, inptemp);
                        if (inptemp != "stop")
                        {
                            inpstr += inptemp + "\n";
                        }
                        else
                        {
                            inpstr.pop_back();
                            break;
                        }
                    }
                    for (int i = 0; i < inpstr.size(); i++)
                    {
                        dskblk[fb]->info[cnt] = inpstr[i];
                        cnt++;
                        if (cnt >= Max)
                        {
                            long long blk = search_diskblock(dskblk);
                            if (blk != -1)
                            {
                                cnt = 0;
                                dskblk[fb]->blockpointer = blk;
                                fb = blk;
                                dskblk[fb]->blockpointer = -2;
                            }
                            else
                            {
                                cout << "Disk is full\n";
                                break;
                            }
                        }
                    }
                    dskblk[fb]->info[cnt] = '\0';
                    cout << "File write Successfull\n";
                    write_back(sb, inode, dskblk, disk_name);
                }
                else
                {
                    cout << "File is not opened in Write mode\n";
                }
            }
            else
            {
                cout << "Invalid File Descriptor\n";
            }
            
        }
        else if (x == 5)
        {
            cout << "Enter file descriptor of file you want to Append\n";
            cin >> check;
            if (fmode.find(check) != fmode.end())
            {
                if (fmode[check] == 2)
                {
                    long long blockno = inode[check]->firstblock;
                    if (blockno == -1)
                    {
                        cout << "Inode is not pointing to any block\n";
                    }
                    int cnt = 0;
                    int end = 0;
                    do
                    {
                        char *currblock = &(dskblk[blockno]->info[0]);

                        for (int i = 0; i < Max; i++)
                        {
                            if (currblock[i] != '\0')
                            {
                                cnt++;
                            }
                            else
                            {
                                end = 1;
                                break;
                            }
                        }
                        if (!end)
                        {
                            blockno = dskblk[blockno]->blockpointer;
                            cnt = 0;
                        }
                        else
                        {
                            break;
                        }
                    } while (blockno >= 0);

                    cout << "Enter content of file\n";
                    string inpstr = "";
                    string inptemp = "";
                    cin.ignore();
                    while (1)
                    {
                        getline(cin, inptemp);
                        if (inptemp != "stop")
                        {
                            inpstr += inptemp + "\n";
                        }
                        else
                        {
                            inpstr.pop_back();
                            break;
                        }
                    }
                    if (blockno >= 0)
                    {
                        for (int i = 0; i < inpstr.size(); i++)
                        {
                            dskblk[blockno]->info[cnt] = inpstr[i];
                            cnt++;
                            if (cnt >= Max)
                            {
                                long long blk = search_diskblock(dskblk);
                                if (blk != -1)
                                {
                                    cnt = 0;
                                    dskblk[blockno]->blockpointer = blk;
                                    blockno = blk;
                                    dskblk[blockno]->blockpointer = -2;
                                }
                                else
                                {
                                    cout << "Disk is full\n";
                                    break;
                                }
                            }
                        }
                        dskblk[blockno]->info[cnt] = '\0';
                        cout << "File Append Successfull\n";
                        write_back(sb, inode, dskblk, disk_name);
                    }
                    else
                    {
                        cout << "Error\nDisk is full\n";
                    }
                }
                else
                {
                    cout << "File is not opened in Append mode\n";
                }
            }
            else
            {
                cout << "Invalid File Descriptor\n";
            }
        }
        else if (x == 6)
        {
            cout << "Enter file Descriptor of the file you want to close\n";
            cin >> check;
            if (fmode.find(check) != fmode.end())
            {
                fmode.erase(check);
                cout << "File closed successfully\n";
            }
            else
            {
                cout << "File descriptor does not exist\n";
            }
        }
        else if (x == 7)
        {
            cout << "Enter File name to be deleted\n";
            cin >> fname;
            long long i = fileexist(inode, fname);
            if (i != -1)
            {
                int rem = i;
                int temp;
                fmode.erase(i);
                int tempblk = inode[i]->firstblock;
                if (tempblk != -1)
                {
                    do
                    {
                        temp = dskblk[tempblk]->blockpointer;
                        dskblk[tempblk] = new diskblock();
                        tempblk = temp;
                    } while (temp != -100 && temp != -1);
                    inode[rem] = new inodeinfo();
                }

                cout << "File deleted successfully\n";
                write_back(sb, inode, dskblk, disk_name);
            }
            else
                cout << "File name not exist\n";
        }
        else if (x == 8)
        {
            cout << "List of all existing files on the disk\n";
            int em = 1;
            for (int i = 0; i < inode.size(); i++)
            {
                if (inode[i]->firstblock != -1)
                {
                    em = 0;
                    string names = inode[i]->name;
                    cout << names << endl;
                }
            }
            if (em)
            {
                cout << "No files exist\n";
            }
        }
        else if (x == 9)
        {
            cout << "List of all existing files which are currently open\n";
            int em = 1;
            for (auto f : fmode)
            {
                em = 0;
                cout << inode[f.first]->name << " " << f.first << " " << dif_modes[f.second] << endl;
            }
            if (em)
            {
                cout << "No files are currently opened\n";
            }
        }
        else if (x == 10)
        {
            cout << "Disk unmount successfull\n";
            break;
        }
    }
    return;
}
void mount_file(string str)
{
    superblock *sb = new superblock();
    vector<inodeinfo *> inode(sb->total_inodes);
    vector<diskblock *> dskblk(sb->total_blocks);
    for (int i = 0; i < sb->total_blocks; i++)
    {
        dskblk[i] = new diskblock();
    }
    for (int i = 0; i < sb->total_inodes; i++)
    {
        inode[i] = new inodeinfo();
    }
    ifstream opfile;
    opfile.open(str, ios::in);
    opfile.read((char *)&(*sb), sizeof(superblock));
    for (long long i = 0; i < sb->total_inodes; i++)
    {
        opfile.read((char *)&(*inode[i]), sizeof(inodeinfo));
    }
    for (long long i = 0; i < sb->total_blocks; i++)
    {
        opfile.read((char *)&(*dskblk[i]), sizeof(diskblock));
    }
    opfile.close();
    cout << "Disk Mounted successfully\n";
    section_2(sb, inode, dskblk, str);
}
void user_interface()
{
    cout << "**************WELCOME*************\n";
    while (1)
    {
        long long ch;
        string str;
        cout << endl;
        cout << "**************Disk Menu*************\n";
        cout << " Press 1 for Create Disk\n";
        cout << " Press 2 for Mount Disk\n";
        cout << " Press 3 for Exit\n";
        cin >> ch;
        if (ch == 1)
        {
            cout << "Enter Unique Disk name\n";
            cin >> str;
            struct stat buffer;
            if (stat(str.c_str(), &buffer) == 0)
            {
                cout << "Disk name already exist\n";
            }
            else
            {
                make_disk(str);
                cout << "Disk Created successfully\n";
            }
        }
        else if (ch == 2)
        {
            string str;
            cout << "Enter Disk name\n";
            cin >> str;
            struct stat buffer;
            if (stat(str.c_str(), &buffer) != 0)
            {
                cout << "Disk name not exist\n";
            }
            else
            {
                mount_file(str);
                
            }
        }
        else if (ch == 3)
        {
            cout << "Good Bye\n";
            return;
        }
        else
        {
            cout << "Invalid choice\n";
        }
    }
}
int main()
{
    user_interface();
    return 0;
}