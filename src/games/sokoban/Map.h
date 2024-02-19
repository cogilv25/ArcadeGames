#pragma once
#include <stdio.h>
#include <memory>

#define SOKOBAN_FLOOR 0
#define SOKOBAN_PLAYER 1
#define SOKOBAN_DIAMOND 2
#define SOKOBAN_CRATE 3
#define SOKOBAN_WALL 4
#define SOKOBAN_CRATE_ON_DIAMOND 5
#define SOKOBAN_PLAYER_ON_DIAMOND 6


struct Map
{
	bool valid;
	unsigned int xOff, yOff, width, height;
	unsigned int nObjectives;
	unsigned char* data;
};
Map createMap(const char* path)
{
	FILE* file = fopen(path, "rb");
	if (file == NULL)
	{
		fprintf(stderr, "Unable to load map at: %s\n", path);
		return Map{ false };
	}

	Map map{ true };
	if (fscanf(file, "%d,%d", &map.width, &map.height) != 2)
	{
		fprintf(stderr, "Invalid map header at: %s\n", path);
		fclose(file);
		return Map{ false };
	}

	map.data = (unsigned char*)malloc(map.width * map.height);
	char c = 0;
	int n = 0;
	while (c >= 0)
	{
		if (n >= map.width * map.height)
			break;

		c = fgetc(file);
		if (c < 65 || c > 90)
		{
			if (c == 32) map.data[n++] = SOKOBAN_FLOOR;
			continue;
		}

		switch (c)
		{
		case 'P':
			map.data[n] = SOKOBAN_PLAYER;
			break;
		case 'D':
			map.data[n] = SOKOBAN_DIAMOND;
			++map.nObjectives;
			break;
		case 'C':
			map.data[n] = SOKOBAN_CRATE;
			break;
		case 'W':
			map.data[n] = SOKOBAN_WALL;
			break;
		}
		++n;
	}

	fclose(file);

	if (n != map.width * map.height)
	{
		fprintf(stderr, "Map dimensions incorrect, continuing..\n");
		map.valid = false;
	}

	return map;
}

void destroyMap(Map& map)
{
	free(map.data);
}
