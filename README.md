# LumOS
A work in progress kernel (maybe more someday) for the i386 arch.
## Building and running on qemu
- Ensure you have `qemu` and `docker` installed. The scripts should take care of the rest.
- To build an iso file, run `docker-compose up photon_build`, which should write the iso to the _theOS.iso_ file in the root directory.
- You can also use the `clean.sh` script to clean up any artifacts from a build.
## DIR structure
```
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
```

# Appendix
1. What would I even do without https://wiki.osdev.org/Expanded_Main_Page ?