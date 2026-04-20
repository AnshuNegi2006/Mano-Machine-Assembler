#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <stdbool.h>
#include <limits.h>
#include <ctype.h>
#include <arpa/inet.h>

#define MAX_MEMORY 4096
#define MAX_INSTLABEL 200
#define INP_MIN -32678
#define INP_MAX 32767
#define DIRECT_INP_MIN -2048
#define DIRECT_INP_MAX 2047
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define ADDR_CLA (1<<0)
#define ADDR_CLE (1<<1)
#define ADDR_CMA (1<<2)
#define ADDR_CME (1<<3)
#define ADDR_CIR (1<<4)
#define ADDR_CIL (1<<5)
#define ADDR_INC (1<<6)
#define ADDR_SPA (1<<7)
#define ADDR_SNA (1<<8)
#define ADDR_SZA (1<<9)
#define ADDR_SZE (1<<10)
#define ADDR_HLT (1<<11)
#define ADDR_INP (1<<0)
#define ADDR_OUT (1<<1)
#define ADDR_SKI (1<<2)
#define ADDR_SKO (1<<3)
#define ADDR_ION (1<<4)
#define ADDR_IOF (1<<5)

FILE *sourcePtr;
FILE *exePtr;

#pragma pack(push,1)
// Instructions define :
// 1-Memory Refrence Instructions
struct AND_MRI_X_000{
    uint16_t addressfeild : 12;
    uint16_t opcode : 3;
    uint16_t i : 1;

} AND;

struct ADD_MRI_X_001{
    uint16_t addressfeild : 12;
    uint16_t opcode : 3;
    uint16_t i : 1;

} ADD;

struct LDA_MRI_X_010{
    uint16_t addressfeild : 12;
    uint16_t opcode : 3;
    uint16_t i : 1;

} LDA;

struct STA_MRI_X_011{
    uint16_t addressfeild : 12;
    uint16_t opcode : 3;
    uint16_t i : 1;

} STA;

struct BUN_MRI_X_100{
    uint16_t addressfeild : 12;
    uint16_t opcode : 3;
    uint16_t i : 1;
} BUN;

struct BSA_MRI_X_101{
    uint16_t addressfeild : 12;
    uint16_t opcode : 3;
    uint16_t i : 1;

} BSA;

struct ISZ_MRI_X_110{
    uint16_t addressfeild : 12;
    uint16_t opcode : 3;
    uint16_t i : 1;

} ISZ;

// 2-Register Refrence Instructions
struct CLA_RRI_0_111{
    uint16_t addressfeild : 12;
    uint16_t opcode : 3;
    uint16_t i : 1;

} CLA;

struct CLE_RRI_0_111{
    uint16_t addressfeild : 12;
    uint16_t opcode : 3;
    uint16_t i : 1;

} CLE;

struct CMA_RRI_0_111{
    uint16_t addressfeild : 12;
    uint16_t opcode : 3;
    uint16_t i : 1;

} CMA;

struct CME_RRI_0_111{
    uint16_t addressfeild : 12;
    uint16_t opcode : 3;
    uint16_t i : 1;

} CME;

struct CIR_RRI_0_111{
    uint16_t addressfeild : 12;
    uint16_t opcode : 3;
    uint16_t i : 1;

} CIR;

struct CIL_RRI_0_111{
    uint16_t addressfeild : 12;
    uint16_t opcode : 3;
    uint16_t i : 1;

} CIL;

struct INC_RRI_0_111{
    uint16_t addressfeild : 12;
    uint16_t opcode : 3;
    uint16_t i : 1;

} INC;

struct SPA_RRI_0_111{
    uint16_t addressfeild : 12;
    uint16_t opcode : 3;
    uint16_t i : 1;

} SPA;

struct SNA_RRI_0_111{
    uint16_t addressfeild : 12;
    uint16_t opcode : 3;
    uint16_t i : 1;

} SNA;

struct SZA_RRI_0_111{
    uint16_t addressfeild : 12;
    uint16_t opcode : 3;
    uint16_t i : 1;

} SZA;

struct SZE_RRI_0_111{
    uint16_t addressfeild : 12;
    uint16_t opcode : 3;
    uint16_t i : 1;

} SZE;

struct HLT_RRI_0_111{
    uint16_t addressfeild : 12;
    uint16_t opcode : 3;
    uint16_t i : 1;

} HLT;

// 3- I/O Instructions
struct INP_IOI_1_111{
    uint16_t addressfeild : 12;
    uint16_t opcode : 3;
    uint16_t i : 1;

} INP;

struct OUT_IOI_1_111{
    uint16_t addressfeild : 12;
    uint16_t opcode : 3;
    uint16_t i : 1;

} OUT;

struct SKI_IOI_1_111{
    uint16_t addressfeild : 12;
    uint16_t opcode : 3;
    uint16_t i : 1;

} SKI;

struct SKO_IOI_1_111{
    uint16_t addressfeild : 12;
    uint16_t opcode : 3;
    uint16_t i : 1;

} SKO;

struct ION_IOI_1_111{
    uint16_t addressfeild : 12;
    uint16_t opcode : 3;
    uint16_t i : 1;

} ION;

struct IOF_IOI_1_111{
    uint16_t addressfeild : 12;
    uint16_t opcode : 3;
    uint16_t i : 1;

} IOF;
#pragma pack(pop)


// Lookup Table Structure Define
typedef struct lookupElement{
    char varname[6];
    uint16_t address;
    int16_t value;
} LOOKUPENTRY;
// Instruction Table Structure Define
typedef struct labelElement{
    char name[6];
    uint16_t address;
} LABELENTRY;

// Cleanup mechanism to delete ".bin" file if error occured.
void cleanup(FILE *source, FILE *binary, char *name){
    if (source)
        fclose(source);
    if (binary)
        fclose(binary);
    remove(name);
}

