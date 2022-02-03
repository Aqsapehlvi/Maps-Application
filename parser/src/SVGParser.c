#include "SVGParser.h"
#include <math.h>

void print_elements(xmlNode * a_node, SVGimage *image);
Circle * createcir(xmlNode * a_node, SVGimage *image, Circle *cir);
Rectangle * createrec(xmlNode * a_node, SVGimage *image, Rectangle *rec);
Group * creategp(xmlNode * a_node, SVGimage *image, Group *gp);
Path * createpath(xmlNode * a_node, SVGimage *image, Path *pt);
Group *createGroup(SVGimage *image, xmlNode *cur_node, Group *gp);
void getgroupsh(List *groups, List *croup);
void deleteGroups(void* data);
void deleteRectangles(void* data);
void deleteCircles(void* data);
void deletePaths(void* data);
xmlDocPtr createTree(SVGimage *doc);
void createRectangle(SVGimage *doc, Node *rhead, xmlNodePtr root_node);
void createCircle(SVGimage *doc, Node *chead, xmlNodePtr root_node);
void createPath(SVGimage *doc, Node *phead, xmlNodePtr root_node);
void createGroups(SVGimage *doc, Node *phead, xmlNodePtr root_node);
void setCircle(SVGimage* image, int elemIndex, Attribute* newAttribute);
void setRect(SVGimage* image, int elemIndex, Attribute* newAttribute);
void setPath(SVGimage* image, int elemIndex, Attribute* newAttribute);
void setGroup(SVGimage* image, int elemIndex, Attribute* newAttribute);

/*

int main(){


printf("\n\nWORKING\n\n");
SVGimage *img = createSVGimage("quad01.svg");
char * ret= SVGtoJSON((const SVGimage*)img);
deleteSVGimage(img);
free(ret);
	return 0;
}
*/

SVGimage* createSVGimage(char* fileName){

	xmlDoc *doc = NULL;
	xmlNode *root_element = NULL;

LIBXML_TEST_VERSION

doc = xmlReadFile(fileName, NULL, 0);

if (doc == NULL) {
		printf("error: could not parse file %s\n", fileName);
		return NULL;
}


SVGimage * image = malloc(sizeof(SVGimage));
image->rectangles = initializeList(&rectangleToString, &deleteRectangle, &compareRectangles);
image->circles = initializeList(&circleToString, &deleteCircle, &compareCircles);
image->paths = initializeList(&pathToString, &deletePath, &comparePaths);
image->groups = initializeList(&groupToString, &deleteGroup, &compareGroups);
image->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);


root_element = xmlDocGetRootElement(doc);
print_elements(root_element, image);
xmlFreeDoc(doc);
xmlCleanupParser();
//int c = numRectsWithArea(image, 200.0);
//numRectsWithArea(image, 200.0);
//deleteSVGimage(image);
///freeList(ret);

	return image;
}


char* SVGimageToString(SVGimage* img){return NULL;}

void deleteSVGimage(SVGimage* img){

	if(img == NULL){
		return;
	}

	freeList(img->rectangles);
  freeList(img->circles);
  freeList(img->paths);
  freeList(img->groups);
	freeList(img->otherAttributes);
  free(img);
}


List* getRects(SVGimage* img){

	if(img==NULL){return NULL;}

List *rects = initializeList(&rectangleToString, &deleteRectangles, &compareRectangles);
// List *groups = initializeList(&groupToString, &deleteGroups, &compareGroups);/
List* groups = getGroups(img);

//printf("\n\n in GetREcts: List length of recieved list is %d\n\n", getLength(groups));

ListIterator iters = createIterator(img->rectangles);
void *elems;
while((elems = nextElement(&iters)) !=NULL)
{
	Rectangle *rec = (Rectangle *)elems;
	insertBack(rects, rec);
}

ListIterator iterr = createIterator(groups);
void* elem;
while((elem = nextElement(&iterr)) !=NULL)
{
	 Group *gp = (Group *)elem;
	 ListIterator iterec = createIterator(gp->rectangles);
	 void *elemr;
	 while((elemr = nextElement(&iterec)) !=NULL)
	 {
		   Rectangle *rec = (Rectangle *)elemr;
		   insertBack(rects, rec);
	 }
 }
 //printf("\n\nTotal length of rect lsit is %d\n\n", getLength(rects));
 freeList(groups);
	return rects;
}


// Function that returns a list of all circles in the image.
List* getCircles(SVGimage* img){

	if(img==NULL){return NULL;}

List *circs = initializeList(&circleToString, &deleteCircles, &compareCircles);
List *groups = getGroups(img);


	ListIterator iterc = createIterator(img->circles);
	void *elemc;
	while((elemc = nextElement(&iterc)) !=NULL)
	{
		Circle *cir = (Circle *)elemc;
		insertBack(circs, cir);
	}

	ListIterator iterr = createIterator(groups);
	void* elem;
	while((elem = nextElement(&iterr)) !=NULL)
	{
		 Group *gp = (Group *)elem;
		 ListIterator itercir = createIterator(gp->circles);
		 void *elemr;
		 while((elemr = nextElement(&itercir)) !=NULL)
		 {
			   Circle *circ = (Circle *)elemr;
			   insertBack(circs, circ);
		 }
	 }

	// printf("\n\nTotal length of rect lsit is %d\n\n", getLength(circs));
	 freeList(groups);
		return circs;
}





List* getGroups(SVGimage* img){

	if(img ==NULL){return NULL;}                                                    //   *****ask about mem leak here****

	//printf("\n\n\nLength of list is %d!\n\n\n",getLength(img->groups));

	List *groups;
	groups = initializeList(&groupToString, &deleteGroups, &compareGroups);
	getgroupsh(img->groups ,groups);

//printf("\n\nLength of added list is %d\n\n", getLength(groups));
	return groups;
}


void getgroupsh(List *group, List *groups){

	ListIterator iterr = createIterator(group);
	void* elem;

	while((elem = nextElement(&iterr)) !=NULL)
	{
		 Group *lpg = (Group *)elem;
		 insertBack(groups, lpg);
		 if(getLength(lpg->groups) > 0){
		//	 printf("\n\n\nLength of this list is %d!\n\n\n",getLength(lpg->groups));
			 getgroupsh(lpg->groups, groups);
		 }
	}
}




