
#include "sxass.h"

char* ReadLabel(){
	j = 0;//zeroing
	while (Buffer[Index] != ' ' && Buffer[Index] != '\t' && Buffer[Index] != '\n')
		Label[j++] = Buffer[Index++];
	Label[j] = '\0';
	return(Label);
}

void SkipSpace(){
	while (Buffer[Index] == ' ' || Buffer[Index] =='\t')
		Index++;
}

char* ReadOprator(){
	j = 0;//zeroing
	while(Buffer[Index] != ' ' && Buffer[Index] != '\t' && Buffer[Index] != '\n')
		Mnemonic[j++] = Buffer[Index++];
	Mnemonic[j] = '\0';
	return(Mnemonic);
}

char* ReadOperand(){
	j = 0;//zeroing
	while(Buffer[Index] != ' ' && Buffer[Index] != '\t' && Buffer[Index] != '\n')
		Operand[j++] = Buffer[Index++];
	Operand[j] = '\0';
	return(Operand);
}

void RecordSymtab(char* label){
	strcpy(SYMTAB[SymtabCounter].Label,label);
	SYMTAB[SymtabCounter].Address = LOCCTR[LocctrCounter-1];
	SymtabCounter++;	
}

int SearchSymtab(char* label)
{

	FoundOnSymtab_flag = 0;
	int k=0;

	for (k=0; k<=SymtabCounter; k++)	{
		if (!strcmp(SYMTAB[k].Label,label)){
			FoundOnSymtab_flag = 1;
			return (FoundOnSymtab_flag);
			break;
		}
	}
	return (FoundOnSymtab_flag);
}

int SearchOptab(char * Mnemonic)
{
	int size = sizeof(OPTAB)/sizeof(SIC_OPTAB);
	FoundOnOptab_flag = 0;

	int i=0;

	printf("foundint: %s, %d\n", Mnemonic, strlen(Mnemonic));

	for(i=0;i<size;i++){
		if(!strcmp(Mnemonic, OPTAB[i].Mnemonic))
		{
			Counter = i;
			FoundOnOptab_flag = 1;
			break;
		}
	}

	return (FoundOnOptab_flag);
}

int StrToDec(char* c){
	int dec_num = 0;
	char temp[10];
	strcpy(temp,c);
	int len = strlen(c);
	int k,l;

	for (k = len-1, l = 1; k>=0; k--)
	{
		dec_num = dec_num+ (int) (temp[k]-'0')*l;
		l = l*10;
	}
	return (dec_num);
}

int StrToHex(char* c)
{
	int hex_num = 0;
	char temp[10];
	strcpy(temp, c);
	int len = strlen(temp);
	int k, l;

	for (k = len-1, l = 1; k >=0; k--)
	{
		if (temp[k]>='0' && temp[k]<='9')
			hex_num = hex_num + (int) (temp[k]-'0')*l;
		else if (temp[k]>='A' && temp[k]<='F')
            hex_num = hex_num + (int) (temp[k]-'A'+10)*l;
		else if (temp[k]>='a' && temp[k]>='f')
            hex_num = hex_num + (int) (temp[k]-'a'+10)*l;
		else ;
		l = l*16;
	}
	return (hex_num);
}

