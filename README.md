# linuxvme

These are drivers that are used on a specific vme setup in linux.  They are here in case we need to recreate the system that is running.  We are unable to hook up a usb or ide drive or cdrom to the vme computers, so we have to install the operating system elsewhere and then physically move the disk to the vme system.

## Making a new version of the system.

*  Install SLC45 on a new disk.  Choose "Custom Install".  In the list of packages that comes up, you can uncheck "Text-based Internet, Office/Productivity, Sound & Video".  You should add "Development Tools" and in the Gnome Desktop Environment, also add the program "eog".

*  Once the system is installed, check if it's running a multiprocessor kernel.  Change /etc/grub.conf to use the single processor kernel.  Reboot so that we start using that kernel.

*  There are a few other packages to install.  I didn't feel like searching for them during the installation process, so I did them by hand later.

ImageMagick
libpng-devel
libjpeg-devel
freetype-devel
libtiff-devel
zlib-devel

*  For some reason, all the links needed were not created in /usr/lib.  I had to add some of these by hand.

```
# cd /usr/lib
# ln -s libjpeg.so.62.0.0 libjpeg.so
# ln -s libpng.so.3 libpng.so
# ln -s libtiff.so.3.6 libtiff.so
# ln -s libfreetype.so.6.3.7 libfreetype.so
# ln -s libz.so.1.2.1.2 libz.so
# ln -s libgd.so.2.0.0 libgd.so
```

*  Download the files in this repo.  The directories named linuxvme, linuxvme_examples and linuxvme_util should be placed in /usr/local.  The directory kvaser should be in the / directory.  The directory kelby should be a user's directory in /home.

```
# ll /
total 3590
drwxr-xr-x   2 root root    4096 Nov 14 08:51 bin
drwxr-xr-x   4 root root    1024 Nov 14 07:21 boot
drwxr-xr-x   9 root root    6480 Nov 14 07:44 dev
drwxr-xr-x  82 root root   12288 Nov 14 08:51 etc
drwxr-xr-x   2 root root    4096 Nov 14 07:56 foo
drwxr-xr-x   3 root root    4096 Nov 14 07:42 home
drwxr-xr-x   2 root root    4096 Feb 17  2005 initrd
drwxr-xr-x   4 root root    4096 Nov 14 07:48 kvaser
-rw-r--r--   1 root root 3497709 Nov 14 08:08 kvaser.tgz
drwxr-xr-x  12 root root    4096 Nov 14 08:51 lib
drwx------   2 root root   16384 Nov 14 01:19 lost+found
drwxr-xr-x   2 root root    4096 Feb 17  2005 media
drwxr-xr-x   2 root root    4096 May  3  2007 misc
drwxr-xr-x   2 root root    4096 Feb 17  2005 mnt
drwxr-xr-x   2 root root    4096 Feb 17  2005 opt
dr-xr-xr-x  91 root root       0 Nov 14 01:44 proc
drwxr-x---  14 root root    4096 Nov 14 09:14 root
drwxr-xr-x   2 root root   12288 Nov 14 08:51 sbin
drwxr-xr-x   2 root root    4096 Nov 14 07:19 selinux
drwxr-xr-x   2 root root    4096 Feb 17  2005 srv
drwxr-xr-x   9 root root       0 Nov 14 01:44 sys
drwxrwxrwt  11 root root    4096 Nov 14 08:59 tmp
drwxr-xr-x  17 root root    4096 Nov 14 07:50 usr
drwxr-xr-x  19 root root    4096 Nov 14 07:24 var

# ll /usr/local
total 1284
drwxr-xr-x  2 root root    4096 Feb 17  2005 bin
drwxr-xr-x  2 root root    4096 Feb 17  2005 etc
drwxr-xr-x  2 root root    4096 Feb 17  2005 games
drwxr-xr-x  2 root root    4096 Feb 17  2005 include
drwxr-xr-x  2 root root    4096 Feb 17  2005 lib
drwxr-xr-x  2 root root    4096 Feb 17  2005 libexec
drwxr-xr-x  8 root root    4096 Nov 14 07:49 linuxvme
-rw-r--r--  1 root root 1214207 Nov 14 08:09 linuxvme_dirs.tgz
drwxr-xr-x  2 root root    4096 Nov 14 07:50 linuxvme_examples
drwxr-xr-x  2 root root    4096 Nov 14 07:50 linuxvme_util
drwxr-xr-x  3 root root    4096 Nov 14 07:23 man
drwxr-xr-x  2 root root    4096 Feb 17  2005 sbin
drwxr-xr-x  4 root root    4096 Nov 14 07:19 share
drwxr-xr-x  2 root root    4096 Feb 17  2005 src
```

