#include<iostream>
#include<vector>
#include<queue>
#include<cmath>
#include<fstream>
#include<string>
#include<iomanip>  
using namespace std;
// ======================= WE ARE MAKING MMU SIMULATION ======================== 
//IN WHICH 2 TXT'S MADE
//1st FOR TLB (TRANSLATION LOOKASIDE BUFFER) -> FAST+ DIRECT ACCESS -> LIMITED MEMORY SPACE(HARDWARE CACHE ) -> TIME CONSUMING IF TLB MISS OCCURRS + USED 
//2nd FOR PAGE TABLE  (LARGE BLOCK) -> SLOWER THAN TLB -> AS IT IS IN RAM & RAM IS SLOW TO ACCESS

// MODULE 1:
// FIRSTLY I WILL MAKE STRUCTS THAT ARE REQUIRE IN FUTURE  SO LESS GO! :)
struct Config {
	// 8 parameters needed in configuring txt
	int ramSize, pageSize, tlbSize;
	int tlbLatency, ramLatency, diskLatency;
	int numFrames, shift;
	void calcDervdValues() {
		shift = (int)log2(pageSize * 1024); //suppose 4KB=12 bits
		numFrames = ramSize / pageSize;  //Total physiscal slots in each page
	}
};
struct AddressParts {
	unsigned int vpn;  //splitting address VPN [actual page number]
	unsigned int offset;
};
//ACTUAL SPLITTING IS DONE HERE WHERE DATATYPE OF SPLITADRR SHOULD BE LIKE ADDRESSPART'S STRUCT.
AddressParts splitAddr(unsigned int addr, int shift ,int pageSize) {
	AddressParts p;  //composition 
	p.vpn = addr >> shift;
	p.offset = addr & ((pageSize * 1024)-1);
	return p;
}
// =====================MODULE 1 COMPLETED =========================


//MODULE 2:
// IN THIS MODULE WE ONLY DEFINE STRUCTURES OF TLB & PAGE TABLE  AND VECTOR FOR BOTH
struct PageTableEntry {
	int frameNumber = -1;
	bool valid = false;
	bool dirty = false;
};
struct TLBentry {
	unsigned int vpn;
	int frameNumber;
	bool valid = false;
};

//Simulation Structures
vector<PageTableEntry> pageTable((int)pow(2, 20));
vector<TLBentry> tlb;  //to configure size

// ======================MODULE 2 COMPLETED=============================


long long totalSimulatedTime = 0;
long long totalAccess = 0;  //required for stats
long long Tlbhits = 0;     
long long pageFaults = 0;  // how many times we bring form disk
queue<unsigned int> fifoQueue;

void updateTLB(unsigned int vpn, int frameNumber, Config& conf) {
	//To overwrite old entries we use totalAccess % tlbSize
	int index = totalAccess % conf.tlbSize;
	tlb[index].vpn = vpn;
	tlb[index].frameNumber = frameNumber;
	tlb[index].valid = true;
	cout << "TLB UPDATED:VPN" << vpn << "At INDEX" << index << endl;
} 

void handlePageFault(unsigned int vpn, char mode, Config& conf) {
	
	pageFaults++;
	totalSimulatedTime += conf.diskLatency; // time taken to bring page from disk

	// Checking if RAM full or not!
	if (fifoQueue.size() >= conf.numFrames) {
		// FIFO: Remove oldest VPN
		unsigned int victimVPN = fifoQueue.front();
		fifoQueue.pop(); 

			// Dirty Bit Logic
			if (pageTable[victimVPN].dirty) {
				// If "W"->if change in page then timetaken by writing to disk
				totalSimulatedTime += conf.diskLatency;
				cout << "Victim VPN " << victimVPN << " was DIRTY. Disk Write triggered." << endl;
			}

		pageTable[victimVPN].valid = false; // left the RAM

		// If victim is in tlb then invalidate it
		for (auto& entry : tlb) {
			if (entry.vpn == victimVPN) entry.valid = false;
		}
	}

	// Adding new page to RAM
	int assignedFrame = fifoQueue.size(); // Simple frame assignment
	pageTable[vpn].frameNumber = assignedFrame;
	pageTable[vpn].valid = true;
	pageTable[vpn].dirty = (mode == 'W'); // 'W' hai to dirty set karein

	fifoQueue.push(vpn);

		// Also update tlb
		updateTLB(vpn, assignedFrame, conf);
}

// ===================================================MODULE 3: MAIN MODULE========================================================
// IN THIS MODULE I AM MAKING "SEARCH & LOGIC SIMULATOR"
// WE'LL DO THREE MAIN TASKS HERE
// 1) TLB LOOKUP [1ST LOOK IN TLB AS IT'S FAST]
// 2) PAGETABLE LOOKUP [IF TLB MISS THEN LOOK IN PAGETABLE]
// 3) PERFORMANCE TRACKING [CALCUATE LATENCY (TIME) FOR EACH STEP]
void processAccess(unsigned int addr, char mode, Config conf) 
{
	// <=============1st use splitter func=======================================>
	AddressParts parts = splitAddr(addr,conf.shift,conf.pageSize);  //funct call
	
	bool TLBhit = false;
	totalSimulatedTime += conf.tlbLatency;   //as mmu always checks tlb first

	//<==============2nd TLB search loop as TLB is small than page table=========>
	for (int i = 0;i< tlb.size();i++) {
		if (tlb[i].valid && tlb[i].vpn == parts.vpn) {
			TLBhit = true;
			Tlbhits++;
			totalSimulatedTime += conf.ramLatency;  //take data from RAM
			break;
		}
	}

	//<==============3rd if tlb miss=============================================>
	   if (!TLBhit) 
	   {
		totalSimulatedTime += conf.ramLatency;  //pagetable search time
		if (pageTable[parts.vpn].valid) {
			updateTLB(parts.vpn, pageTable[parts.vpn].frameNumber,conf);
		}
		else
		{
			//Page Fault not in RAM
			handlePageFault(parts.vpn, mode, conf);
		}
	   }

}

void DisplayDetails() {
	cout << endl;
	cout << "============================================" << endl;
	cout << "           MMU SIMULATION DETAILS           " << endl;
	cout << "============================================" << endl;
	cout << "Total Memory Accesses: " << totalAccess << endl;
	cout << "TLB Hits:             " << Tlbhits << endl;
	cout << "Page Faults:          " << pageFaults << endl;

	double hitRatio = ((double)Tlbhits / totalAccess) * 100;
	double EAT = (double)totalSimulatedTime / totalAccess;

	cout << fixed << setprecision(2);  //used in iomanip lib to control decimal-type numbers
	cout << "TLB Hit Ratio:        " << hitRatio << "%" << endl;
	cout << "Avg Access Time (EAT):" << EAT << " ns" << endl;
	cout << "============================================" << endl;
}


int main() {
	Config conf;
	ifstream configFile("D:/4th sem all/OS lec+notes/config.txt");
	string key;
	if (!configFile) {
		cout << "Config file not fount here!" << endl;
		return 1;
	}

	//Config file reading logic
	configFile >> key >> conf.ramSize;    
	configFile >> key >> conf.pageSize;
	configFile >> key >> conf.tlbSize;
	configFile >> key >> conf.tlbLatency;
	configFile >> key >> conf.ramLatency;
	configFile >> key >> conf.diskLatency;
	conf.calcDervdValues();

	tlb.resize(conf.tlbSize);

	ifstream traceFile("D:/4th sem all/OS lec+notes/trace.txt");
	string addrHex;
	char mode;

	if (!traceFile) {
		cout << "Trace file not found" << endl;
		return 1;
	}

	while(traceFile>>addrHex>>mode){
		totalAccess++;
		unsigned int address = stoul(addrHex, nullptr, 16);
		processAccess(address, mode, conf);
	}

	DisplayDetails();
	return 0;

}