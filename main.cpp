/*
Main program for the virtual memory project.
Make all of your modifications to this file.
You may add or rearrange any code or data as you need.
The header files page_table.h and disk.h explain
how to use the page table and disk interfaces.
*/
#include "lru_cache.h"
#include "page_table.h"
#include "disk.h"
#include "program.h"

#include <cassert>
#include <iostream>
#include <string.h>
#include <queue>
#include <assert.h>

using namespace std;

// Prototype for test program
typedef void (*program_f)(char *data, int length);


// Number of physical frames
int nframes;
int page_faults = 0;
int disk_reads = 0;
int disk_writes = 0;
int algorithm_type;

//queue for fifo algorithm
std::queue<int> frame_queue;

// Pointer to disk for access from handlers
struct disk *disk = nullptr;

// LRUCahe
LRUCache *cache = nullptr;


static void page_fault_handler(struct page_table *pt, int nframes, int type);
static int evict_frame_algo(struct page_table *pt, int nframes);
static pair<int, int> evict_lru(struct page_table *pt, int nframes);


// Simple handler for pages == frames
void page_fault_handler_example(struct page_table *pt, int page) {
	cout << "page fault on page #" << page << endl;

	// Print the page table contents
	cout << "Before ---------------------------" << endl;
	page_table_print(pt);
	cout << "----------------------------------" << endl;
	page_table_set_entry(pt, page, page, PROT_READ | PROT_WRITE);
	// exit(1);

	// Print the page table contents
	cout << "After ----------------------------" << endl;
	page_table_print(pt);
	cout << "----------------------------------" << endl;
}
/*
Handle page fault using random replacement algorithm
@param pt
@param page
*/
void page_fault_handler_rand(struct page_table *pt, int page) {
	page_fault_handler(pt, page, 1);
}

/*
Handle page fault using fifo replacement algorithm
@param pt
@param page
*/
void page_fault_handler_fifo(struct page_table *pt, int page) {
	page_fault_handler(pt, page, 2);
}

/*
Handle page fault using lru replacement algorithm
@param pt
@param page
*/
void page_fault_handler_lru(struct page_table *pt, int page) {
	// page_fault_handler(pt,page,3);
	int frame, bits;
  	page_table_get_entry(pt, page, &frame, &bits); 

	if((bits == PROT_READ)){
		//if the page residents in memory
		page_table_set_entry(pt, page, frame, bits | PROT_WRITE);
		cache->get(page);
	}
	else{
		//if the page does not resident in memory
		page_faults ++;

		if (disk_reads >= nframes) { // there are no free frames, apply lru replacement algorithm
			pair<int, int> victim = evict_lru(pt, nframes);
			frame = victim.second;
  		}
		else{ //there are free frames,;
			frame = disk_reads;
		}
		//read data into memory and update page table
  		page_table_set_entry(pt, page, frame, PROT_READ);
		disk_read(disk, page, &page_table_get_physmem(pt)[frame * PAGE_SIZE]);
  		disk_reads++;
		cache->put(page, frame);
	}
}
static pair<int, int> evict_lru(struct page_table *pt, int nframes){
	pair<int, int> victim = cache->evict();
	int evict_page = victim.first;
	int evict_frame = victim.second;
	int tmp_frame, tmp_bits, evict_bits;
	page_table_get_entry(pt, evict_page, &tmp_frame, &tmp_bits);
	assert(tmp_frame==evict_frame);
	evict_bits = tmp_bits;

	//if the page is dirty, write it to disk
	if(evict_bits & PROT_WRITE){
		disk_write(disk, evict_page, &page_table_get_physmem(pt)[evict_frame*PAGE_SIZE]);
		disk_writes++;
	}

	//update the page table
	page_table_set_entry(pt, evict_page, evict_frame, 0); //-1

	//return <page, frame> pair
	return victim;
}

static void page_fault_handler(struct page_table *pt, int page, int type) {
	int frame, bits;
  	page_table_get_entry(pt, page, &frame, &bits); 

	// // Print the page table contents
	// cout << "Before ---------------------------" << endl;
	// page_table_print(pt);
	// cout << "----------------------------------" << endl;

	if((bits == PROT_READ)){
		//if the page residents in memory
		page_table_set_entry(pt, page, frame, bits | PROT_WRITE);
	}
	else{
		//if the page does not resident in memory
		page_faults ++;

		if (disk_reads >= nframes) { // there are no free frames, apply fifo replacement algorithm
			algorithm_type = type;
			frame = evict_frame_algo(pt, nframes);
  		}
		else{ //there are free frames,;
			frame = disk_reads;
		}

		//read data into memory and update page table
  		page_table_set_entry(pt, page, frame, PROT_READ);
		disk_read(disk, page, &page_table_get_physmem(pt)[frame * PAGE_SIZE]);
  		disk_reads++;
		frame_queue.push(frame); // if condition required
	}
	
	// // Print the page table contents
	// cout << "After ----------------------------" << endl;
	// page_table_print(pt);
	// cout << "----------------------------------" << endl;

}

// algorithm to find the page that needs to be evicted
static int evict_frame_algo(struct page_table *pt, int nframes){
	//get the frame to be evicted based on types
	//type 1 represents rand
	//type 2 represents fifo
	//type 3 represents lru
	int evict_frame;
	if(algorithm_type == 1){
		evict_frame = rand()% nframes;
	}
	else if(algorithm_type == 2){
		evict_frame = frame_queue.front();
		frame_queue.pop();
	}
	else if(algorithm_type == 3){

	}	
	
	//find page for that frame in page table
	int i, tmp_frame, tmp_bits, evict_page, evict_bits;
	int npages = page_table_get_npages(pt);
	for(i=0;i<npages;i++)  {
		page_table_get_entry(pt, i, &tmp_frame, &tmp_bits);
		if(tmp_frame == evict_frame && tmp_bits != 0){
			break;
		}
	}

	evict_page = i;
	evict_bits = tmp_bits;

	// cout << "evict_page = " << evict_page << endl;
	
	//if the page is dirty, write it to disk
	if(evict_bits & PROT_WRITE){
		disk_write(disk, evict_page, &page_table_get_physmem(pt)[evict_frame*PAGE_SIZE]);
		disk_writes++;
	}

	//update the page table
	page_table_set_entry(pt, evict_page, 0, 0); //-1

	//return frame
	return evict_frame;
}


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

	// Validate the algorithm specified
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
	page_faults = disk_reads = disk_writes = 0;
	cache = new LRUCache(nframes);

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

	cout << page_faults << " page faults, " 
    << disk_reads << " disk reads, " 
    << disk_writes << " disk writes" << endl;


	// Clean up the page table and disk
	page_table_delete(pt);
	disk_close(disk);

	return 0;
}

// int main2(int argc, char *argv[]) {
// 	LRUCache* obj = new LRUCache(2);
// 	obj->put(1,10);
// 	obj->put(2,20);
// 	assert(obj->get(1)==10);
// 	assert(obj->put(3,30)==false);
// 	assert(obj->evict().first==2);
// 	assert(obj->put(3,30)==true);

// 	assert(obj->get(2)==-1);
// 	obj->put(4,40);
// 	// assert(obj->get(1)==-1);
// 	// assert(obj->get(3)==30);
// 	// assert(obj->get(4)==40);
// 	// obj->put(5,50);
// }