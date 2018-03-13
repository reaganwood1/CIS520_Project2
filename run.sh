#!/bin/sh
clear
clear
echo entering directory /examples ...
cd ../examples/
make

echo Entering directory /userprog ...
cd ../userprog/
make
# Stop for the user to enter key
read -rp 'Press enter to continue' secondyn </dev/tty

echo Entering directory /userprog/build ...
cd build
pintos-mkdisk filesys.dsk --filesys-size=2;
pintos -f -q;
pintos -p ../../examples/echo -a echo -- -q;
pintos -q run 'echo x' ;
