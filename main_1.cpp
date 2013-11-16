#include<iostream>
#include<SDL/SDL.h>
// #include<SDL/SDL_image.h>
#include<GL/gl.h>
#include<GL/glu.h>
#include<Box2D/Box2D.h>
#include<math.h>
#include<time.h>

// g++ main_1.cpp -lSDL -lGL -lBox2D

const int WIDTH = 640;
const int HEIGHT = 480;
const int NO_OF_COINS = 8;
const float MAGNETIC_RADIUS = 1.0f;
const float MAGNETIC_STRENGTH = 3.0f;

const float M2P = 60;
const float P2M = 1/M2P;
b2World *world;

char STRIKER_ID = 's';
char COIN_ID = 'c';
char WALL_ID = 'w';

void drawSquare(b2Vec2* points, b2Vec2 center, float angle, float color = 1.0) {
	// glColor3f(color, color, color);
	glColor3f(0.5,0.5,0.5);
	glPushMatrix();
	glTranslatef(center.x * M2P, center.y * M2P, 0);
	glRotatef(angle * 180 / M_PI, 0, 0, 1);
	
	glBegin(GL_QUADS);
	for(int i=0; i<4;i++)
		glVertex2f(points[i].x * M2P, points[i].y * M2P);
	glEnd();
	
	glPopMatrix();
}

void drawCircle(b2Vec2 center, float r, float angle, char flag) {
	const float PRECISION = 15;
	// glColor3f(1,1,1);
	glPushMatrix();
	glTranslatef(center.x * M2P, center.y * M2P, 0);
	glRotatef(angle * 180/M_PI, 0, 0, 1);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(0, 0);
	int c=0;
	for(float i=0.0; i<=360.0; i+= 360.0/PRECISION) {
		if(flag == 'c') {
			/*if(c%2 == 0) glColor3f(1,0,0);
			else glColor3f(0,0,1);*/
			glColor3f(0.444,0.333,0.111);
		}
		else glColor3f(0.555,0.777,0.444);
		glVertex2f((cos(i * M_PI/180) * r) * M2P, (sin(i * M_PI/180) * r) * M2P);
		c++;
	}
	
	glEnd();
	glPopMatrix();
}

b2Body* addRect(int x, int y, int w, int h, bool dyn = true) {
	// create bodydef (position,type)
	b2BodyDef bodydef;
	bodydef.position.Set(x * P2M, y * P2M);
	if(dyn)
		bodydef.type = b2_dynamicBody;
		
	// world->CreateBody
	b2Body *body = world->CreateBody(&bodydef);
	
	// fixturedef (shape ..)
	b2PolygonShape shape;
	shape.SetAsBox(w/2 * P2M, h/2 * P2M);
	
	b2FixtureDef fixturedef;
	fixturedef.shape = &shape;
	fixturedef.density = 1.0;
	// fixturedef.friction = 100.0;
	
	// body->CreateFixture
	body->CreateFixture(&fixturedef);
	body->SetUserData(&WALL_ID);
}

b2Body* addCircle(int x, int y, int r, bool dyn = true) {
		// create bodydef (position,type)
	b2BodyDef bodydef;
	bodydef.position.Set(x * P2M, y * P2M);
	if(dyn)
		bodydef.type = b2_dynamicBody;
	bodydef.linearDamping = 1.5f;					// 0.5f;
	bodydef.angularDamping = 2.5f; 				//	0.5f;
	bodydef.fixedRotation = false;
		
	// world->CreateBody
	b2Body *body = world->CreateBody(&bodydef);
	
	// fixturedef (shape ..)
	b2CircleShape shape;
	shape.m_radius = r * P2M;
	shape.m_p.Set(0, 0);
	
	b2FixtureDef fixturedef;
	fixturedef.shape = &shape;
	fixturedef.density = 5.0;
	fixturedef.friction = 1.5;							// 0.5;
	fixturedef.restitution = 0.53f;
	
	// body->CreateFixture
	body->CreateFixture(&fixturedef);
	body->SetUserData(&COIN_ID);
	
	return body;
}

b2Body* addStriker(int x, int y, int r, bool dyn = true) {
		// create bodydef (position,type)
	b2BodyDef bodydef;
	bodydef.position.Set(x * P2M, y * P2M);
	if(dyn)
		bodydef.type = b2_dynamicBody;
	bodydef.linearDamping = 1.5f;
	bodydef.angularDamping = 2.5f;
	bodydef.fixedRotation = false;
		
	// world->CreateBody
	b2Body *body = world->CreateBody(&bodydef);
	
	// fixturedef (shape ..)
	b2CircleShape shape;
	shape.m_radius = r * P2M;
	shape.m_p.Set(0, 0);
	
	b2FixtureDef fixturedef;
	fixturedef.shape = &shape;
	fixturedef.density = 1.0;
	fixturedef.friction = 0.5;
	fixturedef.restitution = 0.53;
	
	// body->CreateFixture
	body->CreateFixture(&fixturedef);
	body->SetUserData(&STRIKER_ID);
	
	return body;
}

b2Body *striker;
b2Body *coins[NO_OF_COINS];

