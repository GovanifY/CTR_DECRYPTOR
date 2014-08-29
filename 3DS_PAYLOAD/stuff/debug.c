#include "debug.h"
#include "string.h"

static reg_watcher register_watch[6];
static page_watcher page_watch;

void print_register_watcher(u32 id){

	int x = 5+id*65;
	int y = 20;
	u32 value;
	char string[9];
	int i;

	blank(x,y,65,200);
	for (i = 0; i < NUM_REGISTER_SAVES;i++){
		value = register_watch[id].values[i];
		if (register_watch[id].mode == REGISTER_MODE_WORD) u32_to_string(value, string, 9);
			else byte_to_bit_string(value, string, 9);
		
		paint_word(string, x, y, (255*(id%2)),0,(255*(id%2)), TOP_LEFT_FRAME1);
		paint_word(string, x, y, (255*(id%2)),0,(255*(id%2)), TOP_LEFT_FRAME2);
		y += 10;
	}

}


void update_register_watcher(u32 id){
	if (register_watch[id].address < 0x10000000) return;
	u32 word;
	if (register_watch[id].mode == REGISTER_MODE_WORD) word = read_word(register_watch[id].address);
	else word = read_byte(register_watch[id].address);
	if (register_watch[id].values[0] == word) return;
	int i;
	for (i = NUM_REGISTER_SAVES-1; i >= 1; i--) register_watch[id].values[i] = register_watch[id].values[i-1];
	register_watch[id].values[0] = word;
	print_register_watcher(id);	
}


void update_page_watcher(){
	int i,x,y;
	u32 word;
	char string[9];
	for (i = 0; i < 6*27; i++){
		word = read_word(page_watch.address+i*8);
		if (word != page_watch.values[i]){
			x = ((i%6)*65)+5;
			y = (i/6)*10;
			blank(x, y , 65, 10);
			u32_to_string(word, string, 9);
			paint_word(string, x, y+2, (255*((i%28)%2)),0,(255*((i%28)%2)), TOP_LEFT_FRAME1);
			paint_word(string, x, y+2, (255*((i%28)%2)),0,(255*((i%28)%2)), TOP_LEFT_FRAME2);
		}
		page_watch.values[i] = word;
	}
}

void init_debug(){
	int i,k;
	for (i = 0; i < 6; i++){
		register_watch[i].address = 0;
		for (k = 0; k < NUM_REGISTER_SAVES; k++){
			register_watch[i].values[k] = 0;
		}
		
	}
	page_watch.address = 0x0;
	for (i = 0; i < 6*28; i++){
		page_watch.values[i] = 0x0;
	}
}

void update_all_watchers(s32 type){
	int id;

	if (type == REGISTER_W){
		for (id = 0; id < 6; id++){
			update_register_watcher(id);		
		}
	} else {
		update_page_watcher();
	}

}

void print_watcher(s32 type, u32 id){
	if (type == REGISTER_W){
		print_register_watcher(id);
	} else {

	}
}

void set_watcher_address(s32 type, u32 id, u32 address){
	if (type == REGISTER_W){
		register_watch[id].address = address;

		char string[9];
		u32_to_string(register_watch[id].address, string, 9);
		paint_word(string, 5+id*65, 10, 255,0,0, TOP_LEFT_FRAME1);
		paint_word(string, 5+id*65, 10, 255,0,0, TOP_LEFT_FRAME2);
	} else {
		page_watch.address = address;
	}	
}

void update_watcher(s32 type, u32 id){
	if (type == REGISTER_W){
		update_register_watcher(id);
	} else {
		update_page_watcher();
	}
}



