# qmake .pro file for QZedStat/doc

TEMPLATE   = app
TARGET     = $(nothing)
doc.files  = *.txt ../*.md ../LICENSE

# Ubuntu / Debian pkg doc path
doc.path = /usr/share/doc/QZedStat

exists( /usr/share/doc/packages ) {
    # SUSE pkg doc path
    doc.path = /usr/share/doc/packages/QZedStat
}

INSTALLS  += doc

