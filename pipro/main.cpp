#include <iostream>	//including all the required header files
#include <fstream>
#include <cassert>
#include <iterator>
#include <set>
#include <string>
#include <vector>
#include <climits>
#include <iomanip>

/*-------------------------------------------------------------------------------------------------
*    Author   : Team DOOFENSMARTZ - CS19B004 Dharani, CS19B028 Kamal, CS19B046 Vedaant
*    Code     : CPP code to simulate a pipeline processor
*    Question : CS2610 A8
-------------------------------------------------------------------------------------------------*/

typedef unsigned int uint;	//using typedef to change names for easier use
typedef unsigned short usint;
typedef unsigned char byte;

#define IF 0
#define ID 1
#define EX 2
#define MM 3
#define WB 4

#define OPC_ADD 0
#define OPC_SUB 1
#define OPC_MUL 2
#define OPC_INC 3
#define OPC_AND 4
#define OPC_OR 5
#define OPC_NOT 6
#define OPC_XOR 7
#define OPC_LD 8
#define OPC_ST 9
#define OPC_JMP 10
#define OPC_BEQZ 11
#define OPC_HALT 15

#define PIPE_IF 1
#define PIPE_ID 2
#define PIPE_EX 3
#define PIPE_MM 4
#define PIPE_WB 5

// returns log of x to base 2
int log2(uint x)
{
    int r = 0;
	// stop when x is exhausted
    while(x > 0)	
    {
		// div by 2
        x = x >> 1;
        r++;
    }

    return r - 1;
}

// returns a power of 2
int pow2(uint n)
{
    int r = 1;
	// same idea as log
    while(n > 0)
    {
        r = r << 1;
        n--;
    }

    return r;
}

/****************************************************************************************************
 * 	Class Name	: Cache
 * 	Inheritences: NIL
 * 	Use			: Used to act as a cache for a processor, as Instruction and Data Cache
****************************************************************************************************/
class Cache
{
	uint cacheSize = 256;
	uint blockSize = 4; // do NOT change! other params are hardcoded
	
	// points to source file
	std::ifstream * srcFile;
	std::vector<uint> sets;
	//Little Endian
public:
	// reads a file into cache
	Cache(std::ifstream * fp);
	
	// read a block from cache
	uint readBlock(byte address);
	// read a byte from cache
	byte readByte(byte address);
	// symmetric methods for writing
	void writeBlock(byte address, uint data);
	void writeByte(byte address, byte data);

	// dumps the cache into a file, with given name
	void dumpCache(std::string filename);
};

Cache::Cache(std::ifstream * fp) {
	// some inits
	this->srcFile = fp;
	sets = std::vector<uint> (cacheSize, 0);

	// get a byte from file
	std::string hexCode;
	uint value;
	int blockNum = 0;
	int offset = 0;
	while(*fp >> hexCode){
		// move to next block
		if(offset == 4)
		{
			offset = 0;
			blockNum++;
		}

		value = std::stoi(hexCode,0,16);
		for(int i = 0; i < offset; i++)
		{
			value = value << 8;
		}
		// and write it onto a block
		sets[blockNum] += value;
		offset++;
	}

    fp->close();
}

// read methods
uint Cache::readBlock(byte address){
	return sets[address >> 2];
}
byte Cache::readByte(byte address){
	uint data = readBlock(address);
	uint offset = address & (blockSize-1);
	for(int i = offset; i < 3; i++)
	{
		data = data << 8;
	}
	for(int i = 0; i < 3; i++)
	{
		data = data >> 8;
	}
	return (byte)data;
}

// write methods
void Cache::writeBlock(byte address, uint data){
	sets[address >> 2] = data;
}
void Cache::writeByte(byte address, byte data){
	uint temp  = (uint)data;
	byte blockNum = address >> 2;
	byte offset = address & (3);
	uint mask = cacheSize-1;
	for(int i = 0; i < (int)offset; i++)
	{
		mask = mask << 8;
		temp = temp << 8;
	}
	mask = UINT_MAX - mask;
	sets[blockNum] = (sets[blockNum] & mask) + temp;
}

// cache dump to file
void Cache::dumpCache(std::string filename)  {
	// init
	std::ofstream outfile;
	outfile.open(filename, std::ofstream::trunc);
	
	uint i;

	// output bytes in hex form, pad to be 2 digits
	outfile << std::setfill('0') << std::setw(2) << std::hex;

	for(i = 0; i < cacheSize; i++)  {
		outfile << std::setw(2) << ((int) readByte(i)) << std::endl;
	}
	outfile.close();
}