*  Recompile the files in these directories.

```
# pwd
/kvaser/linuxcan_v4

# make clean
make[1]: Entering directory `/kvaser/linuxcan_v4/canlib'
rm -f  canlib.o  linkedlist.o  VCanFunctions.o  ../common/osif_functions_user.o libcanlib.so.1.0.1  canlib.d  linkedlist.d  VCanFunctions.d  ../common/osif_functions_user.d libcanlib.so libcanlib.so.1.0.1 libcanlib.so.1 canlib-1.0.1.tar.gz *~
make -C examples clean
make[2]: Entering directory `/kvaser/linuxcan_v4/canlib/examples'
rm -f busparms readTimerTest canmonitor writeloop cancount simplewrite listChannels *.o *~
make[2]: Leaving directory `/kvaser/linuxcan_v4/canlib/examples'
make[1]: Leaving directory `/kvaser/linuxcan_v4/canlib'
make[1]: Entering directory `/kvaser/linuxcan_v4/pcican'
rm -f *.o *.ko *.mod.c 
make[1]: Leaving directory `/kvaser/linuxcan_v4/pcican'
make[1]: Entering directory `/kvaser/linuxcan_v4/pcicanII'
rm -f *.o *.ko *.mod.c 
make[1]: Leaving directory `/kvaser/linuxcan_v4/pcicanII'
make[1]: Entering directory `/kvaser/linuxcan_v4/usbcanII'
rm -f *.o *.ko *.mod.c
make[1]: Leaving directory `/kvaser/linuxcan_v4/usbcanII'
make[1]: Entering directory `/kvaser/linuxcan_v4/leaf'
rm -f *.o *.ko *.mod.c
make[1]: Leaving directory `/kvaser/linuxcan_v4/leaf'
make[1]: Entering directory `/kvaser/linuxcan_v4/virtualcan'
rm -f *.o *.ko *.mod.c 
make[1]: Leaving directory `/kvaser/linuxcan_v4/virtualcan'


# make install
--------------------------------------------------------------------
building CANLIB
--------------------------------------------------------------------
cd canlib
cp ./canlib/Makefile_26 ./canlib/Makefile -f
cd ..
make -C canlib sub
make[1]: Entering directory `/kvaser/linuxcan_v4/canlib'
cc -M -Wall -DCANLIB_NAME_STRING=\"libcanlib.so.1.0.1\" -O2 -fomit-frame-pointer   -DSPLIT -DLINUX=1 -D_LINUX=1  -I. -I../include canlib.c > canlib.d 
cc -M -Wall -DCANLIB_NAME_STRING=\"libcanlib.so.1.0.1\" -O2 -fomit-frame-pointer   -DSPLIT -DLINUX=1 -D_LINUX=1  -I. -I../include canlib.c | sed s/\\.o/.d/> canlib.d 
cc -M -Wall -DCANLIB_NAME_STRING=\"libcanlib.so.1.0.1\" -O2 -fomit-frame-pointer   -DSPLIT -DLINUX=1 -D_LINUX=1  -I. -I../include linkedlist.c > linkedlist.d 
cc -M -Wall -DCANLIB_NAME_STRING=\"libcanlib.so.1.0.1\" -O2 -fomit-frame-pointer   -DSPLIT -DLINUX=1 -D_LINUX=1  -I. -I../include linkedlist.c | sed s/\\.o/.d/> linkedlist.d 
cc -M -Wall -DCANLIB_NAME_STRING=\"libcanlib.so.1.0.1\" -O2 -fomit-frame-pointer   -DSPLIT -DLINUX=1 -D_LINUX=1  -I. -I../include VCanFunctions.c > VCanFunctions.d 
cc -M -Wall -DCANLIB_NAME_STRING=\"libcanlib.so.1.0.1\" -O2 -fomit-frame-pointer   -DSPLIT -DLINUX=1 -D_LINUX=1  -I. -I../include VCanFunctions.c | sed s/\\.o/.d/> VCanFunctions.d 
cc -M -Wall -DCANLIB_NAME_STRING=\"libcanlib.so.1.0.1\" -O2 -fomit-frame-pointer   -DSPLIT -DLINUX=1 -D_LINUX=1  -I. -I../include ../common/osif_functions_user.c > ../common/osif_functions_user.d 
cc -M -Wall -DCANLIB_NAME_STRING=\"libcanlib.so.1.0.1\" -O2 -fomit-frame-pointer   -DSPLIT -DLINUX=1 -D_LINUX=1  -I. -I../include ../common/osif_functions_user.c | sed s/\\.o/.d/> ../common/osif_functions_user.d 
cc -Wall -DCANLIB_NAME_STRING=\"libcanlib.so.1.0.1\" -O2 -fomit-frame-pointer   -DSPLIT -DLINUX=1 -D_LINUX=1  -I. -I../include -Wall -DCANLIB_NAME_STRING=\"libcanlib.so.1.0.1\" -O2 -fomit-frame-pointer   -DSPLIT -DLINUX=1 -D_LINUX=1  -I. -I../include -D_REENTRANT -fPIC  -c -o canlib.o canlib.c 
cc -Wall -DCANLIB_NAME_STRING=\"libcanlib.so.1.0.1\" -O2 -fomit-frame-pointer   -DSPLIT -DLINUX=1 -D_LINUX=1  -I. -I../include -Wall -DCANLIB_NAME_STRING=\"libcanlib.so.1.0.1\" -O2 -fomit-frame-pointer   -DSPLIT -DLINUX=1 -D_LINUX=1  -I. -I../include -D_REENTRANT -fPIC  -c -o linkedlist.o linkedlist.c 
cc -Wall -DCANLIB_NAME_STRING=\"libcanlib.so.1.0.1\" -O2 -fomit-frame-pointer   -DSPLIT -DLINUX=1 -D_LINUX=1  -I. -I../include -Wall -DCANLIB_NAME_STRING=\"libcanlib.so.1.0.1\" -O2 -fomit-frame-pointer   -DSPLIT -DLINUX=1 -D_LINUX=1  -I. -I../include -D_REENTRANT -fPIC  -c -o VCanFunctions.o VCanFunctions.c 
cc -Wall -DCANLIB_NAME_STRING=\"libcanlib.so.1.0.1\" -O2 -fomit-frame-pointer   -DSPLIT -DLINUX=1 -D_LINUX=1  -I. -I../include -Wall -DCANLIB_NAME_STRING=\"libcanlib.so.1.0.1\" -O2 -fomit-frame-pointer   -DSPLIT -DLINUX=1 -D_LINUX=1  -I. -I../include -D_REENTRANT -fPIC  -c -o ../common/osif_functions_user.o ../common/osif_functions_user.c 
cc -Wall -DCANLIB_NAME_STRING=\"libcanlib.so.1.0.1\" -O2 -fomit-frame-pointer   -DSPLIT -DLINUX=1 -D_LINUX=1  -I. -I../include -shared -Wl,-soname,libcanlib.so.1 -o libcanlib.so.1.0.1  canlib.o  linkedlist.o  VCanFunctions.o  ../common/osif_functions_user.o -lc -lpthread
ln -sf libcanlib.so.1.0.1 libcanlib.so
ln -sf libcanlib.so.1.0.1 libcanlib.so.1
make -C examples sub 
make[2]: Entering directory `/kvaser/linuxcan_v4/canlib/examples'
gcc -Wall -O2  -D_REENTRANT  -I../../include  -L.. -lcanlib -lpthread   busparms.c   -o busparms
gcc -Wall -O2  -D_REENTRANT  -I../../include  -L.. -lcanlib -lpthread   readTimerTest.c   -o readTimerTest
gcc -Wall -O2  -D_REENTRANT  -I../../include  -L.. -lcanlib -lpthread   canmonitor.c   -o canmonitor
gcc -Wall -O2  -D_REENTRANT  -I../../include  -L.. -lcanlib -lpthread   writeloop.c   -o writeloop
gcc -Wall -O2  -D_REENTRANT  -I../../include  -L.. -lcanlib -lpthread   cancount.c   -o cancount
gcc -Wall -O2  -D_REENTRANT  -I../../include  -L.. -lcanlib -lpthread   simplewrite.c   -o simplewrite
gcc -Wall -O2  -D_REENTRANT  -I../../include  -L.. -lcanlib -lpthread   listChannels.c   -o listChannels
make[2]: Leaving directory `/kvaser/linuxcan_v4/canlib/examples'
make[1]: Leaving directory `/kvaser/linuxcan_v4/canlib'
----------------------------------
installing CANLIB
----------------------------------
--------------------------------------------------------------------
building PCIcan
Kernel src: /lib/modules/2.6.9-55.EL/build
--------------------------------------------------------------------
cd ./pcican
cp ./pcican/Makefile_26 ./pcican/Makefile -f
make -C /lib/modules/`uname -r`/build SUBDIRS=/kvaser/linuxcan_v4/pcican modules KV_DEBUG_ON=0
make[1]: Entering directory `/usr/src/kernels/2.6.9-55.EL-i686'
  CC [M]  /kvaser/linuxcan_v4/pcican/PciCanHwIf.o
  CC [M]  /kvaser/linuxcan_v4/pcican/dallas.o
  CC [M]  /kvaser/linuxcan_v4/pcican/../common/VCanOsIf.o
  CC [M]  /kvaser/linuxcan_v4/pcican/../common/osif_functions_kernel.o
  CC [M]  /kvaser/linuxcan_v4/pcican/../common/queue.o
  LD [M]  /kvaser/linuxcan_v4/pcican/kvpcican.o
  Building modules, stage 2.
  MODPOST
  CC      /kvaser/linuxcan_v4/pcican/kvpcican.mod.o
  LD [M]  /kvaser/linuxcan_v4/pcican/kvpcican.ko
