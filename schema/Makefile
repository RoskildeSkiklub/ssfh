DB=test.db
UNITTESTDB=unittest.db
SHELL=/bin/bash

all:	reset-db unittest-db db_consts.cpp

clean:
	rm -f *~ $(DB) db_consts.cpp db_consts.h postnr.*

postnr.utf8:	../doc/postnr.xls
	iconv -f latin1 -t utf8 < ../doc/postnr.xls > postnr.utf8

postnr.utf8.csv:	postnr.utf8
	./postnr-to-csv.pl < postnr.utf8 | sort | uniq > postnr.utf8.csv

reset-db:	postnr.utf8.csv
	@test -f $(DB) && rm $(DB) ; true
	cat schema.sql | sqlite3 -init - $(DB) 
	echo ".genfkey --exec" | sqlite3 $(DB)
	cat rsh-setup.sql | sqlite3 $(DB)
	( echo .separator "\t" ; echo ".import postnr.utf8.csv zipcity" ) | sqlite3 $(DB)
	cat test-data.sql | sqlite3 $(DB)

unittest-db:	postnr.utf8.csv
	@test -f $(UNITTESTDB) && rm $(UNITTESTDB) ; true
	cat schema.sql | sqlite3 -init - $(UNITTESTDB) 
	echo ".genfkey --exec" | sqlite3 $(UNITTESTDB)
	cat unittest-setup.sql unittest-data.sql | sqlite3 $(UNITTESTDB)
	( echo .separator "\t" ; echo ".import postnr.utf8.csv zipcity" ) | sqlite3 $(UNITTESTDB)

db_consts.cpp:	schema.sql
	@./create-db-consts.pl < schema.sql 
	@if ! test -a ../db_consts.h ; then \
		cp db_consts.h .. ; \
	fi
	@if ! cmp -s db_consts.h ../db_consts.h ; then \
		cp db_consts.h .. ; \
	fi 
	@if ! test -a ../db_consts.cpp ; then \
		cp db_consts.cpp .. ; \
	fi
	@if ! cmp -s db_consts.cpp ../db_consts.cpp ; then \
		cp db_consts.cpp .. ; \
	fi 