// Function that returns a list of all paths in the image.
List* getPaths(SVGimage* img){

	if(img==NULL){return NULL;}

List *paths = initializeList(&pathToString, &deletePaths, &comparePaths);
List *groups = getGroups(img);

//printf("\n\n in GetREcts: List length of recieved list is %d\n\n", getLength(groups));

ListIterator iters = createIterator(img->paths);
void *elems;
while((elems = nextElement(&iters)) !=NULL)
{
	Path *pa = (Path *)elems;
	insertBack(paths, pa);
}

ListIterator iterr = createIterator(groups);
void* elem;
while((elem = nextElement(&iterr)) !=NULL)
{
	 Group *gp = (Group *)elem;
	 ListIterator iterec = createIterator(gp->paths);
	 void *elemr;
	 while((elemr = nextElement(&iterec)) !=NULL)
	 {
		   Path *p = (Path *)elemr;
		   insertBack(paths, p);
	 }
 }

 //printf("\n\nTotal length of rect lsit is %d\n\n", getLength(paths));
 freeList(groups);

	return paths;
}



int numRectsWithArea(SVGimage* img, float area){

if(img ==NULL){return 0;}
if(area<0){return 0;}
int count = 0;

List *rects = getRects(img);

ListIterator iterr = createIterator(rects);
void* elem;
while((elem = nextElement(&iterr)) !=NULL)
{
    Rectangle *rec = (Rectangle *)elem;
		float area2 = 0;
		float w = rec->width;
		float h = rec->height;
		area2 = ceilf(w*h);
		printf("\n\area: %f\n\n", area2);
		if(ceilf(area) == area2){count++;}
}
  freeList(rects);
	return count;
}


int numCirclesWithArea(SVGimage* img, float area){

	if(img ==NULL){return 0;}
	if(area<0){return 0;}
	int count = 0;

	List *circles = getCircles(img);

	ListIterator iterr = createIterator(circles);
	void* elem;
	while((elem = nextElement(&iterr)) !=NULL)
	{
	    Circle *cir = (Circle *)elem;
			float area2 = 0;
			float r = cir->r;
			area2 =3.14159*(r*r);
			area2= ceilf(area2);
			if(ceilf(area) == area2){count++;}
	}
	freeList(circles);
		return count;
}


int numPathsWithdata(SVGimage* img, char* data){

	if(img ==NULL){return 0;}
	if(data ==NULL){return 0;}
	int count = 0;

	List *paths = getPaths(img);

	ListIterator iterr = createIterator(paths);
	void* elem;
	while((elem = nextElement(&iterr)) !=NULL)
	{
	    Path *pt = (Path *)elem;

			if(strcmp(pt->data, data)==0){count++;}
	}
	freeList(paths);
		return count;
}


int numGroupsWithLen(SVGimage* img, int len){

	if(img ==NULL){return 0;}
	if(len<0){return 0;}
	int count = 0;
	int leng = 0;

	List *groups = getGroups(img);

	ListIterator iterr = createIterator(groups);
	void* elem;
	while((elem = nextElement(&iterr)) !=NULL)
	{
			Group *gp = (Group *)elem;
			leng = getLength(gp->rectangles);
			leng = leng + getLength(gp->circles);
			leng = leng + getLength(gp->groups);
			leng = leng + getLength(gp->paths);
			if(leng == len){count++;}
	}
	freeList(groups);
		return count;
}


int numAttr(SVGimage* img){

  if(img==NULL){return 0;}
	List *groups = getGroups(img);
	List *rect = getRects(img);
	List *cir = getCircles(img);
	List*paths = getPaths(img);
	int len = 0;
	int len1=0;
	int len2 = 0;
	int len3 = 0;
	int len4 = 0;

	ListIterator iterr = createIterator(groups);
	void* elem;
	while((elem = nextElement(&iterr)) !=NULL)
	{
		  Group *gp = (Group *)elem;
      len = len + getLength(gp->otherAttributes);
	}

	ListIterator iter = createIterator(rect);
	void* elemr;
	while((elemr = nextElement(&iter)) !=NULL)
	{
		  Rectangle *rec = (Rectangle *)elemr;
      len1 = len1 + getLength(rec->otherAttributes);
	}

	ListIterator iterc = createIterator(cir);
	void* elemc;
	while((elemc = nextElement(&iterc)) !=NULL)
	{
		  Circle *c = (Circle *)elemc;
      len2 = len2 + getLength(c->otherAttributes);
	}

	ListIterator iterp = createIterator(paths);
	void* elemp;
	while((elemp = nextElement(&iterp)) !=NULL)
	{
		  Path *pt = (Path *)elemp;
      len3 = len3 + getLength(pt->otherAttributes);
	}

	len4 = len+len1+len2+len3+getLength(img->otherAttributes);

	freeList(groups);
	freeList(cir);
	freeList(rect);
	freeList(paths);

	return len4;
}


/* ******************************* List helper functions  - MUST be implemented *************************** */

void deleteAttribute( void* data){

	Attribute* ab = (Attribute*) data;
	if (ab == NULL)
	{
		return;
	}
		free(ab->name);
	 free(ab->value);
		free(ab);
}

char* attributeToString( void* data){return NULL;}
int compareAttributes(const void *first, const void *second){return 0;}

void deleteGroup(void* data){

	Group* gp = (Group*) data;
	if (gp == NULL)
	{
		return;
	}
		freeList(gp->rectangles);
		freeList(gp->circles);
	  freeList(gp->paths);
	  freeList(gp->groups);
		freeList(gp->otherAttributes);
		free(gp);
}

char* groupToString( void* data){return NULL;}
int compareGroups(const void *first, const void *second){return 0;}

void deleteRectangle(void* data){

	Rectangle* rec = (Rectangle*) data;
 if (rec == NULL)
  {
	 return;
 }
	 freeList(rec->otherAttributes);
	 free(rec);
}


char* rectangleToString(void* data){return NULL;}
int compareRectangles(const void *first, const void *second){return 0;}




void deleteCircle(void* data){
	Circle* cir = (Circle*) data;
	if (cir == NULL)
	{
		return;
	}
		freeList(cir->otherAttributes);
		free(cir);
}


char* circleToString(void* data){return NULL;}
int compareCircles(const void *first, const void *second){return 0;}

void deletePath(void* data){

	Path* path = (Path*) data;
  if (path == NULL)
  {
	  return;
  }
	 freeList(path->otherAttributes);
	 free(path->data);
	 free(path);
}

