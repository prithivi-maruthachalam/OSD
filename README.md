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

## Info
- Only the i386 architecture

## Instructions for use
- Add the cross/bin folder of your cross compiler to the system PATH.
- Make sure all the relevant files in the following folders have executable permisison
    - cross/bin
    - cross/libexec/gcc/i686-elf/10.2.0
    - cross/i686-elf/bin
    
    where cross is the root folder of our cross compiler build

- Run the relevant script in the project root to build.