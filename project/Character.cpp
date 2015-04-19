// Charcter.cpp
// Implementation of the Character class. Contains definitions for all member functions.

#include "Character.h"
#include <iostream>
#include <string>
using namespace std;
// default constructor (calls non-default of GamePiece which sets xpos and ypos to 0)
Character::Character(){
  direction = 0;			// direction starts at 0
  phase = 1;				// phase starts at 1 (stand still)
  character_texture = NULL;		// no image
  counter = 0;
  alive = 0;				// with non-default, make the character dead
  selected = 0;
}

// non-default construtor. Calls non-default of GamePiece to set xpos and ypos to passed in values
Character::Character(string path,int x, int y, SDL_Renderer* renderer,vector<vector <int> > tile_prop):GamePiece(x,y), vb(tile_prop[0].size(),tile_prop.size(),"../media/blue_highlight.png","../media/red_highlight.png",renderer){
  SDL_Surface* loadedSurface = IMG_Load(path.c_str());				// loads the image into character_texture
  character_texture = SDL_CreateTextureFromSurface(renderer,loadedSurface);
  direction = 0;								// direction starts at 0
  phase = 1;									// phase starts at 1 (stand still phase)
  SDL_FreeSurface(loadedSurface);						// frees the surface
  loadedSurface = NULL;								// grounds pointer
  counter = 0;
  alive = 1;									// indicates the character is alive
  selected = 0;
  tile_properties = tile_prop;
}

// deconstructor
Character::~Character(){
  if(character_texture!=NULL){				// if the character_texture isn't already Null,
    SDL_DestroyTexture(character_texture);		// destroy it
    character_texture = NULL;				// ground the pointer
  }
  direction = 0;				
  phase = 0;
}

// draw function. QUESTIONS ABOUT THIS: HOW CAN WE MAKE HERO USE THIS SAME FUNCTION
void Character::draw(SDL_Renderer* renderer){
  if(alive == 0){
    return;
  }
  if(selected == 1){
    vb.draw(renderer);
  }
  SDL_Rect destRect = {xpos*32, ypos*32, 32, 32};			// destination rectangle. Based on xpos and ypos of GamePiece
  SDL_Rect clipRect = {phase*32, direction*32, 32, 32};			// source rect. Clips the sprite sheet based on the phase.
  SDL_RenderCopy(renderer, character_texture, &clipRect, &destRect);	// draws it to the renderer that was passed in
}

// function to set the direction of the Character
void Character::change_direction(int face_direction){
  switch(face_direction){				// changed direction: 0 is up, 1 is right, 2 is down, and 3 is left
    case 0:
      direction=3;
      break;
    case 1:
      direction=2;
      break;
    case 2:
      direction=0;
      break;
    case 3:
      direction=1;
      break;
  }
}

// function to set phase
void Character::next_phase(){
  phase++;			// increments phase variable
  if(phase > 2) phase = 0;	// if it is greater than 2, reset to 0
}

// function that returns the current phase of the sprite
int Character::get_phase(){
	return phase;
}

// function that takes in a vector of vector and returns it populated with 1s where a character can move
void Character::check_valid_move(int x, int y, int movement_remaining){
// BE CAREFUL WITH COORDINATE SYSTEM - IT IS SLIGHTLY CONFUSING WITH GRAPHICS COORDINATES AND REGULAR VECTOR ARGUMENTS
  if(movement_remaining < 0){  	// if the character is out of movement
    return;		
  }
  if(terrain_effect[tile_properties[y][x]]==0) 
    return;							// if the character can't move onto the x,y coordinate, end function
  vb.set_tile(1,y,x);						// if it makes it through checks, the position is valid
  // repeat in all direction (recurrsion)
  if(y-1 >= 0){
    check_valid_move(x,y-1,movement_remaining-terrain_effect[tile_properties[y-1][x]]);
  }
  if(x+1 < tile_properties.size()){
    check_valid_move(x+1,y,movement_remaining-terrain_effect[tile_properties[y][x+1]]);
  }
  if(y+1 < tile_properties[0].size()){
    check_valid_move(x,y+1,movement_remaining-terrain_effect[tile_properties[y+1][x]]);
  }
  if(x-1 >= 0){
    check_valid_move(x-1,y,movement_remaining-terrain_effect[tile_properties[y][x-1]]);
  }
}

// updates counter of the character to determine when to change phase
void Character::update(){
  if(hitpoints==0){
    alive = 0;
  }
  if(selected==1){
    counter++;
    if(counter == 15){		// changes phase and resets every 15 counts
      counter = 0;
      next_phase();
    }
    vb.add_attack_spots(attack_range);
  }
}
  
//function that return's the character's current hitpoints
int Character::getHitpoints(){
  return hitpoints;
}

// function that sets the character's hitpoints
void Character::setHitpoints(int new_hp){
  if(new_hp < 0) new_hp = 0;
  hitpoints = new_hp;
}

// function to get character's movement
int Character::getMobility(){
  return mobility;
}

// returns the terrain's effect on mobility
int Character::get_terrain_effect(int tile){
  if(tile < 0) return -1;
  return terrain_effect[tile];
}

// changes selected data member to 1
void Character::select(){
  selected = 1;
}

// changes selected data member to 0
void Character::unselect(){
  selected = 0;
  phase = 1;
  direction = 0;
}

int Character::get_select(){
    return selected;
}

// functio to move the character
void Character::move(int change, int max_width, int max_height){
  switch(change){
    case(0):
      change_direction(0);
      if(ypos==0) return;
      else ypos = ypos-1;
      break;
    case(1):
      change_direction(1);
      if(xpos == max_width-1) return;
      else xpos = xpos+1;
      break;
    case(2):
      change_direction(2);
      if(ypos == max_height-1) return;
      else ypos = ypos+1;
      break;
    case(3):
      change_direction(3);
      if(xpos==0) return;
      else xpos = xpos-1;
      break;
  }
}

// function that moves a series of direction that are in a vector
void Character::process_move_vector(vector<int> moves,int max_width,int max_height){
  for(int i = 0; i<moves.size(); i++){
    move(moves[i],max_width,max_height);
  }
}
