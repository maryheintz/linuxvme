# This file is executed from xdm or xinitrc


# If the user doesn't want to call Desktop, she should comment 
# the following lines.

if [ $PD_WM = mwm ] || [ $PD_WM = twm ]; then

  if [ ! -f $HOME/XTerm ]; then
    cp -p /usr/lib/X11/pd/XTerm $HOME
  fi
  if [ ! -f $HOME/.xbmbrowserrc ]; then
    cp -p /usr/lib/X11/xbmbrowserrc $HOME/.xbmbrowserrc
  fi

  if [ ! -f $HOME/.mwmrc ]; then
    cp -p /usr/lib/X11/pd.mwmrc $HOME/.mwmrc
  fi

  if [ $PD_WM = mwm ]; then
    prio 20 mwm &
  else
    prio 20 twm &
  fi

fi
prio 19 pd > /dev/null 2>&1 &
#### Here the user can insert her own commands ######

#### End of user's commands #######

# The last command will hang until the user exits from the console xterm.
xterm -ls -T "LynxOS Console" -name Console -iconic

