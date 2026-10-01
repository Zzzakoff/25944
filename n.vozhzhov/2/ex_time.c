#include <stdio.h>
#include <time.h>

int main(){
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    rawtime -= (8 * 3600);
    timeinfo = gmtime(&rawtime);
    printf("%02d.%02d.%d %02d:%02d:%02d\n",
           timeinfo->tm_mday,
           timeinfo->tm_mon + 1,
           timeinfo->tm_year + 1900,
           timeinfo->tm_hour,
           timeinfo->tm_min,
           timeinfo->tm_sec);
    return 0;
}