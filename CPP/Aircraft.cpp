#include "Aircraft.H"
#include "SimpleBehaviors.H"
#include "Objects.H"

//#include <string>

using namespace std;

Aircraft::Aircraft(char* name, int num_order) : GrObject(name, num_order)
{
	rideAbove = AIRSHIP_HEIGHT;
	ridable = 1; 
	destroyable = true;
}

void Aircraft::draw(DrawingState* d)
{

}

void Aircraft::getCamera(Matrix camera) // override GrObject's get Camera
{

	Matrix tmpTransform;
	Matrix t1, t2, t3; // adjust the camera
	Matrix delta, newT;
	Matrix t4, t5, t6; // adjust the transform



	rotMatrix(t4, 'Y', -direction); // adjust the transform
	multMatrix(t4, transform, t6);	// t3 is the result

	rotMatrix(t5, 'X', pitch);
	multMatrix(t5, t6, transform);

	direction = 0;
	pitch = 0;

	transform[3][0] -= transform[2][0] * (camForw - camBack);
	transform[3][1] -= transform[2][1] * (camForw - camBack);
	transform[3][2] -= transform[2][2] * (camForw - camBack);
	camForw = 0;
	camBack = 0;

	copyMatrix(transform, tmpTransform); // adjust the camera

	tmpTransform[3][0] += transform[2][0] * 40;
	tmpTransform[3][1] = tmpTransform[3][1] + transform[2][1] * 40 + rideAbove;
	tmpTransform[3][2] += transform[2][2] * 40;


	copyMatrix(tmpTransform, camera); // copy transform to camera
	invertMatrix(camera); // what's this for?


}


Airship::Airship(char* name, const std::string& fileName) : Aircraft(name), count(0), texture(fileName)
{
	longitudes = AIRSHIP_LONGITUDE;
	latitudes = AIRSHIP_LATITUDE;
	direction = 2.5;
	pitch = 0.5;
#if DEBUG
	cout << "Airship created\n";
#endif
}