char* pathToString(void* data){return NULL;}
int comparePaths(const void *first, const void *second){return 0;}


/********************************************Aqsa's Helper functions***************************************/

void print_elements(xmlNode * a_node, SVGimage *image){

  xmlNode *cur_node = NULL;

	for (cur_node = a_node->children; cur_node != NULL; cur_node = cur_node->next)
	{

		  if (cur_node->type == XML_ELEMENT_NODE)
		  {
				  char *Name = (char *)cur_node->name;
					 strcpy(image->namespace, "http://www.w3.org/2000/svg");


					if(strcmp(Name, "title")==0)
					{
						char *cont = (char *)xmlNodeGetContent(cur_node);
						strcpy(image->title, cont);
						// printf("\n\n Title: %s\n", image->title);
						 xmlFree(cont);

					}

					else if(strcmp(Name, "desc")==0)
					{
						char *cont = (char *)xmlNodeGetContent(cur_node);
						strcpy(image->description, cont);
						xmlFree(cont);
						// printf("\n\n description: %s\n", image->description);
					}

					else if(strcmp(Name, "g")==0)
					{
						// printf("\ncreating new 0 group\n");

					 	Group *gp = malloc(sizeof(Group));
					 	gp->rectangles = initializeList(&rectangleToString, &deleteRectangle, &compareRectangles);
					 	gp->circles = initializeList(&circleToString, &deleteCircle, &compareCircles);
					 	gp->paths = initializeList(&pathToString, &deletePath, &comparePaths);
					 	gp->groups = initializeList(&groupToString, &deleteGroup, &compareGroups);
					 	gp->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
            gp = createGroup(image, cur_node, gp);
						insertBack(image->groups, gp);

					}

					else if(strcmp(Name, "path")==0)
					{
						 // printf("\nnew SVG path\n");
						  Path * pt = malloc(sizeof(Path));
					  	pt = createpath(cur_node, image, pt);
					  	insertBack(image->paths, pt);
						  //printf("\nending new SVG path\n");
					}

					else if(strcmp(Name, "circle")==0)
					{
						  //printf("\n new SVG circle\n");
						  Circle * cir = malloc(sizeof(Circle));
						  cir = createcir(cur_node, image, cir);
					 	  insertBack(image->circles, cir);
						//	printf("\nending new SVG cricle\n");

					}

					else if(strcmp(Name, "rect")==0)
					{
						 //printf("\nnew SVG rectangle\n");
						  Rectangle * rec = malloc(sizeof(Rectangle));
						  rec = createrec(cur_node, image, rec);
					 	  insertBack(image->rectangles, rec);
							//printf("\nending new svgrectangle\n");
					}

					else
					{
						return;
					}
		  }
	}

	xmlAttr *attr;
 for (attr = a_node->properties; attr != NULL; attr = attr->next)
 {
		 xmlNode *value = attr->children;
		 char *attrName = (char *)attr->name;
		 char *cont = (char *)(value->content);

		 if(strcmp(attrName, "xmlns")==0)
		 {
				 strcpy(image->namespace, cont);
				// printf("\nnamespace: %s, attribute value = %s\n", attrName, image->namespace);
		 }
		 else
		 {
				// printf("\tNew SVG Attribute! attribute name: %s, attribute value = %s\n", attrName, cont);
				 Attribute *at = malloc(sizeof(Attribute));
				 at->name = malloc(sizeof(char)* strlen(attrName)+1);
				 at->value = malloc(sizeof(char)* strlen(cont)+1);
				 strcpy(at->name, attrName);
				 strcpy(at->value, cont);
				 insertBack(image->otherAttributes, at);
		 }
	 }

	//print_elements(cur_node->children, image);
}


Circle * createcir(xmlNode * a_node, SVGimage *image, Circle *cir){


	cir->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);

	xmlAttr *attr;
	for (attr = a_node->properties; attr != NULL; attr = attr->next)
	{
			xmlNode *value = attr->children;
			char *attrName = (char *)attr->name;
			char *cont = (char *)(value->content);
		//	float f = atof(cont);

			if(strcmp(attrName, "cx")==0)
			{
			  	char *pend;
			  	float f1 = strtof(cont, &pend);
  			  cir->cx = f1;
					strcpy(cir->units, pend);
					//printf("\tattribute name: %s, attribute value = %f\n", attrName, cir->cx);
			}

			else if(strcmp(attrName, "cy")==0)
			{
				  char *pend;
			  	float f1 = strtof(cont, &pend);
  			  cir->cy = f1;
					strcpy(cir->units, pend);
					//printf("\tattribute name: %s, attribute value = %f\n", attrName, cir->cy);
			}

			else if(strcmp(attrName, "r")==0)
			{
			 	  char *pend;
			  	float f1 = strtof(cont, &pend);
  			  cir->r = f1;
					strcpy(cir->units, pend);
					//printf("\tattribute name: %s, attribute value = %f\n", attrName, cir->r);
			}


			else
			{
              //printf("\tNew Attribute! attribute name: %s, attribute value = %s\n", attrName, cont);
							Attribute *at = malloc(sizeof(Attribute));
							at->name = malloc(sizeof(char)* strlen(attrName)+1);
							at->value = malloc(sizeof(char)* strlen(cont)+1);
							strcpy(at->name, attrName);
							strcpy(at->value, cont);
							insertBack(cir->otherAttributes, at);
			}
	}
	return cir;
}




