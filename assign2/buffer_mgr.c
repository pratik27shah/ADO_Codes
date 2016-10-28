#include <stdio.h>
#include "buffer_mgr.h"
#include "storage_mgr.h"
#include "dberror.h"
#include "dt.h"


#define MAX_PAGE_SIZE_1 15000
#define MAX_FRAMES 50

typedef struct  Frames_List
{
int Frame_Number;
int Page_Number;	
struct Frames_List *next;
struct Frames_List *previous;
struct Frames_List *head;
struct Frames_List *last;
}Frames_List;

/*Purpose:Intializes stuct of pageFrames*/
typedef struct PageFrames
{
int FrameNum;
int FrameNumber;
int DirtyCount;
int FixCount;
int Pining;
int Page_Hit_Count;
char *Data_Value;
PageNumber PageNumberValue;
struct PageFrames *next;
struct PageFrames *previous;
}PageFrames;

/*Purpose:Intializes struct of buffer manager*/
PageFrames *PageFrame;
typedef struct BufferManager
{
int Map_Page_To_Frame[MAX_PAGE_SIZE_1];
int Map_Frame_To_Page[MAX_FRAMES];
bool DirtyValue[MAX_PAGE_SIZE_1];
int PagesFixedCount[MAX_FRAMES];
int Total_ReadIO;
int Total_WriteIO;
char *PageFileName;
int frameCount;
int numPages;
ReplacementStrategy strategy;
struct PageFrames *FrameList;
struct  PageFrames *HeadNode;
struct PageFrames *LastNode;
struct Frames_List *Frame_List_Value;
SM_FileHandle fileHandler_StorageMgr;
}BufferManager;

/* Purpose: Allocates memory for frame node */
Frames_List *CreateFrameNodes()
{
Frames_List *node_frames= malloc(sizeof(Frames_List));	
node_frames->Frame_Number=0;
node_frames->Page_Number=NO_PAGE;
node_frames->next=NULL;
node_frames->previous=NULL;
return node_frames;
}

/* Purpose: Allocates node memory and initializes params like pining and dirty count */
PageFrames *CreateNodes()
{
PageFrames *node= malloc(sizeof(PageFrames));
node->FrameNum=0;
node->Pining=0;	
node->FixCount=0;
node->DirtyCount=0;
node->PageNumberValue=NO_PAGE;
node->Page_Hit_Count=0;

node->Data_Value=(char *)malloc(PAGE_SIZE*sizeof(SM_FileHandle));
node->next=NULL;
node->previous=NULL;
return node;
}

/* Purpose:creates and populates frame list for LRU */
Frames_List *CreateFrameList_forLRU(int numPages)
{
int i;
Frames_List *bframeList=malloc(sizeof(Frames_List));
Frames_List *bframeList1;
bframeList->head=bframeList->last=CreateFrameNodes();

for(i=1;i<numPages;i++)
	{
       
	bframeList->last->Frame_Number=i-1;
	bframeList->last->next=CreateFrameNodes();
	bframeList->last->next->previous=bframeList->last;
	bframeList->last=bframeList->last->next;
	}
bframeList->last->Frame_Number=i-1;
bframeList->head->previous=NULL;
bframeList->last->next=NULL;
return  bframeList;
}


/*Purpose:Frees up the allocated memory */
void RemoveFrameList(Frames_List *ReplaceNode)
{
Frames_List *TempTraverseNode;

TempTraverseNode=ReplaceNode->head;
while(TempTraverseNode!=NULL)
{free(TempTraverseNode->previous);
	 TempTraverseNode=TempTraverseNode->next;
	 
	}
    free(TempTraverseNode);
	
}

