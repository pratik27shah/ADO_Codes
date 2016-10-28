#include <stdio.h>
#include "record_mgr.h"
#include "string.h"
#include <math.h>
#include "buffer_mgr.h"
#include "stdlib.h"
#include "tables.h"
#include "rm_serializer.h"
#include "storage_mgr.h"
/*
 * function variable namings fnname_variable name
 * structure variabel naming structurename_variablename
 * 
 * */
 
typedef struct tombNode {
  RID id;
  struct tombNode *next;
}tombNode;

int TupleFlag=0,total_tuples;
 
 typedef struct tableMgmtInfo{

	 int tableMgmtInfo_maxSlots;
    int tableMgmtInfo_recordStartPage;
    int tableMgmtInfo_tNodeLen;
    int tableMgmtInfo_recordLastPage;
    int tableMgmtInfo_SchemaLength;
    int tableMgmtInfo_numTuples;
    int tableMgmtInfo_slotSize;
    tombNode *tableMgmtInfo_tstone_root; //decalred in rm_Serializer.h
    BM_BufferPool *bm;
    SM_FileHandle fileHandler_StorageMgr;
}tableMgmtInfo;

char *String_Strtok(int prmValue)
{
     if(prmValue==101) return strtok(NULL,"<");
     else if(prmValue==102) return strtok(NULL,">");
     else if(prmValue==103) return strtok(NULL,"(");
     else if(prmValue==104) return strtok(NULL,")");
     else if(prmValue==105) return strtok(NULL,"[");
     else if(prmValue==106) return strtok(NULL,"]");
     else if(prmValue==107) return strtok(NULL,", ");
     else if(prmValue==108) return strtok(NULL,": ");
     else if(prmValue==109) return strtok(NULL,") ");

}

int String_Strtol(char prmValue1){
 char *temp_str2;
 return strtol(prmValue1,&temp_str2,BaseValue);}



extern RC initRecordManager(void *data)
{return RC_OK;
	}

/*change-3 Pratik fn type changed from string to varstring ref line:159*/
VarString *GetTableInfo(tableMgmtInfo *prmInfo,VarString *prmVarString)
{
	int tuples;
	int GetTableInfo_SetnodeValue,flag=1;
	char *GetTableInfo_SetResult;
	MAKE_VARSTRING(prmVarString);
  tombNode *tombNode_head;
tombNode_head=prmInfo->tableMgmtInfo_tstone_root;
GetTableInfo_SetnodeValue=GetTnodeLength(tombNode_head);
 tuples=prmInfo->tableMgmtInfo_numTuples;
 APPEND(prmVarString, "SchemaLength <%d> FirstRecordPage <%d> LastRecordPage <%d> NumTuples <%d> SlotSize <%d> MaxSlots <%d> tNodeLen <%d> <" , prmInfo->tableMgmtInfo_SchemaLength, prmInfo->tableMgmtInfo_recordStartPage, prmInfo->tableMgmtInfo_recordLastPage
 ,tuples, prmInfo->tableMgmtInfo_slotSize,
  prmInfo->tableMgmtInfo_maxSlots,GetTableInfo_SetnodeValue);

	while(tombNode_head!=NULL)
	{

		if(tombNode_head->next==NULL)
			flag=0;
			
		if(flag==1)
		{	

		APPEND(prmVarString,"%i:%i ",tombNode_head->id.page,tombNode_head->id.slot);		
		}
		
		else {
			APPEND(prmVarString,"%i:%i%s ",tombNode_head->id.page,tombNode_head->id.slot," ");		
		}
		
		
		tombNode_head=tombNode_head->next;
	}
	APPEND_STRING(prmVarString,">\n");
				
	return prmVarString;
}	

/* length of tombnode list */
int GetTnodeLength(tombNode *prmTombNode_head)
{
int value=0;	
while(prmTombNode_head!=NULL){
prmTombNode_head=prmTombNode_head->next;
value++;
}
return value;
}


tombNode *SetDeletePositions(tombNode *temp_node,int Page,int Slot,int prmval)
{
	 tombNode *temp_node2;
if(prmval==1){
	//if(temp_node->tableMgmtInfo_tstone_root!=NULL)
	while(temp_node!=NULL)
//	if( temp_node!=NULL )
	temp_node=temp_node->next;
    temp_node2=(tombNode *)malloc(sizeof(tombNode));
    

	temp_node2->next=NULL;
//temp_node=temp_node->next;
temp_node=temp_node2;
	
}
	temp_node->id.page=Page;
	temp_node->id.slot=Slot;
	temp_node->id.indicate_tomb_stone=1;
return temp_node;}

extern RC deleteRecord (RM_TableData *rel, RID id){
    tableMgmtInfo *table_info = (tableMgmtInfo *) (rel->mgmtData);
    if(table_info->tableMgmtInfo_numTuples==0) return RC_NO_MORE_TUPLES;
    tombNode *temp_node=table_info->tableMgmtInfo_tstone_root;
    tombNode *temp_node2;
    if(table_info->tableMgmtInfo_tstone_root==NULL) 
    {
		temp_node=(tombNode *)malloc(sizeof(tombNode));
		table_info->tableMgmtInfo_tstone_root=temp_node;
		temp_node->next=NULL;
		
		}
		else{
	
//	printf("%d  %d  %d\n",temp_node->id.page,id.page,id.slot);
	temp_node=SetDeletePositions(temp_node,id.page,id.slot,1);	
    
} 
temp_node=SetDeletePositions(temp_node,id.page,id.slot,0);	
    
   table_info->tableMgmtInfo_numTuples--;
  
Map_TabletoFile(rel->name, table_info); 
return RC_OK;
}


