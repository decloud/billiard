#include "Ball.h"
#include "Table.h"

void drawCircle(float radius);

void drawTable(void);
void drawBalls(void);

bool collideWith(Ball *ball, Table *table);
bool collideWith(Ball *ball1, Ball *ball2);

bool collisionDetection(Ball *ball);

void setupGame();
void initLights(void);
void setupRenderingContext(void);
void display(void);
void update(void);
void reshape(int width, int height);
void keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);