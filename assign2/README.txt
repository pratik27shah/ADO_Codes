__________________________________________________________________________________________________________________________________________________________________________
Steps to compile and run the code:
__________________________________________________________________________________________________________________________________________________________________________

1. Open Terminal (Ctrl+Alt+T)
2. Go to the folder containing the code
3. Type "make"
4. RUN : ./test_assign2_1
5. CLEAN : "make clean"

__________________________________________________________________________________________________________________________________________________________________________
Code Structure:
__________________________________________________________________________________________________________________________________________________________________________

->There are mainly 3 files to get a basic code idea-
	1. test_assign2_1.c	
	2. storage_mgr.c 
	3. buffer_mgr.c
->The functions called in function main() in test_assign2_1.c are in the following order::
	1. initStorageManager
	2. testCreatingAndReadingDummyPages
	3. testReadPage
	4. testFIFO
	5. testLRU
	Following other functions are present::
	1. createDummyPages
	2. checkDummyPages
->The file buffer_mgr.c contains the following functions::
	1. CreateFrameNodes - allocates memory for frame nodes.
	2. CreateNodes - allocates node memory and initializes params like pining and dirty count.
	3. CreateFrameList(_forLRU) - creates and populates frame list.
	4. RemoveFrameList - Frees up the allocated memory.
	5. Update_Frame_Position - updates frame position considering if the page is pinned.
	6. CheckFrame_Unpinned - checks if any unpinned frame is available.
	7. Find_Page_Exsist - returns the page if found to the calling function.
	8. page_exist_in_memory - calls Find_Page_Exsist and populates values if it returns NULL.
	9. fifo - implements first in first out page replacement logic.
	10. LRU - implements least recently used  page replacement logic.
	11. Replace_Page_in_Frame - replaces page inside the frame by checking various constraints such as dirty count.
	12. initBufferPool - initiates the buffer pool by the creation of a doubly linked list.
	13. pinPage - checks if the buffer/page is initialized and also returns fifo or lru after implementation to the calling function in test_assign2_1.c
	14. shutdownBufferPool - frees up all the allocated memory and removes the frame list and the pointer.
	15. forceFlushPool - when buffer pool is to be shut down, this function will write the pages which are dirty to the memory.
	16. markDirty - Marks a bit as dirty.
	17. unpinPage - updates if the page is unpinned and exists.
	18. forcePage - returns if the write has failed or it is fine.
	19. getFrameContents - it is a statistics interface returning the frame contents mapping frame to page.
	20. getDirtyFlags - returns the dirty value.
	21. getFixCounts - returns the pages fixed count.
	22. getNumReadIO - returns the total read IO.
	23. getNumWriteIO - returns the total write IO.
__________________________________________________________________________________________________________________________________________________________________________
Idea(s) behind the Code Structure:
__________________________________________________________________________________________________________________________________________________________________________

-> The functionality of this code is to implement a buffer manager that manages a buffer of blocks in memory including reading/flushing to disk and block replacement (flushing blocks to disk to make space for reading new blocks from disk).

-> The code is created onto the earlier code which was maintaining the information regarding the file which is getting created and getting operated with various file functionalities.

-> The following test cases are used to check the functionality of the buffer manager ::
 
1. The Test Function "testCreatingAndReadingDummyPages()" in test_assign2_1.c will create n pages with content "Page X" and read them back to check whether the content is right.

2. The Test Function testReadPage(), testFIFO(), testLRU in test_assign2_1.c will test the functionality if the pages can be read, test the FIFO and LRU page replacement strategies respectively.
3. Extra Test Cases has been implemented.
_________________________________________________________________________________________________________________________________________________________________________
__________________________________________________________________________________________________________________________________________________________________________