extern RC createTable (char *name, Schema *schema){
	//	printf("\n Into createTable_forInsert \n");

	RC createTable_Status;
	char *createTable_InfoMgnt,*Schema_str;
	int createTable_SchemaSize,createTable_MaxSlotSize;
	 SM_FileHandle fh;
	VarString *createTable_VarString;
	float createTable_fileSize;
	if(CheckForTableExsist(name)!=1)
	{
	
if((createTable_Status=createPageFile(name))!=RC_OK)
return 	createTable_Status;

createTable_SchemaSize=SetGetSchemaSize(schema);
createTable_fileSize= (int)ceil((float)createTable_SchemaSize/(float)PAGE_SIZE);
//printf("\n\n(%d , %d %d)",PAGE_SIZE,createTable_SchemaSize,createTable_fileSize);
//createTable_fileSize=(int)ceil(createTable_fileSize);
createTable_MaxSlotSize=SetslotSize(schema,ReturnMaxSlots);
	if ((createTable_Status=openPageFile(name, &fh)) == RC_OK){
	tableMgmtInfo *tableMgmtInfo_Info=(tableMgmtInfo *)malloc(sizeof(tableMgmtInfo));
		/*Stroing table info in file*/
		if ((createTable_Status=ensureCapacity(createTable_fileSize, &fh)) == RC_OK){
		tableMgmtInfo_Info->tableMgmtInfo_numTuples=0;
total_tuples=0;
		tableMgmtInfo_Info->tableMgmtInfo_recordLastPage=createTable_fileSize+1;
		tableMgmtInfo_Info->tableMgmtInfo_maxSlots=createTable_MaxSlotSize;
		tableMgmtInfo_Info->tableMgmtInfo_slotSize=SetslotSize(schema,ReturnSlotSize);
		tableMgmtInfo_Info->tableMgmtInfo_recordStartPage=createTable_fileSize+1;
		/**/
		tableMgmtInfo_Info->tableMgmtInfo_SchemaLength=createTable_SchemaSize;
		tableMgmtInfo_Info->tableMgmtInfo_tstone_root=NULL;
		}
		else{  return createTable_Status;}
	/*Stroing schema info in table*/
  if ((createTable_Status=ensureCapacity(createTable_fileSize+1, &fh)) == RC_OK){
	GET_STRING(createTable_InfoMgnt,GetTableInfo(tableMgmtInfo_Info,createTable_VarString));
	
	
	if ((createTable_Status=writeBlock(0, &fh,createTable_InfoMgnt)) == RC_OK){
					    char *Schema_str = serializeSchema(schema);	
					    
	if ((createTable_Status=writeBlock(1, &fh, Schema_str)) == RC_OK){
		
	if ((createTable_Status=closePageFile(&fh)) != RC_OK){ 
	
		return createTable_Status;}

//printf("\n(%s)",Schema_str);
  tableMgmtInfo_Info->fileHandler_StorageMgr=fh;
  } /*write block-1 if closed*/
        
        else return createTable_Status; /*write block-1*/
		} /*write block-0 closed*/
				
		else{ return createTable_Status;} /*write block-0*/
	}/*ensure capacity closed*/
			
		else{ return createTable_Status;} /*ensure capacity*/
}
        
      else return createTable_Status;
				//printf("\n Reched 0\n");


	return RC_OK; /*success returns*/
	}
else {return RC_TABLE_ALREADY_EXITS;}

}

Record *deserializeRecord(char *record_str, RM_TableData *rel){
    char *temp_str1, *temp_str2;
    int i, int_value;
    float float_value;
    bool bool_value;
    Record *deserializeRecord_record = (Record *) malloc(sizeof(Record));
    tableMgmtInfo *info = (tableMgmtInfo *) (rel->mgmtData);
    Schema *deserializeRecord_schema = rel->schema;
    Value *value;

    deserializeRecord_record->data = (char *)calloc(sizeof(char), info->tableMgmtInfo_slotSize);
    char record_data[strlen(record_str)];
    strcpy(record_data, record_str);

    temp_str1 = strtok(record_data,"-");
    temp_str1 = String_Strtok(RightSquare);
    /* int slot; */
    /* slot = strtol(temp_str1, &temp_str2, 10); */
    temp_str1 = String_Strtok(LeftCircular);

    for(i=0; i<deserializeRecord_schema->numAttr; i++){
        temp_str1 = String_Strtok(Colon);
        if((i+1) != (deserializeRecord_schema->numAttr)){
            temp_str1 = String_Strtok(Comma);
        }else{
            temp_str1 = String_Strtok(RightCircular);
        }

        if(deserializeRecord_schema->dataTypes[i] == DT_INT){
                int_value = strtol(temp_str1, &temp_str2, 10);
                MAKE_VALUE(value, DT_INT, int_value);
                setAttr (deserializeRecord_record, deserializeRecord_schema, i, value);
                freeVal(value);
        }else if(deserializeRecord_schema->dataTypes[i] == DT_STRING){
                MAKE_STRING_VALUE(value, temp_str1);
                setAttr (deserializeRecord_record, deserializeRecord_schema, i, value);
                freeVal(value);
        }else if(deserializeRecord_schema->dataTypes[i] == DT_BOOL){
                if(temp_str1[0] == 't'){
                    bool_value = TRUE;
                }else{
                    bool_value = FALSE;
                }
                MAKE_VALUE(value, DT_BOOL, bool_value);
                setAttr (deserializeRecord_record, deserializeRecord_schema, i, value);
                freeVal(value);            
        }else if(deserializeRecord_schema->dataTypes[i] == DT_FLOAT){
                float_value = strtof(temp_str1, NULL);
                MAKE_VALUE(value, DT_FLOAT, float_value);
                setAttr (deserializeRecord_record, deserializeRecord_schema, i, value);
                freeVal(value);
        }

    }
    free(record_str);
    return deserializeRecord_record;
}