//SyntaxCheck in pass 1.
bool syntaxChecker(char * keyword,char * value,int linecounter){
    int keywordSize=strlen(keyword);
    if(keywordSize==5){
        if(strcmp(keyword,"LABEL")==0)return true;
        else{
            fprintf(stderr,"Error: Illegal syntax, not recognised as label/token.\n Refrence line: %d",linecounter);
            return false;
        }
    }
    if(keywordSize!=3){
        fprintf(stderr,"Error: Illegal syntax, not recognised as label/token.\n Refrence line: %d",linecounter);
        return false;
    }
    int valueFeildCheck=0;
    if(strcmp(keyword,"AND")==0){
        valueFeildCheck=1;
    }
    else if(strcmp(keyword,"ADD")==0){
        valueFeildCheck=1;
        
    }
    else if(strcmp(keyword,"LDA")==0){
        valueFeildCheck=1;
        
    }
    else if(strcmp(keyword,"STA")==0){
        valueFeildCheck=1;
        
    }
    else if(strcmp(keyword,"BUN")==0){
        valueFeildCheck=0;
        
    }
    else if(strcmp(keyword,"BSA")==0){
        valueFeildCheck=0;
        
    }
    else if(strcmp(keyword,"ISZ")==0){
        valueFeildCheck=0;

    }
    else if(strcmp(keyword,"CLA")==0){
        valueFeildCheck=2;
    }
    else if(strcmp(keyword,"CLE")==0){
        valueFeildCheck=2;
        
    }
    else if(strcmp(keyword,"CMA")==0){
        valueFeildCheck=2;
        
    }
    else if(strcmp(keyword,"CME")==0){
        valueFeildCheck=2;
        
    }
    else if(strcmp(keyword,"CIR")==0){
        valueFeildCheck=2;
        
    }
    else if(strcmp(keyword,"CIL")==0){
        valueFeildCheck=2;
        
    }
    else if(strcmp(keyword,"INC")==0){
        valueFeildCheck=2;
        
    }
    else if(strcmp(keyword,"SPA")==0){
        valueFeildCheck=2;
        
    }
    else if(strcmp(keyword,"SNA")==0){
        valueFeildCheck=2;
        
    }
    else if(strcmp(keyword,"SZA")==0){
        valueFeildCheck=2;
        
    }
    else if(strcmp(keyword,"SZE")==0){
        valueFeildCheck=2;
        
    }
    else if(strcmp(keyword,"HLT")==0){
        valueFeildCheck=2;
        
    }
    else if(strcmp(keyword,"INP")==0){
        valueFeildCheck=2;
        
    }
    else if(strcmp(keyword,"OUT")==0){
        valueFeildCheck=2;
        
    }
    else if(strcmp(keyword,"SKI")==0){
        valueFeildCheck=2;
        
    }
    else if(strcmp(keyword,"SKO")==0){
        valueFeildCheck=2;
        
    }
    else if(strcmp(keyword,"ION")==0){
        valueFeildCheck=2;
        
    }
    else if(strcmp(keyword,"IOF")==0){
        valueFeildCheck=2;

    }
    else{
        fprintf(stderr,"Error: Not recognised as any label/token.\n Refrence line: %d.1 ",linecounter);
        return false;
    } 


    if(valueFeildCheck==2){
        if(value!=NULL){
            fprintf(stderr,"Error: Illegal syntax, found value/multiple instructions.\n Refrence line: %d",linecounter);
            return false;
        }
    }
    else if(valueFeildCheck==0){
        char * nanStart;
        int16_t valueExtract;
        if(value!=NULL)valueExtract=(int16_t)strtol(value,&nanStart,10);
        if(value==NULL || nanStart[0]!=value[0]){
            fprintf(stderr,"Error: Illegal syntax, expects argument as address.\n Refrence line: %d",linecounter);
            return false;
        }
    }
    else if(valueFeildCheck==1){
        char * nanStart;
        int16_t valueExtract;
        if(value!=NULL)valueExtract=(int16_t)strtol(value,&nanStart,10);
        if(value==NULL){
            fprintf(stderr,"Error: Illegal syntax, expects argument as address/value.\n Refrence line: %d",linecounter);
            return false;
        }
        else if(nanStart[0]=='\0' && valueExtract>INP_MAX || valueExtract<INP_MIN){
            fprintf(stderr,"Error: Value  out of range of ["STR(INP_MIN)","STR(INP_MAX)"].\n Refrence line: %d",linecounter);
            return false;
        }
        else if(nanStart[0]==value[0]){
            if(strlen(value)>5){
                fprintf(stderr,"Error: Label/Variable name exceeds the max character limit '5'.\n Refrence line: %d",linecounter);
                return false;
            }
            int alphacount;
            for(alphacount=0;alphacount<strlen(value);alphacount++){
                if(isalnum(value[alphacount])==0)break;
            }
            if(alphacount!=strlen(value)){
                fprintf(stderr,"Error: Illegal syntax, identifier expectes alphanumeric character only.\n Refrence line: %d",linecounter);
                return false;
            }
        }
        else{
            return true;
        }
    }
    return true;
}

