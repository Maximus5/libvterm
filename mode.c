#include "ecma48_internal.h"

#include <stdio.h>

void ecma48_state_initmodes(ecma48_t *e48)
{
  ecma48_mode mode;
  for(mode = ECMA48_MODE_NONE; mode < ECMA48_MODE_MAX; mode++) {
    int val = 0;

    switch(mode) {
    case ECMA48_MODE_DEC_CURSORBLINK:
    case ECMA48_MODE_DEC_CURSORVISIBLE:
      val = 1;
      break;

    default:
      break;
    }

    ecma48_state_setmode(e48, mode, val);
  }
}

void ecma48_state_setmode(ecma48_t *e48, ecma48_mode mode, int val)
{
  ecma48_state_t *state = e48->state;

  int done = 0;
  if(state->callbacks && state->callbacks->setmode)
    done = (*state->callbacks->setmode)(e48, mode, val);

  switch(mode) {
  case ECMA48_MODE_NONE:
  case ECMA48_MODE_MAX:
    break;

  case ECMA48_MODE_KEYPAD:
    e48->mode.keypad = val;
    break;

  case ECMA48_MODE_DEC_CURSOR:
    e48->mode.cursor = val;
    break;

  case ECMA48_MODE_DEC_CURSORBLINK:
    e48->mode.cursor_blink = val;
    break;

  case ECMA48_MODE_DEC_CURSORVISIBLE:
    e48->mode.cursor_visible = val;
    break;

  case ECMA48_MODE_DEC_ALTSCREEN:
    /* Only store that we're on the alternate screen if the usercode said it
     * switched */
    if(done)
      e48->mode.alt_screen = val;
    if(done && val) {
      if(state->callbacks && state->callbacks->erase) {
        ecma48_rectangle_t rect = {
          .start_row = 0,
          .start_col = 0,
          .end_row = e48->rows,
          .end_col = e48->cols,
        };
        (*state->callbacks->erase)(e48, rect, state->pen);
      }
    }
    break;

  case ECMA48_MODE_DEC_SAVECURSOR:
    e48->mode.saved_cursor = val;
    if(val) {
      state->saved_pos = state->pos;
    }
    else {
      ecma48_position_t oldpos = state->pos;
      state->pos = state->saved_pos;
      if(state->callbacks && state->callbacks->movecursor)
        (*state->callbacks->movecursor)(e48, state->pos, oldpos, e48->mode.cursor_visible);
    }
    break;

  }
}