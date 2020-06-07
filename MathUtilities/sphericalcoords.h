#ifndef _SPHERICALCOORDS_H
#define _SPHERICALCOORDS_H

class SphericalCoords
{
public:
	float rho, phi, theata;

	SphericalCoords()
	{
		rho = 0;
		phi = 0;
		theata = 0;
	}

	SphericalCoords(float newRho, float newPhi, float newTheata)
	{
		rho = newRho; phi = newPhi; theata = newTheata;
	}
};

#endif