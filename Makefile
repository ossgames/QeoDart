#############################################################################
# Makefile for building: QeoDart
# Generated by qmake (2.01a) (Qt 4.7.2) on: Fr. Aug 5 12:16:42 2011
# Project:  QeoDart.pro
# Template: app
# Command: /usr/bin/qmake -spec /usr/share/qt4/mkspecs/linux-g++ QMLJSDEBUGGER_PATH=/usr/share/qtcreator/qml/qmljsdebugger -o Makefile QeoDart.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = -DQT_NO_DEBUG -DQT_XML_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED
CFLAGS        = -pipe -O2 -Wall -W -D_REENTRANT $(DEFINES)
CXXFLAGS      = -pipe -O2 -Wall -W -D_REENTRANT $(DEFINES)
INCPATH       = -I/usr/share/qt4/mkspecs/linux-g++ -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4/QtXml -I/usr/include/qt4 -I. -Icpp -I. -I.
LINK          = g++
LFLAGS        = -Wl,-O1
LIBS          = $(SUBLIBS)  -L/usr/lib -lQtXml -lQtGui -lQtCore -lpthread 
AR            = ar cqs
RANLIB        = 
QMAKE         = /usr/bin/qmake
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
STRIP         = strip
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = cpp/dart.cpp \
		cpp/io.cpp \
		cpp/main.cpp \
		cpp/qtwin.cpp \
		cpp/dialogs.cpp moc_dart.cpp \
		moc_io.cpp \
		moc_dialogs.cpp
OBJECTS       = dart.o \
		io.o \
		main.o \
		qtwin.o \
		dialogs.o \
		moc_dart.o \
		moc_io.o \
		moc_dialogs.o
DIST          = /usr/share/qt4/mkspecs/common/g++.conf \
		/usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/modules/qt_phonon.pri \
		/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/release.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		QeoDart.pro
QMAKE_TARGET  = QeoDart
DESTDIR       = 
TARGET        = QeoDart

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET): ui_mainWindow.h ui_resultWindow.h $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

Makefile: QeoDart.pro  /usr/share/qt4/mkspecs/linux-g++/qmake.conf /usr/share/qt4/mkspecs/common/g++.conf \
		/usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/modules/qt_phonon.pri \
		/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/release.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		/usr/lib/libQtXml.prl \
		/usr/lib/libQtGui.prl \
		/usr/lib/libQtCore.prl
	$(QMAKE) -spec /usr/share/qt4/mkspecs/linux-g++ QMLJSDEBUGGER_PATH=/usr/share/qtcreator/qml/qmljsdebugger -o Makefile QeoDart.pro
/usr/share/qt4/mkspecs/common/g++.conf:
/usr/share/qt4/mkspecs/common/unix.conf:
/usr/share/qt4/mkspecs/common/linux.conf:
/usr/share/qt4/mkspecs/qconfig.pri:
/usr/share/qt4/mkspecs/modules/qt_phonon.pri:
/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri:
/usr/share/qt4/mkspecs/features/qt_functions.prf:
/usr/share/qt4/mkspecs/features/qt_config.prf:
/usr/share/qt4/mkspecs/features/exclusive_builds.prf:
/usr/share/qt4/mkspecs/features/default_pre.prf:
/usr/share/qt4/mkspecs/features/release.prf:
/usr/share/qt4/mkspecs/features/default_post.prf:
/usr/share/qt4/mkspecs/features/warn_on.prf:
/usr/share/qt4/mkspecs/features/qt.prf:
/usr/share/qt4/mkspecs/features/unix/thread.prf:
/usr/share/qt4/mkspecs/features/moc.prf:
/usr/share/qt4/mkspecs/features/resources.prf:
/usr/share/qt4/mkspecs/features/uic.prf:
/usr/share/qt4/mkspecs/features/yacc.prf:
/usr/share/qt4/mkspecs/features/lex.prf:
/usr/share/qt4/mkspecs/features/include_source_dir.prf:
/usr/lib/libQtXml.prl:
/usr/lib/libQtGui.prl:
/usr/lib/libQtCore.prl:
qmake:  FORCE
	@$(QMAKE) -spec /usr/share/qt4/mkspecs/linux-g++ QMLJSDEBUGGER_PATH=/usr/share/qtcreator/qml/qmljsdebugger -o Makefile QeoDart.pro

