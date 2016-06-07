typedef struct Flags{
   int depth;
   int humanRead;
   int total;
   int all;
   int dflag;
} Flags;

void InitFlags(Flags *flag);

int CheckDirectory(char *path);

void PrintHumanDirect(double count);

void PrintDirect(char *path, double count, Flags *flag, int deep);

double PrintHumanSize(struct stat buf, int deep, Flags *flag);

double Print(Flags *flag, char *path, int deep, char *bd, struct dirent* fi);

double Execute(char *path, char *build, Flags *flag, int deep);

void ReadCommands(char **args, int argc, Flags *flag);

