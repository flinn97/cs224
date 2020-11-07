#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

const int MAX_MEM_SIZE  = (1 << 13);

int hex(int digit) {
return 0;
}

void fetchStage(int *icode, int *ifun, int *rA, int *rB, wordType *valC, wordType *valP) {
//get pc icode and ifun
wordType pc = getPC();
byteType byte = getByteFromMemory(pc);
*icode = (byte >>4) & 0xf;
*ifun = byte & 0xf;

//fetch halt
if (*icode == HALT) {
	*valP = pc +1;
	setStatus(STAT_HLT);}

//fetch NOP
if (*icode == NOP) {
	*valP = pc +1;}

//fetch IRMOVQ
if (*icode == IRMOVQ) {
	byte = getByteFromMemory(pc +1);
	*rA = (byte>>4) & 0xf;
	*rB = byte & 0xf;
	*valC = getWordFromMemory(pc+2);
	*valP = pc + 10;}

//fetch RRMOVQ
if (*icode == RRMOVQ) {
	byte = getByteFromMemory (pc+1);
	*rA = (byte >>4) & 0xf;
	*rB = byte & 0xf;
	*valP = pc + 2;}

//fetch RMMOVQ
if(*icode == RMMOVQ) {
	byte = getByteFromMemory (pc +1);
	*rA = (byte >>4) & 0xf;
	*rB = byte & 0xf;
	*valC = getWordFromMemory(pc+2);
	*valP = pc +10;}

//fetch MRMOVQ
if(*icode ==MRMOVQ) {
	byte = getByteFromMemory (pc +1);
	*rA = (byte >>4) & 0xf;
	*rB = byte & 0xf;
	*valC = getWordFromMemory(pc+2);
	*valP = pc +10;}

//fetch OPQ
if(*icode == OPQ) {
byte = getByteFromMemory (pc +1);
        *rA = (byte >>4) & 0xf;
	        *rB = byte & 0xf;
		*valP = pc +2;}

//fetch PUSHQ
if(*icode == PUSHQ){
	byte = getByteFromMemory (pc +1);
	*rA = (byte >>4) & 0xf;
	*rB = byte & 0xf;
	*valP = pc +2;}

//fetch POPQ
if(*icode == POPQ){                                                                                                                                                                                                                         
	byte = getByteFromMemory (pc +1);
	*rA = (byte >>4) & 0xf;
	*rB = byte & 0xf;
	*valP = pc +2;}

//fetch CALL
if(*icode == CALL){
	*valC = getWordFromMemory(pc +1);
	*valP = pc +9;}

//fetch JXX
if(*icode == JXX){
	*valC = getWordFromMemory(pc +1);
	*valP = pc+9;}

//fetch RET
if(*icode == RET){
	*valP = pc +1;}
}

void decodeStage(int icode, int rA, int rB, wordType *valA, wordType *valB) {

//Decode RRMOVQ, RMMOVQ, MRMOVQ, OPQ, PUSHQ, POPQ, CALL, RET 

if (icode == RRMOVQ) {
	*valA = getRegister(rA);}

if (icode == RMMOVQ){
	*valA = getRegister(rA);
	*valB = getRegister(rB);}

if (icode == MRMOVQ){
	*valB= getRegister(rB);	}

if (icode == OPQ){
	*valA = getRegister(rA);
	*valB = getRegister(rB);}

if (icode ==PUSHQ){
	*valA = getRegister(rA);
	*valB = getRegister(4);}

if (icode == POPQ){
	*valA = getRegister(4);
	*valB = getRegister(4);}

if (icode == CALL){
	*valB = getRegister(4);}

if(icode == RET){
	*valA = getRegister(4);
	*valB = getRegister(4);}

}

void executeStage(int icode, int ifun, wordType valA, wordType valB, wordType valC, wordType *valE, bool *Cnd) {

	//Execute IRMOVQ, RRMOVQ, RMMOVQ, OPQ, PUSHQ, POPQ, CALL, JXX, RET
	
if (icode == IRMOVQ) {
	  *valE = 0 + valC;}

if (icode == RRMOVQ) {
	*valE = valA;}

if (icode == RMMOVQ) {
	*valE = valB +valC;}

if(icode == MRMOVQ){
	*valE = valB +valC;}

if(icode == OPQ){
	int sf= 0;
	int of= 0;
	int zf= 0;

	if (ifun== ADD){
		*valE = valA + valB; 
		if(valA>=0 && valB >=0 && *valE < 0)
		{of = 1;}
		if(valA<=0 && valB <= 0 && *valE > 0)                                                                   
	       	{of = 1;}
		if (*valE == 0)
			{ zf = 1;}
		if(*valE < 0)
		{sf = 1;}

	}
	if (ifun == SUB){
		*valE = valB - valA;
		if(valA<=0 && valB >=0 && *valE < 0) 
		{of = 1;}     
		if(valA>=0 && valB <= 0 && *valE > 0) 
		{of = 1;}
		if (*valE == 0)
		{zf = 1;}
		if (*valE < 0)
		{sf = 1;}
	}
	if (ifun == AND){
		*valE = valB & valA;
		printf("valE: %8lx", *valE);
		if( *valE == 0x0)
		{zf = 1;}
		if(*valE < 0)
		{sf =1;}
	}
	if (ifun == XOR){
		*valE = valB ^ valA;
		if( *valE == 0)
		{zf =1;}
		if(*valE <0)
		{sf =1;}
		
	}
		setFlags(sf, zf, of);
	}
	
if(icode == PUSHQ) {
	*valE = valB +(-8);}

if(icode == POPQ) {
	*valE = valB +8;}

if(icode == CALL){
	*valE = valB + (-8);}

if(icode == JXX){
	*Cnd = Cond(ifun);}

if(icode == RET){
	*valE = valB +8;}

}

