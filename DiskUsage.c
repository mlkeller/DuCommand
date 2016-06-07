#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "DiskUsage.h"

#define PATH_LEN 1024
#define BYTEROUND 10
#define OFFSET 1000.0
#define KBYTE 1000.0
#define MBYTE 1000000.0
#define MAXPLACE 99.0
#define MILLP 10.0
#define MACH 4096



void InitFlags(Flags *flag) {
   flag->depth = -1;
   flag->humanRead = 0;
   flag->total = 0;
   flag->all = 0;
   flag->dflag = 0;
}

int CheckDirectory(char *path) {
   int rv = 0;
   struct stat buf;
   if (stat(path, &buf) != 0) {
      rv = 0;
   }
   else {
      rv = S_ISDIR(buf.st_mode);
   }
    return rv;
}

void PrintHumanDirect(double count) {
   count = count / 2.0;
   if (count == 0) {
      printf("  0B\t");
   }
   else if (count < 10) {
      printf("%.1fK\t", count);
   }
   else if (count < MAXPLACE) {
      printf(" %0.0lfK\t", count);
   }
   else if (count < KBYTE) {
      printf("%.0lfK\t", count);
   }
   else if (count < MBYTE) {
      count = count / KBYTE;
      count -= .05;
      if (count < MILLP) {
         printf("%.1lfM\t", count);
      }
      else if (count <= MAXPLACE) {
         printf(" %.0lfM\t", count);
      }
      else {
         printf("%.0lfM\t", count);
      }
   }
   else {
      count = count / MBYTE;
      count -= .05;
      if (count < MILLP) {
         printf("%.1lfG\t", count);
      }
      else if (count <= MAXPLACE) {
         printf(" %.0lfG\t", count);
      }
      else {
         printf("%.0lfG\t", count);
      }
   }
}

void PrintDirect(char *path, double count, Flags *flag, int deep) {
   char *temp = path;

   while (*temp != '\0') {
      temp++;
   }
   temp--;
   *temp = '\0';
   if ((!flag->dflag || (deep > 0))) {
      if (flag->humanRead) {
         PrintHumanDirect(count);
      }
      else {
         printf("%g\t", count);
      }
      printf("%s\n", path);
   }
}


double PrintHumanSize(struct stat buf, int deep, Flags *flag) {
   double size = 0;

   size = buf.st_blocks / 2.0;
   if (size > BYTEROUND) {
      if (size >= OFFSET) {
         size = size / KBYTE;
         if (flag->all && (!flag->dflag || (deep > 0))) {
            printf("%.1gM\t", size);
         }
      }
      else {
         if (flag->all && (!flag->dflag || (deep > 0))) {
            if (size > MAXPLACE) {
               printf("%.0lfK\t", size);
            }
            else {
               printf(" %0.0lfK\t", size);
            }
         }
      }
   }
   else {
      if (size == 0) {
         if (flag->all && (!flag->dflag || (deep > 0))) {
            printf("  0B\t");
         }
      }
      else {
         if (flag->all && (!flag->dflag || (deep > 0))) {
            printf("%.1lfK\t", size);
         }  
      }
   }
   size = buf.st_blocks / 2.0;
   return size;
}

double Print(Flags *flag, char *path, int deep, char *bd, struct dirent* fi) {
   struct stat buf;
   char temp[PATH_LEN];
   double size = 0;

   strcpy(temp, bd);
   strcat(temp, fi->d_name);

   if (stat(temp, &buf) == -1) {
         printf("Error.\n");
   }
   if (flag->humanRead) {
      size = PrintHumanSize(buf, deep, flag);
      strcpy(temp, bd);
      strcat(temp, fi->d_name);
      if (flag->all && (!flag->dflag || (deep > 0))) {
         printf("%s\n", temp);
      }
   }
   else if (flag->all && (!flag->dflag || (deep > 0))) {
      size = buf.st_blocks;
      if (buf.st_blksize > MACH) {
         size = size / 2;
      }
      printf("%.0lf\t", size); 
      strcpy(temp, bd);
      strcat(temp, fi->d_name);
      printf("%s\n", temp);
   }
   if (buf.st_blksize > MACH && !flag->humanRead) {
      return buf.st_blocks / 2;
   }
   return buf.st_blocks;
}

double Execute(char *path, char *build, Flags *flag, int deep) {
   char temp[PATH_LEN];
   char pathTemp[PATH_LEN];
   struct dirent* fi;
   double count = 0, count2 = 0;
   DIR* direct;

   strcat(build, "/");
   if ((direct = opendir(path)) == NULL) {
      printf("Could not open directory.\n");
      exit(EXIT_FAILURE);
   }
   while ((fi = readdir(direct))) {
      if (!strcmp(fi->d_name, ".")) {
         continue;
      }
      if (!strcmp(fi->d_name, "..")) {
         continue;
      }
      strcpy(temp, build);
      strcat(temp, fi->d_name);
      if (CheckDirectory(temp)) {
         strcpy(pathTemp, path);
         strcat(pathTemp, "/");
         strcat(pathTemp, fi->d_name);
         count2 += Execute(pathTemp, temp, flag, deep - 1);
         PrintDirect(temp, count2, flag, deep);
         count += count2;
         count2 = 0;
      }
      else {
         count += Print(flag, temp, deep, build, fi);
      }
   }
   return count;
}

void ReadCommands(char **args, int argc, Flags *flag) {
   int count = 1;
   int activated = 1;
   int value;
   char *temp, *token;

   while (count < argc) {
      if (!strcmp(args[count], "-a")) {
         flag->all = 1;
      }
      if (!strcmp(args[count], "-h")) {
         flag->humanRead = 1;
      }
      if (!strcmp(args[count], "-c")) {
         flag->total = 1;

      }
      else if (activated) {
         temp = args[count];
         token = strtok(temp, "=");
         if(token != NULL && !strcmp(token, "-max-depth")) {
            activated = 0;
            token = strtok(NULL, " ");
            if (token != NULL) {
               value = atoi(token);
               if (value >= 0) {
                  flag->depth = value;
                  flag->dflag = 1;
               }
            }
         }
      }
      count++;
   }
}

int main(int argc, char **argv) {

   Flags *flag;
   char path[PATH_LEN];
   char temp[PATH_LEN];
   double total;

   flag = malloc(sizeof(Flags));
   InitFlags(flag);
   ReadCommands(argv, argc, flag);
   strcpy(temp, ".");
   getcwd(path, sizeof(path));
   total = Execute(path, temp, flag, flag->depth);
   if (flag->humanRead) {
      PrintHumanDirect(total);
   }
   else {
      printf("%g\t", total);
   }
   printf(".\n");
   if (flag->total && (!flag->dflag || (flag->depth > 0))) {
      if (flag->humanRead) {
         PrintHumanDirect(total);
      }
      else {
         printf("%g\t", total);
      }
      printf("total\n");
   }
   free(flag);
   return 0;

}