int main(int argc,char *argv[]){
    if(argc!=2){
        fprintf(stderr, "Error: Missing/Wrong required argument.\nUsage: ./assembler [Filename].sys");
        return 1;
    }

    // Opening Assembley file and seting pointer.(Error Handling: 1-.sysp present 2-File present or not)

    // Checking if at last .sys extention present or not
    char extent[5];
    for(int i=strlen(argv[1])-4,j=0;i<=strlen(argv[1]);i++,j++){
        extent[j]=argv[1][i];
    }
    if(strstr(extent,".sys")==0){
        fprintf(stderr, "Error: Illegal argument, file \".sys\" not found");
        return 1;
    }

    // Opening Source File and setting pointer, checking errno variable value to know if error occured or not.
    errno = 0;
    sourcePtr=fopen(argv[1],"r");
    if(errno!=0){
        fprintf(stderr, "Error: Could not open file '%s'.\n%s", argv[1], strerror(errno));
        errno = 0;
        return 1;
    }
    clearerr(sourcePtr); // Reseting errno value to 0 for tracking next error.

    // Creating name of the binary file according to the source file name.
    int length=strlen(argv[1]) + 1;
    int exelen=length;
    char exename[exelen];
    exename[exelen - 1] = '\0';
    exename[exelen - 2] = 'n';
    exename[exelen - 3] = 'i';
    exename[exelen - 4] = 'b';
    exename[exelen - 5] = '.';
    for(int i=exelen-6;i>=0;i--){
        exename[i]=argv[1][i];
    }

    exePtr=fopen(exename,"wb");
    if (errno!=0){
        fprintf(stderr, "Error : %s", strerror(errno));
    }
    clearerr(exePtr);

    // Initializing Instructions :
    // 1-MRI
    AND.opcode = 0;
    ADD.opcode = 1;
    LDA.opcode = 2;
    STA.opcode = 3;
    BUN.i = 1;
    BUN.opcode = 4;
    BSA.i = 1;
    BSA.opcode = 5;
    ISZ.i = 1;
    ISZ.opcode = 6;
    // 2-RRI
    CLA.i = 0;
    CLA.opcode = 7;
    CLA.addressfeild=ADDR_CLA;
    CLE.i = 0;
    CLE.opcode = 7;
    CLE.addressfeild=ADDR_CLE;
    CMA.i = 0;
    CMA.opcode = 7;
    CMA.addressfeild=ADDR_CMA;
    CME.i = 0;
    CME.opcode = 7;
    CME.addressfeild=ADDR_CME;
    CIR.i = 0;
    CIR.opcode = 7;
    CIR.addressfeild=ADDR_CIR;
    CIL.i = 0;
    CIL.opcode = 7;
    CIL.addressfeild=ADDR_CIL;
    INC.i = 0;
    INC.opcode = 7;
    INC.addressfeild=ADDR_INC;
    SPA.i = 0;
    SPA.opcode = 7;
    SPA.addressfeild=ADDR_SPA;
    SNA.i = 0;
    SNA.opcode = 7;
    SNA.addressfeild=ADDR_SNA;
    SZA.i = 0;
    SZA.opcode = 7;
    SZA.addressfeild=ADDR_SZA;
    SZE.i = 0;
    SZE.opcode = 7;
    SZE.addressfeild=ADDR_SZE;
    HLT.i = 0;
    HLT.opcode = 7;
    HLT.addressfeild=ADDR_HLT;
    // I/O Instructions
    INP.i = 1;
    INP.opcode = 7;
    INP.addressfeild=ADDR_INP;
    OUT.i = 1;
    OUT.opcode = 7;
    OUT.addressfeild=ADDR_OUT;
    SKI.i = 1;
    SKI.opcode = 7;
    SKI.addressfeild=ADDR_SKI;
    SKO.i = 1;
    SKO.opcode = 7;
    SKO.addressfeild=ADDR_SKO;
    ION.i = 1;
    ION.opcode = 7;
    ION.addressfeild=ADDR_ION;
    IOF.i = 1;
    IOF.opcode = 7;
    IOF.addressfeild=ADDR_IOF;

    // Two file pointer created :
    // 1- sourcePtr - Pointer to Source Code file.
    // 2- exePtr - Pointer to Executable file.

    // Pass-1st (Create variable lookup table && struct is its member refrence line-190)
    
    LOOKUPENTRY LookupTable[MAX_MEMORY];
    LABELENTRY LabelTable[MAX_INSTLABEL];
    int LC = 0;   // Location counter(Variable)
    int ILC = 0;  // Instruction location counter(Instruction)
    int IC = 500; // Instruction Counter(Instruction starts from address 500)
    int linecounter = 0;
    char line[256];
    bool isEmptyLookupFlag = 1;
    while (fgets(line,sizeof(line),sourcePtr)){
        // Collecting each source line in a array called line.
        linecounter++;
        if(IC>4095){
            cleanup(sourcePtr,exePtr,exename);
            fprintf(stderr,"Error: Instruction overflow, maximum instruction limit '3596' reached.\n Refrence line: %d",linecounter);
            return 1;
        }
        int allSpace,orignalLineSize=strlen(line);
        for(allSpace=0;allSpace<orignalLineSize;allSpace++){
            if(line[allSpace]!=' ')break;
        }
        if(allSpace==orignalLineSize)continue;
        line[strcspn(line,"\n")]='\0';
        int linesize=strlen(line);
        if (linesize==0)continue;
        for(int checkSeprator=linesize-1;checkSeprator>=0;checkSeprator--){
            if(line[checkSeprator]==';')break;
            else if(line[checkSeprator]==' ')continue;
            else{
                cleanup(sourcePtr, exePtr, exename);
                fprintf(stderr, "Error: expected ';' at the end.\n Reference line:%d", linecounter);
                return 1;
            }
        }
        
        // Label founder & handler.
        //----------------------------------------------------
        
        char labelLineCopy[256];
        strcpy(labelLineCopy,line);
        labelLineCopy[strcspn(line,"\n")]='\0';
        char *labelkeyword=strtok(labelLineCopy,":");
        if (strlen(labelkeyword)!=linesize){
            // 2- If,after delimiting the string result in other value than LABEL then it is an error.
            char labelstr[6];
            for(int i=0;i<=4;i++){
                labelstr[i]=labelkeyword[i];
            }
            labelstr[5]='\0';
            if (strcmp(labelstr,"LABEL")!=0){
                cleanup(sourcePtr,exePtr,exename);
                fprintf(stderr,"Error: '%s' not recognised as any token.\n Reference line:%d.1", labelstr, linecounter);
                return 1;
            }

            // 3- Fetch LabelName && check max limit of label keyword is - '3' characters.
            char *labelname=strtok(NULL,":");
            int wordcount=0,wordfound=0;
            char finallabel[6];
            finallabel[5]='\0';
            int j=0;
            for(int i=0;labelname[i]!='\0';i++){
                // Continue from here...(Label name extracting in- finallabel)
                if(labelname[i]==' '){
                    if (wordfound==1)wordfound=2;
                    continue;
                }
                else if(isalpha(labelname[i])){
                    if(wordfound==2){
                        cleanup(sourcePtr,exePtr,exename);
                        fprintf(stderr, "Error: Expects single Label Name, found multiple.\n Reference line:%d.2", linecounter);
                        return 1;
                    }
                    if(wordcount<5){
                        wordfound=1;
                        wordcount++;
                        finallabel[j]=labelname[i];
                        j++;
                    }
                    else{
                        cleanup(sourcePtr,exePtr,exename);
                        fprintf(stderr, "Error: Label name exceeds expected character limit '5'.\n Reference line:%d.2", linecounter);
                        return 1;
                    }
                }
                else{
                    cleanup(sourcePtr,exePtr,exename);
                    fprintf(stderr, "Error: Expected letters in Label Name, found non-alphabatical characters.\n Reference line:%d", linecounter);
                    return 1;
                }
            }
            if(j==0){
                cleanup(sourcePtr,exePtr,exename);
                fprintf(stderr, "Error: Label Name cannot be empty.\n Reference line:%d", linecounter);
                return 1;
            }
            if(j<3)
                finallabel[j]='\0';

            // 4- So, no need to proceed further break loop.
            char *nextKeyword=strtok(NULL," \t:;");
            if(nextKeyword==NULL){
                cleanup(sourcePtr, exePtr, exename);
                fprintf(stderr, "Error: Label '%s' not followed by an instruction.\n Reference line:%d", finallabel, linecounter);
                return 1;
            }
            int nextKeySize=0;
            nextKeySize=strlen(nextKeyword);
            if(nextKeySize!=3 && nextKeySize!=5){
                cleanup(sourcePtr, exePtr, exename);
                fprintf(stderr, "Error: '%s' not recognised as any keyword or literal.\n Reference line:%d.2", nextKeyword, linecounter);
                return 1;

            }
            else if(nextKeySize==3 && strcmp(nextKeyword,"VAR")==0){
                cleanup(sourcePtr, exePtr, exename);
                fprintf(stderr, "Error: Illegal use of 'VAR' keyword with 'LABEL' statement.\n Reference line:%d.2", linecounter);
                return 1;
            }
            else if (nextKeySize==5 && strcmp(nextKeyword,"LABEL")==0){
                cleanup(sourcePtr, exePtr, exename);
                fprintf(stderr, "Error: Illegal use of 'LABEL' keyword with 'LABEL' statement.\n Reference line:%d.2", linecounter);
                return 1;
            }
            
            //-------------------------------------------
            // 1-LABEL labelKeyword
            // 2- Label name - finallabel
            //-------------------------------------------
            for(int i=0;i<ILC;i++){
                if(strlen(LabelTable[i].name)==strlen(finallabel) && strcmp(LabelTable[i].name,finallabel)==0){
                    cleanup(sourcePtr, exePtr, exename);
                    fprintf(stderr, "Error: Duplicate label name found.\n Reference line:%d.2", linecounter);
                    return 1;
                }
            }
            if(ILC>200){
                cleanup(sourcePtr, exePtr, exename);
                fprintf(stderr, "Error: Label Overflow, maximum label limit '200' reached.\n Reference line:%d.2", linecounter);
                return 1;
            }
            strcpy(LabelTable[ILC].name, finallabel);
            LabelTable[ILC].address = IC;
            ILC++;
            IC++;
            continue;
        }
        
        //----------------------------------------------------
        
        IC++;

        //Var founder & handler.
        //----------------------------------------------------

        char varLineCopy[256];
        strcpy(varLineCopy,line);
        char *keyword=strtok(varLineCopy," :\t;");
        if(keyword==NULL)continue;
        // Identify if VAR keyword is found or not.
        if(strlen(keyword)==3 && strcmp(keyword,"VAR")==0){
            // Robust Invalid keyword length trapping, checking if ";" present or not.
            if(strlen(keyword)!=3 && strlen(keyword)!=5){
                cleanup(sourcePtr, exePtr, exename);
                fprintf(stderr, "Error: Not identified as keyword error.\n Reference line:%d.1", linecounter);
                return 1;
            }
            
            // Get Identifier & valid variable_name length check.
            char*identifier=strtok(NULL," \t:;");
            if(identifier==NULL){
                cleanup(sourcePtr, exePtr, exename);
                fprintf(stderr, "Error: No identifier found.\n Reference line:%d.2", linecounter);
                return 1;
            }
            if(strlen(identifier)>5){
                cleanup(sourcePtr, exePtr, exename);
                fprintf(stderr, "Error: Variable name exceeds expected character limit '5'.\n Reference line:%d.2", linecounter);
                return 1;
            }
            for(int i=0;i<LC;i++){
                if(strcmp(LookupTable[i].varname,identifier)==0){
                    cleanup(sourcePtr, exePtr, exename);
                    fprintf(stderr, "Error: Duplicate variable name '%s' found.\n Reference line%d.2", identifier, linecounter);
                    return 1;
                }
            }
            
            char *valueptr=strtok(NULL," ;");
            // Check if syntax : "VAR variable_name value;" is voilated or not.
            if(strtok(NULL," ;\t:")!=NULL){
                cleanup(sourcePtr, exePtr, exename);
                fprintf(stderr, "Error: Invalid syntax, expected 'VAR {variableName} {value}; or multiple instruction found in one line.'\n Reference line:%d.3", linecounter);
                return 1;
            }
            
            // Value extraction and robust error handling.
            char *errorvalPtr;
            long tempvalue=strtol(valueptr,&errorvalPtr,10);
            if(*errorvalPtr!='\0'){
                cleanup(sourcePtr, exePtr, exename);
                fprintf(stderr, "Error: Expected integer value, found non-numeric value '%c'.\n Reference line:%d.3", *errorvalPtr, linecounter);
                return 1;
            }
            if(valueptr==errorvalPtr){
                cleanup(sourcePtr, exePtr, exename);
                fprintf(stderr, "Error: Expected integer value, no value found.\n Reference line:%d.3", linecounter);
                return 1;
            }
            if(!(tempvalue>=INT16_MIN && tempvalue<=INT16_MAX)){
                cleanup(sourcePtr, exePtr, exename);
                fprintf(stderr, "Error: Value exceeded integer range ["STR(INP_MIN) ","STR(INP_MAX)"].\n Reference line:%d.3", linecounter);
                return 1;
            }
            int16_t value=(int16_t)tempvalue;
            
            //---------------------------------------------------------------
            // Instruction: VAR var_name value; decomposed and extracted
            // var_name : identifier , Value : value
            // In LookupTable 3-Feilds : varname,address,value
            //---------------------------------------------------------------
            if(LC>=500){
                cleanup(sourcePtr, exePtr, exename);
                fprintf(stderr, "Error: Memory Overflow, reached max variable limit '500'.\n Reference line:%d.1", linecounter);
                return 1;
            }
            strcpy(LookupTable[LC].varname, identifier);
            LookupTable[LC].address = LC;
            LookupTable[LC].value = value;
            isEmptyLookupFlag = 0;
            LC++;
            IC--;
            continue;
        }
        
        //----------------------------------------------------


        //If no Label no Var. Syntax check:
        //----------------------------------------------------
        
        keyword=strtok(line," \t;");
        char *value=strtok(NULL," \t;");
        char *extra=strtok(NULL," \t;");
        if(extra!=NULL){
            cleanup(sourcePtr,exePtr,exename);
            fprintf(stderr,"Error: Invalid syntax, expect only one valid instruction per line.\n Refrence line: %d",linecounter);
            return 1;
        }
        if(syntaxChecker(keyword,value,linecounter)==false){
            cleanup(sourcePtr,exePtr,exename);
            return 1;
        }
        
        //----------------------------------------------------
    }

    rewind(sourcePtr);
    clearerr(sourcePtr);
    clearerr(exePtr);
    linecounter = 0;
    
    // 2nd Pass- Instruction to binary.
    //Use this to correctly map Binary(Ignoring Bit flipping and optimisation done by Morden Systems.)
    //Write all the LookupTable variable in the file
    for(int i=0;i<LC;i++){
        uint16_t rawBits=*(uint16_t *)&LookupTable[i].value;
        uint16_t finalBits=htons(rawBits);
        fwrite(&finalBits,sizeof(finalBits),1,exePtr);
    }
    fseek(exePtr, 500 * 2, SEEK_SET);

    while(fgets(line, sizeof(line),sourcePtr)){
        linecounter++;
        line[strcspn(line,"\n")]='\0';
        if(line[0]=='\0')continue;
        int allCount,orignalLineSize=strlen(line);
        for(allCount=0;allCount<orignalLineSize;allCount++){
            if(line[allCount]==' ' || line[allCount]==';')allCount++;
            else break;
        }
        if(allCount==orignalLineSize+1)continue;
        

        char *keyword=strtok(line," :\n\t;");
        //For LABEL/VAR - Continue
        if(strlen(keyword)==5){
            if(strcmp(keyword,"LABEL")==0){
                strtok(NULL," :\t;");
                keyword=strtok(NULL," :\n\t;");
            }
            else{
                cleanup(sourcePtr,exePtr,exename);
                fprintf(stderr,"Error: Illegal instruction trapped.\n Reference line: %d",linecounter);
                return 1;
            }
        }
        if(strlen(keyword)==3 && strcmp(keyword,"VAR")==0)continue;

        //Instruction Finder
        int instructionNumber=0;
        if(strlen(keyword)==3){
            if(strcmp(keyword,"AND")==0){
                instructionNumber=1;
            }
            else if(strcmp(keyword,"ADD")==0){
                instructionNumber=2;
            }
            else if(strcmp(keyword,"LDA")==0){
                instructionNumber=3;
            }
            else if(strcmp(keyword,"STA")==0){
                instructionNumber=4;
            }
            else if(strcmp(keyword,"BUN")==0){
                instructionNumber=5;
            }
            else if(strcmp(keyword,"BSA")==0){
                instructionNumber=6;
            }
            else if(strcmp(keyword,"ISZ")==0){
                instructionNumber=7;
            }
            else if(strcmp(keyword,"CLA")==0){
                instructionNumber=8;
            }
            else if(strcmp(keyword,"CLE")==0){
                instructionNumber=9;
            }
            else if(strcmp(keyword,"CMA")==0){
                instructionNumber=10;
            }
            else if(strcmp(keyword,"CME")==0){
                instructionNumber=11;
            }
            else if(strcmp(keyword,"CIR")==0){
                instructionNumber=12;
            }
            else if(strcmp(keyword,"CIL")==0){
                instructionNumber=13;
            }
            else if(strcmp(keyword,"INC")==0){
                instructionNumber=14;
            }
            else if(strcmp(keyword,"SPA")==0){
                instructionNumber=15;
            }
            else if(strcmp(keyword,"SNA")==0){
                instructionNumber=16;
            }
            else if(strcmp(keyword,"SZA")==0){
                instructionNumber=17;
            }
            else if(strcmp(keyword,"SZE")==0){
                instructionNumber=18;
            }
            else if(strcmp(keyword,"HLT")==0){
                instructionNumber=19;
            }
            else if(strcmp(keyword,"INP")==0){
                instructionNumber=20;
            }
            else if(strcmp(keyword,"OUT")==0){
                instructionNumber=21;
            }
            else if(strcmp(keyword,"SKI")==0){
                instructionNumber=22;
            }
            else if(strcmp(keyword,"SKO")==0){
                instructionNumber=23;
            }
            else if(strcmp(keyword,"ION")==0){
                instructionNumber=24;
            }
            else if(strcmp(keyword,"IOF")==0){
                instructionNumber=25;
            }
            else{
                instructionNumber=0;
            }
        }
        
        char *value=strtok(NULL," ;:\t");
        if(syntaxChecker(keyword,value,linecounter)==false){
            cleanup(sourcePtr,exePtr,exename);
            return 1;
        }
        bool addrFlag=false;
        int16_t operandValue;
        if(value!=NULL){

            char * nanStart;
            long tempValue=strtol(value,&nanStart,10);
            if(nanStart[0]=='\0')addrFlag=false;
            else if(nanStart[0]==value[0]) addrFlag=true;
            if(addrFlag==false && (tempValue>DIRECT_INP_MAX ||tempValue<DIRECT_INP_MIN)){
                cleanup(sourcePtr,exePtr,exename);
                fprintf(stderr,"Error: Direct Operand Value  out of range ["STR(DIRECT_INP_MIN)","STR(DIRECT_INP_MAX)"].\n Refrence line: %d",linecounter);
                return 1;
            }
            operandValue=(int16_t)tempValue;
        }
        switch(instructionNumber){
            case 1:{
                if(addrFlag==true){
                    AND.i=1;
                    int index;
                    for(index=0;index<LC;index++){
                        if(strlen(value)==strlen(LookupTable[index].varname) && strcmp(value,LookupTable[index].varname)==0){
                            AND.addressfeild=LookupTable[index].address;
                            break;
                        }
                    }
                    if(index==LC){
                        cleanup(sourcePtr,exePtr,exename);
                        fprintf(stderr,"Error: Unrecognised variable name found.\n Refrence line: %d",linecounter);
                        return 1;
                    }

                }
                else{
                    AND.i=0;
                    AND.addressfeild=operandValue;
                }
                uint16_t rawBits=*(uint16_t *)&AND;
                uint16_t bigEndian=htons(rawBits);
                if(fwrite(&bigEndian,sizeof(bigEndian),1,exePtr)!=1){
                    cleanup(sourcePtr,exePtr,exename);
                    fprintf(stderr,"Error: Disk write failed.\n Refrence line: %d",linecounter);
                    return 1;
                };
                break;
            }
            case 2:{
                if(addrFlag==true){
                    ADD.i=1;
                    int index;
                    for(index=0;index<LC;index++){
                        if(strlen(value)==strlen(LookupTable[index].varname) && strcmp(value,LookupTable[index].varname)==0){
                            ADD.addressfeild=LookupTable[index].address;
                            break;
                        }
                    }
                    if(index==LC){
                        cleanup(sourcePtr,exePtr,exename);
                        fprintf(stderr,"Error: Unrecognised variable name found.\n Refrence line: %d",linecounter);
                        return 1;
                    }

                }
                else{
                    ADD.i=0;
                    ADD.addressfeild=operandValue;
                }
                uint16_t rawBits=*(uint16_t *)&ADD;
                uint16_t bigEndian=htons(rawBits);
                if(fwrite(&bigEndian,sizeof(bigEndian),1,exePtr)!=1){
                    cleanup(sourcePtr,exePtr,exename);
                    fprintf(stderr,"Error: Disk write failed.\n Refrence line: %d",linecounter);
                    return 1;
                };
                break;
            }
            case 3:{
                if(addrFlag==true){
                    LDA.i=1;
                    int index;
                    for(index=0;index<LC;index++){
                        if(strlen(value)==strlen(LookupTable[index].varname) && strcmp(value,LookupTable[index].varname)==0){
                            LDA.addressfeild=LookupTable[index].address;
                            break;
                        }
                    }
                    if(index==LC){
                        cleanup(sourcePtr,exePtr,exename);
                        fprintf(stderr,"Error: Unrecognised variable name found.\n Refrence line: %d",linecounter);
                        return 1;
                    }

                }
                else{
                    LDA.i=0;
                    LDA.addressfeild=operandValue;
                }
                uint16_t rawBits=*(uint16_t *)&LDA;
                uint16_t bigEndian=htons(rawBits);
                if(fwrite(&bigEndian,sizeof(bigEndian),1,exePtr)!=1){
                    cleanup(sourcePtr,exePtr,exename);
                    fprintf(stderr,"Error: Disk write failed.\n Refrence line: %d",linecounter);
                    return 1;
                };
                break;
            }
            case 4:{
                if(addrFlag==true){
                    STA.i=1;
                    int index;
                    for(index=0;index<LC;index++){
                        if(strlen(value)==strlen(LookupTable[index].varname) && strcmp(value,LookupTable[index].varname)==0){
                            STA.addressfeild=LookupTable[index].address;
                            break;
                        }
                    }
                    if(index==LC){
                        cleanup(sourcePtr,exePtr,exename);
                        fprintf(stderr,"Error: Unrecognised variable name found.\n Refrence line: %d",linecounter);
                        return 1;
                    }

                }
                else{
                    cleanup(sourcePtr,exePtr,exename);
                    fprintf(stderr,"Error: Illegal syntax, direct value not permitted in variable feild.\n Refrence line: %d",linecounter);
                    return 1;
                }
                uint16_t rawBits=*(uint16_t *)&STA;
                uint16_t bigEndian=htons(rawBits);
                if(fwrite(&bigEndian,sizeof(bigEndian),1,exePtr)!=1){
                    cleanup(sourcePtr,exePtr,exename);
                    fprintf(stderr,"Error: Disk write failed.\n Refrence line: %d",linecounter);
                    return 1;
                };
                break;
            }
            case 5:{
                if(addrFlag==true){
                    BUN.i=1;
                    int index;
                    for(index=0;index<ILC;index++){
                        if(strlen(value)==strlen(LabelTable[index].name) && strcmp(value,LabelTable[index].name)==0){
                            BUN.addressfeild=LabelTable[index].address;
                            break;
                        }
                    }
                    if(index==ILC){
                        cleanup(sourcePtr,exePtr,exename);
                        fprintf(stderr,"Error: Unrecognised label name found.\n Refrence line: %d",linecounter);
                        return 1;
                    }
                    
                }
                else{
                    cleanup(sourcePtr,exePtr,exename);
                    fprintf(stderr,"Error: Illegal syntax, direct address not permitted in label name.\n Refrence line: %d",linecounter);
                    return 1;   
                }
                uint16_t rawBits=*(uint16_t *)&BUN;
                uint16_t bigEndian=htons(rawBits);
                if(fwrite(&bigEndian,sizeof(bigEndian),1,exePtr)!=1){
                    cleanup(sourcePtr,exePtr,exename);
                    fprintf(stderr,"Error: Disk write failed.\n Refrence line: %d",linecounter);
                    return 1;
                };
                break;
            }
            case 6:{
                if(addrFlag==true){
                    BSA.i=1;
                    int index;
                    for(index=0;index<ILC;index++){
                        if(strlen(value)==strlen(LabelTable[index].name) && strcmp(value,LabelTable[index].name)==0){
                            BSA.addressfeild=LabelTable[index].address;
                            break;
                        }
                    }
                    if(index==ILC){
                        cleanup(sourcePtr,exePtr,exename);
                        fprintf(stderr,"Error: Unrecognised label name found.\n Refrence line: %d",linecounter);
                        return 1;
                    }
                    
                }
                else{
                    cleanup(sourcePtr,exePtr,exename);
                    fprintf(stderr,"Error: Illegal syntax, direct address not permitted in label feild.\n Refrence line: %d",linecounter);
                    return 1;   
                }
                uint16_t rawBits=*(uint16_t *)&BSA;
                uint16_t bigEndian=htons(rawBits);
                if(fwrite(&bigEndian,sizeof(bigEndian),1,exePtr)!=1){
                    cleanup(sourcePtr,exePtr,exename);
                    fprintf(stderr,"Error: Disk write failed.\n Refrence line: %d",linecounter);
                    return 1;
                };
                break;
            }
            case 7:{
                if(addrFlag==true){
                    ISZ.i=1;
                    int index;
                    for(index=0;index<LC;index++){
                        if(strlen(value)==strlen(LookupTable[index].varname) && strcmp(value,LookupTable[index].varname)==0){
                            ISZ.addressfeild=LookupTable[index].address;
                            break;
                        }
                    }
                    if(index==LC){
                        cleanup(sourcePtr,exePtr,exename);
                        fprintf(stderr,"Error: Unrecognised variable name found.\n Refrence line: %d",linecounter);
                        return 1;
                    }

                }
                else{
                    cleanup(sourcePtr,exePtr,exename);
                    fprintf(stderr,"Error: Illegal syntax, direct value not permitted in variable feild.\n Refrence line: %d",linecounter);
                    return 1; 
                }
                uint16_t rawBits=*(uint16_t *)&ISZ;
                uint16_t bigEndian=htons(rawBits);
                if(fwrite(&bigEndian,sizeof(bigEndian),1,exePtr)!=1){
                    cleanup(sourcePtr,exePtr,exename);
                    fprintf(stderr,"Error: Disk write failed.\n Refrence line: %d",linecounter);
                    return 1;
                };
                break;
            }
            case 8:{
                uint16_t rawBits=*(uint16_t *)&CLA;
                uint16_t bigEndian=htons(rawBits);
                if(fwrite(&bigEndian,sizeof(bigEndian),1,exePtr)!=1){
                    cleanup(sourcePtr,exePtr,exename);
                    fprintf(stderr,"Error: Disk write failed.\n Refrence line: %d",linecounter);
                    return 1;
                };
                break;
            }
            case 9:{
                uint16_t rawBits=*(uint16_t *)&CLE;
                uint16_t bigEndian=htons(rawBits);
                if(fwrite(&bigEndian,sizeof(bigEndian),1,exePtr)!=1){
                    cleanup(sourcePtr,exePtr,exename);
                    fprintf(stderr,"Error: Disk write failed.\n Refrence line: %d",linecounter);
                    return 1;
                };
                break;
            }
            case 10:{
                uint16_t rawBits=*(uint16_t *)&CMA;
                uint16_t bigEndian=htons(rawBits);
                if(fwrite(&bigEndian,sizeof(bigEndian),1,exePtr)!=1){
                    cleanup(sourcePtr,exePtr,exename);
                    fprintf(stderr,"Error: Disk write failed.\n Refrence line: %d",linecounter);
                    return 1;
                };
                break;
            }
            case 11:{
                uint16_t rawBits=*(uint16_t *)&CME;
                uint16_t bigEndian=htons(rawBits);
                if(fwrite(&bigEndian,sizeof(bigEndian),1,exePtr)!=1){
                    cleanup(sourcePtr,exePtr,exename);
                    fprintf(stderr,"Error: Disk write failed.\n Refrence line: %d",linecounter);
                    return 1;
                };
                break;
            }
            case 12:{
                uint16_t rawBits=*(uint16_t *)&CIR;
                uint16_t bigEndian=htons(rawBits);
                if(fwrite(&bigEndian,sizeof(bigEndian),1,exePtr)!=1){
                    cleanup(sourcePtr,exePtr,exename);
                    fprintf(stderr,"Error: Disk write failed.\n Refrence line: %d",linecounter);
                    return 1;
                };
                break;
            }
            case 13:{
                uint16_t rawBits=*(uint16_t *)&CIL;
                uint16_t bigEndian=htons(rawBits);
                if(fwrite(&bigEndian,sizeof(bigEndian),1,exePtr)!=1){
                    cleanup(sourcePtr,exePtr,exename);
                    fprintf(stderr,"Error: Disk write failed.\n Refrence line: %d",linecounter);
                    return 1;
                };
                break;
            }
            case 14:{
                uint16_t rawBits=*(uint16_t *)&INC;
                uint16_t bigEndian=htons(rawBits);
                if(fwrite(&bigEndian,sizeof(bigEndian),1,exePtr)!=1){
                    cleanup(sourcePtr,exePtr,exename);
                    fprintf(stderr,"Error: Disk write failed.\n Refrence line: %d",linecounter);
                    return 1;
                };
                break;
            }
            case 15:{
                uint16_t rawBits=*(uint16_t *)&SPA;
                uint16_t bigEndian=htons(rawBits);
                if(fwrite(&bigEndian,sizeof(bigEndian),1,exePtr)!=1){
                    cleanup(sourcePtr,exePtr,exename);
                    fprintf(stderr,"Error: Disk write failed.\n Refrence line: %d",linecounter);
                    return 1;
                };
                break;
            }
            case 16:{
                uint16_t rawBits=*(uint16_t *)&SNA;
                uint16_t bigEndian=htons(rawBits);
                if(fwrite(&bigEndian,sizeof(bigEndian),1,exePtr)!=1){
                    cleanup(sourcePtr,exePtr,exename);
                    fprintf(stderr,"Error: Disk write failed.\n Refrence line: %d",linecounter);
                    return 1;
                };
                break;
            }
            case 17:{
                uint16_t rawBits=*(uint16_t *)&SZA;
                uint16_t bigEndian=htons(rawBits);
                if(fwrite(&bigEndian,sizeof(bigEndian),1,exePtr)!=1){
                    cleanup(sourcePtr,exePtr,exename);
                    fprintf(stderr,"Error: Disk write failed.\n Refrence line: %d",linecounter);
                    return 1;
                };
                break;
            }
            case 18:{
                uint16_t rawBits=*(uint16_t *)&SZE;
                uint16_t bigEndian=htons(rawBits);
                if(fwrite(&bigEndian,sizeof(bigEndian),1,exePtr)!=1){
                    cleanup(sourcePtr,exePtr,exename);
                    fprintf(stderr,"Error: Disk write failed.\n Refrence line: %d",linecounter);
                    return 1;
                };
                break;
            }
            case 19:{
                uint16_t rawBits=*(uint16_t *)&HLT;
                uint16_t bigEndian=htons(rawBits);
                if(fwrite(&bigEndian,sizeof(bigEndian),1,exePtr)!=1){
                    cleanup(sourcePtr,exePtr,exename);
                    fprintf(stderr,"Error: Disk write failed.\n Refrence line: %d",linecounter);
                    return 1;
                };
                break;
            }
            case 20:{
                uint16_t rawBits=*(uint16_t *)&INP;
                uint16_t bigEndian=htons(rawBits);
                if(fwrite(&bigEndian,sizeof(bigEndian),1,exePtr)!=1){
                    cleanup(sourcePtr,exePtr,exename);
                    fprintf(stderr,"Error: Disk write failed.\n Refrence line: %d",linecounter);
                    return 1;
                };
                break;
            }
            case 21:{
                uint16_t rawBits=*(uint16_t *)&OUT;
                uint16_t bigEndian=htons(rawBits);
                if(fwrite(&bigEndian,sizeof(bigEndian),1,exePtr)!=1){
                    cleanup(sourcePtr,exePtr,exename);
                    fprintf(stderr,"Error: Disk write failed.\n Refrence line: %d",linecounter);
                    return 1;
                };
                break;
            }
            case 22:{
                uint16_t rawBits=*(uint16_t *)&SKI;
                uint16_t bigEndian=htons(rawBits);
                if(fwrite(&bigEndian,sizeof(bigEndian),1,exePtr)!=1){
                    cleanup(sourcePtr,exePtr,exename);
                    fprintf(stderr,"Error: Disk write failed.\n Refrence line: %d",linecounter);
                    return 1;
                };
                break;
            }
            case 23:{
                uint16_t rawBits=*(uint16_t *)&SKO;
                uint16_t bigEndian=htons(rawBits);
                if(fwrite(&bigEndian,sizeof(bigEndian),1,exePtr)!=1){
                    cleanup(sourcePtr,exePtr,exename);
                    fprintf(stderr,"Error: Disk write failed.\n Refrence line: %d",linecounter);
                    return 1;
                };
                break;
            }
            case 24:{
                uint16_t rawBits=*(uint16_t *)&ION;
                uint16_t bigEndian=htons(rawBits);
                if(fwrite(&bigEndian,sizeof(bigEndian),1,exePtr)!=1){
                    cleanup(sourcePtr,exePtr,exename);
                    fprintf(stderr,"Error: Disk write failed.\n Refrence line: %d",linecounter);
                    return 1;
                };
                break;
            }
            case 25:{
                uint16_t rawBits=*(uint16_t *)&IOF;
                uint16_t bigEndian=htons(rawBits);
                if(fwrite(&bigEndian,sizeof(bigEndian),1,exePtr)!=1){
                    cleanup(sourcePtr,exePtr,exename);
                    fprintf(stderr,"Error: Disk write failed.\n Refrence line: %d",linecounter);
                    return 1;
                };
                break;
            }
        }
    
    }
    return 0;
}

//Had great time & learning building this - 😁