void Airship::draw(DrawingState* ds)//DrawingState* d)
{
	int textureSwitch = 1;
	Vertex vertices[AIRSHIP_LATITUDE][ AIRSHIP_LONGITUDE];
	float x, y, z, r;
	count += 0.1f;
	for (int latCount = 0; latCount<latitudes; latCount++)
	{ // along z axis
		for (int longCount = 0; longCount < longitudes; longCount++)
		{ // x and y
			float tmp1 = PI*latCount / (latitudes - 1); // ranges from 0 to PI
			float tmp2 = 2 * PI*longCount / (longitudes - 1); // ranges from 0 to 2*PI
			z = cos(tmp1) * AIRSHIP_LENGTH;
			r = sin(tmp1) * AIRSHIP_RADIUS;
			x = cos(tmp2)*r;// / 5;
			y = sin(tmp2)*r;// / 5;
			vertices[latCount][longCount] = Vertex(glm::vec3(x, y, z), glm::vec2(0, 0), glm::vec3(x, y, z));
		}
	}
#if DEBUG
	cout << "Draw airship\n";
#endif

	char* error; // not elegant
	GLuint program = loadShader("../Shaders/airship.vs", "../Shaders/airship.fs", error, NULL);
	if (texture == "Procedure")
	{
		textureSwitch = 0;
	}
	else
	{
		fetchTexture(texture.c_str());
	}
	
	//GLuint program = loadShader("airship.vs", "airship.fs", error);
	if (error)
		std::cerr << error << std::endl;
	glUseProgram(program);
	Vertex camera = Vertex(glm::vec3(0,0,0));
	GLint loc1 = glGetUniformLocation(program, "uCamera");
	glUniform3f(loc1, camera.pos.x, camera.pos.y, camera.pos.z);
	Vertex lightPos = Vertex(glm::vec3(0, 10000, 0));
	GLint loc2 = glGetUniformLocation(program, "uLightPos");
	glUniform3f(loc2, lightPos.pos.x, lightPos.pos.y, lightPos.pos.z);
	GLint loc3 = glGetUniformLocation(program, "uTimeOfDay");
	glUniform1i(loc3, ds->timeOfDay);
	GLint loc4 = glGetUniformLocation(program, "uTextureSwitch");
	glUniform1i(loc4, textureSwitch);
	glPushMatrix();
	glTranslatef(0, AIRSHIP_HEIGHT, 0);
	glRotatef(-90, 0, 0, 1);
	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_QUADS);
	for (int latCount = 0; latCount < latitudes-1; latCount++)
	{ // along z axis
		for (int longCount = 0; longCount < longitudes-1; longCount++)
		//for (int longCount = 0; longCount < longitudes-1; longCount+=2)
		{ 
			glTexCoord2f(((float)latCount) / (latitudes-1), ((float)longCount) / (longitudes-1));
			glNormal3f(vertices[latCount][longCount].normal.x, vertices[latCount][longCount].normal.y, vertices[latCount][longCount].normal.z);
			glVertex3f(vertices[latCount][longCount].pos.x, vertices[latCount][longCount].pos.y, vertices[latCount][longCount].pos.z);

			glTexCoord2f(((float)latCount) / (latitudes-1), ((float)(longCount+1)) / (longitudes-1));
			glNormal3f(vertices[latCount][longCount + 1].normal.x, vertices[latCount][longCount + 1].normal.y, vertices[latCount][longCount + 1].normal.z);
			glVertex3f(vertices[latCount][longCount + 1].pos.x, vertices[latCount][longCount + 1].pos.y, vertices[latCount][longCount + 1].pos.z);

			glTexCoord2f(((float)(latCount+1)) / (latitudes - 1), ((float)(longCount + 1)) / (longitudes - 1));
			glNormal3f(vertices[latCount + 1][longCount + 1].normal.x, vertices[latCount + 1][longCount + 1].normal.y, vertices[latCount + 1][longCount + 1].normal.z);
			glVertex3f(vertices[latCount + 1][longCount + 1].pos.x, vertices[latCount + 1][longCount + 1].pos.y, vertices[latCount + 1][longCount + 1].pos.z);

			glTexCoord2f(((float)(latCount + 1)) / (latitudes - 1), ((float)(longCount)) / (longitudes - 1));
			glNormal3f(vertices[latCount + 1][longCount].normal.x, vertices[latCount + 1][longCount].normal.y, vertices[latCount + 1][longCount].normal.z);
			glVertex3f(vertices[latCount + 1][longCount].pos.x, vertices[latCount + 1][longCount].pos.y, vertices[latCount + 1][longCount].pos.z);
			
		}
	}
	glEnd();
	glPopMatrix();
	glUseProgram(0);
	glDeleteProgram(program);
}

Helicopter::Helicopter(char* name, bool ridable) : Aircraft(name)
{
	this->ridable = ridable;
	speed = 2000; // 10 = stopped
	direction = 0;
	pitch = 0;
	rideAbove = 10;
	movingSpeedFactor = 10;
}

void Helicopter::getCamera(Matrix camera) // override the GrObject's get Camera
{

	Matrix tmpTransform;
	Matrix t1, t2, t3; // adjust the camera
	Matrix delta, newT;
	Matrix t4, t5, t6; // adjust the transform

	

	rotMatrix(t4, 'Y', -direction); // adjust the transform
	multMatrix(t4, transform, t6);  // t3 is result

	rotMatrix(t5, 'X', pitch);
	multMatrix(t5, t6, transform);

	direction = 0;
	pitch = 0;

	transform[3][0] -= transform[2][0] * (camForw - camBack);
	transform[3][1] -= transform[2][1] * (camForw - camBack);
	transform[3][2] -= transform[2][2] * (camForw - camBack);
	camForw = 0;
	camBack = 0;

	copyMatrix(transform, tmpTransform); // adjust the camera

	tmpTransform[3][0] += transform[2][0] * 40;
	tmpTransform[3][1] = tmpTransform[3][1] + transform[2][1] * 40+ rideAbove;
	tmpTransform[3][2] += transform[2][2] * 40;


	copyMatrix(tmpTransform, camera);   // copy transform to camera
	invertMatrix(camera); 				// what's this for?


}

