#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <SDL/SDL_opengl.h>
#include <math.h>
#include <string.h>
#include "drawtk.h"
#include "shapes.h"

#define TWO_PI ((float)(2.0*M_PI))

API_EXPORTED
dtk_hshape dtk_create_circle(struct dtk_shape* shp, float cx, float cy, float r, int isfull, const float* color, unsigned int numpoints)
{	                  
	unsigned int i,j;
	struct single_shape* sinshp;
	GLuint numvert = isfull ? numpoints+1 : numpoints;
	GLuint numind = isfull ? numpoints+2 : numpoints;
	GLenum primtype = isfull ? GL_TRIANGLE_FAN : GL_LINE_LOOP;

	shp = create_generic_shape(shp, numvert, NULL, NULL,numind, NULL, primtype, color,NULL);
	if (!shp)
		return NULL;

	sinshp = shp->data;

	i = j = 0;
	// Put center and finish fan loop if full
	if (isfull) {
		sinshp->vertices[0] = cx;
		sinshp->vertices[1] = cy;
		sinshp->indices[0] = 0;
		sinshp->indices[numind-1] = 1;
		i++;
	}

	// Create the circle, radius is expressed in width relative coordinates
	while (i<numvert) { 
		sinshp->indices[i] = i;
		sinshp->vertices[2*i]=r*cos((float)j*TWO_PI/numpoints) +cx;
		sinshp->vertices[2*(i++)+1]=r*sin((float)(j++)*TWO_PI/numpoints) +cy;
	}
	
	return shp;
} 

API_EXPORTED
dtk_hshape dtk_create_cross(struct dtk_shape* shp, float cx, float cy, float width, const float* color) 
{
	GLfloat vertices[8];
	GLuint indices[4] = {0, 2, 1, 3};
	GLenum primtype = GL_LINES;

	// To decide: center position. For the time being the shape is drawn
	// respect to the center
	//cx = cx - width/2;
	//cy = cy - width/2;

	vertices[0] = -width/2 + cx;
	vertices[1] = 0 + cy;
	
	vertices[2] = cx;
	vertices[3] = width/2 + cy;
	
	vertices[4] = width/2 + cx;
	vertices[5] = 0 + cy;
	
	vertices[6] = cx;
	vertices[7] = -width/2 + cy;

	shp = create_generic_shape(shp, 4, vertices, NULL,4, indices, primtype, color,NULL);
	
	return shp;
}

API_EXPORTED
dtk_hshape dtk_create_rectangle_2p(struct dtk_shape* shp, float p1_x, float p1_y, float p2_x, float p2_y, int isfull, const float* color)
{
	float vertices[8];
	GLuint indices[4] = {0, 1, 2, 3};
	GLenum primtype = isfull ? GL_TRIANGLE_FAN : GL_LINE_LOOP;
	
	vertices[0] = p1_x;
	vertices[1] = p1_y;
	
	vertices[2] = p1_x;
	vertices[3] = p2_y;
	
	vertices[4] = p2_x;
	vertices[5] = p2_y;
	
	vertices[6] = p2_x;
	vertices[7] = p1_y;

	shp = create_generic_shape(shp, 4, vertices, NULL,4, indices, primtype, color,NULL);
	
	return shp;
}


API_EXPORTED
dtk_hshape dtk_create_rectangle_hw(struct dtk_shape* shp, float cx, float cy, float width, float height, int isfull, const float* color)
{
	float vertices[8];
	GLuint indices[4] = {0, 1, 2, 3};
	GLenum primtype = isfull ? GL_TRIANGLE_FAN : GL_LINE_LOOP;
	
	vertices[0] = 0 + cx;
	vertices[1] = 0 + cy;
	
	vertices[2] = 0 + cx;
	vertices[3] = height + cy;
	
	vertices[4] = width + cx;
	vertices[5] = height + cy;
	
	vertices[6] = width + cx;
	vertices[7] = 0 + cy;

	shp = create_generic_shape(shp, 4, vertices, NULL,4, indices, primtype, color,NULL);
	
	return shp;
}


API_EXPORTED
dtk_hshape dtk_create_arrow(struct dtk_shape* shp, float cx, float cy, float width, float height, int isfull, const float* color)
{
	float vertices[14];
	GLuint indices[7] = {0, 1, 2, 3, 4, 5, 6};
	GLenum primtype = isfull ? GL_TRIANGLE_FAN : GL_LINE_LOOP;
	float Wtot = width;
	float Htot = height;
	float Warr = Wtot/3;
	float Hrec = Htot/2;

	// To decide: center position. For the time being the shape is drawn
	// respect to the center
	cx = cx - width/2;
	cy = cy;

	vertices[0] = 0 + cx;
	vertices[1] = 0 + cy;
	
	vertices[2] = Warr + cx;
	vertices[3] = -Htot/2 + cy;
	
	vertices[4] = Warr + cx;
	vertices[5] = -Hrec/2 + cy;
	
	vertices[6] = Wtot + cx;
	vertices[7] = -Hrec/2 + cy;
	
	vertices[8] = Wtot + cx;
	vertices[9] = Hrec/2 + cy;
	
	vertices[10] = Warr + cx;
	vertices[11] = Hrec/2 + cy;
	
	vertices[12] = Warr + cx;
	vertices[13] = Htot/2 + cy;

	shp = create_generic_shape(shp, 7, vertices, NULL, 7, indices, primtype, color,NULL);
	return shp;
}