/* Purpose:updates frame position considering if the page is pinned */
Frames_List *Update_Frame_Position(Frames_List *ReplaceNode,int PageNum,int FoundFlag,BM_BufferPool *const bm)
{
Frames_List *Temp,*LastNode,*Temp2;
int FrameNumberValue,PageNumberValue,i;
bool PinedPageFound=false;
Temp=ReplaceNode->head;
Frames_List *NewNode=CreateFrameNodes();
LastNode=ReplaceNode->last;
Temp=ReplaceNode->head;
int *fixCount = getFixCounts(bm);
if(FoundFlag==0)
{
	while(Temp!=NULL)
    {
		if(fixCount[Temp->Frame_Number]==1){
		Temp=Temp->next;
        PinedPageFound=true;
        FoundFlag=1;
        }
        else
            break;
    }
}

if(FoundFlag==1 && 	ReplaceNode->head->Page_Number!=PageNum)
{
        if(LastNode->Page_Number==PageNum) return ReplaceNode;
        while(Temp!=NULL && !(PinedPageFound))
        {
				if(Temp->Page_Number==PageNum)
                    break;
				Temp=Temp->next;
        }
		FrameNumberValue=Temp->Frame_Number;
		PageNumberValue=Temp->Page_Number;
		Temp->previous->next=Temp->next;
		Temp->next->previous=Temp->previous;
		LastNode->next=NewNode;
		NewNode->next=NULL;
		NewNode->Frame_Number=FrameNumberValue;
		NewNode->Page_Number=PageNumberValue;
		NewNode->previous=LastNode;
		free(Temp);
		ReplaceNode->last=NewNode;
        return ReplaceNode;
}
        FrameNumberValue=Temp->Frame_Number;
		ReplaceNode->head=Temp->next;
		Temp->next->previous=NULL;
		Temp->previous=NULL;
		free(Temp);
		NewNode->next=NULL;
		NewNode->Frame_Number=FrameNumberValue;
		NewNode->Page_Number=PageNum;
		LastNode->next=NewNode;
		NewNode->next=NULL;
		NewNode->previous=LastNode;
		ReplaceNode->last=NewNode;
		Temp=ReplaceNode->head;
		return ReplaceNode;
}

/* Purpose: checks if any unpinned frame is available */
RC CheckFrame_Unpinned(PageFrames *HeadNode, const PageNumber pageNum,int TotalFrames)
{
	PageFrames *Page_Frames = HeadNode;
	int PinedFrames=0;
	while(Page_Frames!=NULL)
	{
		if(Page_Frames->FixCount!=0)
            {
                PinedFrames++;
            }
        Page_Frames=Page_Frames->next;
    }
    if(PinedFrames<TotalFrames)
        return RC_OK;
    return RC_NO_EMPTY_FRAME;
}

/* Purpose:returns the page if found to the calling function */
PageFrames *Find_Page_Exsist (PageFrames *HeadNode, const PageNumber pageNum,bool CheckForPinnedPages)
{
    PageFrames *pagefound = HeadNode;
    while(pagefound != NULL )
    {
		if(CheckForPinnedPages)
        {
			if(pagefound->PageNumberValue == pageNum && pagefound->Pining==0)
            {
                return pagefound;
            }
        }
        else
        {
			if(pagefound->PageNumberValue == pageNum)
            {
                return pagefound;
            }
		}
        pagefound = pagefound->next;
    
    }
    return NULL;
}


/* Purpose: calls Find_Page_Exsist and populates values if it returns NULL */
PageFrames *page_exist_in_memory(BM_BufferPool *const bm, BM_PageHandle *const page,const PageNumber pageNum)
{
	PageFrames *findingpage;	
	BufferManager *Set_Buffer_Manager_Ptr = (BufferManager *)bm->mgmtData;
    PageFrames *Pass_head=Set_Buffer_Manager_Ptr->HeadNode;
    if((Set_Buffer_Manager_Ptr->Map_Page_To_Frame)[pageNum] != NO_PAGE)
    {
        if((findingpage = Find_Page_Exsist(Pass_head, pageNum,true)) == NULL)
        {
			return NULL;
        }
        
        page->pageNum = pageNum;
        page->data = findingpage->Data_Value;
        findingpage->FixCount++;
        return findingpage;
    }
   return NULL;
}


/* Purpose : implements first in first out page replacement logic */
RC fifo (BM_BufferPool *const bm, BM_PageHandle *const page,const PageNumber pageNum)
{
	int i;
	PageFrames *findpage;
	PageFrames *PageFrames_TraveseList;
    RC msg;
	BufferManager *BufferManager_ValueStorage = (BufferManager *)bm->mgmtData;
	Frames_List  *Pass_List;
	PageFrames_TraveseList=BufferManager_ValueStorage->HeadNode;
	findpage=BufferManager_ValueStorage->HeadNode;
    if((findpage = page_exist_in_memory(bm, page, pageNum)) != NULL)
    {
        return RC_OK;
    }
    else
    {
        if((msg=CheckFrame_Unpinned(PageFrames_TraveseList,pageNum,BufferManager_ValueStorage->frameCount))!=RC_OK)
        {
            return msg;
        }
	
        BufferManager_ValueStorage->Frame_List_Value=Update_Frame_Position(BufferManager_ValueStorage->Frame_List_Value,pageNum,0,bm);
        Pass_List=BufferManager_ValueStorage->Frame_List_Value->head;

        for(i=0;i<BufferManager_ValueStorage->Frame_List_Value->last->Frame_Number;i++)
        {
            PageFrames_TraveseList=PageFrames_TraveseList->next;
        }
		findpage=PageFrames_TraveseList;
		msg=Replace_Page_in_Frame(bm,findpage->FrameNum,findpage,page,pageNum);
        PageFrames_TraveseList=BufferManager_ValueStorage->HeadNode;
        return RC_OK;
    }
}


