##############################################################################
# SETUP.csh  - set up the user environment for lynxos development.
#
# This script sets up the user environment for LynxOS development.
# It should be sourced prior to running any makefiles under 
# $(ENV_PREFIX)/src.
#
# Usage:
#
# csh$ source SETUP.csh
#
##############################################################################

echo ""

                                        # ENV_PREFIX must point to the install
                                        # tree direcory.
setenv ENV_PREFIX /usr/lynx/3.0.1/ppc

 setenv LYNXTARGET ppc		# PowerPC.
 setenv OBJSFORMAT xcoff
#(mips) setenv LYNXTARGET mips		# Mips
#(mips) setenv OBJSFORMAT elf

setenv NO_RTN "-n"			# No Carriage-Return on echo.
 setenv NO_RTN ""			# AIX doesnt support echo -n
set path=(/usr/ucb $path)               # so "echo -n" works on Solaris2


echo     "  Setup : ENV_PREFIX is an environment variable that points to "
echo     "        : the root of the build/release directory.  The ENVIRONMENT"
echo     "        : file is found as ENV_PREFIX/ENVIRONMENT."
echo $NO_RTN "        : Set ENV_PREFIX to $ENV_PREFIX?	[y]  "
set input=$<
if ( "$input" != "y" ) then
  if ( "$input" != "Y" ) then
    if ( "$input" != "" ) then
      echo  $NO_RTN "  Enter : New ENV_PREFIX path.  <ret> for root dir : "
      set input=$<
      setenv ENV_PREFIX $input
      echo ""
    endif
  endif
endif
echo ""

##############################################################################
setenv HOST_OS_REVISION `uname -r`
switch (`uname`) 
  case "CYGWIN32/"*: 
		setenv HOST_OS win32; breaksw;
  case "Linux": 
		setenv HOST_OS linux; breaksw;
  case "LynxOS": 
		setenv HOST_OS lynxos; breaksw;
  case "SunOS":  
		setenv HOST_OS sunos;
		set major=`expr $HOST_OS_REVISION : '\(.\)\.'`
		switch ($major)
			case "5":
				setenv HOST_OS_TYPE solaris; breaksw;
			case "4":
				setenv HOST_OS_TYPE sunos; breaksw;
			default:
				echo "Host OS revision unkown: $HOST_OS_REVISION";
				breaksw;
		endsw
		breaksw;
  case "AIX":    
		setenv HOST_OS aix; breaksw;
  case "HP-UX":    
		setenv HOST_OS hpux; breaksw;
  default:       
		echo "Could not determine host type, exiting.";
		exit 1;
  		breaksw;
endsw
##############################################################################

set PW_CSH=($ENV_PREFIX/usr/tools/pw.csh);

if ( "$HOST_OS" != "lynxos" ) then
    setenv HPATH $ENV_PREFIX/cdk/$HOST_OS-$OBJSFORMAT-$LYNXTARGET
    echo     "  Setup : Setting up Xdev tools: $HPATH"
    set path=($HPATH/bin $HPATH/usr/bin $ENV_PREFIX/usr/tools $path);	
	if $?MANPATH then
    set MANPATH=($ENV_PREFIX/usr/man $MANPATH );	
	else
    set MANPATH=($ENV_PREFIX/usr/man);	
    endif
	if ( -f $PW_CSH ) then
	  source $PW_CSH
    endif
else
    set path=($ENV_PREFIX/usr/tools $ENV_PREFIX/usr/bin $path); 
endif

echo ""
echo "  Setup : Complete! "
echo ""
##############################################################################
