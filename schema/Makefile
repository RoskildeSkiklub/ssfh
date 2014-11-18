DB=test.db
UNITTESTDB=unittest.db
EMPTYDB=empty.db
SHELL=/bin/bash
SRCDIR=../src/

all:	reset-db unittest-db db_consts.cpp empty.db

clean:
	rm -f *~ $(DB) db_consts.cpp db_consts.h postnr.*

postnr.utf8:	../doc/postnr.xls
	iconv -f latin1 -t utf8 < ../doc/postnr.xls > postnr.utf8

postnr.utf8.csv:	postnr.utf8
	./postnr-to-csv.pl < postnr.utf8 | sort | uniq > postnr.utf8.csv

reset-db:	postnr.utf8.csv
	@test -f $(DB) && rm $(DB) ; true
	cat schema.sql | sqlite3 -init - $(DB) 
	cat rsh-setup.sql | sqlite3 $(DB)
	( echo .separator "\t" ; echo ".import postnr.utf8.csv zipcity" ) | sqlite3 $(DB)
	cat test-data.sql | sqlite3 $(DB)

unittest-db:	postnr.utf8.csv
	@test -f $(UNITTESTDB) && rm $(UNITTESTDB) ; true
	cat schema.sql | sqlite3 -init - $(UNITTESTDB) 
	cat unittest-setup.sql unittest-data.sql | sqlite3 $(UNITTESTDB)
	( echo .separator "\t" ; echo ".import postnr.utf8.csv zipcity" ) | sqlite3 $(UNITTESTDB)

empty.db:   postnr.utf8.csv
	@test -f $(EMPTYDB) && rm $(EMPTYDB) ; true
	cat schema.sql | sqlite3 -init - $(EMPTYDB)
	cat rsh-setup.sql | sqlite3 $(EMPTYDB)
	( echo .separator "\t" ; echo ".import postnr.utf8.csv zipcity" ) | sqlite3 $(EMPTYDB)

db_consts.cpp:	schema.sql
	@./create-db-consts.pl < schema.sql 
	@if ! test -a $(SRCDIR)/db_consts.h ; then \
		cp db_consts.h $(SRCDIR) ; \
	fi
	@if ! cmp -s db_consts.h $(SRCDIR)/db_consts.h ; then \
		cp db_consts.h $(SRCDIR) ; \
	fi 
	@if ! test -a $(SRCDIR)/db_consts.cpp ; then \
		cp db_consts.cpp $(SRCDIR) ; \
	fi
	@if ! cmp -s db_consts.cpp $(SRCDIR)/db_consts.cpp ; then \
		cp db_consts.cpp $(SRCDIR) ; \
	fi 