/***Create Ends */

/**********OPEN TABLE**************/
tableMgmtInfo *ConvertStr_to_TableDetails(char *prminfo_str,char *prminfo_data)
{
	//char TabDetails_infoStr[prminfo_str_len];
	tableMgmtInfo *tableMgmtInfo_info = (tableMgmtInfo*) malloc(sizeof(tableMgmtInfo));
	char *temp_str1,*temp_str2;
	int i=0,Open_PageValue,Open_tnodeLen=0;
	   temp_str1 = strtok (prminfo_str,"<");
	   temp_str1 = strtok (NULL,">");
tableMgmtInfo_info->tableMgmtInfo_SchemaLength=strtol(temp_str1, &temp_str2, BaseValue);

	for(i=1;i<=6;i++)
	{
		temp_str1 = String_Strtok (LeftAngular);
   temp_str1 = String_Strtok (RightAngular);
   
    switch(i)
    {
		case 1:
	tableMgmtInfo_info->tableMgmtInfo_recordStartPage=strtol(temp_str1,&temp_str2,10);
	//String_Strtol(*temp_str1,&temp_str2);
		break;
		case 2:
tableMgmtInfo_info->tableMgmtInfo_recordLastPage=strtol(temp_str1, &temp_str2, BaseValue);
		break;
		case 3:
tableMgmtInfo_info->tableMgmtInfo_numTuples=strtol(temp_str1, &temp_str2, BaseValue);
total_tuples=tableMgmtInfo_info->tableMgmtInfo_numTuples;
		break;
		case 4:
	tableMgmtInfo_info->tableMgmtInfo_slotSize=strtol(temp_str1, &temp_str2, BaseValue);

		break;
		case 5:
tableMgmtInfo_info->tableMgmtInfo_maxSlots=strtol(temp_str1, &temp_str2, BaseValue);
		break;
		
		default:
		tableMgmtInfo_info->tableMgmtInfo_tNodeLen=strtol(temp_str1, &temp_str2, BaseValue);
 temp_str1 = strtok (NULL,"<");
		break;
		}
		
	}
	tombNode *tempNode;
	int flag=1,Open_SlotValue;
	i=0;
		 while(i<tableMgmtInfo_info->tableMgmtInfo_tNodeLen)
		 
		 {
			 temp_str1 = strtok(NULL,":");
      Open_PageValue = strtol(temp_str1, &temp_str2, BaseValue);
      Open_tnodeLen=tableMgmtInfo_info->tableMgmtInfo_tNodeLen;
        if(i==(Open_tnodeLen-1)) {temp_str1=strtok(NULL,",");}
        else {temp_str1=String_Strtok (RightAngular);}
		i++;	
		Open_SlotValue=strtol(temp_str1, &temp_str2, BaseValue);
			
			if(flag==1)
			{
				flag=0;
		
		tableMgmtInfo_info->tableMgmtInfo_tstone_root = (tombNode *)malloc(sizeof(tombNode));
		tempNode =   tableMgmtInfo_info->tableMgmtInfo_tstone_root;
	tableMgmtInfo_info->tableMgmtInfo_tstone_root->id.page = Open_PageValue;
        tableMgmtInfo_info->tableMgmtInfo_tstone_root->id.slot = Open_SlotValue;
       
			}
			
			else {
				tempNode->next = (tombNode *)malloc(sizeof(tombNode));
				tempNode = tempNode->next;
				}
		 tempNode->id.page = Open_PageValue;
            tempNode->id.slot = Open_SlotValue;

}
return tableMgmtInfo_info;
	 }