void Helicopter::draw(DrawingState* ds) {
	long unixTime = ds->timeOfDay;
	/*if (speed == 10) {
		// Stop blade rotation
	}
	else if (unixTime % speed == 0) {*/
		bladeRotation +=  speed;
	//}

	//glScaled(.2, .2, .2);

	 glPushMatrix();
	 drawTale();
	 glPopMatrix();
	 glPushMatrix();
	 drawBody();
	 glPopMatrix();
	 glPushMatrix();
	 drawTopBlades(true);
	 glPopMatrix();
	 glPushMatrix();
	 drawWings();
	 glPopMatrix();
}

void Helicopter::drawBody() 
{
	//Material.green(gl);
	 glRotated(90, 0, 1, 0);
	 glTranslated(0, -1.2, 0);
	 glScaled(3, 2, 1);
	 glColor3d(.4, .5, .5);
	// Front, bottom, back, top: Surounding quad strip
	double maxWidth = 3;
	double minWidth = 2;
	 glBegin( GL_QUAD_STRIP);
	
		 glVertex3d(0, 0, -maxWidth);
		 glVertex3d(0, 0, maxWidth);
		 glVertex3d(0, 1.5, -maxWidth);
		 glVertex3d(0, 1.5, maxWidth);
		 glVertex3d(2, 3, -minWidth);
		 glVertex3d(2, 3, minWidth);
		 glVertex3d(3, 4, -minWidth);
		 glVertex3d(3, 4, minWidth);
		 glVertex3d(5, 4, -minWidth);
		 glVertex3d(5, 4, minWidth);
		 glVertex3d(6, 3, -minWidth);
		 glVertex3d(6, 3, minWidth);
		 glVertex3d(6, 0, -maxWidth);
		 glVertex3d(6, 0, maxWidth);
		 glVertex3d(5, -1, -maxWidth);
		 glVertex3d(5, -1, maxWidth);
		 glVertex3d(1, -1, -maxWidth);
		 glVertex3d(1, -1, maxWidth);
		 glVertex3d(0, 0, -maxWidth);
		 glVertex3d(0, 0, maxWidth);
	
	 glEnd();

	// Draw left side
	 glColor3ub(CHIMNEY_BLACK);
	 glBegin( GL_POLYGON);
	{
		 glVertex3d(0, 0, -maxWidth);
		 glVertex3d(0, 1.5, -maxWidth);
		 glVertex3d(2, 3, -minWidth);
		 glVertex3d(3, 4, -minWidth);
		 glVertex3d(5, 4, -minWidth);
		 glVertex3d(6, 3, -minWidth);
		 glVertex3d(6, 0, -maxWidth);
		 glVertex3d(5, -1, -maxWidth);
		 glVertex3d(1, -1, -maxWidth);
		 glVertex3d(0, 0, -maxWidth);

	}
	 glEnd();

	// Draw right side
	 glColor3d(1, 1, 0);
	 glBegin( GL_POLYGON);
	{
		 glVertex3d(0, 0, maxWidth);
		 glVertex3d(0, 1.5, maxWidth);
		 glVertex3d(2, 3, minWidth);
		 glVertex3d(3, 4, minWidth);
		 glVertex3d(5, 4, minWidth);
		 glVertex3d(6, 3, minWidth);
		 glVertex3d(6, 0, maxWidth);
		 glVertex3d(5, -1, maxWidth);
		 glVertex3d(1, -1, maxWidth);
		 glVertex3d(0, 0, maxWidth);

	}
	 glEnd();

	 glTranslated(6, 0, 0);
	 glColor3d(0, 0, 1);
	// Draw cockpit GLASS: top
	//Material.glass(gl);
	 glBegin( GL_QUAD_STRIP);
	
		 glVertex3d(0, 3, -minWidth);
		 glVertex3d(0, 3, minWidth);
		 glVertex3d(2, 1.5, -minWidth);
		 glVertex3d(2, 1.5, minWidth);
		 glVertex3d(3, 0, -minWidth);
		 glVertex3d(3, 0, minWidth);
	
	 glEnd();

	// GLASS: left
	 glBegin( GL_POLYGON);
	
		 glVertex3d(0, 3, -minWidth);
		 glVertex3d(2, 1.5, -minWidth);
		 glVertex3d(3, 0, -minWidth);
		 glVertex3d(2, .5, -minWidth);
		 glVertex3d(0, 1, -minWidth);
	
	 glEnd();

	// GLASS: right
	 glBegin( GL_POLYGON);
	
		 glVertex3d(0, 3, minWidth);
		 glVertex3d(2, 1.5, minWidth);
		 glVertex3d(3, 0, minWidth);
		 glVertex3d(2, .5, minWidth);
		 glVertex3d(0, 1, minWidth);
	
	 glEnd();

	// Draw rest of cockpit thats not glass
	// COCKPIT: left
	//Material.green(gl);
	 glColor3d(0, 1, 0);
	 glBegin( GL_POLYGON);
	{
		 glVertex3d(0, 1, -minWidth);
		 glVertex3d(2, .5, -minWidth);
		 glVertex3d(3, 0, -minWidth);
		 glVertex3d(2, 0, -minWidth);
		 glVertex3d(0, 0, -maxWidth);
	}
	 glEnd();
	// COCKPIT: left, bottom
	 glColor3d(0, 1, 1);
	 glBegin( GL_POLYGON);
	{
		 glVertex3d(0, 0, -maxWidth);
		 glVertex3d(2, 0, -minWidth);
		 glVertex3d(3, 0, -minWidth);
		 glVertex3d(2, -1, -minWidth);
		 glVertex3d(-1, -1, -minWidth);
	}
	 glEnd();

	// COCKPIT: right
	 glColor3d(0, 1, 0);
	 glBegin( GL_POLYGON);
	{
		 glVertex3d(0, 1, minWidth);
		 glVertex3d(2, .5, minWidth);
		 glVertex3d(3, 0, minWidth);
		 glVertex3d(2, 0, minWidth);
		 glVertex3d(0, 0, maxWidth);
	}
	 glEnd();
	// COCKPIT: right, bottom
	 glColor3d(0, 1, 1);
	 glBegin( GL_POLYGON);
	{
		 glVertex3d(0, 0, maxWidth);
		 glVertex3d(2, 0, minWidth);
		 glVertex3d(3, 0, minWidth);
		 glVertex3d(2, -1, minWidth);
		 glVertex3d(-1, -1, minWidth);
	}
	 glEnd();

	// COCKPIT: Floor (bottom)
	 glColor3d(0, 1, 0);
	 glBegin( GL_QUAD_STRIP);
	{
		 glVertex3d(-1, -1, -minWidth);
		 glVertex3d(-1, -1, minWidth);

		 glVertex3d(2, -1, -minWidth);
		 glVertex3d(2, -1, minWidth);

		 glVertex3d(3, 0, -minWidth);
		 glVertex3d(3, 0, minWidth);

	}
	 glEnd();

	// Draw rotor stand
	 GLUquadricObj *obj = gluNewQuadric();
	 glShadeModel( GL_SMOOTH);
	 glRotated(-90, 1, 0, 0);
	 glTranslated(-2, 0, 4);
	 gluCylinder(obj, 1, .4, .6, 15, 15);

}

