/* 
	 Fundamentals of Computer Programming 
	 Silesian University of Technology <http://polsl.pl/> 
	 Program: (2) Conway's life
	 Author: Bartłomiej Wolny <bartwol206@student.polsl.pl>
	 Tutor: Piotr Fabian
	 Copyright (c) 2013, Bartlomiej Wolny. All rights reserved. 
*/
//#define ALLEGRO_STATICLINK
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <allegro5\allegro5.h>
//#include <allegro5\allegro_image.h>
#include <allegro5\allegro_native_dialog.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
//ALLEGRO_BITMAP  *image   = NULL;

#define CURRENT_ARRAY (!(generation_number%2))

int total_rows=0;
int total_columns=0;
char ***cell_map;

int alive_cells_number = 0;
int generation_number = 1;

int check_ndb(int row, int column)
{
	// neighbours checking order:
	// 123
	// 4#5
	// 678
	int nbd_counter=0;

	if(row > 0 && column > 0) 
		if(cell_map[CURRENT_ARRAY][row-1][column-1]==1 ) nbd_counter++;
	if(row > 0)
		if(cell_map[CURRENT_ARRAY][row-1][column  ]==1 ) nbd_counter++;
	if(row > 0 && column < total_columns-1)
		if(cell_map[CURRENT_ARRAY][row-1][column+1]==1 ) nbd_counter++;
	if(column > 0)
		if(cell_map[CURRENT_ARRAY][row  ][column-1]==1 ) nbd_counter++;
	if(column < total_columns-1)
		if(cell_map[CURRENT_ARRAY][row  ][column+1]==1 ) nbd_counter++;
	if(row < total_rows-1 && column > 0)
		if(cell_map[CURRENT_ARRAY][row+1][column-1]==1 ) nbd_counter++;
	if(row < total_rows-1)
		if(cell_map[CURRENT_ARRAY][row+1][column  ]==1 ) nbd_counter++;
	if(row < total_rows-1 && column < total_columns-1)
		if(cell_map[CURRENT_ARRAY][row+1][column+1]==1 ) nbd_counter++;
	//if(row > total_rows || column > total_columns)
		//printf("row lub column poza zakresem!!!", _getch());

	return nbd_counter;
}

///////////////////////////////////
#define GRID_SIZE 15
#define POSITION_OFFSET 50
int draw_in_console(int generation_number)
{
	system("cls");
	printf("Conway's life			  GENERATION: %d\n\n	", generation_number);

	al_clear_to_color(al_map_rgb(0,0,20));
	al_draw_rectangle(POSITION_OFFSET, POSITION_OFFSET, total_columns*GRID_SIZE+POSITION_OFFSET, total_rows*GRID_SIZE+POSITION_OFFSET, al_map_rgb(255, 255, 255), 1);
	for(int i=1; i<total_columns; i++)
		al_draw_line(POSITION_OFFSET+i*GRID_SIZE, POSITION_OFFSET, POSITION_OFFSET+i*GRID_SIZE, total_rows*GRID_SIZE+POSITION_OFFSET, al_map_rgb(25, 255, 100), 1);
	for(int i=1; i<total_rows; i++)
		al_draw_line(POSITION_OFFSET, POSITION_OFFSET+i*GRID_SIZE, total_columns*GRID_SIZE+POSITION_OFFSET, i*GRID_SIZE+POSITION_OFFSET, al_map_rgb(25, 255, 100), 1);
	

	for(int i=0; i < total_rows; i++)
	{
		for(int j=0; j < total_columns; j++)
		{
			if(cell_map[CURRENT_ARRAY][i][j])
				al_draw_filled_circle(50+(GRID_SIZE/2)+j*GRID_SIZE, 50+(GRID_SIZE/2)+i*GRID_SIZE, 5, al_map_rgb(255, 255, 0));
			else
				al_draw_filled_circle(50+(GRID_SIZE/2)+j*GRID_SIZE, 50+(GRID_SIZE/2)+i*GRID_SIZE, 5, al_map_rgb(80, 50, 50));
		}
	}

	al_flip_display();

	//printf("\n\n	");
	for(int i=0; i<total_rows; i++)
	{
		for(int j=0; j < total_columns; j++)
		{
			//printf(cell_map[CURRENT_ARRAY][i][j]==0 ? "." : "%d", check_ndb(i, j));
			printf(cell_map[CURRENT_ARRAY][i][j] == 0 ? "." : "#");
		}
		printf("\n	");
	}
	return 1;
}

