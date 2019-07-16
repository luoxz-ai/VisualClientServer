#include "../../stdafx.h"
#include "NetworkData.h"

bool CompareBulletsById(stBulletInfo A, stBulletInfo B)
{
	return A.Id < B.Id;
}