Rectangle * createrec(xmlNode * a_node, SVGimage *image, Rectangle *rec){

	xmlAttr *attr;
	rec->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
	for (attr = a_node->properties; attr != NULL; attr = attr->next)
	{
			xmlNode *value = attr->children;
			char *attrName = (char *)attr->name;
			char *cont = (char *)(value->content);
		//	float cx = atof(cont);
			rec->units[0]= '\0';

			if(strcmp(attrName, "x")==0)
			{
			  	char *pend;
				  float f1 = strtof(cont, &pend);
		  		rec->x = f1;
		  		strcpy(rec->units, pend);
					//printf("\tattribute name: %s, attribute value = %f\n", attrName, rec->x);
			}

			else if(strcmp(attrName, "y")==0)
			{
			  	char *pend;
			  	float f1 = strtof(cont, &pend);
			  	rec->y = f1;
			  	strcpy(rec->units, pend);
					//printf("\tattribute name: %s, attribute value = %f\n", attrName, rec->y);
			}

			else if(strcmp(attrName, "width")==0)
			{
					char *pend;
				  float f1 = strtof(cont, &pend);
		  		rec->width = f1;
		  		strcpy(rec->units, pend);
					//printf("\tattribute name: %s, attribute value = %f\n", attrName, rec->width);
			}

			else if(strcmp(attrName, "height")==0)
			{
			  	char *pend;
			  	float f1 = strtof(cont, &pend);
			  	rec->height = f1;
			  	strcpy(rec->units, pend);
					//printf("\tattribute name: %s, attribute value = %f\n", attrName, rec->height);
			}

			else
			{
            //  printf("\tNew Attribute! attribute name: %s, attribute value = %s\n", attrName, cont);
							Attribute *at = malloc(sizeof(Attribute));
							at->name = malloc(sizeof(char)* strlen(attrName)+1);
							at->value = malloc(sizeof(char)* strlen(cont)+1);
							strcpy(at->name, attrName);
							strcpy(at->value, cont);
							insertBack(rec->otherAttributes, at);
			}
	}
	return rec;
}



Path * createpath(xmlNode * a_node, SVGimage *image, Path *pt){

	xmlAttr *attr;
 pt->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
 for (attr = a_node->properties; attr != NULL; attr = attr->next)
 {
		 xmlNode *value = attr->children;
		 char *attrName = (char *)attr->name;
		 char *cont = (char *)(value->content);

		 if(strcmp(attrName, "d")==0)
		 {
			   pt->data = malloc(sizeof(char)* strlen(cont)+1);
			   strcpy(pt->data, cont);
				 //printf("\nData: %s, attribute value = %s\n", attrName, pt->data);
		 }
		 else
		 {
			  // printf("\tNew Path Attribute! attribute name: %s, attribute value = %s\n", attrName, cont);
			   Attribute *at = malloc(sizeof(Attribute));
			   at->name = malloc(sizeof(char)* strlen(attrName)+1);
			   at->value = malloc(sizeof(char)* strlen(cont)+1);
			   strcpy(at->name, attrName);
			   strcpy(at->value, cont);
			   insertBack(pt->otherAttributes, at);
		 }
	 }
	return pt;
}


Group *createGroup(SVGimage *image, xmlNode *a_node, Group *gp){

  //printf("\nGoing one group deeper...\n");
	xmlNode *cur_node = NULL;

	for (cur_node = a_node->children; cur_node != NULL; cur_node = cur_node->next)
	{

		  if (cur_node->type == XML_ELEMENT_NODE)
		  {
				  char *Name = (char *)cur_node->name;

					 if(strcmp(Name, "g")==0)
					{
						//printf("\n\nNew group within\n\n");
					 Group *gpc = malloc(sizeof(Group));
					 gpc->rectangles = initializeList(&rectangleToString, &deleteRectangle, &compareRectangles);
					 gpc->circles = initializeList(&circleToString, &deleteCircle, &compareCircles);
					 gpc->paths = initializeList(&pathToString, &deletePath, &comparePaths);
					 gpc->groups = initializeList(&groupToString, &deleteGroup, &compareGroups);
					 gpc->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
					 createGroup(image, cur_node, gpc);
					 insertBack(gp->groups, gpc);
					}

					else if(strcmp(Name, "path")==0)
					{
						 // printf("\n\nnew group path\n");
						  Path * pt = malloc(sizeof(Path));
					  	pt = createpath(cur_node, image, pt);
					  	insertBack(gp->paths, pt);
						 // printf("\nending new group path\n");
					}

					else if(strcmp(Name, "circle")==0)
					{
						//  printf("\n\nnew group circle\n");
						  Circle * cir = malloc(sizeof(Circle));
						  cir = createcir(cur_node, image, cir);
					 	  insertBack(gp->circles, cir);
						//	printf("\nending new froup cricle\n");

					}

					else if(strcmp(Name, "rect")==0)
					{
						printf("\nnew group rectangle\n");
						  Rectangle * rec = malloc(sizeof(Rectangle));
						  rec = createrec(cur_node, image, rec);
					 	  insertBack(gp->rectangles, rec);
						//	printf("\nending new grourectangle\n");
					}

					else
					{return NULL;}
		  }
	}
				xmlAttr *attr;
			for (attr = a_node->properties; attr != NULL; attr = attr->next)
			{
					xmlNode *value = attr->children;
					char *attrName = (char *)attr->name;
					char *cont = (char *)(value->content);

					Attribute *at = malloc(sizeof(Attribute));
					 at->name = malloc(sizeof(char)* strlen(attrName)+1);
					 at->value = malloc(sizeof(char)* strlen(cont)+1);
					 strcpy(at->name, attrName);
					 strcpy(at->value, cont);
					 insertBack(gp->otherAttributes, at);
				// printf("\nNew group Attribute! attribute name: %s, attribute value = %s\n", attrName, cont);
			 }
	return gp;
}


void deleteGroups(void* data){



}


void deleteRectangles(void* data){


}


void deleteCircles(void* data){



}


void deletePaths(void* data){



}



 //*********************************** A2 stuf  *****************************************************


bool validateSVGimage(SVGimage* doc, char* schemaFile){

	if(schemaFile==NULL||doc==NULL){return false;}
/*
	xmlDocPtr xmldoc = createTree(doc);
	xmlSchemaPtr schema = NULL;
	xmlSchemaParserCtxtPtr ctxt;

	xmlLineNumbersDefault(1);

	ctxt = xmlSchemaNewParserCtxt(schemaFile);
	xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
  schema = xmlSchemaParse(ctxt);
  xmlSchemaFreeParserCtxt(ctxt);

	if(xmldoc == NULL){return false;}
	else
	{

		xmlSchemaValidCtxtPtr ctxt;
		int ret;
		ctxt = xmlSchemaNewValidCtxt(schema);
				xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf,
								(xmlSchemaValidityWarningFunc) fprintf, stderr);
				ret = xmlSchemaValidateDoc(ctxt, xmldoc);
			//	xmlSchemaDump(stdout, schema);

         if(ret == 0)
				 {
					 xmlSchemaFreeValidCtxt(ctxt);
					 xmlFreeDoc(xmldoc);
					 if(schema!=NULL){xmlSchemaFree(schema);}
					 return true;
				 }

				 else if(ret >0)
				 {
				 	xmlSchemaFreeValidCtxt(ctxt);
				 	xmlFreeDoc(xmldoc);
				 	if(schema!=NULL){xmlSchemaFree(schema);}
				 	return false;
				 }

           xmlSchemaFreeValidCtxt(ctxt);
 				 	xmlFreeDoc(xmldoc);

	}

	xmlSchemaCleanupTypes();
	xmlCleanupParser();
	xmlMemoryDump();
*/
return false;
}