int recalculate_environment()
{
	//int current_row=0, current_column=0;
	
	// CHECKING FOR NUMBER OF NBD and changing their state
	for(int current_row=0; current_row < total_rows; current_row++) // in every row
	{	
		for(int current_column=0; current_column < total_columns; current_column++) // for every column
		{	
			if(cell_map[CURRENT_ARRAY][current_row][current_column] == 0 && check_ndb(current_row, current_column) == 3) // if dead and 3 nbd then alive
				cell_map[!(CURRENT_ARRAY)][current_row][current_column] = 1; 
			else if(cell_map[CURRENT_ARRAY][current_row][current_column] == 0) //if just dead and NOT 3 nbd
				cell_map[!(CURRENT_ARRAY)][current_row][current_column] = cell_map[CURRENT_ARRAY][current_row][current_column];
			else // if alive
			{
				alive_cells_number++;
				switch(check_ndb(current_row, current_column)) //count number of neighbours
				{
				case 0: 
				case 1: //cell dies from isolation or overcrowd
				case 4: 
				case 5:
				case 6:
				case 7:
				case 8: cell_map[!(CURRENT_ARRAY)][current_row][current_column] = 0; break; 
				default: cell_map[!(CURRENT_ARRAY)][current_row][current_column] = 1; break;
				}
			}
		}
	}

	// TO ZERO NOT CURRENT ARRAY
	for(int current_row=0; current_row < total_rows; current_row++) // in every row
		for(int current_column=0; current_column < total_columns; current_column++) // for every column
			cell_map[CURRENT_ARRAY][current_row][current_column] = 0;
	return 1;
}

////////////////////
#define FILENAME ".\\input.txt"
////////////////////
int read_starting_positions_from_file()
{
	FILE *f;
	char file_found;
	if(file_found = !(f = fopen(FILENAME, "r")))
	{
		printf("\n\n\n   Could not open the file %s\n\n   Random values will be generated.\n", FILENAME);
		srand(time(NULL)); // init of rand
		total_columns = rand() % 30 + 10;
		total_rows = rand() % 25 + 10;
		printf("\n   Generated numbers\n   columns = %d\n   rows = %d", total_columns, total_rows);
		_getch();
		//return 0;
	}
	else //file exists
	{
		total_columns = 0, total_rows = 0;
	
		// count columns
		while(fgetc(f) != '\n')
			total_columns++;
		//total_columns+=1; // korekta
		fclose(f);
		f=fopen(FILENAME, "r");

		//count rows
		while(!feof(f))
			if(fgetc(f) == '\n')
				total_rows++;
		total_rows+=1; //korekta
		fclose(f);
	}

		//creating dynamic multidimensional array
		cell_map = (char***) calloc(2, sizeof(char*)); //CURRENT ARRAY array
		if(cell_map == NULL)
		{
			printf("Cannot alocate array for data");
			return 0;
		}
	
		for(int i=0; i<2; i++)
		{
			cell_map[i] = (char**) calloc(total_rows, sizeof(char*));
			if(cell_map == NULL)
			{
				printf("Cannot alocate array for data - possibly out of memory or something");
				return 0;
			}
			for(int j=0; j<total_rows; j++)
			{
				cell_map[i][j] = (char*) calloc(total_columns, sizeof(char*));
				if(cell_map == NULL)
				{
					printf("Cannot alocate array for data - possibly out of memory or something - 3. step");
					return 0;
				}
			}
		}

		if(!file_found) // 0-not, !0-yes
		{
			// reading file and saving data to array
			f = fopen(FILENAME, "r"); 
			for(int current_row=0; current_row < total_rows; current_row++) // in every row
			{	
				for(int current_column=0; current_column < total_columns; current_column++) // for every column
					cell_map[CURRENT_ARRAY][current_row][current_column] = fgetc(f)-48;

				fgetc(f); //omit of '\n'
			}
			fclose(f);
		}
		else
		{
			for(int current_row=0; current_row < total_rows; current_row++) // in every row
			{	
				for(int current_column=0; current_column < total_columns; current_column++) // for every column
					cell_map[CURRENT_ARRAY][current_row][current_column] = rand() % 2;
			}
		}

	//_getch();
	return 1;
}

