# LumOS
A work in progress kernel (maybe more someday) for the i386 arch.

## Building and Running
### Requirements
Make sure you have the following tools installed
- `qemu` - While the build process is dockerized, qemu is how we run the iso file that the builder generates.
- `docker` - Need this to run the build as the cross compiler is built into a container.
### Things you should do
- **Building** : Running `docker-compose up lumos-builder-build` should build the kernel and write it as a bootable image to _theOS.iso_ in the root directory.
- **Running** : If the iso is available, you can simply run `bin/qemu.sh` to run the kernel on qemu
- **Cleanup** : Run the `bin/clean.sh` script to remove all artifacts from a build.

## Appendix
### Links that have kept LumOS going
- What would I even do without https://wiki.osdev.org/Expanded_Main_Page ?
### DIR Structure
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