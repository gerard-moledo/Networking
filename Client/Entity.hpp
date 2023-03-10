#ifndef ENTITY_H
#define ENTITY_H

enum struct Place { deck, hand, field };

struct EntityState
{
	float x;
	float y;
};

class Entity
{
public:
	float x;
	float y;
	float width = 50;
	float height = 70;

	bool isTargeting = false;
	float targetX;
	float targetY;
	float tTarget = 0.0f;

	Place place = Place::deck;
	bool isSelected = false;

	EntityState state;
	EntityState statePrev;

	Entity(float x, float y);

	void SetPosition(float x, float y);
	bool CheckPointInBody(float x, float y);
	bool CheckBodyOnField();

	void SetTarget(float x, float y);
	void PlaceOnField(float* fieldX, float* fieldY);

	void Update(float dt);
	void Render(float t);

	EntityState Interpolate(float t);
};

#endif