void Helicopter::drawTale() 
{
	 //Material.green(gl);
	 glColor3ub(CHIMNEY_BLACK);
	 GLUquadricObj *body = gluNewQuadric();
	 glShadeModel( GL_SMOOTH);

	 // Tale 
	 gluCylinder(body, 1.5, .8, 15, 50, 15);
	 glTranslated(0, 1, 0);
	 glColor3d(0, 1, 0);
	 gluCylinder(body, .8, .4, 15, 50, 15);

	 // End tale
	 glTranslated(0, -1, 15);
	 glColor3ub(CHIMNEY_BLACK);
	 gluSphere(body, .8, 15, 15);

	 glTranslated(0, 1, 0);
	 glColor3d(0, 1, 0);
	 gluSphere(body, .4, 15, 15);

	 // Build the windshaft back
	 glRotated(-90, 0, 1, 0);
	 glTranslated(-4, 0, -.1);
	 glScaled(1.4, 1.4, 1.4);
	 glBegin( GL_QUADS);
	 {
		double width = .2;
		 glColor3d(0, 0, 1);

		 // Left side
		 glVertex3d(0, 0, 0);
		 glVertex3d(3, 2, 0);
		 glVertex3d(4, 2, 0);
		 glVertex3d(3, 0, 0);

		 // Right side
		 glVertex3d(0, 0, width);
		 glVertex3d(3, 2, width);
		 glVertex3d(4, 2, width);
		 glVertex3d(3, 0, width);

		 // Front
		 glVertex3d(0, 0, 0);
		 glVertex3d(0, 0, width);
		 glVertex3d(3, 2, width);
		 glVertex3d(3, 2, 0);

		 // Back
		 glVertex3d(3, 0, 0);
		 glVertex3d(3, 0, width);
		 glVertex3d(4, 2, width);
		 glVertex3d(4, 2, 0);

		 // Top
		 glVertex3d(3, 2, 0);
		 glVertex3d(3, 2, width);
		 glVertex3d(4, 2, width);
		 glVertex3d(4, 2, 0);
	 }
	 glEnd();

	 // Build rotors on windshaft
	 glTranslated(2.8, .4, 0);
	 glScaled(.07, .07, .07);
	 glRotated(90, 1, 0, 0);
	 drawTopBlades( false);
}

