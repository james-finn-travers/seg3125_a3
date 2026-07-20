#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>


//generates string representing random pages numbered (0-9)
char *pages_simulate(void){
    int length = 25;
    char *str = malloc(length + 1);
	if (!str) {
    perror("malloc");
    exit(EXIT_FAILURE);
	}
    for (int i = 0; i < length; i++) str[i] = '0' + rand() % 10;
    str[length] = '\0';
    return str;
}

bool fault(char page, char frames[], int frame_count){
	for (int i = 0; i<frame_count; i++){
		if (page ==frames[i]) return false;
	}
	return true;
}

int print_frames(char frames[], int num_frames){
	printf("|");
	for (int i = 0; i < num_frames; i++)
	    printf("%c|", frames[i]);
	printf("\n");
}

//struct for helper function (step1)
typedef struct{
	char *frames;
	int *last_referenced_timestamp; //only applies for lru
	char *pages;
	int num_pages;
	int num_pages_handled;
	int num_faults;
	bool *faults_by_page;
} step_1_results;

//fills up frames; common step between lru and fifo
step_1_results first_fill(int num_frames){

	//initialize variables
	char *frames = malloc(num_frames + 1);
	if (frames == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	printf("Pages: %d\n",frames);

	for (int i = 0; i < num_frames; i++) frames[i] = '-';
	frames[num_frames] = '\0';

	int *last_referenced_timestamp = malloc(num_frames * sizeof(int)); //only applies for lru
	for (int i = 0; i < num_frames; i++) last_referenced_timestamp[i] = -1;
	char *pages = pages_simulate();
	char page_selected;
	int num_pages_handled = 0;
	int num_pages = strlen(pages);
	int num_faults = 0;
	bool *faults_by_page = malloc(num_pages * sizeof(bool));
	if (!faults_by_page) {
    	perror("malloc");
    	exit(EXIT_FAILURE);
	}

	printf("Initial Frames\n");
	printf("%s\n", frames); //initial print
	while (num_faults < num_frames && num_pages_handled < num_pages){
		printf("Page %d\n",num_pages_handled);
		page_selected = pages[num_pages_handled];
		//iterate to see if page already in frames
		//add to new frame
		if (fault(page_selected,frames,num_faults)){
			printf("FAULT\n");
			frames[num_faults] = page_selected;
			last_referenced_timestamp[num_faults] = num_pages_handled;
			num_faults++;
			faults_by_page[num_pages_handled] = true;
		} else faults_by_page[num_pages_handled] = false;

		print_frames(frames,num_frames);
		num_pages_handled++;
	}
	step_1_results step_1_results = {
		frames,
		last_referenced_timestamp,
		pages,
		num_pages,
		num_pages_handled,
		num_faults,
		faults_by_page
	};
	return step_1_results;
}



void print_faults(int num_faults,bool faults_by_page[],char pages[],int num_pages){
    printf("%d Faults:\n", num_faults);

    for (int i = 0; i < num_pages; i++) {
        if (faults_by_page[i]) printf("Page %d\n", i);
    }
}


int fifo(int num_frames){
	step_1_results step_1_results = first_fill(num_frames);
	bool *faults_by_page = step_1_results.faults_by_page;
	char *frames = step_1_results.frames;
	char *pages = step_1_results.pages;
	int num_pages = step_1_results.num_pages;
	int num_pages_handled = step_1_results.num_pages_handled;
	int num_faults = step_1_results.num_faults;
	int *last_referenced_timestamp = step_1_results.last_referenced_timestamp;
	int oldest_frame_index = 0;


	while (num_pages_handled < num_pages) {
		char handling_page = pages[num_pages_handled];
		printf("Page %d \n", num_pages_handled);
		if (fault(handling_page, frames, num_frames)) {
			printf("FAULT\n");
			faults_by_page[num_pages_handled] = true;
			frames[oldest_frame_index] = handling_page;
			last_referenced_timestamp[oldest_frame_index] = num_pages_handled;
			oldest_frame_index = (oldest_frame_index + 1) % num_frames;
			num_faults++;
		}
		else faults_by_page[num_pages_handled] = false;
		print_frames(frames,num_frames);
		num_pages_handled++;
	}


	print_faults(num_faults,faults_by_page,pages,num_pages);

	free(frames);
	free(pages);
	free(last_referenced_timestamp);
	free(faults_by_page);

	return num_faults;
}

int lru(int num_frames){

	step_1_results step_1_results = first_fill(num_frames);
	
	char *frames = step_1_results.frames;
	char *pages = step_1_results.pages;
	int num_pages = step_1_results.num_pages;
	int num_pages_handled = step_1_results.num_pages_handled;
	int *last_referenced_timestamp = step_1_results.last_referenced_timestamp;
	bool *faults_by_page = step_1_results.faults_by_page;
	int num_faults = step_1_results.num_faults;

	char page_selected;
	int replacement_index;

	while (num_pages_handled < num_pages){
		printf("Page %d\n", num_pages_handled);
		page_selected = pages[num_pages_handled];
		if (fault(page_selected,frames,num_frames)){
			faults_by_page[num_pages_handled] = true;
			printf("FAULT\n");
			num_faults++;
			//find replacement index by taking the index to the smallest timestamp value;
			replacement_index = 0;
			for (int i = 1; i<num_frames; i++){
				if (last_referenced_timestamp[replacement_index]>last_referenced_timestamp[i]) replacement_index = i;
			}

			//replace page, update timestamp
			frames[replacement_index] = page_selected;
			last_referenced_timestamp[replacement_index] = num_pages_handled;
		} else {
			faults_by_page[num_pages_handled] = false;
			for (int i = 0; i < num_frames; i++) {
				if (frames[i] == page_selected) {
					last_referenced_timestamp[i] = num_pages_handled;
					break;
				}
			}
		}
		print_frames(frames,num_frames);
		num_pages_handled++;
	}

	print_faults(num_faults,faults_by_page,pages,num_pages);


	free(frames);
	free(pages);
	free(last_referenced_timestamp);
	free(faults_by_page);

	return num_faults;
}


int main(int argc, char *argv[])
{
    srand((unsigned)time(NULL));

    if (argc != 3){
        printf("Usage: %s <algorithm> <num_frames>\n", argv[0]);
        printf("Example: %s fifo 3\n", argv[0]);
        printf("Example: %s lru 3\n", argv[0]);
        return 1;
    }


    int num_frames = atoi(argv[2]);


    if (num_frames <= 0){
        printf("Number of frames must be greater than 0\n");
        return 1;
    } if (strcmp(argv[1], "fifo") == 0){
        printf("Fifo %d frame:\n", num_frames);
        fifo(num_frames);
    } else if (strcmp(argv[1], "lru") == 0){
        printf("Lru %d frame:\n", num_frames);
        lru(num_frames);
    } else {
        printf("Unknown algorithm: %s\n", argv[1]);
        printf("Use either fifo or lru\n");
        return 1;
    }
    return 0;
}