/*Purpose:implements least recently used  page replacement logic*/
RC LRU (BM_BufferPool *const bm, BM_PageHandle *const page,PageNumber pageNum)
{
	int i;
	PageFrames *PageFrames_FindPage;
	PageFrames *PageFrames_TraveseList;
    RC msg;
	BufferManager *BufferManager_ValueStorage  = (BufferManager *)bm->mgmtData;
	Frames_List  *Pass_List;
	PageFrames_TraveseList=BufferManager_ValueStorage->HeadNode;
	PageFrames_FindPage=BufferManager_ValueStorage->HeadNode;
    if((PageFrames_FindPage = page_exist_in_memory(bm, page, pageNum)) != NULL)
    {
        BufferManager_ValueStorage->Frame_List_Value=Update_Frame_Position(BufferManager_ValueStorage->Frame_List_Value,pageNum,1,bm);
        return RC_OK;
    }
    else
    {
        if((msg=CheckFrame_Unpinned(PageFrames_TraveseList,pageNum,BufferManager_ValueStorage->frameCount))!=RC_OK)
        {
            return msg;
        }
        BufferManager_ValueStorage->Frame_List_Value=Update_Frame_Position(BufferManager_ValueStorage->Frame_List_Value,pageNum,0,bm);
        Pass_List=BufferManager_ValueStorage->Frame_List_Value->head;

        for(i=0;i<BufferManager_ValueStorage->Frame_List_Value->last->Frame_Number;i++)
        {
   
            PageFrames_TraveseList=PageFrames_TraveseList->next;
        }
		PageFrames_FindPage=PageFrames_TraveseList;
		msg=Replace_Page_in_Frame(bm,PageFrames_FindPage->FrameNum,PageFrames_FindPage,page,pageNum);
    PageFrames_FindPage=NULL;
PageFrames_TraveseList=NULL;
        return msg;

    }
}

/* Purpose:replaces page inside the frame by checking various constraints such as dirty count */
RC Replace_Page_in_Frame (BM_BufferPool *const bm,int FrameNumberValue,PageFrames *PageFramesStruct,BM_PageHandle *const page, const PageNumber PageNumValue)
{

RC msg;
BufferManager *Set_Mgmt_Info = (BufferManager *)bm->mgmtData;
SM_FileHandle fh;

if (PageFramesStruct->DirtyCount == 1)
{
    if((msg = ensureCapacity(PageNumValue, &Set_Mgmt_Info->fileHandler_StorageMgr)) != RC_OK)
    {
        return msg;
    }

    if((msg = writeBlock(PageFramesStruct->PageNumberValue,&Set_Mgmt_Info->fileHandler_StorageMgr, PageFramesStruct->Data_Value)) != RC_OK)
    {
        return msg;
    }
    (Set_Mgmt_Info->Total_WriteIO)++;
}

	(Set_Mgmt_Info->Map_Page_To_Frame)[PageNumValue] = NO_PAGE;
    
    if(PageNumValue!=-1)
    {
        if((msg = ensureCapacity(PageNumValue, &Set_Mgmt_Info->fileHandler_StorageMgr)) != RC_OK)
        {
            return msg;
        }
        
        if((msg = readBlock(PageNumValue,&Set_Mgmt_Info->fileHandler_StorageMgr, PageFramesStruct->Data_Value)) != RC_OK)
        {
            return msg;
        }
  
        (Set_Mgmt_Info->Total_ReadIO)++;
    }
    page->pageNum =PageNumValue;
    page->data = PageFramesStruct->Data_Value;
    PageFramesStruct->PageNumberValue=PageNumValue;
    PageFramesStruct->FixCount= PageFramesStruct->FixCount+1;
	(Set_Mgmt_Info->Map_Page_To_Frame)[PageNumValue] = PageFramesStruct->FrameNum;
	(Set_Mgmt_Info->Map_Frame_To_Page)[PageFramesStruct->FrameNum] = PageNumValue;
  
    return RC_OK;
}