void init() {
	glMatrixMode(GL_PROJECTION);
	glOrtho(0,WIDTH,HEIGHT,0,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glClearColor(0,0,0,1);
	
	world = new b2World(b2Vec2(0.0,0.0));
	
	addRect(WIDTH/2, HEIGHT - 100, 300, 30, false);
	addRect(WIDTH/2, HEIGHT - 400, 300, 30, false);
	addRect(WIDTH - 160, HEIGHT/2, 30, 300, false);
	addRect(WIDTH - 480, HEIGHT/2, 30, 300, false);
	// addRect(WIDTH/2, HEIGHT/2, 300, 300, false);
	
	// b2Body *floor = addRect(WIDTH/2, HEIGHT/2, 300, 300, false);
	
	striker = addStriker(200, 200, 20, true);
	
	int i, offset = 0;
	for(i=0;i<5;i++) {
		coins[i] = addCircle(200 + offset, 100 + offset, 15, true);
		offset += 50;
	}
	for(i=5;i<NO_OF_COINS;i++) {
		coins[i] = addCircle(500 - offset, 500 - offset, 15, true);
		offset -= 50;	
	}
}

int run_count = 0;
void addForces(b2Body *body_list) {
	char id;
	int i;
	float dx,dy,rad2;
	float d;
	while(body_list) {
		if((id = *((char *) body_list->GetFixtureList()->GetBody()->GetUserData())) == 'c') {
			// printf("Found coins %d.\n",run_count++);
			b2Body *coin = body_list->GetFixtureList()->GetBody();
			for(i=0;i<NO_OF_COINS;i++) {
				dx = coins[i]->GetPosition().x - coin->GetPosition().x;
				dy = coins[i]->GetPosition().y - coin->GetPosition().y;
				rad2 = dx*dx + dy*dy;
				d = sqrt(rad2);
				// printf("D : %f\n",d);
				if(d != 0 && d < MAGNETIC_RADIUS) {
					coin->ApplyLinearImpulse(P2M * b2Vec2((MAGNETIC_STRENGTH*dx)/rad2, (MAGNETIC_STRENGTH*dy)/rad2), coin->GetWorldCenter());
				}
			}
		}
		body_list = body_list->GetNext();
	}
}

void random_strike() {
	float dx = striker->GetPosition().x;
	float dy = striker->GetPosition().y;
	// dx = 1/dx;
	// dy = 1/dy;
	float rad2 = dx*dx + dy*dy;
	float d = sqrt(rad2);
	striker->ApplyLinearImpulse(P2M * (5000) * b2Vec2(rand()*10, rand()*10), striker->GetWorldCenter());
}

int display_counter = 0;
void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	
	int x,y;
	SDL_GetMouseState(&x, &y);
	b2Vec2 mouse(x, y);
	// mouse -= (M2P * myRect->GetWorldCenter());
	// myRect->ApplyForce(mouse, myRect->GetWorldCenter());
	
	b2Body *tmp = world->GetBodyList();
	b2Vec2 points[4];
	
	while(tmp) {
		if(tmp->GetFixtureList()->GetShape()->GetType() == 0) {
			b2CircleShape *c = ((b2CircleShape *) tmp->GetFixtureList()->GetShape());
			if((*((char *) tmp->GetFixtureList()->GetBody()->GetUserData())) == 'c') 
				drawCircle(tmp->GetWorldCenter(), c->m_radius, tmp->GetAngle(),'c');
			else
				drawCircle(tmp->GetWorldCenter(), c->m_radius, tmp->GetAngle(),'s');
		}
		else {
			for(int i=0; i<4; i++) {
				points[i] = ((b2PolygonShape *) tmp->GetFixtureList()->GetShape())->GetVertex(i);
			}	
			drawSquare(points, tmp->GetWorldCenter(), tmp->GetAngle());
		}
		tmp = tmp->GetNext();
	}
	
	b2Body *body_list = world->GetBodyList();
	addForces(body_list);
	
	display_counter++;
	// if(display_counter%100 == 0) random_strike();
}

int main() {
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_SetVideoMode(1024,768,32,SDL_OPENGL);
	Uint32 start;
	SDL_Event event;
	bool running = true;
	float force = 1000;
	
	init();
	
	while(running) {
		start = SDL_GetTicks();
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_QUIT : running = false;
								break;
				case SDL_KEYDOWN : switch(event.key.keysym.sym) {
										case SDLK_ESCAPE : running = false;
														   break;
										case SDLK_SPACE : striker->ApplyLinearImpulse(P2M * 10000 * b2Vec2(1,1), striker->GetWorldCenter());
														  break;
								   }
								   break;
								   
				case SDL_MOUSEBUTTONDOWN :  float dx = striker->GetPosition().x - (float) event.button.x/100.0;
											float dy = striker->GetPosition().y - (float) event.button.y/100.0;
											// dx = 1/dx;
											// dy = 1/dy;
											float rad2 = dx*dx + dy*dy;
											float d = sqrt(rad2);
											striker->ApplyLinearImpulse(P2M * (force) * b2Vec2(-(dx)/rad2, -(dy)/rad2), striker->GetWorldCenter());
											break;
			}
		}
		display();
		world->Step(1.0/30.0,8,3);				// update
		SDL_GL_SwapBuffers();
		if(1000.0/30 > SDL_GetTicks()-start)
			SDL_Delay(1000.0/30-(SDL_GetTicks()-start));
	}
	SDL_Quit();
}
