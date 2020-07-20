#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <time.h>

//global structures
struct disk_array disk;
struct jobs_array jobs;
int allocated_count;


/*
 * Program to simulate Contiguous Allocation Algorithms
 */
void print_usage() {
    fprintf(stderr,"\nUsage: simulate <FF|WF|BF> -NMAX <number> -TMAX <number> -JMAX <number> -PALLOC <number> -PDEALLOC <number>\n\n");
    exit(0);
}


/*
 * Jobs structure
 * has pointer to int array and size of array properties
 */
struct jobs_array {
    int *jobs;
    unsigned int size;
};


/*
 * Disk structure 
 * has pointer to int array and size of array properties
 */
struct disk_array {
    int *data;
    unsigned int size;
};


/*
 * Hole structure
 * A hole in memory has an index and size
 */
struct hole {
    int index;
    unsigned int size;
    int hole_count;
};


/*
 * Random probability calculator between 0 and 1
 */
static double rand_prob(){
    return (double)rand() / (double)RAND_MAX ;
}


/* 
 * Scan disk looks for holes in the array
 * Identifies index and length of consecutive empty blocks
 * Returns array of arrays
 */
struct hole* scan_disk(struct disk_array disk){
    int count = 0;
    int index = 0;
    int hole_count = 0;
    struct hole *holes = calloc (disk.size /2, sizeof(struct hole));
    
    while(index < disk.size){
        if(disk.data[index] == 0){
            while(disk.data[index] == 0 && index < disk.size){
                index++;
                count++;
            }
            holes[hole_count].index = index-count;
            holes[hole_count].size = count;
            holes[0].hole_count = hole_count;
            hole_count++;
            
        }
        else{
            index++;
            count = 0;
        }
    }
    return holes;
}


/*
 * Prints out block index's and values
 * Takes disk structure
 * Returns nothing
 */
void print_disk_state(struct disk_array disk){
    int index;
    //Print Disk State
    printf("|");
    for(index = 0; index < disk.size; index++) {
        printf(" %d |", disk.data[index]);
    }
    printf("\n");
}

/*
 * Allocates a job by writing id
 * of job for size of job in array
 * Returns nothing
 */
void allocate_job(int job_index, int job_id, int job_size){
    
    //jobs.jobs = realloc(jobs.jobs, (jobs.size) * sizeof(int));
    jobs.jobs[jobs.size] = allocated_count;
    jobs.size++;
    
    for(int i = 0; i < job_size; i++){
        if(disk.data[job_index + i] == 0){
            disk.data[job_index + i] = job_id;
        }
    }
}

/*
 * Deallocates a job by removing it from memory
 * finds all blocks with job id sets to zero
 * Returns nothing
 */
void deallocate_job(int job_id){
    for(int i = 0; i < disk.size; i++){
        if(disk.data[i] == job_id){
            disk.data[i] = 0;
        }
    }
}


/*
 * Getopt long options structure
 */
static struct option long_options[] =
{
    {"NMAX", required_argument, NULL, 'n'},
    {"TMAX", required_argument, NULL, 't'},
    {"JMAX", required_argument, NULL, 'j'},
    {"PALLOC", required_argument, NULL, 'a'},
    {"PDEALLOC", required_argument, NULL, 'd'},
    {NULL, 0, NULL, 0}
};


/*
 * Remove element from array my moving and reducing size
 */
int remove_element(int*from, int total, int index) {
    if(index != (total-1))
        from[index] = from[total-1];
    return total -1;
}


/*
 * Simulate Main method
 */
