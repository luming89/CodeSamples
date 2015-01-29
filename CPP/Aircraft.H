#include <string>
#include "../GrTown_PCH.H"
#include "../DrawUtils.H"
#include "Utilities/Texture.H"
#include "../DrawingState.H"
#include "../GrObject.H"
#include "../mesh.h"
#include "../Macros.h"
#include "Utilities/ShaderTools.H"

using namespace std;

class Texture;

class Aircraft : public GrObject 
{
public:

	Aircraft(char* name,int num_order = 0);

	virtual void draw(DrawingState*);
	virtual void getCamera(Matrix camera);


};

class Airship : public Aircraft
{
public:
	Airship(char* name, const std::string& fileName = "Procedure");
	virtual void draw(DrawingState*);
private:
	string texture;
	int longitudes;
	int latitudes;
	float count;
};

class Helicopter : public Aircraft
{
public:
	Helicopter(char* name, bool ridable=true);
	virtual void draw(DrawingState*);
	virtual void getCamera(Matrix camera);
	float bladeRotation; // Blade rotating speed
	long speed; 
private:
	void drawWing();
	void drawBody();
	void drawTale();
	void drawTopBlades(boolean wingWaighted);
	void drawBlad(boolean wingWaighted);
	void drawWings();
};

class Fighter : public Aircraft
{
public:
	Fighter(char* name, const std::string& fileName, const std::string& tex, int rideAbove = 25, float scale = 1.0);
	Fighter::~Fighter();
	virtual void draw(DrawingState*);
	virtual void getCamera(Matrix camera);
	virtual int handle(int e);
private:
	Mesh* mesh;
	string fileName;
	IndexedModel STDARM; // The missile model
	int missileSlot;
};

class Bullet : public GrObject
{
public:
	Bullet(char* name);
	Bullet::~Bullet();
	virtual void draw(DrawingState*);
private:
	Mesh* mesh;
	string fileName;
	float scale;

};

class Missile : public GrObject
{
public:
	Missile(char* name, const std::string& tex, IndexedModel& model, float ertialSpeed);
	Missile::~Missile();
	virtual void draw(DrawingState*);
	inline float getInertialSpeed() const { return inertialSpeed; }
private:
	Mesh* mesh;
	string fileName;
	float scale;
	float inertialSpeed;
};
