__________________________________________________________________________________________________________________________________________________________________________
Steps to compile and run the code:
__________________________________________________________________________________________________________________________________________________________________________
1. Open Terminal (Ctrl+Alt+T)

2. Go to the folder containing the code

3. Type "make"

4. RUN : ./test_assign3_1

5. CLEAN : "make clean"

Code Structure:
__________________________________________________________________________________________________________________________________________________________________________
->There are mainly 5 files to get a basic code idea-

	1. test_assign3_1.c
	2. test_expr.c
	3. storage_mgr.c 
	4. buffer_mgr.c
	5. record_mgr.c
__________________________________________________________________________________________________________________________________________________________________________
Idea(s) behind the Code Structure:
__________________________________________________________________________________________________________________________________________________________________________

-> The functionality of this code is to implement a record manager that manages tables with fix schema.The user can insert, update, delete and scan the records from tables.Each table has been stored in a separate page file and record manager access the pages of the file through the buffer 
manager implemented in the last assignment.

->The Record Manager defines a structure named as tableMgmtInfo which consists of various members required by the storage manager to handle the record and their details.

->The Record Manager defines one more structure named as Record_Mgmt_Info which has been used for accessing the scannig functionality.

->The Record Manager implements following functions :

1.createTable:This function creates the table with specified name

2.OpenTable: This function is used to open the file with given table name

3.CloseTable: This function is used to close the file and buffermanager which been associated with it

4.DeleteTable: This function deletes the file containing table

5.Insert Record: This function inserts the record into the file containing table. In this function we are checking whether the Tombstone list is available for insertion, if it is available then, new record is inserted in that list. Else the record is inserted at the end of the existing records.Here we will check maximum number of slots available per page. If it is full then new page is added and the record is inserted in the new page

6.Update Record: This function updates the specified record. The buffer manager functions are updated for the respective record and is written to the file containing the record

7.Delete Record: This function deletes the specified record. Here it checks whether the Tombstone list is emapty or not. If it finds it empty then it creates the new node and the values of the record getting deleted is assigned to it and node is added back to the list.Else a new node is added to the list and values are assigned accordingly.The modified table is written back to the file.

8.Get Record: This function is used to fetch the record as per the RID. This function also performs the validation checks to confirm the record requested does not belong to Tomb Stone list and its not e
ceeding the total number of records. 

9. Start Scan: This function is used to initialize the node which carries the information about the record that is to be searched

10.next: This function is used to scan through the pages of the file for fetching slot number of the record that is to be searched. It also checks whether the record belongs to the tomb stone list or not.It also checks the record's slot number and validates whether it is the required record. If fetched record is not the required one then this function is iterated recursively to get the desired record.

11.Close Scan: All the resources which has been allocated are freed and indication is given to the record manager

12.GetTableInfo & ConvertStr_to_TableDetails : These functions are used convert the details of table into the string format to store them in file

13. Serializerecord: This function is used to get the record in proper format

14. Deserializerecord: This function removes the formatting and retrives record


* Additional Feature *
-------------------------
Implementation of Tomb Stone:

-> This feature helps the scanning functions to work more efficiently.
-> We are keeping the track of the RID which are members of Tomb Stone list. The scanning functions always checks whether the requested record belongs to the Tomb Stone list,if it does then they simply skip the records. 
-> This functions improves the efficiency of the scanning functions by scanning the appropriate records


* Additional Test Cases *
--------------------------
In addition to the default test cases mentioned in test_expr.c, we have implemented additional ones to validate the successful implementation of the assignment



