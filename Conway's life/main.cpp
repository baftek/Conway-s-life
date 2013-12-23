/* 
	 Fundamentals of Computer Programming 
	 Silesian University of Technology <http://polsl.pl/> 
	 Program: (2) Conway's life
	 Author: Bartłomiej Wolny <bartwol206@student.polsl.pl>
	 Tutor: Piotr Fabian
	 Copyright (c) 2013, Bartlomiej Wolny. All rights reserved. 
*/
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <allegro5\allegro5.h>
#include <allegro5\allegro_native_dialog.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_FONT *font = NULL;

#define CALC_POS_Y(x) ( 50+((x)*(TEXT_SIZE+2)) )
#define XPOS 25 // x axis position of welcome text
#define TEXT_COLOR 255, 255, 255

#define CURRENT_ARRAY (!(generation_number%2))

int total_rows=0;
int total_columns=0;
char ***cell_map; 
char should_borders_be_considered = 1;
char file_not_found = 0;
char filename[] = "input.txt";
char *pointer_to_filename = filename;

int alive_cells_number = 0;
int generation_number = 1;

int check_ndb(int row, int column)
{
	// neighbours checking order:
	// 123
	// 4#5
	// 678
	int nbd_counter=0;

	// if checking should not exceed borders it's enough to leave only the false fragment in defines
#define ROW_CHECK_MINUS (row==0 ? total_rows-1 : row-1) // if row==0 then we must also check last row
#define COL_CHECK_MINUS (column==0 ? total_columns-1 : column-1)
#define ROW_CHECK_PLUS (row+1==total_rows ? 0 : row+1) // if row==total_columns then we must also check 0'th row
#define COL_CHECK_PLUS (column+1==total_columns ? 0 : column+1)

	if(cell_map[CURRENT_ARRAY][ROW_CHECK_MINUS][COL_CHECK_MINUS]==1 ) nbd_counter++;
	if(cell_map[CURRENT_ARRAY][ROW_CHECK_MINUS][column  ]==1 ) nbd_counter++;
	if(cell_map[CURRENT_ARRAY][ROW_CHECK_MINUS][COL_CHECK_PLUS]==1 ) nbd_counter++;
	if(cell_map[CURRENT_ARRAY][row  ][COL_CHECK_MINUS]==1 ) nbd_counter++;
	if(cell_map[CURRENT_ARRAY][row  ][COL_CHECK_PLUS]==1 ) nbd_counter++;
	if(cell_map[CURRENT_ARRAY][ROW_CHECK_PLUS][COL_CHECK_MINUS]==1 ) nbd_counter++;
	if(cell_map[CURRENT_ARRAY][ROW_CHECK_PLUS][column  ]==1 ) nbd_counter++;
	if(cell_map[CURRENT_ARRAY][ROW_CHECK_PLUS][COL_CHECK_PLUS]==1 ) nbd_counter++;

	return nbd_counter;
}