/*Purpose:Intiates a buffer Pool with specific properties*/
RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName, const int numPages, ReplacementStrategy strategy,void *stratData)
{
SM_FileHandle fh;
int i=0,flag=1;

if(openPageFile((char *)pageFileName,&fh)==RC_OK)
{
 
    BufferManager *bfmanger=malloc(sizeof(BufferManager));
    bfmanger->Total_ReadIO=0;
    bfmanger->Total_WriteIO=0;
    bfmanger->frameCount=numPages;
    bfmanger->strategy=strategy;
    memset(bfmanger->DirtyValue,false,MAX_FRAMES*sizeof(bool));
    memset(bfmanger->PagesFixedCount,0,MAX_FRAMES*sizeof(int));
    memset(bfmanger->Map_Frame_To_Page,NO_PAGE,MAX_FRAMES*sizeof(int));
    memset(bfmanger->Map_Page_To_Frame,NO_PAGE,MAX_PAGE_SIZE_1*sizeof(int));
    bm->strategy=strategy;
    bm->numPages=numPages;
    bfmanger->PageFileName=(char *)pageFileName;
    for( i=0;i<numPages;i++) /*Creates a Doubly Linked List*/
    {
        if(flag==1)
        {
      
            bfmanger->LastNode=bfmanger->HeadNode=CreateNodes();
            flag=0;
            bfmanger->HeadNode->previous=NULL;
            bfmanger->LastNode->next=NULL;
        }
        else
        {
    
            bfmanger->LastNode->next=CreateNodes();
            bfmanger->LastNode->next->previous=bfmanger->LastNode;
            bfmanger->LastNode=bfmanger->LastNode->next;
            bfmanger->LastNode->next=NULL;
        }
        bfmanger->LastNode->FrameNumber=i;
        bfmanger->LastNode->FrameNum=i;
    }
    bfmanger->fileHandler_StorageMgr=fh;
    bfmanger->Frame_List_Value=CreateFrameList_forLRU(numPages);
    bm->mgmtData=bfmanger;
    
return RC_OK;
}
    
RC_message = "initBufferPool";
return RC_FILE_NOT_FOUND;			  
}


RC pinPage (BM_BufferPool *const bm, BM_PageHandle *const page,const PageNumber pageNum)
{
PageFrames *findpage;
if(bm==NULL){RC_message= "pinPage"; return RC_BUFFER_NOT_INIT;}	

if(pageNum==NO_PAGE){RC_message= "pinPage"; return RC_PAGE_NOT_INIT;}		

RC msg;
switch(bm->strategy)
{
case RS_FIFO:
return fifo(bm,page,pageNum);
break;
case RS_LRU:
return LRU(bm,page,pageNum);
break;
default:
break;
}

return RC_OK;
}


/* Purpose: frees up all the allocated memory and removes the frame list and the pointer */
RC shutdownBufferPool(BM_BufferPool *const bm)
{
	if(forceFlushPool(bm)!=RC_OK)
    {
        return RC_BUFFER_NOT_INIT;
    }

BufferManager *mgmtDataInfo=(BufferManager *)bm->mgmtData;
PageFrames *TempTraverseNode=mgmtDataInfo->HeadNode;
PageFrames *DeleteTraverseNode=NULL;

if(mgmtDataInfo->HeadNode!=NULL)
{
	
	while(TempTraverseNode!=NULL)
	{
     free(TempTraverseNode->previous);  
        TempTraverseNode=TempTraverseNode->next;
	 
	}
    free(TempTraverseNode);
}
RemoveFrameList(mgmtDataInfo->Frame_List_Value);
free(mgmtDataInfo);
return RC_OK;
}

/*Purpose: when buffer pool is to be shut down, this function will write the pages which are dirty to the memory */
RC forceFlushPool(BM_BufferPool *const bm)
{

BufferManager *mgmtDataInfo=(BufferManager *)bm->mgmtData;
PageFrames *TempTraverseNode=mgmtDataInfo->HeadNode;
RC msg;
if(TempTraverseNode==NULL){return RC_FRAME_NOT_INIT;} 
mgmtDataInfo->Total_WriteIO=getNumWriteIO(bm);

while(TempTraverseNode!=NULL)
{
	if(TempTraverseNode->Pining==0){
	if(TempTraverseNode->DirtyCount==1)
	{
		if((msg = ensureCapacity(TempTraverseNode->PageNumberValue, &mgmtDataInfo->fileHandler_StorageMgr)) != RC_OK){
            return msg;
        }
		if((msg=writeBlock(TempTraverseNode->PageNumberValue,&mgmtDataInfo->fileHandler_StorageMgr,TempTraverseNode->Data_Value))==RC_OK)
		{
            TempTraverseNode->DirtyCount=0;
            mgmtDataInfo->DirtyValue[(TempTraverseNode->PageNumberValue)-1]=false;
            mgmtDataInfo->Total_WriteIO+=1;
		}
        else
        {
            RC_message = "forceFlushPool";
            RC_WRITE_FAILED;
        }
    }
}
else return RC_PINED_PAGE_FOUND;

TempTraverseNode=TempTraverseNode->next;
}

closePageFile(&mgmtDataInfo->fileHandler_StorageMgr);
free(TempTraverseNode);
return RC_OK;
}