void Helicopter::drawTopBlades(boolean wingWaighted) 
{
	//Material.Chrome(gl);
	 glScaled(2, 2, 2);
	 glColor3ub(CHIMNEY_BLACK);
	 glTranslated(0, 4, -6);
	 glRotated(bladeRotation, 0, 1, 0);
	 drawBlad(wingWaighted);
	 glRotated(90, 0, 1, 0);
	 drawBlad(wingWaighted);
	 glRotated(90, 0, 1, 0);
	 drawBlad(wingWaighted);
	 glRotated(90, 0, 1, 0);
	 drawBlad(wingWaighted);
}

void Helicopter::drawBlad(boolean wingWaighted)
{
	double wingDrop = 0;
	double wingPeak = 0.2;
	double wingDropFormula;
	if (wingWaighted) {
		wingDropFormula = 1.0 / speed;
	}
	else {
		wingDropFormula = 0;
	}

	// Draw topside of wing
	glPushMatrix();
	glBegin( GL_QUAD_STRIP);
	{
		int i;
		for (i = 0; i<15; i++) {
			wingDrop -= i*wingDropFormula;
			 glVertex3d(i, wingDrop + wingPeak, 0);
			 glVertex3d(i, wingDrop, .8);
		}
	}
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 0, -.8);
	glBegin( GL_QUAD_STRIP);
	{
		wingDrop = 0;
		int i;
		for (i = 0; i<15; i++) {
			wingDrop -= i*wingDropFormula;
			 glVertex3d(i, wingDrop, 0);
			 glVertex3d(i, wingDrop + wingPeak, .8);
		}
	}
	glEnd();
	glPopMatrix();

	// Draw bottomside of wing
	glPushMatrix();
	glBegin( GL_QUAD_STRIP);
	{
		wingDrop = 0;
		int i;
		for (i = 0; i<15; i++) {
			wingDrop -= i*wingDropFormula;
			 glVertex3d(i, wingDrop - wingPeak, 0);
			 glVertex3d(i, wingDrop, .8);
		}
	}
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 0, -.8);
	glBegin( GL_QUAD_STRIP);
	{
		wingDrop = 0;
		int i;
		for (i = 0; i<15; i++) {
			wingDrop -= i*wingDropFormula;
			 glVertex3d(i, wingDrop, 0);
			 glVertex3d(i, wingDrop - wingPeak, .8);
		}
	}
	glEnd();
	glPopMatrix();
}