SVGimage* createValidSVGimage(char* fileName, char* schemaFile){

  if(fileName==NULL){return NULL;}
	if(schemaFile==NULL){return NULL;}

	// bool val = false;

	 SVGimage *img = NULL;

	 img = createSVGimage(fileName);

	 //val = true;

	 return img;
}



bool writeSVGimage(SVGimage* image, char* fileName){

	if(image==NULL){return false;}
	if(fileName==NULL){return false;}
/*
	xmlDocPtr xmldoc=createTree(image);
  if(xmldoc==NULL){return false;}

	int ret = xmlSaveFormatFileEnc(fileName, xmldoc, "UTF-8", 1);

	if(ret == -1){
		xmlFreeDoc(xmldoc);
		return false;
	}
	else{
		xmlFreeDoc(xmldoc);
		return true;
	}*/
	  return true;
	}


	void setAttribute(SVGimage* image, elementType elemType, int elemIndex, Attribute* newAttribute){

		if(image==NULL){return;}
		//if(elemType==0){return;}
		if(newAttribute==NULL){return;}

	//	if(elemType==SVG_IMAGE){printf("here");}

	/*	if(elemType==CIRC)
		{
			   setCircle(image, elemIndex, newAttribute);
	  }

		if(elemType==RECT)
		{
			   setRect(image, elemIndex, newAttribute);
	  }

		if(elemType==PATH)
		{
			   setPath(image, elemIndex, newAttribute);
	  }

		if(elemType==GROUP)
		{
			   setGroup(image, elemIndex, newAttribute);
	  }

*/
	}



	void addComponent(SVGimage* image, elementType type, void* newElement){

  if(image==NULL){return;}
	if(newElement==NULL){return;}

		if(type==CIRC)
	 {
       insertBack(image->circles, (Circle*)newElement);
 	 }

	 if(type==RECT)
	 {
		 insertBack(image->rectangles, (Rectangle*)newElement);
	 }

	 if(type==PATH)
	 {
		 insertBack(image->paths, (Path*)newElement);
	 }
	 else{return;}

	}


	char* attrToJSON(const Attribute *a){
   if(!a){return NULL;}

	char* ret;
	ret= malloc(sizeof(char)*1000);
	strcpy(ret, "{\"name\":\"");
	strcat(ret, a->name);
	strcat(ret, "\",\"value\":\"");
	strcat(ret, a->value);
	strcat(ret, "\"}");
		return ret;
	}

	char* circleToJSON(const Circle *c){

		if(!c){return NULL;}

 	char* ret;
 	ret= malloc(sizeof(char)*1000);
 	strcpy(ret, "{\"cx\":");
	char *s = malloc(10);
	sprintf(s, "%.2f", c->cx);
	strcat(ret, s);
 	strcat(ret, ",\"cy\":");
	sprintf(s, "%.2f", c->cy);
	strcat(ret, s);
	strcat(ret, ",\"r\":");
	sprintf(s, "%.2f", c->r);
	strcat(ret, s);
	strcat(ret, ",\"numAttr\":");
	int i = getLength(c->otherAttributes);
	sprintf(s, "%d", i);
	strcat(ret, s);
	strcat(ret, ",\"units\":\"");
	strcat(ret, c->units);
	strcat(ret, "\"}");
	//do units
  free(s);
	return ret;
	}


	char* rectToJSON(const Rectangle *r){

		if(r==NULL){return NULL;}

		char* ret;
	 	ret= malloc(sizeof(char)*1000);
	 	strcpy(ret, "{\"x\":");
		char *s = malloc(10);
		sprintf(s, "%.2f", r->x);
		strcat(ret, s);
	 	strcat(ret, ",\"y\":");
		sprintf(s, "%.2f", r->y);
		strcat(ret, s);
		strcat(ret, ",\"w\":");
		sprintf(s, "%.2f", r->width);
		strcat(ret, s);
		strcat(ret, ",\"h\":");
		sprintf(s, "%.2f", r->height);
		strcat(ret, s);
		strcat(ret, ",\"numAttr\":");
		int i = getLength(r->otherAttributes);
		sprintf(s, "%d", i);
		strcat(ret, s);
		strcat(ret, ",\"units\":\"");
		strcat(ret, r->units);
		strcat(ret, "\"}");

    free(s);
		return ret;
}


	char* pathToJSON(const Path *p){

		if(!p){return "{}";}

		char* ret;
		char *s = malloc(10);
	 	ret= malloc(sizeof(char)*1000);
	 	strcpy(ret, "{\"dVal\":\"");
		strcat(ret, p->data);
		strcat(ret, "\",\"numAttr\":");
		int i = getLength(p->otherAttributes);
		sprintf(s, "%d", i);
		strcat(ret, s);
		strcat(ret, "}");
		free(s);
		return ret;
	}


	char* groupToJSON(const Group *g){


    if(g==NULL){return NULL;}
		char* ret;
		int l = 0;
		int a = 0;
		char *s = malloc(10);
		l = getLength(g->paths) + getLength(g->rectangles) + getLength(g->circles) + getLength(g->groups);
    a = getLength(g->otherAttributes);
		ret= malloc(sizeof(char)*1000);
	 	strcpy(ret, "{\"children\":");
		sprintf(s, "%d", l);
		strcat(ret, s);
		strcat(ret, ",\"numAttr\":");
		sprintf(s, "%d", a);
		strcat(ret, s);
		strcat(ret, "}");
    free(s);
		return ret;

	}


	char* attrListToJSON(const List *list){

		if(!list){return "[]";}

		List *list1 = (List *)list;
			char *ret= NULL;
			ret= malloc(sizeof(char)*10000);
			strcpy(ret, "[");
			Node * head = list1->head;
			Node *temp = head;

			while(temp)
			{
				char *ret1 = attrToJSON((Attribute *)temp->data);
				strcat(ret, ret1);
				free(ret1);
				temp = temp->next;
				if(temp)
				{
					strcat(ret, ",");
				}
			}
			strcat(ret, "]");
			return ret;
	}


	char* circListToJSON(const List *list){

		if(!list){return "[]";}
		List *list1 = (List *)list;
			char *ret= NULL;
			ret= malloc(sizeof(char)*10000);
			Node * head = list1->head;
			Node *temp = head;
			strcpy(ret, "[");

			while(temp)
			{
				char *ret1 = circleToJSON((Circle *)temp->data);
				strcat(ret, ret1);
				free(ret1);
				temp = temp->next;
				if(temp)
				{
					strcat(ret, ",");
				}
			}
			strcat(ret, "]");
			return ret;
	}

	char* rectListToJSON(const List *list){

		if(!list){return "[]";}
		List *list1 = (List *)list;
			char *ret= NULL;
			ret= malloc(sizeof(char)*10000);
		 Node * head = list1->head;
		 Node *temp = head;
		 strcpy(ret, "[");

		 while(temp)
		 {
			 char *ret1 = rectToJSON((Rectangle *)temp->data);
			 strcat(ret, ret1);
			 free(ret1);
			 temp = temp->next;
			 if(temp)
			 {
				 strcat(ret, ",");
			 }
		 }
		 strcat(ret, "]");
		 return ret;
	}


	char* pathListToJSON(const List *list){

		if(!list){return "[]";}
		List *list1 = (List *)list;
			char *ret= NULL;
			ret= malloc(sizeof(char)*10000);
		 Node * head = list1->head;
		 Node *temp = head;
		 strcpy(ret, "[");

		 while(temp)
		 {
			 char *ret1 = pathToJSON((Path *)temp->data);
			 strcat(ret, ret1);
			 free(ret1);
			 temp = temp->next;
			 if(temp)
			 {
				 strcat(ret, ",");
			 }
		 }
		 strcat(ret, "]");
		 return ret;
	}

	char* groupListToJSON(const List *list){
		if(!list){return "[]";}
		List *list1 = (List *)list;
			char *ret= NULL;
			ret= malloc(sizeof(char)*10000);
		 Node * head = list1->head;
		 Node *temp = head;
		 strcpy(ret, "[");


			while(temp)
			{
				char *ret1= groupToJSON((Group *)temp->data);
				strcat(ret, ret1);
				free(ret1);
				temp = temp->next;
				if(temp)
				{
					strcat(ret, ",");
				}
			}
			strcat(ret, "]");
			return ret;
	}


	char* SVGtoJSON(const SVGimage* imge){

		if(imge==NULL){return NULL;}

		SVGimage *img = (SVGimage *) imge;

		char* ret;
		int r = 0;
		int p = 0;
		int c = 0;
		int g= 0;
		char *s = malloc(10);

		List *p1 = getPaths(img);
		List *r1 = getRects(img);
		List *c1 = getCircles(img);
		List *g1 = getGroups(img);

		p = getLength(p1);
		r = getLength(r1);
		c = getLength(c1);
		g = getLength(g1);

		if(g>0){}

		ret= malloc(sizeof(char)*10000);
 	  strcpy(ret, "{\"numRect\":");
		sprintf(s, "%d", r);
		strcat(ret, s);
		strcat(ret, ",\"numCirc\":");
		sprintf(s, "%d", c);
		strcat(ret, s);
		strcat(ret, ",\"numPaths\":");
		sprintf(s, "%d", p);
		strcat(ret, s);
		strcat(ret, ",\"numGroups\":");
		sprintf(s, "%d", g);
		strcat(ret, s);
		strcat(ret, "}");
    free(s);
		freeList(p1);
		freeList(g1);
		freeList(r1);
		freeList(c1);

		return ret;
}





	SVGimage* JSONtoSVG(const char* svgString){return NULL;}

	Rectangle* JSONtoRect(const char* svgString){return NULL;}

	Circle* JSONtoCircle(const char* svgString){return NULL;}



