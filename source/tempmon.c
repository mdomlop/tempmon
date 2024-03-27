#include <glob.h>
#include <stdio.h>
#include <libgen.h>  /* POSIX basename */
#include <string.h>
#include <stdlib.h> /* exit */
#include <unistd.h>  /* POSIX access */

#define PROGRAM     "TempMon"
#define EXECUTABLE  "tempmon"
#define DESCRIPTION "Simple console temperature monitor for Linux."
#define VERSION     "0.9a"
#define URL         "https://github.com/mdomlop/tempmon"
#define LICENSE     "GPL3"
#define AUTHOR      "Manuel Domínguez López"
#define NICK        "mdomlop"
#define MAIL        "zqbzybc@tznvy.pbz"

#define SIZE 2550

char content[SIZE];

const char * readfile(char * fileaddr)
{
    FILE * fptr;
    int fscanf_ok;

    if ((fptr = fopen(fileaddr, "r")) == NULL)
    {
        fprintf(stderr, "\nERROR: File not found: %s\n", fileaddr);

        exit(1);
    }

    /* Reads text until newline is encountered */
    fscanf_ok = fscanf(fptr, "%2550[^\n]", content);
    fclose(fptr);

    if (fscanf_ok)
        return content;
    return 0;
}

char * cut(char * s)
{
    /* Cutting string */
    int i = 0;
    while (s[i] != '_')  /* Read string until '_' char */
        i++;
    s[i] = '\0';  /* cut here with '\0' */

    return s;
}


void printlabel(char * path)
{
    strcat(path, "_label");

    if(access(path, F_OK) != -1 )
        printf("%s: ", readfile(path));
    else
        printf("None: ");
}


void printgrades(char * path)
{
    strcat(path, "_input");

    if(access(path, F_OK) != -1 )
    {
        printf("%.2f\n", atof(readfile(path)) / 1000);
    }
    else
        puts("None");
}


int main(int argc, char ** argv)
{
    glob_t deviceglobbuf = {0};
    glob_t sensorglobbuf = {0};

    int i, j;

    char device[SIZE];
    char aux[SIZE];
    char prev[SIZE];

    glob("/sys/class/hwmon/*/name", GLOB_DOOFFS, NULL, &deviceglobbuf);

    for (i = 0; i < deviceglobbuf.gl_pathc; i++)
    {
        strcpy(device, readfile(deviceglobbuf.gl_pathv[i]));

        strcpy(aux, dirname(deviceglobbuf.gl_pathv[i]));
        strcat(aux, "/temp*_*");
        glob(aux, GLOB_DOOFFS, NULL, &sensorglobbuf);

        for (j = 0; j < sensorglobbuf.gl_pathc; j++)
        {
            strcpy(aux, cut(sensorglobbuf.gl_pathv[j]));

            if(j == 0)
            {
				printf("[%s] ", device);
                printlabel(cut(sensorglobbuf.gl_pathv[j]));
                printgrades(cut(sensorglobbuf.gl_pathv[j]));
            }
			else
			{
				strcpy(prev, cut(sensorglobbuf.gl_pathv[j-1]));
				if(strcmp(aux, prev))
				{
					printf("[%s] ", device);
					printlabel(cut(sensorglobbuf.gl_pathv[j]));
					printgrades(cut(sensorglobbuf.gl_pathv[j]));
				}
                /*
				else
					puts("Fail"); */
			}
        }

        globfree(&sensorglobbuf);
        puts("");
    }

    globfree(&deviceglobbuf);

    return 0;
}
