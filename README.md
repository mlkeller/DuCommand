# DuCommand
Mimics the du command used in terminal

To run the DiskUsage program, copy over all the files. 
Run make with the makefile and it will create an executable named DiskUsage.

DiskUsage has four flags in total.

The -a flag will show the diskusage for files as well as directories and if a max depth is not included then it will show all every file in the hierarchy.

The -max-depth=N flag will allow you to specify how many directories deep you wish the du command to print out. [N] being the depth number you specify.

The -c flag will include the total disk usage for all files in the hierarchy.

The -h flag will show the output in human readable sizes such as bytes, kilobytes, megabytes, and gigabytes.

All flags may be run simultaniously if desired. 