/****************************************************************************************************
 * 	Class Name	: RegFile
 * 	Inheritences: NIL
 * 	Use			: Used to act as a the register file for the processor
****************************************************************************************************/
class RegFile
{
	int regSize = 16;

	// source file stream
	std::ifstream * srcFile;
	// stores actual data
	std::vector<byte> RF;
	// stores status of registers being written to
	std::vector<bool> status;
public:
	// init regfile from a file
	RegFile(std::ifstream * fp);

	// read a byte from R-index
	byte read(byte index);
	// write a byte to R-index
	void write(byte index, byte data);
    
	// sets status of a register - true indicates it is being written to
	void setStatus(byte index, bool currStatus);
    // returns status of a register
	bool isOpen(byte index);
};

RegFile::RegFile(std::ifstream * fp){
	this->srcFile = fp;
	RF = std::vector<byte> (regSize);
	status = std::vector<bool> (regSize,false);

    // read file into regfile
	std::string hexCode;
	byte value;
	int regNum = 0;
	while(*fp >> hexCode){
		value = std::stoi(hexCode,0,16);
		RF[regNum] = value;
		regNum++;
	}
    fp->close();
};

// read/write methods
byte RegFile::read(byte index){
	return RF[index];
}
void RegFile::write(byte index,byte data){
    if(index == 0)  {return;}
	RF[index] = data;
}

// status methods
bool RegFile::isOpen(byte index){
	return status[index];
}
void RegFile::setStatus(byte index, bool currStatus){
	status[index] = currStatus;
}
/****************************************************************************************************
 * 	Class Name	: Processor
 * 	Inheritences: NIL
 * 	Use			: Used to act as the pipeline processor for the simulator
****************************************************************************************************/
class Processor  {
private:
	Cache* iCache;	//cache object pointers to store instruction and data caches
    Cache* dCache;
	RegFile* regFile;	//register file object pointer to allot the object later

	// haltScheduled schedules a halt and waits for current pipeline stages to end
	bool haltScheduled = false;
	// halted indicates true halt
	bool halted = false;

	// if any is true, that stage will be stalled
	bool stallIF = false;
	bool stallID = false;
	bool stallEX = false;
	bool stallMM = false;
	bool stallWB = false;

	// IF pipeline registers here
	byte REG_IF_PC = 0u; // first instruction
	bool IF_run = true;
	void fetchStage();

	// ID pipeline registers here
	usint REG_ID_IR = 0u; // add R0 to R0 and store in R0
	byte  REG_ID_PC = 0u;
	bool  ID_run = false;
	void decodeStage();

	// EX pipeline registers here
	usint REG_EX_IR = 0u;
	byte  REG_EX_PC = 0u;
	byte  REG_EX_A  = 0u;
	byte  REG_EX_B  = 0u;
	bool  EX_run = false;
	void executeStage();

	// MM pipeline registers here
	usint REG_MM_IR = 0u;
	byte  REG_MM_AO = 0u;
    bool  REG_MM_COND = false;
	bool  MM_run = false;
	void memoryStage();

	// WB pipeline registers here
	usint REG_WB_IR   = 0u;
	byte  REG_WB_AO   = 0u;
	byte  REG_WB_LMD  = 0u;
	bool  REG_WB_COND = false;
	bool  WB_run = false;
	void writebackStage();

	void flushPipeline();
public:
	// init with caches and rf
	Processor(std::ifstream * Icache, std::ifstream * Dcache, std::ifstream * RegFile);
	~Processor();

	// initiates run, runs until halted
	void run();
	// run one cycle
	void cycle();

	// is processor halted?
	bool isHalted();

	// dump stats and cache
	void dumpdata(std::string fnameCache, std::string fnameOut);

	// stats
	int stat_instruction_count = 0;
	int stat_instruction_count_arith = 0;
	int stat_instruction_count_logic = 0;
	int stat_instruction_count_data = 0;
	int stat_instruction_count_control = 0;
	int stat_instruction_count_halt = 0;
	int stat_cycles = 0;
	int stat_stalls = 0;
	int stat_stalls_data = 0;
	int stat_stalls_control = 0;
};

// init: get files
Processor::Processor(std::ifstream * Icache, std::ifstream * Dcache, std::ifstream * regFile)  {
	iCache = new Cache(Icache);
	dCache = new Cache(Dcache);
	this->regFile = new RegFile(regFile);
}

// destruct
Processor::~Processor()  {
	delete iCache;
	delete dCache;
	delete regFile;
}

