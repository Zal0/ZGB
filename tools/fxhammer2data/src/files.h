#ifndef _FILES_H
#define _FILES_H

uint8_t * file_read_into_buffer(char * filename, size_t *ret_size);
bool file_write_from_buffer(char * filename, uint8_t * p_buf, size_t data_len);

#endif // _FILES_H
