# DO THE FOLLOWING PERIODICALLY
# Start keychain, passing as args all private keys to be cached
#keychain -q $HOME/.ssh/id_rsa
PATH=$PATH:$HOME/bin/mw-scripts:$HOME/bin/mw-cmd:/usr/local/bin/i686-pc-linux-gnu:/mnt/soft/bin:/sw/bin:/usr/local/moredata/scilab-5.2.2/bin:/home/rfabbri/src/apache-maven-3.0.4-bin/bin:/opt/local/libexec/qt4/bin:.
export PATH

if [[ "`uname`" != Linux ]]; then
  MYOS="OSX"
else
  MYOS="Linux"
fi
export MYOS

#, other_key1, other_key2, ...
# That creates a file which must be sourced to set env vars
#source $HOME/.keychain/cortex.lems.brown.edu-sh
       
# This file is sourced by bash for login shells.  The following line
# runs your .bashrc and is recommended by the bash info pages.
if [ -n "$BASH" ]; then
  [[ -f ~/.bashrc ]] && . ~/.bashrc

  [[ -f ~/.git-completion.bash ]] && . ~/.git-completion.bash

  __git_complete gg __git_main
  __git_complete ggco _git_checkout
  __git_complete ggbr _git_branch
  __git_complete ggd _git_diff
  __git_complete ggdi _git_diff

  [[ -f ~/bin/switchpath.incl.sh ]] && . ~/bin/switchpath.incl.sh

  #xmodmap $HOME/.xmodmap
  if [[ "$MYOS" = Linux ]]; then
      myshuf="shuf"
  else
      myshuf="gshuf"
      # echo NOT IN LINUX, assuming IN MAC;
  fi
dayprinciple=`echo "여의 
염치 
인내 
극기 
백절불굴" | $myshuf -n 1`

  echo U+24B6 $dayprinciple
fi

##
# Your previous /Users/rfabbri/.bash_profile file was backed up as /Users/rfabbri/.bash_profile.macports-saved_2014-10-21_at_00:43:28
##

# MacPorts Installer addition on 2014-10-21_at_00:43:28: adding an appropriate PATH variable for use with MacPorts.
#export PATH="/opt/local/bin:/opt/local/sbin:$PATH"
# Finished adapting your PATH environment variable for use with MacPorts.


##
# Your previous /Users/rfabbri/.bash_profile file was backed up as /Users/rfabbri/.bash_profile.macports-saved_2015-11-10_at_13:52:23
##

# MacPorts Installer addition on 2015-11-10_at_13:52:23: adding an appropriate PATH variable for use with MacPorts.
if [[ "$MYOS" != Linux ]]; then
  export PATH="`cat /etc/paths.d/Tex`:/opt/local/bin:/opt/local/sbin:$PATH"
fi

# Finished adapting your PATH environment variable for use with MacPorts.


test -r /sw/bin/init.sh && . /sw/bin/init.sh
PATH=$HOME/bin:/usr/local/bin:/Library/TeX/texbin:/opt/local/bin:/opt/local/sbin:$PATH

export MANPATH="$HOME/lib/doc/man:$MANPATH"

# Setting PATH for Python 3.6
# The original version is saved in .bash_profile.pysave
PATH="/Library/Frameworks/Python.framework/Versions/3.6/bin:${PATH}"
export PATH

test -e "${HOME}/.iterm2_shell_integration.bash" && source "${HOME}/.iterm2_shell_integration.bash"

function git_touchbar {
  branch=`git branch 2>/dev/null |grep '\*'|cut -f 2 -d ' '`
  if test -n "$branch"; then
    it2setkeylabel set status $branch
  else
    it2setkeylabel set status "oggi `date '+%d'` $dayprinciple"
  fi
}
precmd_functions+=(git_touchbar)
