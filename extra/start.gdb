# GDB startup script for debugging a program in flash ROM

# Connect to the GDB server

target remote localhost:3333

# Reset target & gain control

monitor reset

# manual memory options

#mem 0x00000000 0x00100000 ro
#mem 0x20000000 0x20020000 rw
#mem 0x10000000 0x10010000 rw

# Display memory regions

info mem

# Break at beginning of main()

break main
#continue
