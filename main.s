        .gba
        .thumb
        .open "BPRE0.gba","build/Option Menu.gba", 0x08000000

        .include "patches/pointer.s"

        .org 0x0872EDE0
        .importobj "build/linked.o"
        .close