int main (int argc, char *argv[]){
    
    //Private variables
    srand((unsigned int)time(NULL));
    extern char *optarg;
    extern int optind;
    int c, err = 0, ticks = 0;
    int nmaxflag=0, tmaxflag=0, jmaxflag=0, pallocflag=0, pdeallocflag=0;
    int nmax, tmax, jmax;
    float palloc, dealloc;
    char fitment[3];
    int job_size;
    int failed_allocations = 0;
    int successful_allocations = 0;
    allocated_count = 1;
    int i = 0;
    int allocated = 0;
    int maximum;
    int location = 0;
    int job_id;
    
    struct hole *holes;
    
    //Get options from command line
    while ((c = getopt_long_only(argc, argv,"n:t:j:a :d", long_options, NULL)) != -1)
        
        switch (c) {
            case 'n':
                nmaxflag = 1;
                nmax = atoi(optarg);
                break;
            case 't':
                tmaxflag = 1;
                tmax = atoi(optarg);
                break;
            case 'j':
                jmaxflag = 1;
                jmax = atoi(optarg);
                break;
            case 'a':
                pallocflag = 1;
                palloc = atof(optarg);
                break;
            case 'd':
                pdeallocflag = 1;
                dealloc = atof(optarg);
                break;
            case '?':
                err = 1;
                break;
        }
    
    //Ensure all options are set otherwise prompt user
    if (nmaxflag == 0 | tmaxflag == 0 | jmaxflag == 0| pallocflag == 0| pdeallocflag == 0) {	/* all options mandatory */
        fprintf(stderr, "%s: missing an option\n", argv[0]);
        print_usage();
        exit(1);
    }
    else if (err) {
        //if unknown options provided print usage
        print_usage();
    }
    
    jobs.jobs = calloc (nmax, sizeof(struct hole));
    strcpy(fitment, argv[11]);                     //save fitment type
    disk.size = nmax;                               //save storage size
    disk.data = (int*)calloc(nmax, sizeof(int));    //initialise storage
    
    holes = scan_disk(disk);//get holes in disk
    
    //while time still exists
    while (ticks < tmax) {
        
        if(nmax <= 16){
            print_disk_state(disk);//print disk
            holes = scan_disk(disk);//get holes in disk
            
            //print holes
            for(int i = 0; i <= holes[0].hole_count; i++){
                printf("Hole at index %d %d blocks long\n",holes[i].index, holes[i].size);
            }
        }
        
        job_size = 1 + (rand() % jmax); //choose random job size
        allocated = 0; //set allocated flag
        
        //if time to deallocate a job
        if(rand_prob() < dealloc && ticks > 100){
            job_id = 1 + (rand() % jobs.size);  //get random job
            printf("Deallocating Job\t%d\n",jobs.jobs[job_id]);
            deallocate_job(jobs.jobs[job_id]);  //deallocate job from disk
            jobs.size = remove_element(jobs.jobs, jobs.size, job_id); //remove job from job list

            
        }
        
        //if time to allocate job
        if(rand_prob() < palloc){

            
            /*
             * First Fit
             * Use first fit memory allocation
             */
            if(strcmp(fitment,"FF") == 0){
                holes = scan_disk(disk);    //get holes
                
                for (c = 0; c < holes[0].hole_count+1 && allocated != 1; c++){  //get first fitting hole
                    if (holes[c].size >= job_size){
                        location = holes[c].index;
                        allocated = 1;
                    }
                }
                
                if(allocated == 1){ //if hole was found allocate job
                    allocate_job(location ,allocated_count,job_size);   //allocate job to disk
                    successful_allocations++;   //increase successful allocations
                    printf("Allocated Job\t\t%d size %d index %d at time %d\n",allocated_count, job_size,location,ticks);
                    holes = scan_disk(disk);    //update disk holes
                    allocated_count++;  //increase allocated count
                }
                else{   //if job count not be allocated
                    failed_allocations++;   //increase failed allocations
                }
                allocated = 0;  //reset allocated flag
                
            }
            
            
            /*
             * Worst Fit
             * Use Worst fit memory allocation
             */
            if(strcmp(fitment,"WF") == 0){
                holes = scan_disk(disk); //get holes
                
                maximum = 0;
                location  = 0;
                
                for (c = 0; c < holes[0].hole_count+1; c++){ //find largest hole that  fits job
                    if (holes[c].size > maximum){
                        maximum = holes[c].size;
                        location  = holes[c].index;
                    }
                }
                if(job_size <= maximum ){ //allocate job to worst (biggest) hole
                    allocate_job(location ,allocated_count,job_size);
                    successful_allocations++;   //increase number of successful allocations
                    printf("Allocated Job\t\t%d size %d index %d at time %d\n",allocated_count, job_size,location,ticks);
                    holes = scan_disk(disk);    //update disk holes
                    allocated_count++;  //increase allocated count
                }
                else{//if job count not be allocated
                    failed_allocations++;   //increase fail allocation count
                }
            }
            
            
            /*
             * Best Fit
             * Use best fit memory allocation
             */
            if(strcmp(fitment,"BF") == 0){
                holes = scan_disk(disk);    //get holes on disk
                maximum = -1;
                location  = -1;
                for (c = 0; c < holes[0].hole_count+1; c++){    //search though holes to find closest size to job
                    if((holes[c].size - job_size) == 0 || (holes[c].size - job_size) > 0){
                        if (holes[c].size - job_size < maximum){
                            maximum = holes[c].size - job_size;
                            location  = holes[c].index;
                        }
                    }
                }
                if(maximum == 0 || maximum > 0){    //if hole is a match or best fits
                    allocate_job(location ,allocated_count,job_size);   //allocate job to disk
                    successful_allocations++;   //increase successful allocations count
                    printf("Allocated Job\t\t%d size %d index %d at time %d\n",allocated_count, job_size,location,ticks);
                    holes = scan_disk(disk);    //update holes
                    allocated_count++;  //increase allocated count
                }
                else{   //if job could not be allocated
                    failed_allocations++; //incease failed allocations count
                }
            }
        }
        ticks++; //incease clock ticks
    }

    //Print final state of disk and allocation success and failures
    printf("\nFinal state:");
    print_disk_state(disk);
    printf("\n%d Failed allocations %.2f percent\n",failed_allocations,((float)failed_allocations/(float)(failed_allocations + (float)successful_allocations))*100 );
    printf("\n%d Successful allocations %.2f percent\n",successful_allocations,((float)successful_allocations/(float)(failed_allocations + (float)successful_allocations))*100);

    //free memory
    free(disk.data);
    free(holes);
    free(jobs.jobs);
    //return zero to end
	return 0;
}
