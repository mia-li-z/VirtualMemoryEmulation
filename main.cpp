/*
Main program for the virtual memory project.
Make all of your modifications to this file.
You may add or rearrange any code or data as you need.
The header files page_table.h and disk.h explain
how to use the page table and disk interfaces.
*/

#include "page_table.h"
#include "disk.h"
#include "program.h"

#include <cassert>
#include <iostream>
#include <string.h>
#include <queue>

using namespace std;

// Prototype for test program
typedef void (*program_f)(char *data, int length);


// Number of physical frames
int nframes;
int page_faults_count = 0;
int disk_reads_count = 0;
int disk_writes_count = 0;

// Pointer to disk for access from handlers
struct disk *disk = nullptr;

//queue for fifo algorithm
std:: queue<int> fifo_queue;


// Simple handler for pages == frames
void page_fault_handler_example(struct page_table *pt, int page) {
	cout << "page fault on page #" << page << endl;

	// Print the page table contents
	cout << "Before ---------------------------" << endl;
	page_table_print(pt);
	cout << "----------------------------------" << endl;
	page_table_set_entry(pt, page, page, PROT_READ | PROT_WRITE);
	exit(1);

	// Print the page table contents
	cout << "After ----------------------------" << endl;
	page_table_print(pt);
	cout << "----------------------------------" << endl;
}

void page_fault_handler_rand(struct page_table *pt, int page) {
	int frame, bits;
  	page_table_get_entry(pt, page, &frame, &bits); 
	if(!(bits & PROT_WRITE)){

	}
	else{
		page_faults_count ++;

		//evict the frame which is the first in the queue
		// int evict_frame;
		

		//read data into memory and update page table
		disk_read(disk, page, &page_table_get_physmem(pt)[frame * PAGE_SIZE]);
  		page_table_set_entry(pt, page, frame, PROT_READ);
  		disk_reads_count ++;
	}
}

//check if the page resident in memory (HOW???)
//if so update the page table
//if not, increase page_fault_count
// 	check if the physical memory is full
//	if so apply corresponding replacement algorithm
//	
//read data and update the page table

/*
Handle page fault using fifo replacement algorithm
@param pt
@param page
*/
void page_fault_handler_fifo(struct page_table *pt, int page) {
	int frame, bits;
  	page_table_get_entry(pt, page, &frame, &bits); 
	            

	if(!(bits & PROT_WRITE)){
		//if the page is in memory
		//set the page to write
		page_table_set_entry(pt, page, frame, bits | PROT_WRITE);
	}
	else{
		//if the page is not in memory, apply fifo replacement algorithm
		page_faults_count ++;

		//evict the frame which is the first in the queue
		// int evict_frame;
		

		//read data into memory and update page table
		disk_read(disk, page, &page_table_get_physmem(pt)[frame * PAGE_SIZE]);
  		page_table_set_entry(pt, page, frame, PROT_READ);
  		disk_reads_count ++;

	}

}



void page_fault_handler_lru(struct page_table *pt, int page) {
	int frame, bits;
  	page_table_get_entry(pt, page, &frame, &bits); 

	if(!(bits & PROT_WRITE)){
		//if the page is in memory
		//set the page to write
		page_table_set_entry(pt, page, frame, bits | PROT_WRITE);
	}
	else{
		page_faults_count ++;

		

		//read data into memory and update page table
		disk_read(disk, page, &page_table_get_physmem(pt)[frame * PAGE_SIZE]);
  		page_table_set_entry(pt, page, frame, PROT_READ);
  		disk_reads_count ++;
	}
}


// TODO - Handler(s) and page eviction algorithms

int main(int argc, char *argv[]) {
	// Check argument count
	if (argc != 5) {
		cerr << "Usage: virtmem <npages> <nframes> <rand|fifo|lru> <sort|scan|focus>" << endl;
		exit(1);
	}

	// Parse command line arguments
	int npages = atoi(argv[1]);
	nframes = atoi(argv[2]);
	const char *algorithm = argv[3];
	const char *program_name = argv[4];

	//// Validate the algorithm specified
	page_fault_handler_t page_fault_handler = NULL;

	if ((strcmp(algorithm, "rand") != 0) &&
	    (strcmp(algorithm, "fifo") != 0) &&
	    (strcmp(algorithm, "lru") != 0)) {
		cerr << "ERROR: Unknown algorithm: " << algorithm << endl;
		exit(1);
	}
	else if(!strcmp(algorithm, "rand")){
		page_fault_handler = page_fault_handler_rand;
	}
	else if(!strcmp(algorithm, "fifo")){
		page_fault_handler = page_fault_handler_fifo;
	}
	else if(!strcmp(algorithm, "lru")){
		page_fault_handler = page_fault_handler_lru;
	}

	// Validate the program specified
	program_f program = NULL;
	if (!strcmp(program_name, "sort")) {
		if (nframes < 2) {
			cerr << "ERROR: nFrames >= 2 for sort program" << endl;
			exit(1);
		}

		program = sort_program;
	}
	else if (!strcmp(program_name, "scan")) {
		program = scan_program;
	}
	else if (!strcmp(program_name, "focus")) {
		program = focus_program;
	}
	else {
		cerr << "ERROR: Unknown program: " << program_name << endl;
		exit(1);
	}

	// TODO - Any init needed
	
	// Create a virtual disk
	disk = disk_open("myvirtualdisk", npages);
	if (!disk) {
		cerr << "ERROR: Couldn't create virtual disk: " << strerror(errno) << endl;
		return 1;
	}
	// Create a page table
	struct page_table *pt = page_table_create(npages, nframes, page_fault_handler /* TODO - Replace with your handler(s)*/);
	if (!pt) {
		cerr << "ERROR: Couldn't create page table: " << strerror(errno) << endl;
		return 1;
	}

	// Run the specified program
	char *virtmem = page_table_get_virtmem(pt);
	program(virtmem, npages * PAGE_SIZE);

	// Clean up the page table and disk
	page_table_delete(pt);
	disk_close(disk);

	return 0;
}