void Helicopter::drawWings()
{
	glPushMatrix();
	glTranslated(-2, 0, -10);
	drawWing();
	glPopMatrix();
	glPushMatrix();
	glTranslated(2, 0, -15);
	glRotated(180, 0, 1, 0);
	drawWing();
	glPopMatrix();
}
void Helicopter::drawWing() 
{
	//Material.green(gl);
	glScaled(2.5, 1, 2.5);
	glBegin( GL_QUAD_STRIP);
	{
		 glVertex3d(0, 1, 0);
		 glVertex3d(-3, 0, 0);
		 glVertex3d(0, 2, 0);
		 glVertex3d(-3, 1, 0);
		 glVertex3d(0, 2, -2);
		 glVertex3d(-3, 1, -2);
		 glVertex3d(0, 1, -2);
		 glVertex3d(-3, 0, -2);
		 glVertex3d(0, 1, 0);
		 glVertex3d(-3, 0, 0);
	}
	glEnd();

	// Close end
	glBegin(GL_QUADS);
	{
		glVertex3d(-3, 0, 0);
		glVertex3d(-3, 1, 0);
		glVertex3d(-3, 1, -2);
		glVertex3d(-3, 0, -2);
	}
	glEnd();
}

Fighter::Fighter(char* name, const std::string& fileName, const std::string& tex, int rideAbove, float scale) 
: Aircraft(name, 0), missileSlot(0)
{
	this->rideAbove = rideAbove;
	this->fileName = fileName;
	this->texture = tex;
	this->scale = scale;
	this->vel = 10;
	this->destroyable = false;
	this->moving = false;
	this->STDARM = OBJModel("./Model/STDARM.obj", 1.5).ToIndexedModel();
	
#if DEBUG
	std::cout << "Fighter Created" << fileName << "\n";
#endif
}

void Fighter::draw(DrawingState*)//DrawingState* d)
{

	if (initialized == false)
	{
		initialized = true;
		mesh = new Mesh(fileName, scale);
#if DEBUG
		std::cout << "Fighter Initialized successfully\n";
#endif
	}
	char* error;
	GLuint program = loadShader("basicShader.vs", "basicShader.fs", error);
	if (error)
		std::cerr << error << std::endl;
	glUseProgram(program);
	
	fetchTexture((char *)(texture.c_str()));

	mesh->Draw();
	glUseProgram(0);
	glDeleteProgram(program);

}
void Fighter::getCamera(Matrix camera) // override the GrObject's get Camera
{

	Matrix tmpTransform;
	Matrix t1, t2, t3; // adjust the camera
	Matrix delta, newT;
	Matrix t4, t5, t6; // adjust the transform
	int offset = 100;  // 60;


	rotMatrix(t4, 'Y', -direction); // adjust the transform
	multMatrix(t4, transform, t6);  // t3 is result

	rotMatrix(t5, 'X', pitch);
	multMatrix(t5, t6, transform);

	direction = 0;
	pitch = 0;

	transform[3][0] -= transform[2][0] * (camForw - camBack);
	transform[3][1] -= transform[2][1] * (camForw - camBack);
	transform[3][2] -= transform[2][2] * (camForw - camBack);
	camForw = 0;
	camBack = 0;

	copyMatrix(transform, tmpTransform); // adjust the camera

	tmpTransform[3][0] += transform[2][0] * offset;
	tmpTransform[3][1] = tmpTransform[3][1] + transform[2][1] * offset + rideAbove;
	tmpTransform[3][2] += transform[2][2] * offset;


	copyMatrix(tmpTransform, camera); // copy transform to camera
	invertMatrix(camera); // what's this for?


}