//******************************** A2 helpers *************************************************

xmlDocPtr createTree(SVGimage *doc){

/* if(doc==NULL){return NULL;}
	xmlDocPtr xdoc = NULL;
	xmlNodePtr root_node = NULL;
	//char buff[256];

	xdoc = xmlNewDoc(BAD_CAST "1.0");
	root_node = xmlNewNode(NULL, BAD_CAST "svg");
	xmlDocSetRootElement(xdoc, root_node);
	//xmlNsPtr name=NULL;
	//name = getNamespace(xdoc);

	//xmlNsPtr ns = NULL;
	//name = getNamespace(xdoc);
//	ns= xmlNewNs(root_node, BAD_CAST "http://www.w3.org/2000/svg", NULL);
//	xmlSetNs(root_node, ns);
		xmlNewProp(root_node, BAD_CAST "title", BAD_CAST doc->title);
	s	xmlNewProp(root_node, BAD_CAST "desc", BAD_CAST doc->description);
		xmlNewChild(root_node,NULL, BAD_CAST "xmlns", BAD_CAST doc->namespace);

		if(doc->rectangles->head!=NULL)
		{
            createRectangle(doc, doc->rectangles->head, root_node);
		}

		if(doc->circles->head!=NULL)
		{
            createCircle(doc, doc->circles->head, root_node);
		}

		if(doc->paths->head!=NULL)
		{
            createPath(doc, doc->paths->head, root_node);
		}

		if(doc->groups->head!=NULL)
		{
            createGroups(doc, doc->groups->head, root_node);
		}

    if(doc->otherAttributes->head!=NULL){
		Node *ahead = ((List *)doc->otherAttributes)->head;
		Node *tempa = ahead;
		if(ahead)
		{
				while(tempa)
				{
						xmlNewChild(root_node,NULL, BAD_CAST ((Attribute *)tempa->data)->name, BAD_CAST ((Attribute *)tempa->data)->value);
						 tempa = tempa->next;
				}
		}
	}*/
	return NULL;
}