// runs until halted
void Processor::run()  {
	// while halt is not called execute instructions
	while(!isHalted())  {
		cycle();	//call the cycle function, the equivalent of one cycle of the processor
		}
	stat_instruction_count = stat_instruction_count_arith + stat_instruction_count_control + 
                                     stat_instruction_count_data + stat_instruction_count_logic +
                                     stat_instruction_count_halt;
}

void Processor::cycle()  {
	// to simulate parallelism we execute the pipeline stages
	// in the order WB MEM EX ID IF in one cycle,
	// so WB's buffer is cleared before MEM tries
	// to write to it
	stat_cycles++;

	writebackStage();
	memoryStage();
	executeStage();
	decodeStage();
	fetchStage();
}

bool Processor::isHalted()  {
	return halted;
}

// EX
void Processor::executeStage()
{
	if(!EX_run || stallEX)  {
		return;
	}

	int opCode = REG_EX_IR >> 12;
	switch(opCode)
	{
		case OPC_ADD :	REG_MM_AO = REG_EX_A + REG_EX_B; 
						stat_instruction_count_arith++;	break;

		case OPC_SUB :	REG_MM_AO = REG_EX_A - REG_EX_B;
						stat_instruction_count_arith++; break;

		case OPC_MUL :	REG_MM_AO = REG_EX_A * REG_EX_B; 
						stat_instruction_count_arith++; break;

		case OPC_INC :	REG_MM_AO = REG_EX_A + 1; 		 
						stat_instruction_count_arith++; break;

		case OPC_AND :	REG_MM_AO = REG_EX_A & REG_EX_B; 
						stat_instruction_count_logic++; 	break;

		case OPC_OR :	REG_MM_AO = REG_EX_A | REG_EX_B; 
						stat_instruction_count_logic++; 	break;

		case OPC_NOT :	REG_MM_AO = ~REG_EX_A;
						stat_instruction_count_logic++; 	break;

		case OPC_XOR :	REG_MM_AO = REG_EX_A ^ REG_EX_B;
						stat_instruction_count_logic++; 	break;

		case OPC_LD :	REG_MM_AO = REG_EX_A + REG_EX_B;
						stat_instruction_count_data++;	break;
						
		case OPC_ST :	REG_MM_AO = REG_EX_A + REG_EX_B;
						stat_instruction_count_data++;	break;

		case OPC_JMP:
			stallEX = true;
			stat_instruction_count_control++;	//counting the control instruction
			REG_MM_AO = REG_EX_PC + (byte) ((usint) REG_EX_A << 1);
			break;

		case OPC_BEQZ:
			stallEX = true;
			stat_instruction_count_control++;	//counting the control instruction
			if((int)REG_EX_A == 0)
			{
				REG_MM_AO =  REG_EX_PC +((usint) ( REG_EX_B << 1));
			}
			else
			{
				REG_MM_AO =  REG_EX_PC;
			}
			break;

		default:	break;
	}
	REG_MM_IR = REG_EX_IR;
	MM_run = true;	//marking that next instruction to be implemented is memmory 
	EX_run = false;	//setting that the stage is finished
}

// IF
void Processor::fetchStage()  {
	// wait for ID to be available
	if(!IF_run)  { // due to backward nature, if ID_run is true, it has been stalling
		return;
	}
	if(ID_run || stallID)  { // due to backward nature, if ID_run is true, it has been stalling
		IF_run = true;
		return;
	}

	REG_ID_IR = (((usint) iCache->readByte(REG_IF_PC)) << 8) + ((usint) iCache->readByte(REG_IF_PC+1));
	ID_run = true;

	// inc PC
	REG_IF_PC += 2;
}