int Fighter::handle(int e)
{
	static int stX = 0, stY = 0;
	static int lX = 0, lY = 0;
	static int buttonDown = 0;

	if (Fl::get_key('d'))
	{
		direction += 0.01;
	}
	if (Fl::get_key('a'))
	{
		direction -= 0.01;
	}
	if (Fl::get_key('s'))
	{

		if (vel>10)
			vel-=10;
		else
		{
			vel = 0;
			moving = false;
		}
			
	}
	if (Fl::get_key('e'))
	{
		
		GrObject* b = new Bullet("Bullet");
		extern std::vector<GrObject*> theObjects;
		copyMatrix(this->transform, b->transform);

		b->transform[3][0] -= (b->transform[2][0] * BULLET_OFS);
		b->transform[3][1] -= (b->transform[2][1] * BULLET_OFS);
		b->transform[3][2] -= (b->transform[2][2] * BULLET_OFS);
		theObjects.push_back(b);
		new ForwardAlways(b, 100);
		return 0;
		
	}
	if (Fl::get_key('q'))
	{
		if (keyBuffeting==false)
		{
			
			GrObject* b = new Missile("Missile","F-15C_Eagle.png", this->STDARM, this->vel);
			extern std::vector<GrObject*> theObjects;
			copyMatrix(this->transform, b->transform);
			if (missileSlot) // transform[0] left, right; transform[1] upvector; transform[2] direction
			{
				b->transform[3][0] += (b->transform[0][0] * MISSILE_OFS - b->transform[1][0] * MISSILE_Y_OFS + b->transform[2][0] * 5);
				b->transform[3][1] += (b->transform[0][1] * MISSILE_OFS - b->transform[1][1] * MISSILE_Y_OFS + b->transform[2][1] * 5);
				b->transform[3][2] += (b->transform[0][2] * MISSILE_OFS - b->transform[1][2] * MISSILE_Y_OFS + b->transform[2][2] * 5);
				
				missileSlot = 0;
			}
			else
			{
				b->transform[3][0] += (-b->transform[0][0] * (20 - MISSILE_OFS) - b->transform[1][0] * MISSILE_Y_OFS + b->transform[2][0] * 5);
				b->transform[3][1] += (-b->transform[0][1] * (20 - MISSILE_OFS) - b->transform[1][1] * MISSILE_Y_OFS + b->transform[2][1] * 5);
				b->transform[3][2] += (-b->transform[0][2] * (20 - MISSILE_OFS) - b->transform[1][2] * MISSILE_Y_OFS + b->transform[2][2] * 5);
				
				missileSlot = 1;
			}
			GrObject* flame = new JetFlame(0.1, 0.2, 0.2);
			b->add(flame, 1.5, 1.8, 6.1, PI);
			theObjects.push_back(b);
			new MissileFly(b, 100);
		}
		keyBuffeting = true;
		return 0;

	}
	if (Fl::get_key('w'))
	{
		moving = true;
		vel+=10;
	}

	if (Fl::get_key('r')) // test purpose
	{
		//float modelview[16];
		/*Matrix modelview;
		glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat*)modelview);
		invertMatrix(modelview);
		//Matrix cam;
		glm::vec3 camPos;
		//this->getCamera(cam);
		camPos.x = modelview[3][0];
		camPos.y = modelview[3][1];
		camPos.z = modelview[3][2];
		//cout << "C:Handle: " << camPos.x << ", " << camPos.y << ", " << camPos.z << endl;
		//cout << "T:Handle: " << this->transform[3][0] << ", " << this->transform[3][1] << ", " << this->transform[3][2] << endl;
		//this->transform[3][0] = camPos.x;
		//this->transform[3][1] = camPos.y;
		//this->transform[3][2] = camPos.z;
		//glLineWidth(50);
		glColor4f(1.0, 1.0, 1.0, 1.0);
		glBegin(GL_LINES);
		glTexCoord2f(0.5f, 0.5f);
		glVertex3f(camPos.x, camPos.y, camPos.z);
		glTexCoord2f(0.5f, 0.5f);
		glVertex3f(this->transform[3][0], this->transform[3][1], this->transform[3][2]);
		glEnd();*/
		
	}
	if (Fl::get_key('t'))
	{
		moving = true;
	}

	switch (e)
	{
	case FL_KEYBOARD: // why this doesn't work?

		switch (Fl::event_key())
		{
		case 'a':
			direction += 1.0f;
			break;
		};

	case FL_PUSH:
		lX = stX = Fl::event_x();
		lY = stY = Fl::event_y();
		buttonDown = Fl::event_button();

		return 1;
	case FL_RELEASE:
		buttonDown = 0;
		return 1;

	case FL_DRAG:
		int mX = Fl::event_x();
		int mY = Fl::event_y();
		if (buttonDown) {
			switch (Fl::event_button()) {
			case 1:
			case 3:

				float dx = static_cast<float>(mX - lX);
				float dy = static_cast<float>(mY - lY);
				if (fabs(dx) > fabs(dy)) {
					direction -= 0.01f * dx;
				}
				else
				{
					pitch += 0.01f * dy;
				}
				break;
			}
		}

		buttonDown = Fl::event_button();
		lX = mX; lY = mY;
		return 1;
	};
	if (vel > 0)
	{
		vel -= 0.5;
		if (vel < 0)
			vel = 0;
	}
	keyBuffeting = false;
	return 0;
}


