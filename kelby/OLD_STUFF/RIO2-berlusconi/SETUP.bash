##############################################################################
# SETUP.bash  - set up the user environment for lynxos development.
#
# This script sets up the user environment for LynxOS development.
# It should be sourced prior to running any makefiles under 
# $(ENV_PREFIX)/src.
#
# Usage:
#
# bash$ . SETUP.bash
#
##############################################################################
echo ""

                                        # ENV_PREFIX must point to the install
                                        # tree root.  e.g. /usr/lynx/2.5.1/ppc
ENV_PREFIX=/usr/lynx/3.0.1/ppc
export ENV_PREFIX

 LYNXTARGET=ppc			# PowerPC.
 OBJSFORMAT=xcoff
#(mips) LYNXTARGET=mips			# Mips
#(mips) OBJSFORMAT=elf
export LYNXTARGET
export OBJSFORMAT

NO_RTN="-n"				# No Carriage-Return on echo.
 NO_RTN=""			# AIX doesnt support echo -n
PATH=/usr/ucb:$PATH			# so "echo -n" works on Solaris2


echo     "  Setup : ENV_PREFIX is an environment variable that points to "
echo     "        : the root of the build/release directory.  The ENVIRONMENT"
echo     "        : file is found as \$ENV_PREFIX/ENVIRONMENT."
echo $NO_RTN "        : Set ENV_PREFIX to $ENV_PREFIX?	[y]  "
read input
if [ "$input" != "y" -a  "$input" != "Y" -a "$input" != "" ]; then
  echo  $NO_RTN "  Enter : New ENV_PREFIX path.  <ret> for root dir : "
  read ENV_PREFIX
  echo ""
fi
echo ""

##############################################################################
HOST_OS_REVISION=`uname -r`
case `uname` in
  "CYGWIN32/"*) HOST_OS=win32;;
  "Linux") HOST_OS=linux;;
  "LynxOS") HOST_OS=lynxos;;
  "SunOS")  HOST_OS=sunos;
			case $HOST_OS_REVISION in
				5*) HOST_OS_TYPE=solaris;;
				4*) HOST_OS_TYPE=sunos;;
				*) echo "Host OS revision unknown: $HOST_OS_REVISION";;
			esac;;
  "AIX")    HOST_OS=aix;;
  "HP-UX")  HOST_OS=hpux;;
  *)        echo "Could not determine host type, exiting.";
	    return 1;;
esac
export HOST_OS HOST_OS_REVISION HOST_OS_TYPE
##############################################################################

PW_SH=$ENV_PREFIX/usr/tools/pw.sh;

if [ $HOST_OS != "lynxos" ]; then
    HPATH=$ENV_PREFIX/cdk/$HOST_OS-$OBJSFORMAT-$LYNXTARGET
    echo     "  Setup : Setting up Xdev tools: $HPATH"
    PATH="$HPATH/bin:$HPATH/usr/bin:$ENV_PREFIX/usr/tools:$PATH";	
    MANPATH="$ENV_PREFIX/usr/man:$MANPATH"
    if [ -f $PW_SH ] ; then
      . $PW_SH
      initialize_pw;
    fi
else
    PATH="$ENV_PREFIX/usr/tools:$ENV_PREFIX/usr/bin:$PATH"; # integration tools.
fi
export PATH
export MANPATH

echo ""
echo "  Setup : Complete! "
echo ""
##############################################################################