Schema *deserializeSchema(char schema_data[]){
    int i, j, Category[3], k=0, numAttr,val_set = 1, val_size = 0;
    char *temp_str1, *temp_str2, *temp_str3, *value_to_fetch;
    Schema *deserializeSchema_schema = (Schema *) malloc(sizeof(Schema));
 if(*schema_data!=NULL){
    temp_str1 = strtok (schema_data,"<");
    temp_str1 = String_Strtok(RightAngular);

    numAttr = strtol(temp_str1, &temp_str2, 10);
    char *values_array[numAttr];

    deserializeSchema_schema->attrNames=(char **)malloc(sizeof(char*)*numAttr);
    deserializeSchema_schema->typeLength=(int *)malloc(sizeof(int)*numAttr);
    deserializeSchema_schema->dataTypes=(DataType *)malloc(sizeof(DataType)*numAttr);
    deserializeSchema_schema->numAttr= numAttr;

    char* reference_String[numAttr];
    temp_str1 = String_Strtok(LeftCircular);

    for(i=0; i<numAttr; ++i){
        temp_str1 = String_Strtok(Colon);
        deserializeSchema_schema->attrNames[i]=(char *)calloc(strlen(temp_str1),sizeof(char));
        strcpy(deserializeSchema_schema->attrNames[i], temp_str1);

        if(i != numAttr-1){
            temp_str1 = String_Strtok(Comma);
        }
        else{
            temp_str1 = String_Strtok(RightCircularSpace);
        }
        Category[k] = strcmp(temp_str1,"INT");
        Category[k+1] = strcmp(temp_str1,"FLOAT");
        Category[k+2] = strcmp(temp_str1,"BOOL");

        reference_String[i] = (char *)calloc(sizeof(char),strlen(temp_str1));

        if(Category[k] != 0)
        {
            if(Category[k+1] != 0)
            {    
                if(Category[k+2] != 0)
                {    
                    strcpy(reference_String[i], temp_str1);
                }
            }    
        }

        if((Category[k] != 0)||(Category[k+1] != 0)||(Category[k+2] != 0))
        {
            deserializeSchema_schema->dataTypes[i] = (Category[k] == 0) ? DT_INT:
            (Category[k+1] == 0) ? DT_FLOAT:
            (Category[k+2] == 0) ? DT_BOOL:
            deserializeSchema_schema->dataTypes[i];
            deserializeSchema_schema->typeLength[i] = ((Category[k] == 0)||(Category[k+1] == 0)||(Category[k+2] == 0)) ? 0 : deserializeSchema_schema->typeLength[i];
        }        
    }

    if((temp_str1 = String_Strtok(LeftCircular)) == NULL){
    }else{
        temp_str1 = String_Strtok(RightCircular);
        value_to_fetch = strtok (temp_str1,", ");
        while(value_to_fetch != NULL){
            values_array[val_size] = (char *)malloc(strlen(value_to_fetch)*sizeof(char));
            strcpy(values_array[val_size], value_to_fetch);
            value_to_fetch = String_Strtok(Comma);
            val_size++;
        }
        val_set = FetchedValues;
    }

    for(i=0; i<numAttr; ++i){
        if(strlen(reference_String[i]) <= 0){
        }else{
            temp_str3 = (char *) malloc(sizeof(char)*strlen(reference_String[i]));
            memset(temp_str3,'\0',sizeof(temp_str3));
            strncpy(temp_str3, reference_String[i],strlen(reference_String[i])); //  memcpy(temp_str3, reference_String[i], strlen(reference_String[i]));
            temp_str1 = strtok (temp_str3,"[");
            temp_str1 = String_Strtok(RightSquare);
            deserializeSchema_schema->typeLength[i] = strtol(temp_str1, &temp_str2, 10);
            deserializeSchema_schema->dataTypes[i] = DT_STRING;
            free(reference_String[i]);
            free(temp_str3);

        }
    }

    if(val_set != FetchedValues){
    }else{
        deserializeSchema_schema->keyAttrs=(int *)malloc(sizeof(int)*val_size);
        deserializeSchema_schema->keySize = val_size;
        for(i=0; i<val_size; i++){
            for(j=0; j<numAttr; j++){
                if(strcmp(deserializeSchema_schema->attrNames[j],values_array[i]) != 0){
                }else{
                    deserializeSchema_schema->keyAttrs[i] = j;
                    free(values_array[i]);
                }

            }
        }
    }
}
    return deserializeSchema_schema;
}

extern RC openTable (RM_TableData *rel, char *name){
BM_BufferPool *bm = (BM_BufferPool *)malloc(sizeof(BM_BufferPool));
BM_PageHandle *page = (BM_PageHandle *)malloc(sizeof(BM_PageHandle));
 int flag=0;
 TupleFlag=0;
if(CheckForTableExsist(name)==1)
	{
	 initBufferPool(bm,name,3,RS_FIFO,NULL);
     pinPage(bm,page, 0);	
     char Open_TabInfo[strlen(page->data)];


  tableMgmtInfo *openTable_info=ConvertStr_to_TableDetails(page->data,Open_TabInfo);
		//printf("2");
		if(Open_DeserialComapre(openTable_info->tableMgmtInfo_SchemaLength,PAGE_SIZE)==1)
				{
				pinPage(bm,page, 1);
				}
				
		else
		{
			flag=1;
		openTable_info->bm=bm;
		rel->name=name;
			}
		if(flag==0)
		{
		openTable_info->bm=bm;
		rel->name=name;
		}
        //printf("Size of page->data is \t %d \n\n",(int)sizeof(&page->data));
         char Schema_deserialize_data[(int)strlen(&page->data)];
         strcpy(Schema_deserialize_data,page->data);
        //printf("Page->data is \t%s\n",Schema_deserialize_data);
rel->schema=deserializeSchema(Schema_deserialize_data);//deserializeSchema(page->data,Open_TabInfo);
        //printf("Size of page->data is \t %d \n\n",(int)sizeof(&page->data));
rel->mgmtData=openTable_info;
		}


else 
return RC_TABLE_NOT_FOUND;
getNumTuples(rel);              

free(page);
return RC_OK;
}




int Open_DeserialComapre(int prmSchemaLength,int prmPageSize)
{
		if(prmSchemaLength<prmPageSize)
		return 1;
	
	return 0;
}


/*Purpose:Check if table exsist or not*/
int CheckForTableExsist(char *prmname)
{ 	/*1->file exsist 0->file not exsist*/
	if(access(prmname,0)!=-1)
	return 1;
	
	return 0;
}

/*
 * int -5
 * float-6
 * bool- 2
 * string-3
 * 
 * 
 * **/
int SetslotSize(Schema *schema,int Operation){
	int i=0;
	int slotSize=15;
/*(< + int + int + >+2 space+())
	*/
	for(i=0;i<schema->numAttr;i++)
	{
		
		if(schema->dataTypes[i]==DT_STRING){slotSize=slotSize+schema->typeLength[i];}
		if(schema->dataTypes[i]==DT_INT){slotSize=slotSize+5;}
		if(schema->dataTypes[i]==DT_FLOAT){slotSize=slotSize+6;}
		if(schema->dataTypes[i]==DT_BOOL){slotSize=slotSize+2;}
		
		
	slotSize=slotSize+strlen(schema->attrNames[i])+2;
	}switch(Operation)
	{
		case ReturnSlotSize: 
		return  slotSize;
		
		
		default:return PAGE_SIZE/slotSize;
		
		}
	return slotSize;
	}

