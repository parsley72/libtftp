METASOURCES = AUTO

# set the include path found by configure
INCLUDES= $(all_includes)

# the library search path.
lib_LTLIBRARIES =  libtftp.la
libtftp_la_LDFLAGS = -module -version-info 0:1:0
libtftp_la_SOURCES = client.cpp clientread.cpp clientwrite.cpp readsession.cpp server.cpp serverread.cpp serverwrite.cpp session.cpp tftp.cpp writesession.cpp

libtftpincludedir = ${includedir}/tftp
libtftpinclude_HEADERS =  client.h clientread.h clientwrite.h readsession.h server.h serverread.h serverwrite.h session.h tftp.h writesession.h
