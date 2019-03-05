struct POS {
  uint8_t x;
  uint8_t y;
};

/* Tile structure
 * active = is this tile currently active
 * addr   = address of the current tile
 * posX   = position of tile in the X direction
 * posY   = position of tile in the Y direction
 * ports  = state of the directional pins
 * ports_pre = state of the directional pins in the previous instance 
*/

struct TILE {
  bool  active;
  int   addr;
  POS   pos;
  int   ports;
  int   ports_pre;
};
