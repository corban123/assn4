#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <vector>
#include <sstream>
#include <map>

#include "nodes.h"
#include "include/GL/glui.h"
typedef int BOOL;

using namespace std;

#define TRUE 1
#define FALSE 0


int n;
static float g_lightPos[4] = { 10, 10, -100, 1 };  // Position of light
static GLfloat PI = atan(1) * 4;
static GLfloat CONVERT = PI/180;
static BOOL g_bButton1Down = FALSE;
static BOOL g_bButton2Down = FALSE;
static BOOL g_bButton3Down = false;
static int g_Width = 600;                          // Initial window width
static int g_Height = 600;                         // Initial window height
static int g_yClick = 0;



GLUI_Listbox * objectList;
GLUI_EditText * objectPath;
GLUI_Button* deleteButton;


int wireframe = 0;
int segments = 8;
int currentLight = 0;
int currentItem = 0;
int lightID = 0;
int objID = 9;

GLUI_Rollout * lightRollout;

GLUI_Button* transButton;

GLfloat transX = 0;

GLfloat transY =0;
GLfloat transZ =0;
GLUI_Rollout* transformationRollout;
GLfloat theta = 0;
GLfloat rotateX =0;
GLfloat rotateY =0;
GLfloat rotateZ =0;
GLfloat scaleX =0;
GLfloat scaleY =0;
GLfloat scaleZ =0;

GLfloat lightParam1 = 0;
GLfloat lightParam2 = 0;
GLfloat lightParam3 = 0;





map<int, object*> objList;
static int g_xClick = 0;
static cameraNode* camera;
static Node* rootNode;
static vector<Trimesh*> v;
int   main_window;

static float g_fyRotation = 0;
static float g_fxRotation = 0;
static GLfloat g_nearPlane = 1;
static GLfloat x_center = 0 ;
static bool view = false;
static GLfloat y_center = 0;
static GLfloat z_center = 0;
static GLfloat x_change = 0;
static GLfloat y_change = 0;
static GLfloat g_farPlane = 1000;
static GLfloat radius = 6;
ifstream in;
static char* objFile;

GLUI *glui, *glui2;
GLUI_Spinner    *light0_spinner, *light1_spinner;
GLUI_RadioGroup *radio;
GLUI_Panel      *obj_panel;



void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     
    glLoadIdentity();
    //v[0] ->name;
    glEnable(GL_LIGHTING);

    

    
    int m;

    if(currentItem <9){
        transformationRollout->disable();
        transButton->disable();
    }
    else{
        transformationRollout->enable();
        transButton->enable();
    }

    if(currentItem <1){

    }

    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(1.0, 0.0, 0.0);
    // draw line for y axis
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 1.0, 0.0);
    // draw line for Z axis
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 1.0);
    glEnd();


    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);

    rootNode->MakeStuffHappen();
    // draw triangles here 
    // use glNormal3f() to submit a vertex normal with each vertex 

    glFlush();
    glutSwapBuffers();
}

void MouseButton(int button, int state, int x, int y) {
  // Respond to mouse button presses.
  // If button1 pressed, mark this state so we know in motion function.
  if (button == GLUT_LEFT_BUTTON)
    {
        g_bButton1Down = (state == GLUT_DOWN) ? TRUE : FALSE;
        g_yClick = y - 3*g_fyRotation;
        g_xClick = x - 3*g_fxRotation;
    }
    else if (button == GLUT_MIDDLE_BUTTON){
    
        g_bButton2Down = (state==GLUT_DOWN) ? TRUE: FALSE;
        g_yClick = y;
        g_xClick = x;
    }
    else if (button == GLUT_RIGHT_BUTTON){
        g_bButton3Down = (state==GLUT_DOWN) ?TRUE: FALSE;
        g_yClick = y;
    }
}