make[1]: Leaving directory `/usr/src/kernels/2.6.9-55.EL-i686'
cd ..
----------------------------------
installing PCIcan
----------------------------------
--------------------------------------------------------------------
building PCIcanII
Kernel src: /lib/modules/2.6.9-55.EL/build
--------------------------------------------------------------------
cd ./pcicanII
cp ./pcicanII/Makefile_26 ./pcicanII/Makefile -f
make -C /lib/modules/`uname -r`/build SUBDIRS=/kvaser/linuxcan_v4/pcicanII modules KV_DEBUG_ON=0
make[1]: Entering directory `/usr/src/kernels/2.6.9-55.EL-i686'
  CC [M]  /kvaser/linuxcan_v4/pcicanII/PciCanIIHwIf.o
  CC [M]  /kvaser/linuxcan_v4/pcicanII/../common/VCanOsIf.o
  CC [M]  /kvaser/linuxcan_v4/pcicanII/../common/osif_functions_kernel.o
  CC [M]  /kvaser/linuxcan_v4/pcicanII/../common/queue.o
  CC [M]  /kvaser/linuxcan_v4/pcicanII/memQ.o
  LD [M]  /kvaser/linuxcan_v4/pcicanII/kvpcicanII.o
  Building modules, stage 2.
  MODPOST
  CC      /kvaser/linuxcan_v4/pcicanII/kvpcicanII.mod.o
  LD [M]  /kvaser/linuxcan_v4/pcicanII/kvpcicanII.ko