void createRectangle(SVGimage *doc, Node *rhead, xmlNodePtr root_node){

		/*Node *head = rhead;
		Node *temp = head;
		while(temp)
		{
			char x[100];
			sprintf(x, "%f%s", ((Rectangle *)temp->data)->x, ((Rectangle *)temp->data)->units);
			char y[100];
      sprintf(y, "%f%s", ((Rectangle *)temp->data)->y, ((Rectangle *)temp->data)->units);
			char width[100];
      sprintf(width, "%f%s", ((Rectangle *)temp->data)->width, ((Rectangle *)temp->data)->units);
			char height[100];
      sprintf(height, "%f%s", ((Rectangle *)temp->data)->height, ((Rectangle *)temp->data)->units);

		  xmlNodePtr rect = NULL;
			rect = xmlNewChild(root_node, NULL, BAD_CAST "rect", NULL);
			if(((Rectangle *)temp->data)->otherAttributes!=NULL)
			{
			Node *ahead = ((List *)((Rectangle *)temp->data)->otherAttributes)->head;
			Node *tempa = ahead;
			if(ahead)
			{
			    while(tempa)
			    {
				     	xmlNewChild(rect,NULL, BAD_CAST ((Attribute *)tempa->data)->name, BAD_CAST ((Attribute *)tempa->data)->value);
					     tempa = tempa->next;
			    }
		  }
		}
			xmlNewProp(rect, BAD_CAST "x",  BAD_CAST x);
			xmlNewProp(rect, BAD_CAST "y",  BAD_CAST y);
			xmlNewProp(rect, BAD_CAST "width",  BAD_CAST width);
			xmlNewProp(rect, BAD_CAST "height",  BAD_CAST height);

			temp = temp->next;
		}*/
	}





	void createCircle(SVGimage *doc, Node *chead, xmlNodePtr root_node){

			Node *head = chead;
			Node *temp = head;
			while(temp)
			{
				char cx[100];
        sprintf(cx, "%f%s", ((Circle *)temp->data)->cx, ((Circle *)temp->data)->units);
				char cy[100];
        sprintf(cy, "%f%s", ((Circle *)temp->data)->cy, ((Circle *)temp->data)->units);
				char r[100];
				sprintf(r, "%f%s", ((Circle *)temp->data)->r, ((Circle *)temp->data)->units);

			  xmlNodePtr circ = NULL;
				circ = xmlNewChild(root_node, NULL, BAD_CAST "circle", NULL);
				if(((Circle *)temp->data)->otherAttributes!=NULL)
				{
				List * at = ((Circle *)temp->data)->otherAttributes;
				Node *ahead = at->head;
				Node *tempa = ahead;
				if(ahead)
				{
				    while(tempa)
				    {
					     	xmlNewChild(circ,NULL, BAD_CAST ((Attribute *)tempa->data)->name, BAD_CAST ((Attribute *)tempa->data)->value);
						     tempa = tempa->next;
				    }
			  }
			}
				xmlNewProp(circ, BAD_CAST "cx",  BAD_CAST cx);
				xmlNewProp(circ, BAD_CAST "cy",  BAD_CAST cy);
				xmlNewProp(circ, BAD_CAST "r",  BAD_CAST r);
				temp = temp->next;
			}
		}



		void createPath(SVGimage *doc, Node *phead, xmlNodePtr root_node){

				Node *head = phead;
				Node *temp = head;
				while(temp)
				{
					xmlNodePtr path = NULL;
					path = xmlNewChild(root_node, NULL, BAD_CAST "path", NULL);
					Node *ahead = ((List *)((Path *)temp->data)->otherAttributes)->head;
					Node *tempa = ahead;
					if(ahead)
					{
							while(tempa)
							{
									xmlNewChild(path,NULL, BAD_CAST ((Attribute *)tempa->data)->name, BAD_CAST ((Attribute *)tempa->data)->value);
									 tempa = tempa->next;
							}
					}
					xmlNewProp(path, BAD_CAST "d",  BAD_CAST ((Path *)temp->data)->data);
					temp = temp->next;
				}
			}



	void createGroups(SVGimage *doc, Node *ghead, xmlNodePtr root_node){
/*
		Node *head = ghead;
		Node *temp = head;
		while(temp)
		{
			Group *gp1 = (Group*) temp;

			xmlNodePtr gp = NULL;
			gp = xmlNewChild(root_node, NULL, BAD_CAST "g", NULL);

			if(gp1->rectangles->head!=NULL)
			{
							createRectangle(doc, gp1->rectangles->head, gp);
			}

			if(gp1->circles->head!=NULL)
			{
							createCircle(doc, gp1->circles->head, gp);
			}

			if(gp1->paths->head!=NULL)
			{
							createPath(doc, gp1->paths->head, gp);
			}

			if(gp1->paths->head!=NULL)
			{
							createGroups(doc, gp1->groups->head, gp);
			}

			Node *ahead = ((List *)gp1->otherAttributes)->head;
			Node *tempa = ahead;
			if(ahead)
			{
					while(tempa)
					{
							xmlNewChild(gp,NULL, BAD_CAST ((Attribute *)tempa->data)->name, BAD_CAST ((Attribute *)tempa->data)->value);
							 tempa = tempa->next;
					}
			}
			temp = temp->next;
		}*/
	}



void setCircle(SVGimage* image, int elemIndex, Attribute* newAttribute){

	ListIterator iter = createIterator(image->circles);
	void *elems;
	int count = 0;
	while((elems = nextElement(&iter)) !=NULL)
	{
			if(count==elemIndex)
			{
				Circle *cir= (Circle*)elems;
				if(strcmp(newAttribute->name, "cx")==0){cir->cx=atof(newAttribute->value);}
				if(strcmp(newAttribute->name, "cy")==0){cir->cy=atof(newAttribute->value);}
				if(strcmp(newAttribute->name, "r")==0){cir->r=atof(newAttribute->value);}

				ListIterator iter = createIterator(cir->otherAttributes);
		  	void *elem;
				int b=0;
		  	while((elem = nextElement(&iter)) !=NULL)
		 	 {
				      Attribute *at = (Attribute*)elem;
             if(strcmp(newAttribute->name, at->name)==0)
						 {
							 at->value = realloc(at->value, sizeof(char)* strlen(newAttribute->value)+1);
							 strcpy(at->value, newAttribute->value);
							 b=1;
						 }
			 }

				//do units toooooo
			 if(b==0)
				{
					 Attribute *at = malloc(sizeof(Attribute));
					 at->name = malloc(sizeof(char)* strlen(newAttribute->name)+1);
					 at->value = malloc(sizeof(char)* strlen(newAttribute->value)+1);
					 strcpy(at->name, newAttribute->name);
					 strcpy(at->value, newAttribute->value);
					 insertBack(cir->otherAttributes, at);
				}
		 }
	 count++;
	}
}

