#ifndef sfml_raycaster_cCoordinates_h
#define sfml_raycaster_cCoordinates_h

struct cCoordinates {
	float x, y;
	bool wallHit;
	float distance;
	cCoordinates(): x { 0 }, y { 0 }, wallHit { false }, distance { 0 } { };
	cCoordinates(float iksz, float ipsz): x  { iksz },  y { ipsz } { };
};

#endif
