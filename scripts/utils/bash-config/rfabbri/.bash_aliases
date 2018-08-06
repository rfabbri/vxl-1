alias dush="du -sh"
alias duh="du -sh"
#alias p="gmplayer -nosound -fs"
alias p="mpv --af=scaletempo"
#alias pp="mplayer -nosound -fs --xineramascreen=1 --shuffle"
alias pp="mpv -no-audio -fs --screen 1 --shuffle"
#alias ppp="mplayer -fs --xineramascreen=1"
alias ppp="mpv --af=scaletempo -fs --screen 1"

# play music displaying small vid when available
alias pm="mpv -xy 0.2 --shuffle"
alias pmm="mpv -xy 0.4"
alias d="ls"
alias ll="ls -l"
alias lt="ls -tr" # last modified files at bottom
#alias gvim=mvim # mac os
alias g=gvim
alias gg=git
alias gt=git
alias ggco='git checkout'
alias ggbr='git branch'
alias ggb='git branch'
alias ggpush='git push'
alias ggpull='git pull'
alias ggst='git status'
alias ggs='git status'
alias ggd='git diff'
alias ggdi='git diff'
alias givm=gvim
#alias gvimdiff="gvim -d"
alias v=vim
alias fox=firefox
alias sci=scilab
if [[ "$MYOS" != Linux ]]; then
#  echo OSX
  alias ds=open
  alias sof='open -a libreoffice'
  alias okular=open
  alias pdf='open'
  alias acro='open'
  alias acroread='open'
  alias oo='myopen'
  alias gimp="/Applications/Gimp.app/Contents/MacOS/gimp-2.8"
  alias bl='/Applications/blender.app/Contents/MacOS/blender'
  alias o=open
else
#  echo Linux
  alias bl=blender
  alias o=gnome-open
  alias acro=acroread
  alias kon=konqueror
  alias ds=display
  #alias sof=/usr/bin/ooffice
  alias sof=soffice
  alias pdf='gnome-open'
  alias acro='gnome-open'
  alias acroread='gnome-open'
  alias oo='myopen'
fi
alias mpg=mpg123
alias cdlvxl="cd $LEMSVXL"
alias cdvxl="cd $VXL_SRC"
alias cdvxd="cd $VXD_SRC"
alias cdvpe="cd $VPE_SRC"
alias cdlvpe="cd $LEMSVPE"
alias mm="mymake -j1 -k"
alias m="mymake"
alias cdsip="cd $HOME/sciprg/toolbx/siptoolbox"
alias cdan="cd $HOME/cprg/autotools/animal"
#alias javac=/opt/jdk1.5.0_02/bin/javac
alias mat=/mnt/soft/bin/matlab
#alias mex=/mnt/soft/bin/mex
alias crv=$LEMSVXL/contrib/curve2d/curve2d_gui
alias bc='bc -l'
alias pw=pwd
#alias as='aa shout'
alias al='aalarm'
alias ap='aa post'
alias pe='pd-extended -alsa -rt'
alias pek='(killall pd-extended; pd-extended -alsa -rt)'
alias cdpde='cd /home/rfabbri/pd-repos/pd-svn-w-extended-git-plus-gem/externals'
alias cdpixcv='cd /home/rfabbri/pd-repos/pd-svn-w-extended-git-plus-gem/externals/pix_opencv'
alias gp='git push'
alias gka='gitk --all'
alias gak='gitk --all'
alias grep='grep --color'
alias cdpet="cd $HOME/pet/pet/core/src/main/java/com/pulapirata/core"
alias id3="id3v2"
#alias ipython="ipython-2.7"
alias pylab="ipython --pylab"
alias loc=locate
alias killpython='killall -9 Python'
alias kp='killall -9 Python'
alias py='python'
alias sai='kill -9 %%'
alias sp='cd `getswitchpath`'
# thanks to:  http://blog.blindgaenger.net/colorize_maven_output.html
# and: http://johannes.jakeapp.com/blog/category/fun-with-linux/200901/maven-colorized
# Colorize Maven Output
alias maven="command mvn"
function color_maven() {
    local BLUE="[0;34m"
    local RED="[0;31m"
    local LIGHT_RED="[1;31m"
    local LIGHT_GRAY="[0;37m"
    local LIGHT_GREEN="[1;32m"
    local LIGHT_BLUE="[1;34m"
    local LIGHT_CYAN="[1;36m"
    local YELLOW="[1;33m"
    local WHITE="[1;37m"
    local NO_COLOUR="[0m"
    maven "$@" | sed \
        -e "s/Tests run: \([^,]*\), Failures: \([^,]*\), Errors: \([^,]*\), Skipped: \([^,]*\)/${LIGHT_GREEN}Tests run: \1$NO_COLOUR, Failures: $RED\2$NO_COLOUR, Errors: $YELLOW\3$NO_COLOUR, Skipped: $LIGHT_BLUE\4$NO_COLOUR/g" \
        -e "s/\(\[\{0,1\}WARN\(ING\)\{0,1\}\]\{0,1\}.*\)/$YELLOW\1$NO_COLOUR/g" \
        -e "s/\(\[ERROR\].*\)/$RED\1$NO_COLOUR/g" \
        -e "s/\(\(BUILD \)\{0,1\}FAILURE.*\)/$RED\1$NO_COLOUR/g" \
        -e "s/\(\(BUILD \)\{0,1\}SUCCESS.*\)/$LIGHT_GREEN\1$NO_COLOUR/g" \
        -e "s/\(\[INFO\].*\)/$LIGHT_GRAY\1$NO_COLOUR/g"
    return $PIPESTATUS
}

alias mvn=color_maven
alias idd=identify
alias ii=identify
alias lh='ls *.h'
alias b="bibtex paper"

# Music
alias aof="mpv ~/bach/*Art*"
alias amo="mpv ~/bach/*Offering*"
alias cff="mpv ~/bach/*/*Chrom*"
alias cantata="mpv --shuffle ~/bach/*Canta*"
alias organ="mpv ~/bach/*Organ*"
alias toccata="mpv ~/bach/*Tocc*"
alias partita="mpv ~/bach/*Part*"