///////////////////////////////////
#define GRID_SIZE 15
#define POSITION_OFFSET 50
int draw_result(int generation_number)
{
	system("cls");
	printf("Conway's life			  GENERATION: %d\n\n	", generation_number);

	al_clear_to_color(al_map_rgb(0,0,20));
	al_draw_rectangle(POSITION_OFFSET, POSITION_OFFSET, total_columns*GRID_SIZE+POSITION_OFFSET, total_rows*GRID_SIZE+POSITION_OFFSET, al_map_rgb(255, 255, 255), 1);
	for(int i=1; i<total_columns; i++)
		al_draw_line(POSITION_OFFSET+i*GRID_SIZE, POSITION_OFFSET, POSITION_OFFSET+i*GRID_SIZE, total_rows*GRID_SIZE+POSITION_OFFSET, al_map_rgb(25, 255, 100), 1);
	for(int i=1; i<total_rows; i++)
		al_draw_line(POSITION_OFFSET, POSITION_OFFSET+i*GRID_SIZE, total_columns*GRID_SIZE+POSITION_OFFSET, i*GRID_SIZE+POSITION_OFFSET, al_map_rgb(25, 255, 100), 1);
	al_draw_textf(font, al_map_rgb(TEXT_COLOR), 50, 25, 0, "Conway's life by baftek        Generation %d", generation_number);

	for(int i=0; i < total_rows; i++)
	{
		for(int j=0; j < total_columns; j++)
		{
			if(cell_map[CURRENT_ARRAY][i][j])
				//al_draw_textf(font, al_map_rgb(255, 255, 0), 50+j*GRID_SIZE, 50+i*GRID_SIZE-3, 0, "%d", check_ndb(i, j));
				al_draw_filled_circle(50+(GRID_SIZE/2)+j*GRID_SIZE, 50+(GRID_SIZE/2)+i*GRID_SIZE, 5, al_map_rgb(255, 255, 0));
			else
				al_draw_filled_circle(50+(GRID_SIZE/2)+j*GRID_SIZE, 50+(GRID_SIZE/2)+i*GRID_SIZE, 5, al_map_rgb(80, 50, 50));
		}
	}

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

	if(alive_cells_number == 0 && generation_number != 1)
		al_draw_text(font, al_map_rgb(255, 220, 220), 50, total_rows*GRID_SIZE+POSITION_OFFSET+50, 0, "All cells are dead. Press Q or escape to quit.");
	else
	{
		alive_cells_number = 0;
		printf("\nPress any key to continue to next generation\nor press q to quit");
		al_draw_text(font, al_map_rgb(TEXT_COLOR), 50, (total_rows*GRID_SIZE+POSITION_OFFSET+50), 0, "Press any key to continue to next generation or Q to quit.");
	}

	al_flip_display();
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
int read_starting_positions_from_file(int argc)
{
	FILE *f;
	if(file_not_found = !(f = fopen(pointer_to_filename, "r")))
	{
		printf("\n\n\n   Could not open the file %s\n\n   Random values will be generated.\n", pointer_to_filename);
		//srand(time(NULL));	// init of rand
		total_columns = 45; //rand() % 15 + 30;
		total_rows = 30;	// rand() % 25 + 10;
		printf("\n   Generated numbers\n   columns = %d\n   rows = %d", total_columns, total_rows);
		//return 0;
	}
	else //file exists
	{
		total_columns = 0, total_rows = 0;
	
		// count columns
		while(fgetc(f) != '\n')
			total_columns++;
		fseek(f, 0, SEEK_SET);

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

		if(!file_not_found) // if found
		{
			// reading file and saving data to array
			f = fopen(pointer_to_filename, "r"); 
			for(int current_row=0; current_row < total_rows; current_row++) // in every row
			{	
				for(int current_column=0; current_column < total_columns; current_column++) // for every column
					cell_map[CURRENT_ARRAY][current_row][current_column] = fgetc(f)-48;

				fgetc(f); //omit of '\n'
			}
			fclose(f);
		}
		else //random cell values generating
		{
			for(int current_row=0; current_row < total_rows; current_row++) // in every row
			{	
				for(int current_column=0; current_column < total_columns; current_column++) // for every column
					cell_map[CURRENT_ARRAY][current_row][current_column] = rand() % 2;
			}
		}

	return 1;
}

int main(int argc, char *argv[])
{	
	if(argc > 1)
		pointer_to_filename = argv[1];
	//printf("\n\n	Conway's life by baftek\n\n	This is a program that simulates a living group of cells.\n	Dots are dead cells, hashes are alive cells.\n	Alive cells die from isolation when they have 0-1 neighbours\n	They also die of overcrowd when they have 4 or more neighbours.\n	They becomes alive when they have exactly 3 neighbours.\n	Press ANY KEY to start. New window will appear.");
	read_starting_positions_from_file(argc);
 
	if(!al_init()) 
	{
		al_show_native_message_box(display, "Error", "Error", "Failed to initialize allegro!", 
                                 NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}

	display = al_create_display(800, 600);
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
	al_flush_event_queue(event_queue);

	al_init_font_addon();
	al_init_ttf_addon();
#define TEXT_SIZE 17
	font = al_load_ttf_font("C:\\Windows\\Fonts\\cour.ttf", TEXT_SIZE, 0);
	if(!font)
	{
		al_destroy_event_queue(event_queue);
		al_show_native_message_box(display, "Error", "Error", "Failed to initialize font!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		al_destroy_display(display);
		return -1;
	}

	al_install_keyboard();
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	ALLEGRO_EVENT ev;
 
	al_init_primitives_addon();
	al_clear_to_color(al_map_rgb(0,0,20));
	al_flip_display();

	al_draw_text(font, al_map_rgb(TEXT_COLOR), XPOS, CALC_POS_Y(0), 0, "Conway's life");
	al_draw_text(font, al_map_rgb(TEXT_COLOR), XPOS, CALC_POS_Y(1), 0, "This is a program that simulates a living group of cells.");
	al_draw_text(font, al_map_rgb(TEXT_COLOR), XPOS, CALC_POS_Y(2), 0, "Alive cells (yellow circles) die from isolation when they have 0-1 neighbours");
	al_draw_text(font, al_map_rgb(TEXT_COLOR), XPOS, CALC_POS_Y(3), 0, "They also die of overcrowd when they have 4 or more neighbours.");
	al_draw_text(font, al_map_rgb(TEXT_COLOR), XPOS, CALC_POS_Y(4), 0, "They becomes alive when they have exactly 3 neighbours.");
	al_draw_text(font, al_map_rgb(TEXT_COLOR), XPOS, CALC_POS_Y(6), 0, "Press ANY KEY to start.");
	if(file_not_found)
		al_draw_textf(font, al_map_rgb(TEXT_COLOR), XPOS, CALC_POS_Y(10), 0, "No desired file found, generated random data:");
	else
		al_draw_textf(font, al_map_rgb(TEXT_COLOR), XPOS, CALC_POS_Y(10), 0, "File found: ");
		al_draw_textf(font, al_map_rgb(TEXT_COLOR), XPOS, CALC_POS_Y(11), 0, "%d rows, %d columns", total_rows, total_columns);

	al_flip_display();

	while(1)
	{
		al_flush_event_queue(event_queue);
		al_wait_for_event(event_queue, &ev);
		if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE || (ev.type == ALLEGRO_EVENT_KEY_DOWN && (ev.keyboard.keycode == ALLEGRO_KEY_Q || ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)))
			break;  //exit program

		while(ev.type != ALLEGRO_EVENT_KEY_UP)
			al_wait_for_event(event_queue, &ev);
			
		draw_result(generation_number);
		recalculate_environment();
		generation_number++;
	}

	al_destroy_event_queue(event_queue);
	al_destroy_display(display);
	return 0;
}