void MouseMotion(int x, int y) {
  // If button1 pressed, zoom in/out if mouse is moved up/down.
    if (g_bButton1Down)
    {
        g_fxRotation = (x- g_xClick) / 3.0;
        g_fyRotation = (y - g_yClick) / 3.0;

    }
    else if (g_bButton2Down){
        x_change = (x-g_xClick) / 1000.0;
        y_change = (y-g_yClick) / 1000.0;
        z_center -= (x_change*cos(CONVERT*g_fxRotation)) + (y_change *sin(CONVERT*g_fyRotation) * sin(CONVERT*g_fxRotation));
        y_center += y_change*cos(CONVERT*g_fyRotation);
        x_center -= (x_change*sin(CONVERT*g_fxRotation)) + (y_change * sin(CONVERT*g_fyRotation) * cos(CONVERT*g_fxRotation));



    }
    else if(g_bButton3Down){
        
        radius -= (y-g_yClick) / 600.0;
        
        if(radius > 6){
            radius = 6;
        }
        else if (radius < 2){
            radius = 2;
        }
        
    }
    GLfloat x_1 = radius*cos(CONVERT*g_fxRotation)*cos(CONVERT*g_fyRotation);
    GLfloat y_1 = radius*sin(CONVERT*g_fyRotation);
    GLfloat z_1 = radius*sin(CONVERT*g_fxRotation)*cos(CONVERT*g_fyRotation);
    camera->updateCam(x_1+x_center, y_1+y_center, z_1+z_center, x_center ,y_center ,z_center);

    glutPostRedisplay();


}

void myReshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(65.0, (float)w / h, g_nearPlane, g_farPlane);

    glMatrixMode(GL_MODELVIEW);
}

void add_item(int x){

    const char* path = objectPath->get_text();
    transNode* trans = new transNode();
    attrNode* attr = new attrNode();
    physNode* phys = new physNode();
    geomNode* geom = new geomNode(new Trimesh(path));
    phys->insert(geom);
    attr->insert(phys);
    trans->insert(attr);
    rootNode->insert(trans);
    objList[objID] = new object(phys,trans,attr);
    string name("Object");
    ostringstream oss;
    oss << objID;

    name += oss.str();
    objectList->add_item(objID, name.c_str());

    objID++;
    glutPostRedisplay();
    //cout << "asd";
}

void transform(int){
    
    vector<GLfloat> transl;
    vector<GLfloat> rota;
    vector<GLfloat> scale;
    objList[currentItem]->tNode->addTranslate(transX, transY, transZ);

    objList[currentItem]->tNode->addRotate(theta, rotateX, rotateY, rotateZ);
    objList[currentItem]->tNode->addScale(scaleX, scaleY, scaleZ);
    
    objList[currentItem]->tNode->addTransformation();
    glutPostRedisplay();
}

void deleteObj(int){

    delete objList[currentItem]->tNode;
    rootNode->remove(objList[currentItem]->tNode);
    objectList->delete_item(currentItem);
    glutPostRedisplay();
}

void newLight(int){

    transNode* lightTrans = new transNode();
    vector<GLfloat>* lights = new vector<GLfloat>();
    lights->push_back(lightParam1);
    lights->push_back(lightParam2);
    lights->push_back(lightParam3);
    lightNode* light;
    switch(lightID){


        case 1:
             light = new lightNode(currentLight, lights, GL_LIGHT1);

        break;

        case 2:
         light = new lightNode(currentLight, lights, GL_LIGHT2);
        break;
        case 3:

           light = new lightNode(currentLight, lights, GL_LIGHT3);
        break;
        case 4:

            light = new lightNode(currentLight, lights, GL_LIGHT4);
        break;
        case 5:

            light = new lightNode(currentLight, lights, GL_LIGHT5);
        break;
        case 6:

            light = new lightNode(currentLight, lights, GL_LIGHT6);
        break;

        case 7:

             light = new lightNode(currentLight, lights, GL_LIGHT7);
        break;

    }
    lightTrans->insert(light);
    rootNode->insert(lightTrans);
    string name("Light");
    ostringstream oss;
    oss << lightID;
    name += oss.str();
    objectList->add_item(lightID, name.c_str());
    glutPostRedisplay();


}

