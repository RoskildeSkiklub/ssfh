DB=test.db
SHELL=/bin/bash

all:	reset-db db_consts.cpp

clean:
	rm -f *~ $(DB) db_consts.cpp db_consts.h

reset-db:
	@test -f $(DB) && rm $(DB) ; true
	cat schema.sql | sqlite3 -init - $(DB) 
	echo ".genfkey --exec" | sqlite3 $(DB)
	cat rsh-setup.sql | sqlite3 $(DB)
	cat test-data.sql | sqlite3 $(DB)

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
