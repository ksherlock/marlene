#
#
# some settings so gsh works better with vt100.
# source vt100.gsh
#
set term=vt100
bindkey backward-delete-char "^H"
bindkey up-history "^[[A"
bindkey down-history "^[[B"
bindkey forward-char "^[[C"
bindkey backward-char "^[[D"