int SetGetSchemaSize (Schema *schema){
    int i=0, SetGetSchemaSize_size = 0;
    /*keyAttr,typelength,keysize*/
    SetGetSchemaSize_size = sizeof(int)*(schema->numAttr)+ sizeof(int)*(schema->numAttr)     
    + sizeof(int); 
    
    if(schema->dataTypes==2)        SetGetSchemaSize_size=sizeof(int)+SetGetSchemaSize_size+sizeof(float);
    else if(schema->dataTypes==3)  SetGetSchemaSize_size=sizeof(int)+SetGetSchemaSize_size+sizeof(bool);
    else                            SetGetSchemaSize_size=sizeof(int)+SetGetSchemaSize_size+sizeof(int);
    
    for (i=0; i<schema->numAttr; i++){
        SetGetSchemaSize_size = SetGetSchemaSize_size+strlen(schema->attrNames[i]);
//printf("\n\nscg[%s]\n",schema->attrNames[i]);
    }
//printf("size--(%d)\n",size);
    return SetGetSchemaSize_size;
}

/**************Deletion of Table********************/	
extern RC deleteTable(char *name)
{
	if(CheckForTableExsist(name)==1)
	{
		if(DeleteTableCreated(name)==1)
		return RC_OK;
	}
return  RC_TABLE_NOT_FOUND;
}
/*Purpose:Deletes file(table)*/
int DeleteTableCreated(char *prmname)
{if(remove(prmname)==0) {return 1;}
return 0;
}
/*****************Deletion ends*****************/


	
	extern RC closeScan (RM_ScanHandle *scan){
    //free(scan);
    return RC_OK;
}

extern int getNumTuples (RM_TableData *rel){
    int local_num_tuples = 0;
    RM_ScanHandle *temp;
    temp = (tableMgmtInfo*)rel->mgmtData;
    local_num_tuples = ((tableMgmtInfo*)temp)->tableMgmtInfo_numTuples;
    if(local_num_tuples<-1){ return 0;}
    //printf("\nlocal_num_tuples = %d\n", local_num_tuples);
        return local_num_tuples;
}

/************Schema creation and close******************************/
extern Schema *createSchema (int numAttr, char **attrNames, DataType *dataTypes, int *typeLength, int keySize, int *keys)
{Schema *createSchema_Schema=(Schema *) malloc(sizeof(Schema));
	if(createSchema_Schema!=NULL){ //return RC_SCHEMA_INVALID;
	createSchema_Schema->numAttr=numAttr;
	createSchema_Schema->dataTypes=dataTypes;
	createSchema_Schema->keySize=keySize;
	createSchema_Schema->typeLength=typeLength;
	createSchema_Schema->keyAttrs=keys;
	createSchema_Schema->attrNames=attrNames;
}
	return createSchema_Schema;}

extern RC freeSchema(Schema *schema){free(schema);return RC_OK;}


extern RC getAttr (Record *record, Schema *schema, int attrNum, Value **value)
{
	if(schema==1) {return RC_SCHEMA_INVALID;}
	
	*value = (Value *)  malloc(sizeof(Value));
	char * attrData;
	int offset;
	int Category=schema->dataTypes[attrNum];
	
  
    attrOffset(schema, attrNum, &offset);
    attrData = record->data + offset;
	  (*value)->dt = Category;
	 if(Category==DT_INT){memcpy(&((*value)->v.intV), attrData,sizeof(int));}
	else if(Category==DT_FLOAT){memcpy(&((*value)->v.floatV),attrData,sizeof(float));}
	else if(Category==DT_STRING){
           char *TempString;
           TempString = (char *) calloc((schema->typeLength[attrNum]),sizeof(char));
           strcpy(TempString, attrData);
         (*value)->v.stringV=TempString;
         ((*value)->v.stringV)[schema->typeLength[attrNum]] = '\0';
		}
	else {memcpy(&((*value)->v.boolV),attrData ,sizeof(bool));}
	return RC_OK;}


	
/**********************Scheama Operation ends******************/
extern RC setAttr(Record *record, Schema *schema, int attrNum, Value *value)
{
	int offset; 
	int Category=schema->dataTypes[attrNum];
	char * attrData;
    attrOffset(schema, attrNum, &offset);
    attrData = record->data + offset;
    if(Category==DT_INT){memcpy(attrData,&(value->v.intV), sizeof(int));}
	else if(Category==DT_FLOAT){memcpy(attrData,&(value->v.floatV), sizeof(float));}
	else if(Category==DT_STRING){memcpy(attrData,(value->v.stringV),schema->typeLength[attrNum]);}
	else {memcpy(attrData,&(value->v.boolV), sizeof(bool));}
	return RC_OK;}


extern RC createRecord (Record **record, Schema *schema){
	 if(schema==NULL) {return RC_SCHEMA_INVALID;}
	 *record = (Record *)malloc(sizeof(Record));
    (*record)->data = (char *)malloc((getRecordSize(schema)));
		return RC_OK;}
int Tuples;
void  SetHighTupleCounter(tableMgmtInfo *info_about_page_insert)
{
	info_about_page_insert->tableMgmtInfo_numTuples=Tuples;
	//printf("%d",info_about_page_insert->tableMgmtInfo_numTuples);
	Tuples=0;
	}

