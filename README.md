# OS Development
Prithivi's project main directory
## DIR structure
.  
├── kernel  
│   ├── arch  
│   │   └── i386  
│   └── kernel  
├── libc  
│   ├── arch  
│   │   └── i386  
│   ├── include  
│   │   └── sys  
│   ├── stdio  
│   ├── stdlib  
│   └── string  
└── README.md  

### Environment Variables
- HOST : has the prefix `i386-elf`
- AR : the **archiver** for the targer host
- CC : **compiler** for the target host
- AS : **assembler** for the target host
- PRE : has the prefix /usr/ for finding the includes and the libraries
- LIB_DIR : location of the lib directory from sysroot
- INCLUDE_DIR : Location of the inlcude directory from the sysroot
- BOOT_DIR : boot directory
- SYSROOT : location of sysroot from the root of the current(development) OS
  
### Bash global
- MAJOR_FOLDERS : List of folders that contains headers, C files or .S files.

