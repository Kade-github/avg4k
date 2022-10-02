#include "AvgWheel.h"
#include "MainerMenu.h"

void AvgWheel::mouseWheel(float amount)
{
	if (wheelObjects.size() == 0 || MainerMenu::lockInput)
		return;

	int mx, my;

	Game::GetMousePos(&mx, &my);

	mx -= parent->x;
	my -= parent->y;

	if (mx > x && my > y && mx < x + w && my < y + h)
	{

		if (amount > 0)
			selectedIndex--;
		if (amount < 0)
			selectedIndex++;


		if (selectedIndex < 0)
			selectedIndex = wheelObjects.size() - 1;
		else if (selectedIndex > wheelObjects.size() - 1)
			selectedIndex = 0;

		callSelect(selectedIndex);
	}
}