//extern RC insertRecord (RM_TableData *rel, Record *record){ return RC_OK;}
extern RC insertRecord (RM_TableData *rel, Record *record,int prmValue){
	
	int insertRecord_page_num, insertRecord_slot_num;
   tableMgmtInfo *info_about_page_insert = (tableMgmtInfo *) (rel->mgmtData);
 //BM_PageHandle *Insert_pghandle_page = (BM_PageHandle *)malloc(sizeof(BM_PageHandle));

     if (info_about_page_insert->tableMgmtInfo_tstone_root == NULL)
	{
	
	insertRecord_page_num = info_about_page_insert->tableMgmtInfo_recordLastPage;
	insertRecord_slot_num = calculate_slot(info_about_page_insert,insertRecord_page_num);
	
	if (info_about_page_insert->tableMgmtInfo_maxSlots != insertRecord_slot_num)
	  { 
		info_about_page_insert->tableMgmtInfo_recordLastPage = insertRecord_page_num; 
		}
	 
	 else {
		//   printf("\n Into Insert Tuple: ELSE ");
		    insertRecord_slot_num = 0;
            insertRecord_page_num++;
	//total_tuples=10000;
		} 

}  
 
     else {
		  insertRecord_page_num = info_about_page_insert->tableMgmtInfo_tstone_root->id.page;
		  insertRecord_slot_num = info_about_page_insert->tableMgmtInfo_tstone_root->id.slot;
		  info_about_page_insert->tableMgmtInfo_tstone_root = info_about_page_insert->tableMgmtInfo_tstone_root->next;		  

		  } 
  
	record->id.page = insertRecord_page_num;
    record->id.slot = insertRecord_slot_num; 
 BM_PageHandle *Insert_pghandle_page = (BM_PageHandle *)malloc(sizeof(BM_PageHandle));
  
if( prmValue>106)
{
	Tuples++;
//info_about_page_insert->tableMgmtInfo_numTuples=prmValue;
TupleFlag=1;
}else
  (info_about_page_insert->tableMgmtInfo_numTuples)++;
 
 
// Insert_Update(rel,info_about_page_insert,record,insertRecord_slot_num,insertRecord_page_num);
 

    char *serialized_record = serializeRecord(record, rel->schema);
if(insertRecord_page_num==-1){return RC_NO_MORE_TUPLES;}
    pinPage(info_about_page_insert->bm, Insert_pghandle_page, insertRecord_page_num);
   
    memcpy(Insert_pghandle_page->data + (insertRecord_slot_num * info_about_page_insert->tableMgmtInfo_slotSize),serialized_record,strlen(serialized_record));
   free(serialized_record); 

    markDirty(info_about_page_insert->bm, Insert_pghandle_page);
    unpinPage(info_about_page_insert->bm, Insert_pghandle_page);
   forcePage(info_about_page_insert->bm, Insert_pghandle_page);

   record->id.indicate_tomb_stone = 0;  
//     (info_about_page_insert->tableMgmtInfo_numTuples)++;
    
if(TupleFlag==1 && Tuples==prmValue)
{
		info_about_page_insert->tableMgmtInfo_numTuples=Tuples;

	//SetHighTupleCounter(info_about_page_insert);
	}
 Map_TabletoFile(rel->name, info_about_page_insert); 
   
 free(Insert_pghandle_page); 

if(TupleFlag==1 && Tuples==prmValue)
{
		info_about_page_insert->tableMgmtInfo_numTuples=Tuples;

	//SetHighTupleCounter(info_about_page_insert);
	}
 return RC_OK;
  
 }



void CalculateTuple()
{total_tuples=Tuples;
//total_tuples);
	}
void Insert_Update(RM_TableData *rel,tableMgmtInfo *info_about_page_insert,Record *record,int insertRecord_slot_num,int insertRecord_page_num)
{
   BM_PageHandle *Insert_pghandle_page = (BM_PageHandle *)malloc(sizeof(BM_PageHandle));
   //total_tuples++;
   
	    char *serialized_record = serializeRecord(record, rel->schema);
    pinPage(info_about_page_insert->bm, Insert_pghandle_page, insertRecord_page_num);
  
  memcpy(Insert_pghandle_page->data + (insertRecord_slot_num * info_about_page_insert->tableMgmtInfo_slotSize),serialized_record,strlen(serialized_record));
   
 markDirty(info_about_page_insert->bm, Insert_pghandle_page);
    unpinPage(info_about_page_insert->bm, Insert_pghandle_page);
   forcePage(info_about_page_insert->bm, Insert_pghandle_page);
   
   record->id.indicate_tomb_stone = 0;  
  
 Map_TabletoFile(rel->name, info_about_page_insert); 
  free(serialized_record);
 free(Insert_pghandle_page);
 
	}



extern RC updateRecord(RM_TableData *rel, Record *record)
{
	
	if(rel==NULL) {return RC_TABLE_NOT_FOUND;}
	 tableMgmtInfo *info_about_page_insert = (tableMgmtInfo *) (rel->mgmtData);
	BM_PageHandle *Insert_pghandle_page = (BM_PageHandle *)malloc(sizeof(BM_PageHandle));

	int insertRecord_page_num, insertRecord_slot_num;
   insertRecord_page_num = info_about_page_insert->tableMgmtInfo_recordLastPage;
	insertRecord_slot_num = calculate_slot(info_about_page_insert,insertRecord_page_num);
	(info_about_page_insert->tableMgmtInfo_numTuples)++;
    char *serialized_record = serializeRecord(record, rel->schema);
    pinPage(info_about_page_insert->bm, Insert_pghandle_page, insertRecord_page_num);
    memcpy(Insert_pghandle_page->data + (insertRecord_slot_num * info_about_page_insert->tableMgmtInfo_slotSize),serialized_record,strlen(serialized_record));
    

    markDirty(info_about_page_insert->bm, Insert_pghandle_page);
    unpinPage(info_about_page_insert->bm, Insert_pghandle_page);
   forcePage(info_about_page_insert->bm, Insert_pghandle_page);

   record->id.indicate_tomb_stone = 0;  
     
    
 Map_TabletoFile(rel->name, info_about_page_insert); 
   free(serialized_record);
 free(Insert_pghandle_page); 
	

	//Insert_Update(rel,record,record->id.slot,record->id.page,info_about_page_insert);
	return RC_OK;} 