int ComputeLen(char* c){
	unsigned int b;
	char len[32];
	
	strcpy(len,c);
	if (len[0] =='C' || len[0] =='c' && len[1] =='\''){
		for (b = 2; b<=strlen(len); b++){
			if (len[b] == '\''){
				b -=2;
				break;
			}
		}
	}
	if (len[0] =='X' || len[0] =='x' && len[1] =='\'')
		b = 1;
	return (b);
}
void CreateProgramList(){
	int loop;
	FILE *fptr_list; 
	fptr_list = fopen("sic.list","w");
    if (fptr_list == NULL)
	{
		printf("ERROE: Unable to open the sic.list.\n");
		exit(1);
	}
	fprintf(fptr_list, "%-4s\t%-10s%-10s%-10s\t%s\n", "LOC", "LABEL", "OPERATOR", "OPERAND", "OBJECT CODE");
	for (loop = 0; loop<ArrayIndex; loop++)
	{
		if (!strcmp(IMRArray[loop]->OperatorField,"START") || !strcmp(IMRArray[loop]->OperatorField,"RESW") || !strcmp(IMRArray[loop]->OperatorField,"RESB") || !strcmp(IMRArray[loop]->OperatorField,"END"))
		    fprintf(fptr_list, "%04x\t%-10s%-10s%-10s\n", IMRArray[loop]->Loc, IMRArray[loop]->LabelField, IMRArray[loop]->OperatorField, IMRArray[loop]->OperandField);
        else
		    fprintf(fptr_list, "%04x\t%-10s%-10s%-10s\t%06x\n", IMRArray[loop]->Loc, IMRArray[loop]->LabelField, IMRArray[loop]->OperatorField, IMRArray[loop]->OperandField, IMRArray[loop]->ObjectCode);
	}
	fclose(fptr_list);
}
void CreateObjectCode(){
	int first_address;
	int last_address;
	int temp_address;
	int temp_objectcode[30];
	int first_index;
	int last_index;
	int x,xx;
	int loop;
	char temp_operator[12][10];
	char temp_operand[12][10];
	FILE *fptr_obj;    
	fptr_obj = fopen("sic.obj","w");
    if (fptr_obj == NULL)
	{
		printf("ERROE: Unable to open the sic.obj.\n");
		exit(1);
	}
	printf("Creating Object Code...\n\n");
	loop = 0;
	if (!strcmp(IMRArray[loop]->OperatorField, "START"))
	{
		printf("H%-6s%06x%06x\n",IMRArray[loop]->LabelField, start_address, program_length);
		fprintf(fptr_obj,"H^%-6s^%06x^%06x\n",IMRArray[loop]->LabelField, start_address, program_length);
		loop++;
	}
	while(1)
	{
		first_address = IMRArray[loop]->Loc;
		last_address = IMRArray[loop]->Loc + 27;
		first_index = loop;
		for (x = 0, temp_address = first_address ; temp_address<=last_address; loop++)
		{
			if (!strcmp(IMRArray[loop]->OperatorField, "END"))
				break;
			else if (strcmp(IMRArray[loop]->OperatorField, "RESB") && strcmp(IMRArray[loop]->OperatorField, "RESW"))
			{
				temp_objectcode[x] = IMRArray[loop]->ObjectCode;
				strcpy(temp_operator[x], IMRArray[loop]->OperatorField);
				strcpy(temp_operand[x], IMRArray[loop]->OperandField);
				last_index = loop+1;
			    x++;
			}
			else ;
			temp_address = IMRArray[loop+1]->Loc;
		}
		printf("T%06x%02x", first_address, (IMRArray[last_index]->Loc - IMRArray[first_index]->Loc));
		fprintf(fptr_obj, "T^%06x^%02x", first_address, (IMRArray[last_index]->Loc - IMRArray[first_index]->Loc));
		for (xx = 0; xx<x; xx++)
		{
			if ((strcmp(temp_operator[xx], "BYTE")==0) && (temp_operand[xx][0]=='X' || temp_operand[xx][0]=='x')){
			printf("%02x", temp_objectcode[xx]);
			fprintf(fptr_obj, "^%02x", temp_objectcode[xx]);
			}
			else{
				printf("%06x", temp_objectcode[xx]);
			    fprintf(fptr_obj, "^%06x", temp_objectcode[xx]);
			}
		}
		printf("\n");
		fprintf(fptr_obj,"\n");
		if (!strcmp(IMRArray[loop]->OperatorField, "END"))
			break;
	}
	printf("E%06x\n\n", start_address);
	fprintf(fptr_obj, "E^%06x\n\n", start_address);
	fclose(fptr_obj);
}

