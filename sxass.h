
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

typedef struct OperationCodeTable
{
	char Mnemonic[8];
	char Format;
	unsigned short int  ManchineCode;
}SIC_OPTAB;

typedef struct SymbolTable
{
	char Label[10];
	int Address;
}SIC_SYMTAB;

typedef struct IntermediateRecord
{
	unsigned short int LineIndex;
	unsigned short int Loc;
	unsigned long int ObjectCode;
	char LabelField[32];
	char OperatorField[32];
	char OperandField[32];
}IntermediateRec;

int Counter;
int LOCCTR[100];
int LocctrCounter = 0;
int ProgramLen;
int Index;
int j;
int ManchineCode;
int SymtabCounter = 0;
int start_address;
int program_length;
int ArrayIndex = 0;

unsigned short int FoundOnSymtab_flag = 0;
unsigned short int FoundOnOptab_flag = 0;
char Buffer[256];
char Label[32];
char Mnemonic[32];
char Operand[32];

SIC_SYMTAB SYMTAB[20];
IntermediateRec* IMRArray[100];

static SIC_OPTAB OPTAB[]=
{
    {   "ADD",  '3',  0x18},
    {   "AND",  '3',  0x40},
    {  "COMP",  '3',  0x28},
    {   "DIV",  '3',  0x24},
    {     "J",  '3',  0x3C},
    {   "JEQ",  '3',  0x30},
    {   "JGT",  '3',  0x34},
    {   "JLT",  '3',  0x38},
    {  "JSUB",  '3',  0x48},
    {   "LDA",  '3',  0x00},
    {  "LDCH",  '3',  0x50},
    {   "LDL",  '3',  0x08},
    {   "LDX",  '3',  0x04},
    {   "MUL",  '3',  0x20},
    {    "OR",  '3',  0x44},
    {    "RD",  '3',  0xD8},
    {  "RSUB\r",  '3',  0x4C},
    {   "STA",  '3',  0x0C},
    {  "STCH",  '3',  0x54},
    {   "STL",  '3',  0x14},
    {  "STSW",  '3',  0xE8},
    {   "STX",  '3',  0x10},
    {   "SUB",  '3',  0x1C},
    {    "TD",  '3',  0xE0},
    {   "TIX",  '3',  0x2C},
    {    "WD",  '3',  0xDC},
};

