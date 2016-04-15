#include "mcv_platform.h"

#include "comp_tracker.h"
#include "entity.h"
#include "entity_parser.h"

void TCompTracker::onCreate(const TMsgEntityCreated &)
{
	//Pruebas
	CHandle boxTest = IdEntities::findById(1);
	if (boxTest.isValid()) {
		CEntity* e = boxTest;
		dbg("---------- Tracker -----------------------------------\n");
		dbg("Añado entidad al tracker: %s\n", e->getName());
		dbg("Longitude = %f\n", longitude);
		for (int i = 0; i < size; i++) {
			dbg("point%d: (%f, %f, %f)\n", i, positions[i].x, positions[i].y, positions[i].z);
			dbg("tangent%d: (%f, %f, %f)\n", i, orientations[i].x, orientations[i].y, orientations[i].z);
		}
		dbg("---------- /Tracker -----------------------------------\n");
	}
}

bool TCompTracker::load(MKeyValue& atts) {
	// Size & Longitude
	longitude = atts.getFloat("long", -1);
	assert(longitude > 0);
	size = atts.getInt("size", -1);
	assert(size > 0);
	assert(size <= MAX_TRACK_POINTS);

	//Positions & Orientations
	char nameAttr[20];
	for (int i = 0; i < size; i++) {
		//Position
		sprintf(nameAttr, "point%d", i);
		positions[i] = atts.getPoint(nameAttr);
		
		//Orientation
		sprintf(nameAttr, "tangent%d", i); // Quaternions!?
		orientations[i] = atts.getPoint(nameAttr);
	}

	return true;
}

void TCompTracker::update(float elapsed) {
	//TODO
}