int main(int argc, char **argv)
{
 /* enter number of subdivision steps here */

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(300, 300);
    main_window = glutCreateWindow("3D Gasket");
    glutReshapeFunc(myReshape);

    rootNode = new Node();
    transNode* camTrans = new transNode();
    transNode* lightTrans = new transNode();
    lightNode* light = new lightNode();
    lightTrans->insert(light);

    camera = new cameraNode();
    camTrans->insert(camera);
    rootNode->insert(camTrans);
    rootNode->insert(lightTrans);
    object* obj = new object(camera, camTrans);
    object* obj2 = new object(light, lightTrans);
    objList[0] =  obj;
    objList[lightID] = obj2;
    GLUI_Master.set_glutDisplayFunc(display);

    
    glutMouseFunc (MouseButton);
    glutMotionFunc (MouseMotion);
    // glEnable(GL_LIGHTING);
    // glEnable(GL_LIGHT0);

    
    glEnable(GL_DEPTH_TEST);
    glClearColor (1.0, 1.0, 1.0, 1.0);
    glui = GLUI_Master.create_glui_subwindow( main_window, 
                        GLUI_SUBWINDOW_LEFT );

    objectList = new GLUI_Listbox(glui, "Objects: ", &currentItem);

    objectPath = new GLUI_EditText(glui,  "Object Path:" );


    transformationRollout = new GLUI_Rollout(glui, "Transformations", false);
    GLUI_Rollout* transRollout = glui->add_rollout_to_panel(transformationRollout, "Translations", false);

    GLUI_Rollout* rotatRollout = glui->add_rollout_to_panel(transformationRollout, "Rotations", false);

    GLUI_Rollout* scaleRollout = glui->add_rollout_to_panel(transformationRollout, "Scale", false);

    glui->add_spinner_to_panel(transRollout, "X", GLUI_SPINNER_FLOAT, &transX);
    glui->add_spinner_to_panel(transRollout, "Y", GLUI_SPINNER_FLOAT, &transY);
    glui->add_spinner_to_panel(transRollout, "Z", GLUI_SPINNER_FLOAT, &transZ);

    glui->add_spinner_to_panel(rotatRollout, "Theta", GLUI_SPINNER_FLOAT, &theta);
    glui->add_spinner_to_panel(rotatRollout, "X", GLUI_SPINNER_FLOAT, &rotateX);
    glui->add_spinner_to_panel(rotatRollout, "Y", GLUI_SPINNER_FLOAT, &rotateY);
    glui->add_spinner_to_panel(rotatRollout, "Z", GLUI_SPINNER_FLOAT, &rotateZ);

    GLUI_Spinner* spin1 = glui->add_spinner_to_panel(scaleRollout, "X", GLUI_SPINNER_FLOAT, &scaleX);
    spin1 ->set_float_limits(1, 99999);
    spin1 = glui->add_spinner_to_panel(scaleRollout, "Y", GLUI_SPINNER_FLOAT, &scaleY);
    spin1 ->set_float_limits(1, 99999);
    spin1 = glui->add_spinner_to_panel(scaleRollout, "Z", GLUI_SPINNER_FLOAT, &scaleZ);
    spin1 ->set_float_limits(1, 99999);
    transButton = glui->add_button_to_panel(transformationRollout, "Transform!", 2, transform);

    lightRollout = new GLUI_Rollout(glui, "Lights", false);
    GLUI_Listbox* asd =glui->add_listbox_to_panel(lightRollout, "Type: ", &currentLight);
    asd->add_item(0, "Point");
    asd->add_item(1, "Directional");
    glui->add_spinner_to_panel(lightRollout, "X", GLUI_SPINNER_FLOAT, &lightParam1);

    glui->add_spinner_to_panel(lightRollout, "Y", GLUI_SPINNER_FLOAT, &lightParam2);
    glui->add_spinner_to_panel(lightRollout, "Y", GLUI_SPINNER_FLOAT, &lightParam3);

    glui->add_button_to_panel(lightRollout, "New Light", 2, newLight);

    new GLUI_Button(glui, "Add Object", 1, add_item);
    deleteButton = new GLUI_Button(glui, "Delete", 1, deleteObj );

    char* name = "camera"; 
    objectList->add_item(0, name);
    name = "Light0";
    
    objectList->add_item(lightID, name);
    
    lightID++;
    glui->set_main_gfx_window( main_window );

    glutMainLoop();
}