int calculate_slot(tableMgmtInfo *tableMgmtInfo_calc, int insertRecord_page_num_calc)
{
	int temp1,temp2,slot_number;
	if(temp1!=NULL){
	temp1 = insertRecord_page_num_calc - tableMgmtInfo_calc->tableMgmtInfo_recordLastPage;
	temp2 = temp1 * (tableMgmtInfo_calc->tableMgmtInfo_maxSlots);
	}
	slot_number = tableMgmtInfo_calc->tableMgmtInfo_numTuples - temp2;
	return slot_number;
}


RC Map_TabletoFile(char *relation_name, tableMgmtInfo *info_about_page){

  // SM_FileHandle map_fh=info_about_page->fileHandler_StorageMgr;
    int map_status;
    char *Map_String;
    VarString *map_string_info,*map_return_string;
    MAKE_VARSTRING(map_return_string);
   if(map_return_string==NULL) {return RC_TABLE_NOT_FOUND;}
   
   if(CheckForTableExsist(relation_name)==0) {
        return RC_TABLE_NOT_FOUND;
    }    
    
    if ((map_status=openPageFile(relation_name, &info_about_page->fileHandler_StorageMgr)) != RC_OK){
        return map_status;
    }
    
    map_return_string = GetTableInfo(info_about_page,map_string_info);
    GET_STRING(Map_String, map_return_string);
      if ((map_status=writeBlock(0, &info_about_page->fileHandler_StorageMgr, Map_String)) != RC_OK){
       
       return map_status;
    }

    if ((map_status=closePageFile(&info_about_page->fileHandler_StorageMgr)) != RC_OK){
        return map_status;
    }

    free(Map_String);
    return RC_OK;
}


extern getRecordSize(Schema *schema)
{
int i,RecordSize_temp=0;
if(schema==NULL) {return RC_SCHEMA_INVALID;}
int Category;
for(i=0;i<schema->numAttr;i++)
{
	Category=schema->dataTypes[i];
	switch(Category)
	{
	case DT_INT:
	RecordSize_temp+=sizeof(int);
	break;
	default:
//	if(Category==DT_INT){}
	if(Category==DT_FLOAT){RecordSize_temp+=sizeof(float);}
	else if(Category==DT_STRING){RecordSize_temp+=schema->typeLength[i];}
	else {RecordSize_temp+=sizeof(bool);}
	//brea
	}
}	//printf("\nGRS(%d)",RecordSize_temp);
return RecordSize_temp;
}

extern RC closeTable(RM_TableData *rel){
	int i=1;
shutdownBufferPool(((tableMgmtInfo *)rel->mgmtData)->bm);
do
{
	i=i+2;
if(rel->mgmtData!=NULL)
 free(rel->mgmtData);
else 
break;
 if(rel->schema->keyAttrs!=NULL)
 free(rel->schema->keyAttrs); 
 else
  break;
 if(rel->schema->dataTypes!=NULL)
  free(rel->schema->dataTypes); 
  else 
  break;
  if(rel->schema!=NULL)
 free(rel->schema); 
  else 
  break;/*
 if(rel!=NULL)
 free(rel);
  else 
  break;*/
}while(i<3);
    return RC_OK;
}

extern RC shutdownRecordManager(){
    TupleFlag=0;
    Tuples=0;
    return RC_OK;
}


extern RC getRecord (RM_TableData *rel, RID id, Record *record)
{
    
    int getRecord_page, getRecord_slot;
    
    int check_tombstone_result;
    
    tableMgmtInfo *getRecord_data = (tableMgmtInfo *) (rel->mgmtData);
        
    BM_PageHandle *getRecord_page_handle = (BM_PageHandle *)malloc(sizeof(BM_PageHandle));  
    getRecord_page = id.page;
    getRecord_slot = id.slot;
 
    record->id.page = getRecord_page;
    record->id.slot = getRecord_slot;
    record->id.indicate_tomb_stone = 0;
       
   tombNode *head_node = getRecord_data->tableMgmtInfo_tstone_root;
    check_tombstone_result = Check_Tombstone(getRecord_page,getRecord_slot,head_node,getRecord_data,id,record);
    if (check_tombstone_result == 0)
{
	//printf("\n Into IF for check_tombstone_result \n");
	return RC_RM_NO_MORE_TUPLES;
}

else{
        pinPage(getRecord_data->bm, getRecord_page_handle, getRecord_page); 
        char *store_record = (char *) malloc(sizeof(char) * getRecord_data->tableMgmtInfo_slotSize);
     //("\n getRecord_page_handle->dat %d\n ",getRecord_page_handle->data);
        memcpy(store_record, getRecord_page_handle->data + ((getRecord_slot)*getRecord_data->tableMgmtInfo_slotSize), sizeof(char)*getRecord_data->tableMgmtInfo_slotSize);
   
        unpinPage(getRecord_data->bm, getRecord_page_handle);
       		Record *temp_record = deserializeRecord(store_record, rel);
            record->data = temp_record->data;
     // printf("%s",*temp_record->data);
       free(temp_record);
    }
    return RC_OK;
} 

/*int Check_Tombstone(int page_pass,int tomb_slot,tombNode *head, tableMgmtInfo *getRecord_data,RID *id, Record *record)
{
	printf("\n value passed : %d \n",page_pass);
	
	return 1;
}	*/