/*Purpose:Marks a bit dirty */
RC markDirty (BM_BufferPool *const bm, BM_PageHandle *const page)
{
BufferManager *mgmtDataInfo=(BufferManager *)bm->mgmtData;
PageFrames *findpage=mgmtDataInfo->HeadNode;
if(bm==NULL)
{
    return RC_BUFFER_INVALID;
}	
if((findpage = Find_Page_Exsist(findpage, page->pageNum,false)) != NULL)
{
	(mgmtDataInfo->DirtyValue)[page->pageNum]=true;
		findpage->DirtyCount=1;	
}
else 
    return RC_NON_EXISTING_PAGE;
return RC_OK;
}

/* Purpose: updates if the page is unpinned and exists */
RC unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page)
{
	
	BufferManager *Set_Buffer_Manager_Ptr_unpin = (BufferManager *)bm->mgmtData;
    PageFrames *Pass_head_to_check_unpin=Set_Buffer_Manager_Ptr_unpin->HeadNode;
    PageFrames *LocatePage=CreateFrameNodes();
    if (!bm || bm->numPages <= 0)
    {
        return RC_BUFFER_INVALID;
    }
      
    if((LocatePage = Find_Page_Exsist(Pass_head_to_check_unpin,page->pageNum,true)) == NULL)
    {
        return RC_NON_EXISTING_PAGE;
    }
    if(LocatePage->FixCount > 0)
    {
        LocatePage->FixCount--;
    }
    return RC_OK;
}

/* Purpose: returns if the write has failed or it is fine */
RC forcePage (BM_BufferPool *const bm, BM_PageHandle *const page)
{
BufferManager *mgmtDataInfo=(BufferManager *)bm->mgmtData;
PageFrames *findpage=mgmtDataInfo->HeadNode;
SM_FileHandle fh;
mgmtDataInfo->Total_WriteIO=getNumWriteIO(bm);
if((findpage = Find_Page_Exsist(findpage,page->pageNum,false)) != NULL)
{
		if(writeBlock(findpage->PageNumberValue,&fh,findpage->Data_Value)==RC_OK)
		{
            mgmtDataInfo->Total_WriteIO+=1;
            findpage->DirtyCount=0;
            mgmtDataInfo->DirtyValue[(findpage->PageNumberValue)-1]=false;
		}
}
else
{
	RC_message = "forceFlushPool";
	RC_WRITE_FAILED;
}
return RC_OK;
}


/* Purpose : it is a statistics interface returning the frame contents mapping frame to page */
PageNumber *getFrameContents (BM_BufferPool *const bm)
{
    return (((BufferManager *)bm->mgmtData)->Map_Frame_To_Page);
}

bool *getDirtyFlags (BM_BufferPool *const bm)
{
BufferManager *mgmtDataInfo=(BufferManager *)bm->mgmtData;
PageFrames *findpage=mgmtDataInfo->HeadNode;
while(findpage!=NULL)
{(mgmtDataInfo->DirtyValue)[findpage->FrameNumber]=findpage->DirtyCount;
	findpage=findpage->next;
	}
return (mgmtDataInfo->DirtyValue);
}

/* Purpose : returns the pages fixed count */
int *getFixCounts (BM_BufferPool *const bm)
{
BufferManager *mgmtDataInfo=(BufferManager *)bm->mgmtData;
PageFrames *findpage=mgmtDataInfo->HeadNode;
while(findpage!=NULL)
{(mgmtDataInfo->PagesFixedCount)[findpage->FrameNumber]=findpage->FixCount;
	findpage=findpage->next;
	}
return (mgmtDataInfo->PagesFixedCount);
}

/* Purpose : returns the total read IO */
int getNumReadIO (BM_BufferPool *const bm)
{
return (((BufferManager *)bm->mgmtData)->Total_ReadIO);
}

/* Purpose : returns the total write IO */
int getNumWriteIO (BM_BufferPool *const bm)
{
return (((BufferManager *)bm->mgmtData)->Total_WriteIO);
}
