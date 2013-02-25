#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <time.h>

typedef struct _cpuOccupy {
    char name[20];
    unsigned int user;
    unsigned int nice;
    unsigned int system;
    unsigned int idle;
    unsigned int iowait;
    unsigned int irq;
    unsigned int softirq;
} cpuOccupy;

typedef struct _memOccupy {
    char name[20];
    unsigned long total;
    unsigned long free;
} memOccupy;

int calCpuOccupy(cpuOccupy *c1, cpuOccupy *c2) {
    unsigned long total1, total2 , id, sd, xd;
    int cpuUsage = 0;

    total1 = c1->user + c1->nice + c1->system + c1->idle + c1->iowait + c1->irq + c1->softirq;
    total2 = c2->user + c2->nice + c2->system + c2->idle + c2->iowait + c2->irq + c2->softirq;

    id = c2->user - c1->user;
    sd = c2->system - c1->system;
    xd = c2->nice - c1->nice;

    if((total2 - total1) != 0)
        cpuUsage = ((sd + id + xd) *100) / (total2 - total1);
    else
        cpuUsage = 0;
    return cpuUsage;
}

void getCpuOccupy(cpuOccupy *cpust) {
    FILE *fp;
    char buf[256];
    cpuOccupy *cpuOcc;
    cpuOcc = cpust;

    fp = fopen("/proc/stat", "r");
    if(!fp) {
        fprintf(stderr, "open /proc/stat error!\n");
        return;
    }
    fgets(buf, sizeof(buf), fp);
    sscanf(buf, "%s %u %u %u %u %u %u %u", cpuOcc->name, &cpuOcc->user, &cpuOcc->nice, &cpuOcc->system, &cpuOcc->idle, &cpuOcc->iowait
            ,&cpuOcc->irq, &cpuOcc->softirq);
    fclose(fp);
}

void getMemOccupy(memOccupy *memst) {
    FILE *fp;
    char buf[256];
    memOccupy *memOcc;
    memOcc = memst;

    fp = fopen("/proc/meminfo", "r");
    if(!fp) {
        fprintf(stderr, "open /proc/meminfo!\n");
        return;
    }
    fgets(buf, sizeof(buf), fp);
    sscanf(buf, "%s %lu %s", memOcc->name, &memOcc->total, memOcc->name); 
    fgets(buf, sizeof(buf), fp);
    sscanf(buf, "%s %lu %s", memOcc->name, &memOcc->free, memOcc->name);
}

void printUsage() {
    cpuOccupy cpuStat1;
    cpuOccupy cpuStat2;
    memOccupy memstat;
    int cpuUsage;
    int memUsage;

    memset(&cpuStat1, 0, sizeof(cpuOccupy));
    memset(&cpuStat2, 0, sizeof(cpuOccupy));
    memset(&memstat, 0, sizeof(memOccupy));
    
    getMemOccupy(&memstat);
    if(memstat.total != 0)
        memUsage = ((memstat.total - memstat.free) * 100) / memstat.total;
    else
        memUsage = 0;

    getCpuOccupy(&cpuStat1);
    sleep(3);
    getCpuOccupy(&cpuStat2);
    cpuUsage = calCpuOccupy(&cpuStat1, &cpuStat2);
    fprintf(stdout, "CPU Usage: %3d%%\t MEM Usage: %3d%%\n", cpuUsage, memUsage);
    fflush(stdout);
}

void printHelp() {
    fprintf(stderr, "Usage: sysInfoRec -t 10\n");
}   

int main(int argc, char ** argv) {
    if(argc < 2) {
        printHelp();
        exit(1);
    }
    int opt;
    int duration;
    while((opt = getopt(argc, argv, "t:")) != -1) {
        switch(opt) {
        case 't':
            duration = atoi(optarg);
            break;
        default:
            break;
        }
    }

    if(duration <= 0)
        duration = 1;

    while(1) {
        printUsage();     
        sleep(duration);
    }
    return 0;
}
