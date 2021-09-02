//Joseph Steeb 11751063
//9-1-2021

#include <stdio.h>
#include <fcntl.h>

#include <sys/types.h>
#include <unistd.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct partition
{
    u8 drive; // drive number FD=0, HD=0x80, etc.

    u8 head;     // starting head
    u8 sector;   // starting sector
    u8 cylinder; // starting cylinder

    u8 sys_type; // partition type: NTFS, LINUX, etc.

    u8 end_head;     // end head
    u8 end_sector;   // end sector
    u8 end_cylinder; // end cylinder

    u32 start_sector; // starting sector counting from 0
    u32 nr_sectors;   // number of of sectors in partition
};

char *dev = "vdisk";
int fd;

// read a disk sector into char buf[512]
int read_sector(int fd, int sector, char *buf)
{
    lseek(fd, sector * 512, SEEK_SET); // lssek to byte sector*512
    read(fd, buf, 512);                // read 512 bytes into buf[ ]
}

int main()
{
    struct partition *p;
    struct partition *current;
    u32 ext_start;

    char buf[512];

    fd = open(dev, O_RDONLY); // open dev for READ
    read_sector(fd, 0, buf);  // read in MBR at sector 0

    p = (struct partition *)(&buf[0x1be]); // p->P1
    current = p;
    // print P1's start_sector, nr_sectors, sys_type;
    printf("p1: start_sector= %u nr_sector= %u sys_type= %u\n\n", p->start_sector, p->nr_sectors, p->sys_type);

    printf("\tstart_sector\tend_sector\tnrsectors\n");
    int i;
    for (i = 0; i < 4; i++)
    {
        printf("p%d: \t%u\t\t%u\t\t%u\n", i + 1, current->start_sector, (current->nr_sectors + current->start_sector), current->nr_sectors);
        if (i != 3)
            current++;
        else
            ext_start = current->start_sector;
    }

    printf("\n");

    //printf("start of first ext: %p\n", current);

    u32 c_start = current->start_sector;
    u32 addition = 0;
    u32 s[2];
    u32 n[2];
    int j = 1;
    do
    {
        read_sector(fd, c_start, buf);
        current = (struct partition *)(&buf[0x1be]);

        for (i = 0; i < 2; i++)
        {
            s[i] = current->start_sector;
            n[i] = current->nr_sectors;
            printf("Entry%d: start_sector: %u end_sector: %u\n", i, s[i], n[i]);
            current++;
        }

        printf("\n\tstart_sector\tend_sector\tnr_sector\n");
        printf("p%d: \t%u\t\t%u\t\t%u\n", j + 4, s[0] + c_start + addition, s[0] + n[0] + c_start - 1, n[0]);
        c_start = s[1] + ext_start;
        if (s[1] != 0)
            printf("next MBR= %u + %u = %u\n\n", s[1], ext_start, c_start);
        else
            printf("End of extended partitions\n");
        j++;
    } while (s[1] != 0);
    // st
    //     printf("p1:\t%u%u%u", )

    // Write code to print all 4 partitions;

    // ASSUME P4 is EXTEND type:
    // Let int extStart = P4's start_sector; print extStart to see it;

    //     localMBR = extStart;
    // loop:
    //     read_sector(fd, localMBR, buf);

    //     // partition table of localMBR in buf[ ] has 2 entries:
    //     // print entry 1's start_sector, nr_sector;
    //     // compute and print P5's begin, end, nr_sectors

    //         if (entry 2's start_sector != 0)
    //     {
    //         compute and print next localMBR sector;
    //         continue loop;
    //     }
}
