#ifndef __NODES_H__
#define __NODES_H__
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <cstring>
#include <algorithm>
#include <GL/glut.h>
#include "loader.h"

using namespace std;


GLfloat colors[6][3] = {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0},
                        {0.0, 0.0, 1.0}, {1.0, 1.0, 0.0},
                        {1.0, 0.0, 1.0}, {1.0, 1.0, 1.0}};

TrimeshLoader ti;

class Node{


	public:
	Node* next;
	Node* prev;
	vector<Node*> children;

	Node() : children() {
		next = NULL;
		prev = NULL;

	}

	~Node(){
		if(next){
			delete next;
			next = NULL;
		}
	}

	virtual Node* insert(Node* node){
		this->next = node;
		this->next->prev = this;
		children.push_back(node);
		return this->next;
	}

	virtual void MakeStuffHappen(){

		for(Node*& node : children){
			node->MakeStuffHappen();
		}

	}

	void remove(Node* node){
		vector<Node*>::iterator x(find(children.begin(), children.end(), node));
		if(x !=children.end()){
			children.erase(x);
		}
	}


};




class geomNode : public Node{
	private:
	 Trimesh* geometry;

	public: 
		bool faceNorm = false;
		bool vertNorm = false;


		geomNode(Trimesh* mesh){
			ti.loadOBJ(mesh->name.c_str(), mesh);
			geometry = mesh;
		}
		~geomNode(){

			cout << "HELPME";
			cout.flush();
		}

		void insert(){

		}

		Trimesh* getMesh(){
			return geometry;
		}
		void drawFaceNorm(Trimesh * mesh){


	        int i;
	        int j;
	        int k;

	            glColor3f(1.0, 0.0, 0.0);

	        for(i = 0; i < mesh->sizeOfFaces(); i++){
	            Face* x = mesh->getFace(i);
	            GLfloat* m = new GLfloat;
	            GLfloat* p = new GLfloat;
	            for(j = 0; j < x->vertices->size(); j++){
	                GLfloat* vertex = x->getFacing(j);
	                for(k = 0; k <3; k++ ){
	           
	                    m[k] += vertex[k];

	                }

	            }
	            m[0] /= 3;
	            m[1] /= 3;
	            m[2] /= 3;
	            glVertex3fv(m);
	            p[0] = m[0] + (-x->getNormal()[0]);
	            p[1] = m[1] + (-x->getNormal()[1]);
	            p[2] = m[2] + (-x->getNormal()[2]);
	            glVertex3fv(p);

	        }
	       
		}

		void drawVertexNorm(Trimesh * mesh){

		    int i;
		    int j;
		    int k;

		        glColor3f(1.0, 0.0, 0.0);


		    for (i = 0; i < mesh->getVertList()->sizeOfList(); i++){
		         mesh->getVertList()->returnVectorAt(i)->resetNorm();
		    }


		    for(i = 0; i < mesh->sizeOfFaces(); i++){
		        Face* x = mesh->getFace(i);
		        GLfloat* m = new GLfloat[3];
		        GLfloat* p = new GLfloat[3];
		        for(j = 0; j < x->vertices->size(); j++){
		            GLfloat* vertex = x->getFacing(j);

		            for(k = 0; k <3; k++ ){
		                m[k] += vertex[k];
		            }

		        }
		        m[0] /= 3;
		        m[1] /= 3;
		        m[2] /= 3;
		        p[0] = m[0] + (-x->getNormal()[0]);
		        p[1] = m[1] + (-x->getNormal()[1]);
		        p[2] = m[2] + (-x->getNormal()[2]);

		        for(j = 0; j < x->vertices->size(); j++){
		            Vertex* vertex = x->getVertex(j);
		            vertex->addFaceNormal(p);
		        }
		    }
	        VertList* vert = mesh->getVertList();
			for(i = 0; i < mesh->getVertList()->sizeOfList(); i++ ){
			Vertex* vertex = vert->returnVectorAt(i);
			glVertex3fv(vertex->returnArray());

	        for(j = 0; j < 3; j++){
	            vertex->normal[j] /= vertex->normCount;
	        }
	        glVertex3fv(vertex->normal);



		    }

		}
		

		void MakeStuffHappen(){


	    int i;
	    int m;
	    int j;
	    int k;
	    
	    glColor3fv(colors[0]); 

	    	if(faceNorm){
	        	glBegin(GL_LINES);

	        	drawFaceNorm(geometry);
	        }
	        else if(vertNorm){
	        	drawVertexNorm(geometry);
	        }
	        glBegin(GL_TRIANGLES);

	        for(i = 0; i < geometry->sizeOfFaces(); i++){
	            Face* x = geometry->getFace(i);
	            glNormal3f(-x->getNormal()[0], -x->getNormal()[1], -x->getNormal()[2]);
	            for(j = 0; j < x->vertices->size(); j++){
	                glVertex3fv(x->getFacing(j));
	            }

	        }
	    	glEnd();

		}

};

class physNode: public Node{

public: 
		geomNode* next;

		bool faceNorm = false;
		bool vertNorm = false;	
		physNode(){

		}
		~physNode(){
			cout << "HELPME PHYS";

		}

		void insert(geomNode* node){
			next = node;
		}

