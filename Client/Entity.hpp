#ifndef ENTITY_H
#define ENTITY_H

enum struct Place { deck, hand, field };

struct CardState
{
	Place place;
};

class Card
{
public:
	float x = 705;
	float y = 550;
	float width = 50;
	float height = 70;

	bool isTargeting = false;
	float targetX = 0.0f;
	float targetY = 0.0f;
	bool shouldInterpolate = true;

	Place place = Place::deck;
	bool isSelected = false;

	CardState state{ };
	CardState statePrev{ };

	Card();
	Card(float x, float y);

	void SetPosition(float x, float y);
	void SetTarget(float x, float y);
	void SetSelection(bool shouldSelect);
	void SetPlace(Place place);

	bool CheckPointInBody(float x, float y);
	bool CheckBodyOnField();

	Place Use();

	void Update(float dt);
	void Render(float t);

	CardState Interpolate(float t);
};

#endif