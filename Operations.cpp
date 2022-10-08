#include <cstdlib>
#include "Operations.h"

inline float rand01()
{
	return (float)((double)rand() / (double)(RAND_MAX+1));
}

void RandomThickness(int width, int height, std::vector<float>& thickness)
{
	size_t num_pix = (size_t)width * (size_t)height;
	for (size_t i = 0; i < num_pix; i++)
	{
		thickness[i] = rand01();
	}
}