make[1]: Leaving directory `/usr/src/kernels/2.6.9-55.EL-i686'
cd ..
----------------------------------
installing PCIcanII
----------------------------------
--------------------------------------------------------------------
building USBcanII
Kernel src: /lib/modules/2.6.9-55.EL/build
--------------------------------------------------------------------
cd ./usbcanII
cp ./usbcanII/Makefile_26 ./usbcanII/Makefile -f
make -C /lib/modules/`uname -r`/build SUBDIRS=/kvaser/linuxcan_v4/usbcanII modules KV_DEBUG_ON=0
make[1]: Entering directory `/usr/src/kernels/2.6.9-55.EL-i686'
  CC [M]  /kvaser/linuxcan_v4/usbcanII/usbcanHWIf.o
  CC [M]  /kvaser/linuxcan_v4/usbcanII/../common/VCanOsIf.o
  CC [M]  /kvaser/linuxcan_v4/usbcanII/../common/osif_functions_kernel.o
  CC [M]  /kvaser/linuxcan_v4/usbcanII/../common/queue.o
  LD [M]  /kvaser/linuxcan_v4/usbcanII/usbcanII.o
  Building modules, stage 2.
  MODPOST
  CC      /kvaser/linuxcan_v4/usbcanII/usbcanII.mod.o
  LD [M]  /kvaser/linuxcan_v4/usbcanII/usbcanII.ko
