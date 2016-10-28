#MakeFile
#Purpose:Starting point of make file
all:  test_assign2_1.exe storage_mgr.o storage_mgr.h.gch test_helper.h.gch dberror.o dberror.h.gch buffer_mgr.o buffer_mgr.h.gch 
#Purpose:Compiles the db_error.c
dberror.o:dberror.c
	gcc -Wall -c dberror.c

#Purpose:Compiles the test_helper.h
test_helper.h.gch: test_helper.h
	gcc -Wall -c test_helper.h

#Purpose:Compiles the storage_mgr.h
storage_mgr.h.gch: storage_mgr.h
	gcc -Wall -c storage_mgr.h
#Purpose:Generates storage_mgr.obj filo
storage_mgr.o:storage_mgr.c
	gcc -Wall -c storage_mgr.c

buffer_mgr.h.gch: buffer_mgr.h
	gcc -Wall -c buffer_mgr.h

buffer_mgr.o:buffer_mgr.c
	gcc -Wall -c buffer_mgr.c

buffer_mgr_stat.o:buffer_mgr_stat.c
	gcc -Wall -c buffer_mgr_stat.c

#Purpose:Generate the exe file from obj
test_assign2_1.exe: test_assign2_1.o
	gcc -Wall dberror.c storage_mgr.c buffer_mgr.c test_assign2_1.c buffer_mgr_stat.c -o test_assign2_1	
#Purpose:Builds  the obj file of test_assign2_1.c
test_assign2_1.o: test_assign2_1.c
	gcc -c test_assign2_1.c
	

	
dberror.h.gch:dberror.c
	gcc -Wall -c dberror.h
#Purpose:Cleans the files by deleting them
clean:
	rm test_assign2_1 storage_mgr.o storage_mgr.h.gch test_helper.h.gch test_assign2_1.o dberror.h.gch