void main (int argc, char** argv)
{
	FILE* fptr;
	char filename[10];
	char label[32], opcode[32], operand[32];
	int loc = 0;
	int line = 0;
	int loop;
	int is_empty_line;
	int is_comment;
	int loader_flag = 0;
	int search_symtab, x;
	int xe = 0;

	if (argc ==2)
		strcpy(filename, argv[1]);
	else if (argc == 3 && strcmp(argv[1], "xe") == 0)
	{
		strcpy(filename, argv[2]);
		xe=1;
	}
	else
	{
		printf("\n./sxass [xe] filename");
		exit(1);
	}

	fptr = fopen(filename,"r");
	if (fptr == NULL)
	{
		printf("ERROE: Unable to open the %s file.\n",filename);
		exit(1);
	}

	while (fgets(Buffer,256,fptr) != NULL)
	{
		is_empty_line = strlen(Buffer);
		Index = 0;
		j = 0;
		strcpy(label,ReadLabel());

        	if (Label[0] == '.')
			is_comment = 1;
		else
			is_comment = 0;

		if (is_empty_line>1 && is_comment!=1)
		{
			Index = 0;
			j = 0;
		
			IMRArray[ArrayIndex] = (IntermediateRec*)malloc(sizeof(IntermediateRec));/* [A] */
			
			IMRArray[ArrayIndex]->LineIndex = ArrayIndex;
			strcpy(label,ReadLabel());
			strcpy(IMRArray[ArrayIndex]->LabelField,label);
			SkipSpace();
			if (line == 0)
			{
				strcpy(opcode,ReadOprator());
				strcpy(IMRArray[ArrayIndex]->OperatorField,opcode);/* [A] */
				if (!strcmp(opcode,"START"))
				{
					SkipSpace();
					strcpy(operand,ReadOperand());
					strcpy(IMRArray[ArrayIndex]->OperandField, operand);/* [A] */
					LOCCTR[LocctrCounter] = StrToHex(operand);
					start_address = LOCCTR[LocctrCounter];
				}
				else
				{
					LOCCTR[LocctrCounter] = 0;
					start_address = LOCCTR[LocctrCounter];
				}
			}
			else
			{
				strcpy(opcode, ReadOprator());
				strcpy(IMRArray[ArrayIndex]->OperatorField,opcode);
				SkipSpace();
				strcpy(operand,ReadOperand());
				strcpy(IMRArray[ArrayIndex]->OperandField,operand);
				if (strcmp(opcode,"END"))
				{
					if (label[0] != '\0')
					{
						if (SearchSymtab(label))
						{
							fclose(fptr);
							printf("ERROE: Duplicate Symbol\n");
							FoundOnSymtab_flag = 0;
							exit(1);
						}
						RecordSymtab(label);
					}
			
					if (SearchOptab (opcode))
						LOCCTR[LocctrCounter] = loc + (int) (OPTAB[Counter].Format-'0');
					else if (!strcmp(opcode,"WORD"))
						LOCCTR[LocctrCounter] = loc + 3;
					else if (!strcmp(opcode,"RESW"))
						LOCCTR[LocctrCounter] = loc + 3 * StrToDec(operand);
					else if (!strcmp(opcode,"RESB"))
						LOCCTR[LocctrCounter] = loc + StrToDec(operand);
					else if (!strcmp(opcode,"BYTE"))
						LOCCTR[LocctrCounter] = loc + ComputeLen(operand);
					else{
						fclose(fptr);
						printf("ERROE: Invalid Operation Code, %s\n", opcode);
						exit(1);
					}
				}
			}

			loc = LOCCTR[LocctrCounter];
			IMRArray[ArrayIndex]->Loc = LOCCTR[LocctrCounter-1];
			LocctrCounter++;
			ArrayIndex++;
		}

		FoundOnOptab_flag = 0;
		line += 1;
	}
	program_length = LOCCTR[LocctrCounter-2]- LOCCTR[0];


	printf("Pass 2 Processing...\n");
	unsigned long inst_fmt;//
	unsigned long inst_fmt_opcode;
	unsigned long inst_fmt_index;
	unsigned long inst_fmt_address;
	for (loop = 1; loop<ArrayIndex; loop++){
		inst_fmt_opcode = 0;
		inst_fmt_index = 0;
		inst_fmt_address = 0;
		strcpy(opcode,IMRArray[loop]->OperatorField);
		if (SearchOptab(opcode)){
			inst_fmt_opcode = OPTAB[Counter].ManchineCode;
		    inst_fmt_opcode <<=16;
		    IMRArray[loop]->ObjectCode = inst_fmt_opcode;
		    strcpy(operand,IMRArray[loop]->OperandField);
			if (operand[strlen(operand)-2] == ',' && operand[strlen(operand)-1] == 'X'){
				inst_fmt_index = 0x008000;
				operand[strlen(operand)-2] = '\0';
			}
			else
				inst_fmt_index = 0x000000;

			for(search_symtab = 0; search_symtab<SymtabCounter; search_symtab++){
				if(!strcmp(operand, SYMTAB[search_symtab].Label))
					inst_fmt_address = (long)SYMTAB[search_symtab].Address;
			}
		    inst_fmt =  inst_fmt_opcode + inst_fmt_index + inst_fmt_address;
			IMRArray[loop]->ObjectCode = inst_fmt;
		}
		else if (!strcmp(opcode, "WORD")){
			strcpy(operand,IMRArray[loop]->OperandField);
			IMRArray[loop]->ObjectCode = StrToDec(operand);
		}
		else if (!strcmp(opcode,"BYTE")){
			strcpy(operand,IMRArray[loop]->OperandField);
            IMRArray[loop]->ObjectCode = 0;
			if(operand[0]=='C' || operand[0]=='c' && operand[1]=='\''){
				for (x = 2; x<=(int) (strlen(operand)-2); x++){
					IMRArray[loop]->ObjectCode=IMRArray[loop]->ObjectCode + (int)operand[x];
					IMRArray[loop]->ObjectCode<<=8;
				}
			}
            if(operand[0]=='X' || operand[0]=='x' && operand[1]=='\''){
				char *operand_ptr;
				operand_ptr = &operand[2];
				*(operand_ptr+2)='\0';
				for (x=2; x<=(int) (strlen(operand)-2); x++){
					IMRArray[loop]->ObjectCode=IMRArray[loop]->ObjectCode + StrToHex(operand_ptr);
					IMRArray[loop]->ObjectCode<<=8;
				}
			}
	    	IMRArray[loop]->ObjectCode>>=8;
		}
		else
		/* do nothing */	;		
	}
	CreateProgramList();
	CreateObjectCode();
	
	for (loop = 0; loop<ArrayIndex; loop++)
        free(IMRArray[loop]);
	printf("Compeleted Assembly\n");
	fclose(fptr);
}