	virtual void MakeStuffHappen(){
		next->faceNorm = this->faceNorm;
		next->vertNorm = this->vertNorm;
		next->MakeStuffHappen();


	}
};

class attrNode : public Node{
		

	public:

		physNode* next;
		physNode* prev;

	enum RenderMode{
		POINT,
		WIREFRAME,
		LINE,
		FILL,
		NORMAL,
		VERTEX

	};
	RenderMode curMode;

	void insert(physNode* node){

		next = node;
		next->prev = this;
	}

	attrNode(){
		curMode = FILL;
	}



	attrNode(RenderMode mode){

		curMode = mode;
	}

	~attrNode(){

		cout << "HELPME ATTR";

	}

	void MakeStuffHappen(){

		switch(curMode){


			case POINT:
				
				glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

			break;

			case WIREFRAME:
				
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);

			break;

			case FILL:

		        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			break;

			case NORMAL:
			next->faceNorm = !this->next->faceNorm;
        	next->vertNorm = false;

			break;

			case VERTEX:
			next->vertNorm = !this->next->vertNorm;
        	next->faceNorm = false;

			break;
		}
		this->next->MakeStuffHappen();
	}
};

class transNode : public Node{
	vector<GLfloat>* rotate;
	vector<GLfloat>* translate;
	vector<GLfloat>* scale;
	vector<vector<GLfloat>*> transformations;
	vector<vector<vector<GLfloat>*>> translatelist;
	public:

		transNode(){
			rotate = new vector<GLfloat>();
			translate = new vector<GLfloat>();
			scale = new vector<GLfloat>();

			for(int i = 0; i < 4; i++){
				rotate->push_back(0);
				translate->push_back(0);
				scale->push_back(1);
			}
		}	
		~transNode(){

			delete rotate;
			delete translate;
			delete scale;
			if(next){
				delete next;
				next = NULL;
			}
			prev->next = NULL;

		}

		void addTranslate(GLfloat x, GLfloat y, GLfloat z){
			translate->clear();
			translate->push_back(x);
			translate->push_back(y);
			translate->push_back(z);
		}
		void addRotate(GLfloat theta,GLfloat x, GLfloat y, GLfloat z){
			rotate->clear();
			rotate->push_back(theta);
			rotate->push_back(x);
			rotate->push_back(y);
			rotate->push_back(z);
		}
		void addScale(float x, float y, float z){
			scale->clear();
			scale->push_back(x);
			scale->push_back(y);
			scale->push_back(z);
		}
		void addTransformation(){
			transformations.clear();
			transformations.push_back(translate);
			transformations.push_back(rotate);
			transformations.push_back(scale);
			translatelist.push_back(transformations);
		}	

		void MakeStuffHappen(){
			for(auto& list : translatelist){
			    glTranslatef(list.at(0)->at(0),list.at(0)->at(1), list.at(0)->at(2));
	        	glRotatef(list.at(1)->at(0),list.at(1)->at(1), list.at(1)->at(2), list.at(1)->at(3));
	        	glScalef(list.at(2)->at(0), list.at(2)->at(1), list.at(2)->at(2));
	        	cout << list.at(0)->at(0) << endl;

			}

        	this->next->MakeStuffHappen();
		}

};

class cameraNode : public physNode{
	GLfloat x_center; 
	GLfloat y_center;
	GLfloat z_center;
	GLfloat x1_center; 
	GLfloat y1_center;
	GLfloat z1_center;


	public:
		cameraNode(){
			x_center = 0; 
			y_center = 0;
			z_center = 0;
			x1_center = 0; 
			y1_center = 0;
			z1_center = 0;
		}
		void updateCam(GLfloat x, GLfloat y, GLfloat z, GLfloat a, GLfloat b, GLfloat c){
			x_center = x;
			y_center = y;
			z_center = z;
			x1_center = a;
			y1_center = b;
			z1_center = c;

		}
		void insert(){

		}

		void MakeStuffHappen(){

			gluLookAt(x_center, y_center,z_center, x1_center ,y1_center ,z1_center, 0, 1, 0);
		}


};

class lightNode : public physNode{
	
	GLenum num;
	GLfloat* pointParams = new GLfloat[4];

	GLfloat* directParams = new GLfloat[3];

	vector<GLfloat>* Params;
	public:

		void insert(){

		}

		lightNode(){
			Params = new vector<GLfloat>;
			num = GL_LIGHT0;
			Params->push_back(0);

			Params->push_back(0);

			Params->push_back(1);

			Params->push_back(1);
		}
		lightNode(int choice, vector<GLfloat>* params, GLenum cur){

			Params = params;
			Params->push_back(choice);
			num = cur;
		}
		~lightNode(){
			glDisable(num);

		}

		void MakeStuffHappen(){
			glEnable(num);
			for(int i = 0; i < 4; i++){
				pointParams[i] = Params->at(i);
			}
			glLightfv(num,GL_POSITION, pointParams);			
			}
};


class object{

public:
	physNode* objNode;
	transNode* tNode;
	attrNode* attNode;

	object(physNode* a, transNode* b, attrNode* c){
		objNode = a;
		tNode = b;
		attNode = c;


	}

	object(physNode* a, transNode* b){
		objNode = a;
		tNode = b;
		attNode = NULL;
	}


};


#endif