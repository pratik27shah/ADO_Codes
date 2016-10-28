#MakeFile
#Purpose:Starting point of make file
all:   assign3_1 assign3_2 storage_mgr.o storage_mgr.h.gch test_helper.h.gch dberror.o dberror.h.gch buffer_mgr.o buffer_mgr.h.gch record_mgr.o record_mgr.h.gch tables.h.gch expr.h.gch expr.o rm_serializer.o rm_serializer.h.gch test_expr.c

#Purpose:Compiles db_error.c
dberror.o:  dberror.c
	gcc -Wall -c dberror.c

#Purpose:Compiles test_helper.h
test_helper.h.gch: test_helper.h
	gcc -Wall -c test_helper.h

#Purpose:Compiles storage_mgr.h
storage_mgr.h.gch: storage_mgr.h
	gcc -Wall -c storage_mgr.h

#Purpose:Generates storage_mgr.obj
storage_mgr.o:storage_mgr.c
	gcc -Wall -c storage_mgr.c

#Purpose:Compiles buffer_mgr.h
buffer_mgr.h.gch: buffer_mgr.h
	gcc -Wall -c buffer_mgr.h


#Purpose:Compiles tables.h
tables.h.gch: tables.h
	gcc -Wall -c tables.h

#Purpose:Compiles record_mgr.h
record_mgr.h.gch: record_mgr.h
	gcc -Wall -c record_mgr.h -lm

#Purpose:Compiles db_error.h
dberror.h.gch: dberror.h
	gcc -Wall -c dberror.h


#Purpose:Compiles expr.h
expr.h.gch: expr.h
	gcc -Wall -c expr.h

#Purpose:Generates buffer_mgr.obj
buffer_mgr.o:buffer_mgr.c
	gcc -Wall -c buffer_mgr.c

#Purpose:Generates buffer_mgr.obj
buffer_mgr_stat.o:buffer_mgr_stat.c
	gcc -Wall -c buffer_mgr_stat.c

#Purpose:Generates record_mgr.obj
record_mgr.o:record_mgr.c
	gcc -Wall -c record_mgr.c

#Purpose:Generates rm_serializer.obj
rm_serializer.o:rm_serializer.c
	gcc -Wall -c rm_serializer.c

#Purpose:Generates rm_serializer.h.gch
rm_serializer.h.gch: rm_serializer.h
	gcc -Wall -c rm_serializer.h

#Purpose:Generates expr.obj
expr.o:expr.c
	gcc -Wall -c expr.c

#Purpose:Generates test_expr.obj
test_expr.o:test_expr.c
	gcc -Wall -c test_expr.c

#Purpose:Generates test_assign3_1.obj
test_assign3_1.o:test_assign3_1.c
	gcc -Wall -c test_assign3_1.c

#Purpose:Generate the exe file from obj
assign3_1: test_assign3_1.o record_mgr.o rm_serializer.o expr.o buffer_mgr.o buffer_mgr_stat.o storage_mgr.o dberror.o
	gcc -Wall test_assign3_1.o record_mgr.o -lm rm_serializer.o expr.o buffer_mgr.o buffer_mgr_stat.o storage_mgr.o dberror.o -o assign3_1

#Purpose:Generate the exe file from obj
assign3_2: test_expr.o record_mgr.o rm_serializer.o expr.o buffer_mgr.o buffer_mgr_stat.o storage_mgr.o dberror.o
	gcc -Wall test_expr.o record_mgr.o -lm rm_serializer.o expr.o buffer_mgr.o buffer_mgr_stat.o storage_mgr.o dberror.o -o assign3_2

#Purpose:Cleans the files by deleting them
clean:
	rm test_expr.o record_mgr.o rm_serializer.o expr.o buffer_mgr.o buffer_mgr_stat.o storage_mgr.o dberror.o assign3_2 assign3_1 test_helper.h.gch storage_mgr.h.gch buffer_mgr.h.gch tables.h.gch record_mgr.h.gch dberror.h.gch rm_serializer.h.gch test_assign3_1.o expr.h.gch