make[1]: Leaving directory `/usr/src/kernels/2.6.9-55.EL-i686'
cd ..
----------------------------------
installing USBcanII
----------------------------------
./installscript_usbcanII_26.sh: line 10: udevcontrol: command not found
--------------------------------------------------------------------
building Leaf
Kernel src: /lib/modules/2.6.9-55.EL/build
--------------------------------------------------------------------
cd ./leaf
cp ./leaf/Makefile_26 ./leaf/Makefile -f
make -C /lib/modules/`uname -r`/build SUBDIRS=/kvaser/linuxcan_v4/leaf modules KV_DEBUG_ON=0
make[1]: Entering directory `/usr/src/kernels/2.6.9-55.EL-i686'
  CC [M]  /kvaser/linuxcan_v4/leaf/leafHWIf.o
  CC [M]  /kvaser/linuxcan_v4/leaf/../common/VCanOsIf.o
  CC [M]  /kvaser/linuxcan_v4/leaf/../common/osif_functions_kernel.o
  CC [M]  /kvaser/linuxcan_v4/leaf/../common/queue.o
  LD [M]  /kvaser/linuxcan_v4/leaf/leaf.o
  Building modules, stage 2.
  MODPOST
  CC      /kvaser/linuxcan_v4/leaf/leaf.mod.o
  LD [M]  /kvaser/linuxcan_v4/leaf/leaf.ko