int Check_Tombstone(int page_pass,int tomb_slot,tombNode *head, tableMgmtInfo *getRecord_data,RID *id, Record *record)
    {
		int temp = getRecord_data->tableMgmtInfo_tNodeLen;
		
		int tuple_position;
		int i=0,tomb_counter=0,indicate_tombstone=0;
		
		//printf("\n Hi before For%d \n",temp); 
		for(i=0;i<temp;i++)
		{
			if (head->id.page != page_pass && head->id.slot != tomb_slot)
			{
				head=head->next;
				tomb_counter = tomb_counter + 1;	
		//		
			}
			
		 else{
			    //printf("\n In For Loop ELSE condition \n");		
			    indicate_tombstone = 1;
		   record->id.indicate_tomb_stone = 1;
                break;
			
			 }
		}
		 
		  
		 if(indicate_tombstone == 1)
		 return 1;
		 
		 else{
			tuple_position = calculate_tuple_position(page_pass,tomb_slot,getRecord_data,tomb_counter);
                //    printf("%d %d",getRecord_data->tableMgmtInfo_numTuples,tuple_position); 
            if (tuple_position > getRecord_data->tableMgmtInfo_numTuples)
            {	        
	       // free(page);	        
            return 0; 
            }
            
            else{
				   return 1;
				}
	      }
}

int calculate_tuple_position(int cal_tomb_page, int cal_tomb_slot, tableMgmtInfo *cal_getRecord_data, int counter)
{    
     int return_result,temp11,temp12;
	temp11 = cal_tomb_page - cal_getRecord_data->tableMgmtInfo_recordStartPage;
     temp12 = temp11 * (cal_getRecord_data->tableMgmtInfo_maxSlots) ;
     return( temp12 + cal_tomb_slot + 1 - counter);	
 }
 
Record_Mgmt_Info *check_id(Record_Mgmt_Info *scan_id)
{

if (scan_id->Record_Mgmt_Info_cslot == scan_id->Record_Mgmt_Info_countslot - 1){
            scan_id->Record_Mgmt_Info_cslot = 0;
            (scan_id->Record_Mgmt_Info_cpage)++;
            return(scan_id);
        }
        else{
            (scan_id->Record_Mgmt_Info_cslot)++;
            return(scan_id);
        }
}
       
Record_Mgmt_Info *check_tombstone_id(Record_Mgmt_Info *scan_node_tomb)
{
	if (scan_node_tomb->Record_Mgmt_Info_cslot == scan_node_tomb->Record_Mgmt_Info_countslot - 1){
            scan_node_tomb->Record_Mgmt_Info_cslot = 0;
            (scan_node_tomb->Record_Mgmt_Info_cpage)++;
            return(scan_node_tomb);
        }
        else{
            (scan_node_tomb->Record_Mgmt_Info_cslot)++;
            return(scan_node_tomb);
        }
	
}
Record_Mgmt_Info *initialize_scan(Record_Mgmt_Info *node,RM_TableData *rel,Expr *condition)//(int Record_Mgmt_Info_countslot, int Record_Mgmt_Info_countpage, int Record_Mgmt_Info_cpage,int Record_Mgmt_Info_cslot,Expr *Record_Mgmt_Info_condition,RM_TableData *rel)
{
	 
	//Record_Mgmt_Info *node = (Record_Mgmt_Info *) malloc(sizeof(Record_Mgmt_Info));  
    node->Record_Mgmt_Info_countslot = ((tableMgmtInfo *)rel->mgmtData)->tableMgmtInfo_maxSlots;
    node->Record_Mgmt_Info_countpage = ((tableMgmtInfo *)rel->mgmtData)->tableMgmtInfo_recordLastPage;
    node->Record_Mgmt_Info_cpage = ((tableMgmtInfo *)rel->mgmtData)->tableMgmtInfo_recordStartPage;
    node->Record_Mgmt_Info_cslot = 0;
    node->Record_Mgmt_Info_condition = condition;
    
    return(node);
} 

extern RC startScan (RM_TableData *rel, RM_ScanHandle *scan, Expr *cond){

	Record_Mgmt_Info *scan_node1;
	/* initialize the RM_ScanHandle data structure */
    scan->rel = rel;

	///* initialize rNode to store the information about the record to be searched and the condition to be evaluated 
    Record_Mgmt_Info *scan_node = (Record_Mgmt_Info *) malloc(sizeof(Record_Mgmt_Info));
 
    scan_node1 = initialize_scan(scan_node,rel,cond);//Record_Mgmt_Info_countslot,Record_Mgmt_Info_countpage,Record_Mgmt_Info_cpage,Record_Mgmt_Info_cslot,Record_Mgmt_Info_condition,con,rel);
    scan->mgmtData = (void *) scan_node1; 

    return RC_OK;
}


extern RC next (RM_ScanHandle *scan, Record *record){

//printf("\n into NEXT \n");
    Record_Mgmt_Info *scan_node,*scan_node1,*scan_node2;
    Value *value;
    scan_node = scan->mgmtData;
    RC return_status;


    record->id.slot = scan_node->Record_Mgmt_Info_cslot;
    record->id.page = scan_node->Record_Mgmt_Info_cpage;

	/* fetch the record as per the page and slot id */
    return_status = getRecord(scan->rel, record->id, record);
     //printf("\n return status : %d",return_status);
    if(return_status == RC_RM_NO_MORE_TUPLES){

        return RC_RM_NO_MORE_TUPLES;
    }

	/* check tombstone id for a deleted record and update record node parameters accordingly */
    else if(record->id.indicate_tomb_stone == 1){
		
		scan_node1=check_tombstone_id(scan_node);
         scan->mgmtData = scan_node1;
        return next(scan, record);
    }
	/* evaluate the given expression to check if the record is the one required by the client */
    else{
        evalExpr(record, scan->rel->schema, scan_node->Record_Mgmt_Info_condition, &value);
        
        scan_node2 = check_id(scan_node);
	
        scan->mgmtData = scan_node2;

	/* If the record fetched is not the required one then call the next function with the updated record id parameters. */
        if(value->v.boolV != 1){
			//printf("\n calling NEXT again as undesired record \n");
            return next(scan, record);
        }
        else{
            return RC_OK;
        }
    }

}