void memoryStage(int icode, wordType valA, wordType valP, wordType valE, wordType *valM) {

	//Memory: RMMVOQ, MRMOVQ, PUSHQ, POPQ, CALL, RET
	
if (icode == RMMOVQ) {
	setWordInMemory(valE, valA);}

if(icode == MRMOVQ) {
	*valM = getWordFromMemory(valE);}

if(icode == PUSHQ){
	setWordInMemory(valE, valA);}

if(icode == POPQ){
	*valM = getWordFromMemory(valA);}

if(icode == CALL){
	setWordInMemory(valE, valP);}

if(icode ==RET){
	*valM = getWordFromMemory(valA);}

}

void writebackStage(int icode, wordType rA, wordType rB, wordType valE, wordType valM) {
 	
	//writeBack: IRMOVQ , MRMOVQ, OPQ, PUSHQ, POPQ, CALL, RET

if (icode == IRMOVQ){
	setRegister(rB, valE);}

if (icode == RRMOVQ){
	setRegister(rB, valE);}

if (icode == MRMOVQ) {
	setRegister(rA, valM);}

if (icode == OPQ){
	setRegister(rB, valE);}

if (icode == PUSHQ){
	setRegister(4, valE);}

if(icode == POPQ){
	setRegister(4, valE);
	setRegister(rA, valM);}

if(icode == CALL){
	setRegister(4, valE);}

if(icode == RET){
	setRegister(4, valE);}

}

void pcUpdateStage(int icode, wordType valC, wordType valP, bool Cnd, wordType valM) {

	//update pc for POPQ, PUSHQ, OPQ, MRMOVQ, HALT, NOP, IRMOVQ, RRMOVQ, RMMOVQ
 	
	if (icode == POPQ || icode == PUSHQ || icode == OPQ || icode == MRMOVQ || icode == HALT || icode == NOP || icode == IRMOVQ || icode ==RRMOVQ || icode==RMMOVQ){
		setPC(valP);}
	
	//update pc for CALL
	if(icode == CALL){
		setPC(valC);}

	//update pc for RET
	if (icode == RET){
	setPC(valM);}

	//update pc for JXX
	if (icode == JXX){
		if (Cnd){
			setPC(valC);}
		else
			setPC(valP);
		}

}

void stepMachine(int stepMode) {
  /* FETCH STAGE */
  int icode = 0, ifun = 0;
  int rA = 0, rB = 0;
  wordType valC = 0;
  wordType valP = 0;
 
  /* DECODE STAGE */
  wordType valA = 0;
  wordType valB = 0;

  /* EXECUTE STAGE */
  wordType valE = 0;
  bool Cnd = 0;

  /* MEMORY STAGE */
  wordType valM = 0;

  fetchStage(&icode, &ifun, &rA, &rB, &valC, &valP);
  applyStageStepMode(stepMode, "Fetch", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);

  decodeStage(icode, rA, rB, &valA, &valB);
  applyStageStepMode(stepMode, "Decode", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);
  
  executeStage(icode, ifun, valA, valB, valC, &valE, &Cnd);
  applyStageStepMode(stepMode, "Execute", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);
  
  memoryStage(icode, valA, valP, valE, &valM);
  applyStageStepMode(stepMode, "Memory", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);
  
  writebackStage(icode, rA, rB, valE, valM);
  applyStageStepMode(stepMode, "Writeback", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);
  
  pcUpdateStage(icode, valC, valP, Cnd, valM);
  applyStageStepMode(stepMode, "PC update", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);

  incrementCycleCounter();
}

/** 
 * main
 * */
int main(int argc, char **argv) {
  int stepMode = 0;
  FILE *input = parseCommandLine(argc, argv, &stepMode);

  initializeMemory(MAX_MEM_SIZE);
  initializeRegisters();
  loadMemory(input);

  applyStepMode(stepMode);
  while (getStatus() != STAT_HLT) {
    stepMachine(stepMode);
    applyStepMode(stepMode);
#ifdef DEBUG
    printMachineState();
    printf("\n");
#endif
  }
  printMachineState();
  return 0;
}