API_EXPORTED
dtk_hshape dtk_create_triangle(struct dtk_shape* shp, float x1, float y1, float x2, float y2, float x3, float y3, int isfull, const float* color)
{
	GLfloat vertices[] = {x1, y1, x2, y2, x3, y3};
	GLuint indices[] = {0, 1, 2};
	GLenum primtype = isfull ? GL_TRIANGLES : GL_LINE_LOOP;
	
	return create_generic_shape(shp, 3, vertices, NULL, 3, indices, primtype, color,NULL);
}


API_EXPORTED
dtk_hshape dtk_create_line(struct dtk_shape* shp, float x1, float y1, float x2, float y2, const float* color)
{
	GLfloat vertices[] = {x1, y1, x2, y2};
	GLuint indices[] = {0, 1};
	
	return create_generic_shape(shp, 2, vertices, NULL, 2, indices, GL_LINES, color,NULL);
}


API_EXPORTED
dtk_hshape dtk_create_shape(struct dtk_shape* shp, unsigned int numvert, const float* vertex_array, int isfull, const float* color)
{
	unsigned int i;
	struct single_shape* sinshp; 
	GLenum primtype = isfull ? GL_TRIANGLE_FAN : GL_LINE_LOOP;

	shp = create_generic_shape(shp, numvert, vertex_array, NULL, numvert, NULL, primtype, color,NULL);
	if (shp) {
		sinshp = shp->data;
		for (i=0; i<numvert; i++)
			sinshp->indices[i] = i;
	}

	return shp;
}


API_EXPORTED
dtk_hshape dtk_create_image(struct dtk_shape* shp, float x, float y, float width, float height, const float* color, dtk_htex image)
{
	GLfloat vertices[8];
	GLfloat textcoords[8];
	GLuint indices[4] = {0, 1, 2, 3};
	GLenum primtype = GL_TRIANGLE_FAN;
	
	// To decide: center position. For the time being the shape is drawn
	// respect to the center
	x = x - width/2;
	y = y - height/2;

	vertices[0] = 0 + x;
	vertices[1] = 0 + y;
	
	vertices[2] = 0 + x;
	vertices[3] = height + y;
	
	vertices[4] = width + x;
	vertices[5] = height + y;
	
	vertices[6] = width + x;
	vertices[7] = 0 + y;
	
	textcoords[0] = 0;
	textcoords[1] = 0;
	
	textcoords[2] = 0;
	textcoords[3] = 1;
	
	textcoords[4] = 1;
	textcoords[5] = 1;
	
	textcoords[6] = 1;
	textcoords[7] = 0;
	
	shp = create_generic_shape(shp, 4, vertices, textcoords, 4, indices, primtype, color, image);
	
	return shp;
}


API_EXPORTED
dtk_hshape dtk_create_string(struct dtk_shape* shp, const char* str_text, float size, float x, float y, const float* color, const char* filepath)
{
	GLfloat primv[8] = {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f};
	float tex_w, tex_h, tx, ty;
	GLint primind[6] = {0, 1, 2, 0, 2, 3};
	GLfloat *vertices, *texcoords;
	GLuint* indices;
	int i, j, len, row, col;
	int row_num = 14, col_num =16;
	GLenum primtype = GL_TRIANGLES;

	len = str_text ? strlen(str_text) : 0;

	shp = create_generic_shape(shp, 4*len, NULL, NULL, 6*len, NULL, primtype, color, dtk_load_image(filepath, 3));
	if (!shp)
		return NULL;
	
	indices = ((struct single_shape*)(shp->data))->indices;
	vertices = ((struct single_shape*)(shp->data))->vertices;
	texcoords = ((struct single_shape*)(shp->data))->texcoords;

	// Size of a letter in texture coordinates
	tex_w = (1.0f/(float)col_num);
	tex_h = (1.0f/(float)row_num);
	
	// Create all other letter vertices
	for(i=0; i<len; i++) {
		// Find coordinates of letter in the image
		row = (str_text[i]-32)/col_num;
		col = (str_text[i]-32)%col_num;

		// Texture coordinate of down left vertex of the letter
		tx = tex_w*(float)col;
		ty = 1.0f - tex_h*(float)(row+1);
		
		for (j=0; j<4; j++) {
			vertices[8*i+2*j  ] = size * primv[2*j  ] + x;
			vertices[8*i+2*j+1] = size * primv[2*j+1] + y;
			
			texcoords[8*i+2*j  ] = tex_w * primv[2*j  ] + tx;
			texcoords[8*i+2*j+1] = tex_h * primv[2*j+1] + ty;
		}
		x += size;
	}

	//Create all indices
	for(i=0;i<len;i++)
		for (j=0; j<6; j++)
			indices[6*i+j] = 4*i + primind[j];
		
	return shp;
}

