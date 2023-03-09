#ifndef ENTITY_H
#define ENTITY_H

class Entity
{
public:
	int x = 100;
	int y = 100;
	int xPrev = 100;
	int yPrev = 100;

	void Render(float t);
};

#endif