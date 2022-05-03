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
