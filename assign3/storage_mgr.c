#include "stdio.h"
#include "storage_mgr.h"
#include "dberror.h"
#include "stdlib.h"
FILE *SetFile_Pointer;
char Error_Message[4][30]={
	" Page Number Invalid",
	" File Not Found",
	" File Not Created",
	"Cannot Write to File"};
	
	
/*Purpose:To initialize function as a blank on first call to storage_mgr.c */
void initStorageManager(void)
{
	
}

/*Purpose:It will create Page File and will fill it with \0 bytes*/
RC createPageFile(char *FileName)
{
	 SetFile_Pointer = fopen(FileName, "w");

   if(SetFile_Pointer==NULL){
	   RC_message=Error_Message[2];
   return RC_FILE_NOT_FOUND ;}
  
  char *Write_Content=(char *) malloc(PAGE_SIZE*sizeof(char));

fwrite(Write_Content,PAGE_SIZE,sizeof(char),SetFile_Pointer);
return RC_OK;
}


/*Purpose:It will open the File*/
RC openPageFile (char *fileName, SM_FileHandle *fHandle)
{
	int file_size,total_pages;	
	FILE *SetFile_Pointer= fopen(fileName,"r+");
	if(SetFile_Pointer==NULL){
	 RC_message=Error_Message[1];
	   return RC_FILE_NOT_FOUND ;
}   	
   	else 
	{
		fHandle->mgmtInfo = SetFile_Pointer;
	   fseek(SetFile_Pointer, 0, SEEK_END); 
	   file_size = ftell(SetFile_Pointer);

      total_pages= file_size/PAGE_SIZE; 
      fHandle->totalNumPages=total_pages;
      fHandle->curPagePos = 0; 
	   fHandle->fileName=fileName;
	} 

	return RC_OK;  
}

/*Purpose:It will close the Page File*/
RC closePageFile (SM_FileHandle *fHandle)
{
fclose(fHandle->mgmtInfo);
//fHandle=NULL;
SetFile_Pointer=NULL;
return RC_OK;
}
/*Purpose:It will destroys the file(Process of Deletion)*/
RC  destroyPageFile(char *fileName)
{
	if(!remove(fileName)) {return RC_OK;}
	 RC_message=Error_Message[1];
	return RC_FILE_NOT_FOUND;
	
}

/*Purpose:It will read the block(page) from the Page*/
RC readBlock(int pageNum,SM_FileHandle *fHandle,SM_PageHandle memPage)
{
	if(pageNum<=-1 || pageNum>fHandle->totalNumPages)
	{
		 RC_message=Error_Message[0];
		return RC_READ_NON_EXISTING_PAGE;
	}

	if(fHandle->mgmtInfo==NULL)
	{ 
		 RC_message=Error_Message[1];
		return RC_FILE_NOT_FOUND;
	}
	int File_Seek_Status=fseek(fHandle->mgmtInfo,(pageNum+1)*(PAGE_SIZE)*sizeof(char),SEEK_SET); 

	if(File_Seek_Status==-1)
	{
		RC_message=Error_Message[0];
		return RC_READ_NON_EXISTING_PAGE;
	}

	fread(memPage,sizeof(char),PAGE_SIZE,fHandle->mgmtInfo);
	fHandle->curPagePos=pageNum;
	return RC_OK;
}

/*Purpose:It will returns current Page Position in a file*/
int getBlockPos (SM_FileHandle *fHandle)
{
	return fHandle->curPagePos;
}

/*Purpose :It will reads first Page from a file*/
RC readFirstBlock(SM_FileHandle *fHandle,SM_PageHandle memPage)
{
	return readBlock(0,fHandle,memPage);
}

/*Purpose:It will read the previos block from the file*/
RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    return readBlock(fHandle->curPagePos-1, fHandle, memPage);
}

/*Purpose:It will read the current block from the file*/
RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    return readBlock(fHandle->curPagePos, fHandle, memPage);
}

/*Purpose:It will read the next block from the file*/
RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    return readBlock(fHandle->curPagePos+1, fHandle, memPage);
}

/*Purpose:It will read the last page*/
RC readLastBlock(SM_FileHandle *fHandle,SM_PageHandle memPage)
{	
	return readBlock(fHandle->totalNumPages,fHandle,memPage);
}

/*Purpose:It will write from the beginning of the file*/
RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	if(pageNum<=-1 || pageNum>fHandle->totalNumPages)
	{
		RC_message=Error_Message[1];
		return RC_READ_NON_EXISTING_PAGE;
	}

	if(fHandle->mgmtInfo == NULL)
	{
		return RC_FILE_NOT_FOUND;
	}	
	fseek(fHandle->mgmtInfo,(pageNum+1)*PAGE_SIZE*sizeof(char),SEEK_SET);	
	fwrite(memPage, sizeof(char), PAGE_SIZE, fHandle->mgmtInfo);
	 fHandle->curPagePos = pageNum;

	return RC_OK;
}

/*Purpose:It will write into file after specific number of bytes*/
RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	return writeBlock(getBlockPos(fHandle), fHandle, memPage);
}

/*Purpose:It will appends an empty page at the end of the file and will allocate the space for it*/
RC appendEmptyBlock (SM_FileHandle *fHandle){
	
int  FseekValue = fseek(fHandle->mgmtInfo,(fHandle->totalNumPages + 1)*PAGE_SIZE*sizeof(char) , SEEK_END);
 if (FseekValue == -1){
	 RC_message=Error_Message[3];
		return RC_WRITE_FAILED;}
    fHandle->curPagePos = fHandle->totalNumPages+1;   
     char *Append_Block = (char *) malloc(PAGE_SIZE*sizeof(char)); 
     fHandle->totalNumPages = fHandle->totalNumPages + 1;
      fwrite(Append_Block, sizeof(char), PAGE_SIZE,fHandle->mgmtInfo);     
      rewind(fHandle->mgmtInfo);
      fseek(fHandle->mgmtInfo, (fHandle->totalNumPages + 1)*PAGE_SIZE*sizeof(char), SEEK_SET);
      free(Append_Block);
        return RC_OK;
}

/*Purpose:It will compare the file size with total number of pages and will add the difference*/
RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle)
{
	if(fHandle->totalNumPages < numberOfPages)
	{
		int Counter=numberOfPages-fHandle->totalNumPages,i=0;
		numberOfPages = fHandle->totalNumPages;
		for(i=0;i<Counter;i++)
		appendEmptyBlock(fHandle);
	}/*
	else{
		RC_message=Error_Message[1];
		return RC_READ_NON_EXISTING_PAGE;
	
	}*/
	return RC_OK;
}