make[1]: Leaving directory `/usr/src/kernels/2.6.9-55.EL-i686'
cd ..
----------------------------------
installing Leaf
----------------------------------
./installscript_leaf_26.sh: line 10: udevcontrol: command not found
--------------------------------------------------------------------
building VIRTUALcan
Kernel src: /lib/modules/2.6.9-55.EL/build
--------------------------------------------------------------------
cd ./virtualcan
cp ./virtualcan/Makefile_26 ./virtualcan/Makefile -f
make -C /lib/modules/`uname -r`/build SUBDIRS=/kvaser/linuxcan_v4/virtualcan modules KV_DEBUG_ON=0
make[1]: Entering directory `/usr/src/kernels/2.6.9-55.EL-i686'
  CC [M]  /kvaser/linuxcan_v4/virtualcan/virtualcan.o
  CC [M]  /kvaser/linuxcan_v4/virtualcan/../common/VCanOsIf.o
  CC [M]  /kvaser/linuxcan_v4/virtualcan/../common/osif_functions_kernel.o
  CC [M]  /kvaser/linuxcan_v4/virtualcan/../common/queue.o
  LD [M]  /kvaser/linuxcan_v4/virtualcan/kvvirtualcan.o
  Building modules, stage 2.
  MODPOST
  CC      /kvaser/linuxcan_v4/virtualcan/kvvirtualcan.mod.o
  LD [M]  /kvaser/linuxcan_v4/virtualcan/kvvirtualcan.ko
make[1]: Leaving directory `/usr/src/kernels/2.6.9-55.EL-i686'
cd ..
----------------------------------
installing VIRTUALcan
----------------------------------
```

Now for the /usr/local/linuxvme directories

```
# cd /usr/local/linuxvme
# make clean
rm -f *.o *.a *.ko .*.cmd *.mod.c 
rm -f -r .tmp_versions
rm -f kernel-2.6.9-55.EL/*.ko	
[root@ipbus3 linuxvme]# make
make -C /lib/modules/2.6.9-55.EL/source SUBDIRS=/usr/local/linuxvme  modules 
make[1]: Entering directory `/usr/src/kernels/2.6.9-55.EL-i686'
  CC [M]  /usr/local/linuxvme/vme_driver.o
  CC [M]  /usr/local/linuxvme/vme_os.o
  CC [M]  /usr/local/linuxvme/vme_proc.o
cp /usr/local/linuxvme/cct_modules/libvmedriver26.a /usr/local/linuxvme/libvmedriver26.a
  LD [M]  /usr/local/linuxvme/vmedriver.o
  Building modules, stage 2.
  MODPOST
  CC      /usr/local/linuxvme/vmedriver.mod.o
  LD [M]  /usr/local/linuxvme/vmedriver.ko
make[1]: Leaving directory `/usr/src/kernels/2.6.9-55.EL-i686'
cp vmedriver.ko kernel-2.6.9-55.EL

# cd /usr/local/linuxvme_examples/
# make clean
rm -f example1 example2 example3 example4 example5 example6 example7 

# make
gcc -Wall example1.c -o example1 libcctvme.a  
gcc -Wall example2.c -o example2 libcctvme.a  
gcc -Wall example3.c -o example3 libcctvme.a  
gcc -Wall example4.c -o example4 libcctvme.a  
gcc -Wall example5.c -o example5 libcctvme.a  
gcc -Wall example6.c -o example6 libcctvme.a  
gcc -Wall example7.c -o example7 libcctvme.a  

# cd /usr/local/linuxvme_util/
# make clean
rm -f main.o thread.o linuxvme *~ core .depend
# make
gcc -Wall -D_REENTRANT -M *.c > .depend
gcc -Wall -D_REENTRANT   -c -o main.o main.c
gcc -Wall -D_REENTRANT   -c -o thread.o thread.c
gcc -Wall -D_REENTRANT -I/usr/include main.o thread.o -o linuxvme -L . -L /usr/lib -lncurses -lpthread -lcctvme
```

*  Edit /etc/rc.d/rc.local to load the drivers at boot.
```
# cat /etc/rc.d/rc.local
#!/bin/sh
#
# This script will be executed *after* all the other init scripts.
# You can put your own initialization stuff in here if you don't
# want to do the full Sys V style init stuff.

touch /var/lock/subsys/local

# Run linuxvme ins command
echo "Run linuxvme ins"
cd /usr/local/linuxvme;./ins;

# usbcanII module
echo "Load usbcanII"
modprobe usbcanII
```

*  Test things.  The system has to be running in a vme system for anything to work.  One test program is ~kelby/BIN/hstst, which should open a window.  
