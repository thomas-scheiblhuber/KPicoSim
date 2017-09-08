# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/debian $(all_includes)

# these are the headers for your project
noinst_HEADERS = kpicosim.h codeeditor.h ksimulator.h kserialview.h kportview.h \
			kport.h kexportdialog.h cassembler.h cinstruction.h cpicoblaze.h hexcodes.h \
			types.h kprocessorview.h jtagdevice.h kjtagdialog.h jtagprogrammer.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kpicosim.pot

KDE_ICON = kpicosim

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = kpicosim

# the application source, library search path, and link libraries
kpicosim_SOURCES = main.cpp kpicosim.cpp codeeditor.cpp ksimulator.cpp \
			kprocessorview.cpp kserialview.cpp kportview.cpp kscratchpadview.cpp kscratchpadview.h \
			kport.cpp kexportdialog.cpp cassembler.cpp cinstruction.cpp cpicoblaze.cpp \
			jtagdevice.cpp kjtagdialog.cpp jtag.cpp jtag.h jtagprogrammer.cpp
kpicosim_LDFLAGS = $(KDE_RPATH) $(all_libraries)
kpicosim_LDADD = -lkio $(LIB_KDEUI)

# this is where the desktop file will go 
shelldesktopdir = $(kde_appsdir)/Development
shelldesktop_DATA = kpicosim.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir = $(kde_datadir)/kpicosim
shellrc_DATA = kpicosimui.rc

katepartdir = $(kde_datadir)/katepart/syntax
katepart_DATA = psm.xml
icondirdir = $(kde_icondir)
kde_icon_KDEICON = hi22-action-dbgrun.png hi22-action-simrun.png \
	hi22-action-simrun.png hi22-action-dbgrun.png hi22-action-dbgrun.png
SUBDIRS = pics