// ID
void Processor::decodeStage()  {
	if((!ID_run) || stallID)  {
		return;
	}

	if(EX_run || stallEX)  {
		ID_run = true;
		return;
	}

	REG_EX_IR = REG_ID_IR;
	byte opcode = (REG_ID_IR & 0xf000) >> 12;
	
	// halt class
	if(opcode == OPC_HALT)  {
		IF_run = false;
		ID_run = false;
		EX_run = true;
		stat_instruction_count_halt++;	//counting number of halt instructions
		// let the other pipeline stages (previous instructions) complete
		return;
	}
	ID_run = false;
	EX_run = true;

	// control class - HAZ
	if(opcode == OPC_JMP)  {
		stallID = true;
		
		REG_EX_A = (REG_ID_IR & 0x0ff0) >> 4;
		REG_EX_PC = REG_IF_PC;
		stat_stalls_control += 2;
		return;
	}
	if(opcode == OPC_BEQZ)  {
		stallID = true;

		if(regFile->isOpen((REG_ID_IR & 0x0f00) >> 8))
		{
			stat_stalls_data++;
			EX_run = false;
			ID_run = true;
            stallID = false; // RAW stalling is different
			return;
		}
		stat_stalls_control += 2;
		REG_EX_PC = REG_IF_PC;
		REG_EX_A = regFile->read((REG_ID_IR & 0x0f00) >> 8);
		REG_EX_B = REG_ID_IR & 0x00ff;
		return;
	}
	// data class
	usint addr1;
	usint addr2;
	if(opcode == OPC_ST)  {
		addr1 = (REG_ID_IR & 0x00f0) >> 4;
		if(regFile->isOpen(addr1))
		{
			stat_stalls_data++;
			EX_run = false;
			ID_run = true;
			return;
		}

		REG_EX_A = regFile->read((REG_ID_IR & 0x00f0) >> 4);
		REG_EX_B = REG_ID_IR & 0x000f;
		return;
	}

	if((opcode == OPC_LD))  {
		addr1 = (REG_ID_IR & 0x00f0) >> 4;
		if(regFile->isOpen(addr1))
		{
			stat_stalls_data++;
			EX_run = false;
			ID_run = true;
			return;
		}
		REG_EX_A = regFile->read((REG_ID_IR & 0x00f0) >> 4);
		REG_EX_B = REG_ID_IR & 0x000f;

        regFile->setStatus((byte) ((REG_ID_IR & 0x0f00) >> 8), true);
		return;
	}

	// arithmetic class - HAZ
	if((opcode >= OPC_ADD) && (opcode <= OPC_MUL))  {
		addr1 = (REG_ID_IR & 0x00f0) >> 4;
		addr2 = REG_ID_IR & 0x000f;
		if(regFile->isOpen(addr1) || regFile->isOpen(addr2))
		{
			stat_stalls_data++;
			EX_run = false;
			ID_run = true;
			return;
		}

		REG_EX_A = regFile->read((REG_ID_IR & 0x00f0) >> 4);
		REG_EX_B = regFile->read(REG_ID_IR & 0x000f);
        regFile->setStatus((byte) ((REG_ID_IR & 0x0f00) >> 8), true);
		return;
	}
	if(opcode == OPC_INC)  {
		addr1 = (REG_ID_IR & 0x0f00) >> 8;
		if(regFile->isOpen(addr1))
		{
			stat_stalls_data++;
			EX_run = false;
			ID_run = true;
			return;
		}

		REG_EX_A = regFile->read((REG_ID_IR & 0x0f00) >> 8);
        regFile->setStatus((byte) ((REG_ID_IR & 0x0f00) >> 8), true);
		return;
	}

	// logical class - HAZ
	if((opcode != OPC_NOT))  {
		addr1 = (REG_ID_IR & 0x00f0) >> 4;
		addr2 = REG_ID_IR & 0x000f;
		if(regFile->isOpen(addr1) || regFile->isOpen(addr2))
		{
			stat_stalls_data++;
			EX_run = false;
			ID_run = true;
			return;
		}

		REG_EX_A = regFile->read((REG_ID_IR & 0x00f0) >> 4);
		REG_EX_B = regFile->read(REG_ID_IR & 0x000f);

        regFile->setStatus((byte) ((REG_ID_IR & 0x0f00) >> 8), true);
		return;
	}

	// here, opcode == OPC_NOT
	if(opcode == OPC_NOT)	{
		addr1 = (REG_ID_IR & 0x00f0) >> 4;
        if(regFile->isOpen(addr1))
        {
        	stat_stalls_data++;
            EX_run = false;
            ID_run = true;
            return;
        }

		REG_EX_A = regFile->read((REG_ID_IR & 0x00f0) >> 4);

        regFile->setStatus((byte) ((REG_ID_IR & 0x0f00) >> 8), true);
	}
}