void setRect(SVGimage* image, int elemIndex, Attribute* newAttribute){

	ListIterator iter = createIterator(image->rectangles);
  void *elems;
  int count = 0;
  while((elems = nextElement(&iter)) !=NULL)
  {
		 if(count==elemIndex)
		 {
			 Rectangle *rec= (Rectangle*)elems;
			 if(strcmp(newAttribute->name, "x")==0){rec->x=atof(newAttribute->value);}
			 if(strcmp(newAttribute->name, "y")==0){rec->y=atof(newAttribute->value);}
			 if(strcmp(newAttribute->name, "width")==0){rec->width=atof(newAttribute->value);}
			 if(strcmp(newAttribute->name, "height")==0){rec->height=atof(newAttribute->value);}

			 ListIterator iter = createIterator(rec->otherAttributes);
			 void *elem;
			 int b=0;
			 while((elem = nextElement(&iter)) !=NULL)
			{
						 Attribute *at = (Attribute*)elem;
						if(strcmp(newAttribute->name, at->name)==0)
						{
							at->value = realloc(at->value, sizeof(char)* strlen(newAttribute->value)+1);
							strcpy(at->value, newAttribute->value);
							b=1;
						}
			}

			 //do units toooooo
			 if(b==0)
			 {
					Attribute *at = malloc(sizeof(Attribute));
					at->name = malloc(sizeof(char)* strlen(newAttribute->name)+1);
					at->value = malloc(sizeof(char)* strlen(newAttribute->value)+1);
					strcpy(at->name, newAttribute->name);
					strcpy(at->value, newAttribute->value);
					insertBack(rec->otherAttributes, at);
			 }
	 }
	count++;
 }
}


void setPath(SVGimage* image, int elemIndex, Attribute* newAttribute){

	ListIterator iter = createIterator(image->paths);
  void *elems;
  int count = 0;
  while((elems = nextElement(&iter)) !=NULL)
  {
		 if(count==elemIndex)
		 {
			 Path *pt = (Path*)elems;
			 if(strcmp(newAttribute->name, "d")==0)
			 {
				 pt->data = realloc(pt->data, sizeof(char)* strlen(newAttribute->value)+1);
				  strcpy(pt->data,newAttribute->value);
			 }

			 ListIterator iter = createIterator(pt->otherAttributes);
			 void *elem;
			 int b=0;
			 while((elem = nextElement(&iter)) !=NULL)
			{
						 Attribute *at = (Attribute*)elem;
						if(strcmp(newAttribute->name, at->name)==0)
						{
							at->value = realloc(at->value, sizeof(char)* strlen(newAttribute->value)+1);
							strcpy(at->value, newAttribute->value);
							b=1;
						}
			}

			 //do units toooooo
			if(b==0)
			 {
					Attribute *at = malloc(sizeof(Attribute));
					at->name = malloc(sizeof(char)* strlen(newAttribute->name)+1);
					at->value = malloc(sizeof(char)* strlen(newAttribute->value)+1);
					strcpy(at->name, newAttribute->name);
					strcpy(at->value, newAttribute->value);
					insertBack(pt->otherAttributes, at);
			 }
	 }
	count++;
 }
}

void setGroup(SVGimage* image, int elemIndex, Attribute* newAttribute){

	ListIterator iter = createIterator(image->groups);
	void *elems;
	int count = 0;
	while((elems = nextElement(&iter)) !=NULL)
	{
		 if(count==elemIndex)
		 {
			 Group *gp = (Group*)elems;

			  ListIterator iter = createIterator(gp->otherAttributes);
		  	void *elem;
				int b=0;
		  	while((elem = nextElement(&iter)) !=NULL)
		 	 {
				      Attribute *at = (Attribute*)elem;
             if(strcmp(newAttribute->name, at->name)==0)
						 {
							 at->value = realloc(at->value, sizeof(char)* strlen(newAttribute->value)+1);
							 strcpy(at->value, newAttribute->value);
							 b=1;
						 }
			 }

			 //do units toooooo
			 if(b==0)
			 {
					Attribute *at = malloc(sizeof(Attribute));
					at->name = malloc(sizeof(char)* strlen(newAttribute->name)+1);
					at->value = malloc(sizeof(char)* strlen(newAttribute->value)+1);
					strcpy(at->name, newAttribute->name);
					strcpy(at->value, newAttribute->value);
					insertBack(gp->otherAttributes, at);
			 }
	 }
	count++;
 }
}

char * SVGJSON(char * fileName);
char * SVGJSON(char * fileName){

   SVGimage *svg = NULL;
   svg = createSVGimage(fileName);
   char * result = SVGtoJSON(svg);
  deleteSVGimage(svg);

  return result;
}

char *PathsJSON(char *fileName);

char *PathsJSON(char *fileName){

  SVGimage *svg = NULL;
   svg = createSVGimage(fileName);
   char * result = pathListToJSON(svg->paths);
  deleteSVGimage(svg);

   return result;
}


char *GroupsJSON(char *fileName);

char *GroupsJSON(char *fileName){

  SVGimage *svg = NULL;
   svg = createSVGimage(fileName);
   char * result = groupListToJSON(svg->groups);
  deleteSVGimage(svg);

   return result;
}

char *CirclesJSON(char *fileName);

char *CirclesJSON(char *fileName){

  SVGimage *svg = NULL;
   svg = createSVGimage(fileName);
   char * result = circListToJSON(svg->circles);
  deleteSVGimage(svg);

   return result;
}


char *RectsJSON(char *fileName);

char *RectsJSON(char *fileName){

  SVGimage *svg = NULL;
   svg = createSVGimage(fileName);
   char * result = rectListToJSON(svg->rectangles);
  deleteSVGimage(svg);

   return result;
}

char* SVGtdJSON(const SVGimage* imge);

char* SVGtdJSON(const SVGimage* imge){

	if(imge==NULL){return NULL;}

	SVGimage *img = (SVGimage *) imge;

	char* ret;
	char *s = malloc(10);

	ret= malloc(sizeof(char)*10000);
	strcpy(ret, "{\"title\":\"");
	strcat(ret, img->title);
	strcat(ret, "\",\"desc\":\"");
	strcat(ret, img->description);
	strcat(ret, "\"}");

	free(s);

	return ret;
}

char *svgandJSON(char *fileName);

char *svgandJSON(char *fileName){

  SVGimage *svg = NULL;
   svg = createSVGimage(fileName);
   char * result = SVGtdJSON(svg);
  deleteSVGimage(svg);

   return result;
}
