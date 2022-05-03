/*
 * circular_buffer.c
 * Mostly gleaned from Embedded Artistry:
 * https://embeddedartistry.com/blog/2017/05/17/creating-a-circular-buffer-in-c-and-c/
 *
 *  Created on: May 3, 2022
 *      Author: Sam
 */

// The hidden definition of our circular buffer structure
struct circular_buf_t {
  uint8_t *buffer;
  size_t head;
  size_t tail;
  size_t max; //of the buffer
  bool full;
};

/*
 * We will be returning a handle to a structure that is allocated inside of the
 * library. Once we've created our container, we need populate the values and
 * call reset on it. Before we return from init, we ensure that the buffer
 * container has been created in an empty state.
 */
cbuf_handle_t circular_buf_init(uint8_t *buffer, size_t size) {
  assert(buffer && size);

  cbuf_handle_t cbuf = malloc(sizeof(circular_buf_t));
  assert(me);

  cbuf->buffer = buffer;
  cbuf->max = size;
  circular_buf_reset(cbuf);

  assert(circular_buf_empty(cbuf));

  return cbuf;
}

/*
 * The purpose of the reset function is to put the buffer into an "empty"
 * state, which requires updating head, tail, and full.
 */
void circular_buf_reset(cbuf_handle_t me) {
  assert(me);

  me->head = 0;
  me->tail = 0;
  me->full = false;
}

/*
 * Since we have a method to create a circular buffer container, we need an
 * equivalent method for destroying the container. In this case, we call free
 * on our container. We do not attempt to free the underlying buffer, since we
 * do not own it.
 */
void circular_buf_free(cbuf_handle_t me) {
  assert(me);
  free(me);
}

/*
 * State check: is full
 */
bool circular_buf_full(cbuf_handle_t me) {
  assert(me);

  return me->full;
}

/*
 * State check: is empty
 */
bool circular_buf_empty(cbuf_handle_t me) {
  assert(me);

  return (!me->full && (me->head == me->tail));
}

/*
 * The capacity of our buffer was supplied during initialization, so we just
 * return that value to the user.
 */
size_t circular_buf_capacity(cbuf_handle_t me) {
  assert(me);

  return me->max;
}

/*
 * If the buffer is full, we know that our capacity is at the maximum. If head
 * is greater-than-or-equal-to the tail, we simply subtract the two values to
 * get our size. If tail is greater than head, we need to offset the difference
 * with max to get the correct size.
 */
size_t circular_buf_size(cbuf_handle_t me) {
  assert(me);

  size_t size = me->max;

  if (!me->full) {
    if (me->head >= me->tail) {
      size = (me->head - me->tail);
    } else {
      size = (me->max + me->head - me->tail);
    }
  }

  return size;
}

/*
 * Advance pointer, used when adding data.
 */
static void advance_pointer(cbuf_handle_t me) {
  assert(me);

  if (me->full) {
    if (++(me->tail) == me->max) {
      me->tail = 0;
    }
  }

  if (++(me->head) == me->max) {
    me->head = 0;
  }
  me->full = (me->head == me->tail);
}

/*
 * Retreat pointer, used when removing data.
 */
static void retreat_pointer(cbuf_handle_t me) {
  assert(me);

  me->full = false;
  if (++(me->tail) == me->max) {
    me->tail = 0;
  }
}

/*
 * Put, version 1; continues to add data if the buffer is full
 * Old data is overwritten
 */
void circular_buf_put(cbuf_handle_t me, uint8_t data) {
  assert(me && me->buffer);

  me->buffer[me->head] = data;

  advance_pointer(me);
}

/*
 * Put, version 2; rejects new data if the buffer is full
 * Returns 0 on success, -1 if buffer is full
 * This is provided for demonstration purposes, not used in our systems.
 */
int circular_buf_put2(cbuf_handle_t me, uint8_t data) {
  int r = -1;

  assert(me && me->buffer);

  if (!circular_buf_full(me)) {
    me->buffer[me->head] = data;
    advance_pointer(me);
    r = 0;
  }

  return r;
}

/*
 * To remove data from the buffer, we access the value at the tail and then
 * update the tail pointer. If the buffer is empty we do not return a value or
 * modify the pointer. Instead, we return an error to the user.
 */
int circular_buf_get(cbuf_handle_t me, uint8_t *data) {
  assert(me && data && me->buffer);

  int r = -1;

  if (!circular_buf_empty(me)) {
    *data = me->buffer[me->tail];
    retreat_pointer(me);

    r = 0;
  }

  return r;
}