// MEM
void Processor::memoryStage(){
	if(!MM_run || stallMM)
	{
		return;
	}
	MM_run = false;
	usint opCode = REG_MM_IR >> 12;
	byte offset = (byte) ((REG_MM_IR & 0x0f00) >> 8);
	if(opCode == OPC_HALT)
	{
		WB_run = true;
		REG_WB_IR = REG_MM_IR;
		return;
	}
	else if((opCode == OPC_JMP) || (opCode == OPC_BEQZ))  {
		// if PC didn't change, carry on with decoding
		if(REG_IF_PC == REG_MM_AO)  {
			ID_run = false;
			stallID = false;
			stallEX = false;
		// if it did, flush pipeline
		}  else  {
			REG_IF_PC = REG_MM_AO;
			flushPipeline();
		}
		WB_run = false;
		return;
	}
	
	else if(opCode == OPC_ST)
	{
		dCache->writeByte(REG_MM_AO,regFile->read((REG_MM_IR & 0x0f00) >> 8));
		MM_run = false;
        REG_WB_AO = 0u;
        REG_WB_IR = 0u;
        REG_WB_LMD = 0u;
        REG_WB_COND = false;
        WB_run = true;
		return;
	}
	else if(opCode == OPC_LD)
	{
		REG_WB_LMD = dCache->readByte(REG_MM_AO);
		REG_WB_IR = REG_MM_IR;
		REG_WB_AO = REG_MM_AO;
		WB_run = true;
	}  
	else  {
		MM_run = false;
        REG_WB_AO = REG_MM_AO;
        REG_WB_IR = REG_MM_IR;
        REG_WB_LMD = 0u;
        REG_WB_COND = REG_MM_COND;
        WB_run = true;
		return;
	}

	MM_run = false;
	return;
}

// WB
void Processor::writebackStage(){
	if(!WB_run || stallWB)
	{
		return;
	}
	usint opCode = REG_WB_IR >> 12;
	byte offset = (byte) ((REG_WB_IR & 0x0f00) >> 8);
	if(opCode == OPC_HALT){
		WB_run = false;
		halted = true;
		return;
	}
	if(opCode == OPC_LD) 
	{
		regFile->write(offset, REG_WB_LMD);
	}
	else 
	{
		regFile->write(offset, REG_WB_AO);
	}
	regFile->setStatus(offset, false);
	WB_run = false;
}

// flushes the pipeline
void Processor::flushPipeline()  {
	IF_run = true;
	ID_run = false;
	EX_run = false;
	MM_run = false;
	WB_run = false;

	REG_ID_IR = 0u;
	REG_ID_PC = 0u;
	REG_EX_A = 0u;
	REG_EX_B = 0u;
	REG_EX_IR = 0u;
	REG_EX_PC = 0u;
	REG_MM_AO = 0u;
	REG_MM_IR = 0u;
	REG_WB_AO = 0u;
	REG_WB_IR = 0u;
	REG_WB_COND = 0;
	REG_WB_LMD = 0u;

	stallIF = false;
	stallID = false;
	stallEX = false;
	stallMM = false;
	stallWB = false;
}

// outputs processor stats
void Processor::dumpdata(std::string fnameCache, std::string fnameOut)  {
	dCache->dumpCache(fnameCache);

	std::ofstream outFile;
	outFile.open(fnameOut);

	outFile << "Total number of instructions executed: " << stat_instruction_count << std::endl;
	outFile << "Number of instructions in each class" << std::endl;
	outFile << "Arithmetic instructions              : " << stat_instruction_count_arith << std::endl;
	outFile << "Logical instructions                 : " << stat_instruction_count_logic << std::endl;
	outFile << "Data instructions                    : " << stat_instruction_count_data << std::endl;
	outFile << "Control instructions                 : " << stat_instruction_count_control << std::endl;
	outFile << "Halt instructions                    : " << stat_instruction_count_halt << std::endl;
	outFile << "Cycles Per Instruction               : " << ((double) stat_cycles / stat_instruction_count) << std::endl;
	outFile << "Total number of stalls               : " << stat_stalls_data+stat_stalls_control << std::endl;
	outFile << "Data stalls (RAW)                    : " << stat_stalls_data << std::endl;
	outFile << "Control stalls                       : " << stat_stalls_control << std::endl;
}

/*-------------------------------------------------------------------------------------------------
*    Function Name : main
*    Args          : Nil
*    Return Type   : int(0)
*    Application   : Entry point to the Proram
-------------------------------------------------------------------------------------------------*/
int main()
{
	std::ifstream Icache, Dcache, RegFile;	//creating objects for file handling

	Icache.open("ICache.txt");	//getting the filepointer of the Instruction cache file
	Dcache.open("DCache.txt");	//getting the filepointer of the data cache file
	RegFile.open("RF.txt");	//the input for the register file

	Processor processor(&Icache, &Dcache, &RegFile);	//sending the adress class as pointers/*change class name*/
	processor.run();	//running the processor

	processor.dumpdata("DCache.out.txt", "Output.txt");

	return 0;
}