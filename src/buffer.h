#ifndef BUFFER_H
#define BUFFER_H

struct Line {
	int size;
	int pos;
	int capacity;
	char *data;
};

struct Buffer {
	int size;
	int pos;
	int pref_line_pos;
	int capacity;
	struct Line **data;
};

struct Line *line_new();
void line_free(struct Line *line);
void line_append(struct Line *line, char c);
int line_move_forward(struct Line *line);
int line_move_backward(struct Line *line);
void line_set(struct Line *line, int index, char c);
void line_add(struct Line *line, char c);
int line_backspace(struct Line *line);

struct Buffer *buffer_new();
void buffer_free(struct Buffer *buf);
void buffer_add(struct Buffer *buf, char c);
void buffer_new_line(struct Buffer *buf);
int buffer_backspace(struct Buffer *buf);
void buffer_move_forward(struct Buffer *buf);
void buffer_move_backward(struct Buffer *buf);
void buffer_move_up(struct Buffer *buf);
void buffer_move_down(struct Buffer *buf);

#endif