Fighter::~Fighter()
{
	delete mesh;
}


Bullet::Bullet(char* name) : GrObject(name)
{
	moving = true;
	vel = BULLET_SPEED;
	ridable = false;
	texture = "N/A";
	suicide = true;
	killing = true;
	Vertex vertices[4];
	unsigned int indices[6];
	vertices[0].pos = glm::vec3(-0.2, 0, 0);
	vertices[1].pos = glm::vec3(0, 0, -50);
	vertices[2].pos = glm::vec3(0.2, 0, 0);
	vertices[3].pos = glm::vec3(0, 0, 0);
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;
	mesh = new Mesh(vertices, sizeof(vertices) / sizeof(vertices[0]), indices, sizeof(indices) / sizeof(indices[0]));
}

void Bullet::draw(DrawingState* ds)
{
	if (texture != "N/A")
	{
		char* error;
		GLuint program = loadShader("basicShader.vs", "basicShader.fs", error);
		if (error)
			std::cerr << error << std::endl;
		glUseProgram(program);
		fetchTexture((char *)(texture.c_str()));
		mesh->Draw();
		glUseProgram(0);
		glDeleteProgram(program);
	}
	else
	{
		glColor3f(1.0f, 0, 0);
		mesh->Draw();
	}
}

Bullet::~Bullet()
{
	delete mesh;
}

Missile::Missile(char* name, const std::string& tex, IndexedModel& model, float inertialSpeed) : GrObject(name), inertialSpeed(inertialSpeed)
{
	
	this->texture = tex;
	moving = true;
	vel = MISSILE_SPEED;
	ridable = false;
	suicide = true;
	killing = true;
	missile = true;
	mesh = new Mesh(model);
}

void Missile::draw(DrawingState* ds)
{
	if (texture != "N/A")
	{
		char* error;
		GLuint program = loadShader("basicShader.vs", "basicShader.fs", error);
		if (error)
			std::cerr << error << std::endl;
		glUseProgram(program);
		fetchTexture((char *)(texture.c_str()));
		mesh->Draw();
		glUseProgram(0);
		glDeleteProgram(program);
	}
	else
	{
		glColor3f(1.0f, 0, 0);
		mesh->Draw();
	}
}

Missile::~Missile()
{
	delete mesh;
}