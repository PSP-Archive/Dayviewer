rm dayviewer.prx
rm kernel/dayviewer_user.h
cd user
make
bin2c dayviewer_user.prx ../kernel/dayviewer_user.h dayviewer_user
make clean
cd..
cd kernel
make
psp-packer dayviewer.prx
move dayviewer.prx ../dayviewer.prx
make clean
cd..