int main()
{	
	//printf("\n\n	Conway's life by baftek\n\n	This is a program that simulates a living group of cells.\n	Dots are dead cells, hashes are alive cells.\n	Alive cells die from isolation when they have 0-1 neighbours\n	They also die of overcrowd when they have 4 or more neighbours.\n	They becomes alive when they have exactly 3 neighbours.\n	Press ANY KEY to start. New window will appear.");
	if(!read_starting_positions_from_file())
		if(_getch())
			return 0;
 
	if(!al_init()) 
	{
		al_show_native_message_box(display, "Error", "Error", "Failed to initialize allegro!", 
                                 NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}

	/*if(!al_init_image_addon()) 
	{
      al_show_native_message_box(display, "Error", "Error", "Failed to initialize al_init_image_addon!", 
                                 NULL, ALLEGRO_MESSAGEBOX_ERROR);
      return 0;
	}*/

	display = al_create_display(730, 480);
	if(!display) 
	{
		al_show_native_message_box(display, "Error", "Error", "failed to create display!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}

	event_queue = al_create_event_queue();
	if(!event_queue) 
	{
		al_show_native_message_box(display, "Error", "Error", "failed to create event_queue!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		al_destroy_display(display);
		return -1;
	}

	al_init_font_addon();
	al_init_ttf_addon();
	#define TEXT_SIZE 17
	ALLEGRO_FONT *font = al_load_ttf_font("C:\\Windows\\Fonts\\cour.ttf", TEXT_SIZE, 0);
	if(!font)
	{
		al_destroy_event_queue(event_queue);
		al_show_native_message_box(display, "Error", "Error", "Failed to initialize font!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		al_destroy_display(display);
		_getch();
		return -1;
	}

	al_install_keyboard();
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	ALLEGRO_EVENT ev;

	/*image = al_load_bitmap(".\\dead.jpg"); //C:\\Users\\Bartek\\Documents\\Visual Studio 2010\\Projects\\Conway's life\\Debug
	if(!image) {
      al_show_native_message_box(display, "Error", "Error", "Failed to load image!", 
                                 NULL, ALLEGRO_MESSAGEBOX_ERROR);
      al_destroy_display(display);
      return 0;
   }*/
 
	al_init_primitives_addon();
	al_clear_to_color(al_map_rgb(0,0,20));
	al_flip_display();

#define CALC_POS_Y(x) ( 50+((x)*(TEXT_SIZE+2)) )
#define XPOS 25
#define TEXT_COLOUR 255, 255, 255

	al_draw_text(font, al_map_rgb(TEXT_COLOUR), XPOS, CALC_POS_Y(0), 0, "Conway's life");
	al_draw_text(font, al_map_rgb(TEXT_COLOUR), XPOS, CALC_POS_Y(1), 0, "This is a program that simulates a living group of cells.");
	al_draw_text(font, al_map_rgb(TEXT_COLOUR), XPOS, CALC_POS_Y(2), 0, "Alive cells (yellow circles) die from isolation when they have 0-1 neighbours");
	al_draw_text(font, al_map_rgb(TEXT_COLOUR), XPOS, CALC_POS_Y(3), 0, "They also die of overcrowd when they have 4 or more neighbours.");
	al_draw_text(font, al_map_rgb(TEXT_COLOUR), XPOS, CALC_POS_Y(4), 0, "They becomes alive when they have exactly 3 neighbours.");
	al_draw_text(font, al_map_rgb(TEXT_COLOUR), XPOS, CALC_POS_Y(5), 0, "Press ANY KEY to start.");
	al_flip_display();

	while(1)
	{
		al_flush_event_queue(event_queue);
		al_wait_for_event(event_queue, &ev);
		if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE || (ev.type == ALLEGRO_EVENT_KEY_DOWN && (ev.keyboard.keycode == ALLEGRO_KEY_Q || ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)))
			goto program_exit;
		else
			al_flush_event_queue(event_queue); 

		draw_in_console(generation_number);
		recalculate_environment();
		generation_number++;

		if(alive_cells_number == 0)
		{
			printf("\n  All cells are dead.\n  Press q to quit.");
			if(_getch() == 'q')
				break;
		}
		else
		{
			alive_cells_number = 0;
			printf("\nPress any key to continue to next generation\nor press q to quit");
		}
	}

program_exit:
	al_destroy_event_queue(event_queue);
	al_destroy_display(display);
	return 0;
}

//TODO: dynamic window size, dynamic grid size, avoid console window, move all text into allegro window