dist: 
	@$(CHK_DIR_EXISTS) .tmp/QeoDart1.0.0 || $(MKDIR) .tmp/QeoDart1.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) .tmp/QeoDart1.0.0/ && $(COPY_FILE) --parents cpp/dart.hpp cpp/io.hpp cpp/myLabels.hpp cpp/qtwin.h cpp/dialogs.hpp .tmp/QeoDart1.0.0/ && $(COPY_FILE) --parents cpp/dart.cpp cpp/io.cpp cpp/main.cpp cpp/qtwin.cpp cpp/dialogs.cpp .tmp/QeoDart1.0.0/ && $(COPY_FILE) --parents ui/mainWindow.ui ui/resultWindow.ui .tmp/QeoDart1.0.0/ && (cd `dirname .tmp/QeoDart1.0.0` && $(TAR) QeoDart1.0.0.tar QeoDart1.0.0 && $(COMPRESS) QeoDart1.0.0.tar) && $(MOVE) `dirname .tmp/QeoDart1.0.0`/QeoDart1.0.0.tar.gz . && $(DEL_FILE) -r .tmp/QeoDart1.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


check: first

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compiler_moc_header_make_all: moc_dart.cpp moc_io.cpp moc_dialogs.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_dart.cpp moc_io.cpp moc_dialogs.cpp
moc_dart.cpp: ui_mainWindow.h \
		cpp/myLabels.hpp \
		cpp/dart.hpp
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) cpp/dart.hpp -o moc_dart.cpp

moc_io.cpp: cpp/io.hpp
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) cpp/io.hpp -o moc_io.cpp

moc_dialogs.cpp: ui_resultWindow.h \
		cpp/dart.hpp \
		ui_mainWindow.h \
		cpp/myLabels.hpp \
		cpp/dialogs.hpp
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) cpp/dialogs.hpp -o moc_dialogs.cpp

compiler_rcc_make_all:
compiler_rcc_clean:
compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_uic_make_all: ui_mainWindow.h ui_resultWindow.h
compiler_uic_clean:
	-$(DEL_FILE) ui_mainWindow.h ui_resultWindow.h
ui_mainWindow.h: ui/mainWindow.ui
	/usr/bin/uic-qt4 ui/mainWindow.ui -o ui_mainWindow.h

ui_resultWindow.h: ui/resultWindow.ui
	/usr/bin/uic-qt4 ui/resultWindow.ui -o ui_resultWindow.h

compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean compiler_uic_clean 

####### Compile

dart.o: cpp/dart.cpp cpp/dart.hpp \
		ui_mainWindow.h \
		cpp/myLabels.hpp \
		cpp/io.hpp \
		cpp/qtwin.h \
		cpp/dialogs.hpp \
		ui_resultWindow.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o dart.o cpp/dart.cpp

io.o: cpp/io.cpp cpp/dart.hpp \
		ui_mainWindow.h \
		cpp/myLabels.hpp \
		cpp/io.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o io.o cpp/io.cpp

main.o: cpp/main.cpp cpp/dart.hpp \
		ui_mainWindow.h \
		cpp/myLabels.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o main.o cpp/main.cpp

qtwin.o: cpp/qtwin.cpp cpp/qtwin.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o qtwin.o cpp/qtwin.cpp

dialogs.o: cpp/dialogs.cpp cpp/dialogs.hpp \
		ui_resultWindow.h \
		cpp/dart.hpp \
		ui_mainWindow.h \
		cpp/myLabels.hpp \
		cpp/qtwin.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o dialogs.o cpp/dialogs.cpp

moc_dart.o: moc_dart.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_dart.o moc_dart.cpp

moc_io.o: moc_io.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_io.o moc_io.cpp

moc_dialogs.o: moc_dialogs.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_dialogs.o moc_dialogs.cpp

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:

