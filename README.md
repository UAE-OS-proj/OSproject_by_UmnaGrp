**MMU Simulator: Because RAM is Expensive, but Logic is Free**
Welcome to the MMU (Memory Management Unit) Simulator! This project was built to simulate the high-stakes drama that happens between the TLB, Page Table, and the Disk every time a CPU asks for data.

**The Current Situation:**
CPU: "I need data from Address 0x1234!"

TLB: "New phone, who dis?" (TLB Miss)

RAM: "I don't have it either, ask the manager." (Page Fault)

Disk: Sighs in 10,000,000ns latency "I'm coming, keep your hair on..."

**The "Relatable Content" Section**

**The "I know this Person" Strategy (TLB Hit)**
When the CPU accesses the same VPN twice, the TLB finally remembers it. It’s like when the canteen guy recognizes you and gives you your tea & Fries without you having to say anything. Efficiency: 10/10.

**What does this thing do?**
->This simulator mimics a hardware MMU. It takes a virtual address and tries to find where it lives in physical memory.

->TLB Lookup: It checks the fast cache first (The "I know this person" approach).

->Page Table Lookup: If the TLB fails, it checks the RAM (The "Let me check the archives" approach).

->Page Fault Handling: If it's not in RAM, it goes to the Disk (The "Call the manager, we're going to be here a while" approach).

**The "Ghosting" Incident**
If you don't implement the Dirty Bit logic, you're basically ghosting the Disk after changing its data. Don't be that person. Write back your changes.

**Features**
->FIFO Replacement: First-In, First-Out. Just like the line at the CIPS canteen, but actually fair and without anyone cutting in.

->Dirty Bit Logic: We track changes. If a page was modified, it gets written back to the disk. No data left behind!

->Performance Metrics: Calculates Hit Ratio and EAT (Effective Access Time). If your EAT is higher than your current GPA, it's time to tweak your config.txt.

**File Structure**
->main.cpp: The brain of the operation (where the magic and the bugs live).

->config.txt: Where you define your hardware limits (RAM size, latencies, etc.).

->trace.txt: The list of addresses your CPU is crying for.

**Warning: Emotional Damage**
If your Effective Access Time (EAT) is ridiculously high, please check your Disk_Latency in config.txt. Disks are slower than the Wi-Fi in the back of the library during finals week. Physics is hard, and life is unfair.

**Contributing**
Found a bug? Or maybe you found a way to make the TLB even faster? Open a Pull Request! Or just star the repo to help me cope with the fact that I spent my weekend talking to a compiler about page tables.
