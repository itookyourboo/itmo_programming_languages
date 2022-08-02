#ifndef SEPIA_FILTER_H
#define SEPIA_FILTER_H

struct image *sepia_image(struct image *input_image);
struct image *sepia_image_sse(struct image *input_image);

#endif //SEPIA_FILTER_H
