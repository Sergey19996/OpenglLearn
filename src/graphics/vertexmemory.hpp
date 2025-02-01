#ifndef  VERTEXMEMORY_HPP
#define VERTEXMEMORY_HPP
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <map>

// class for buffer object
// -VBOs, EBos, etc
class BufferObject {
public:
	//value/location
	GLuint val;
	//type of buffer (GL_ARRAY_BUFFER | GL_ELEMENT_ARRAY_BUFFER , etc)
	GLenum type;

	BufferObject() {}

	BufferObject(GLenum type):type(type) {
	}
	//generate object
	void generate() {
		glGenBuffers(1, &val);
	}

	//bind object
	void bind() {
		glBindBuffer(type, val);
	}

	

	//set data
	template<typename T>
	void setData(GLuint noElements, T* data, GLenum usage) {
		glBufferData(type, noElements * sizeof(T), data, usage);

	}
	//update 
	template<typename T>
	void updateData(GLintptr offset, GLuint noElements, T* data) {
		glBufferSubData(type, offset, noElements * sizeof(T), data);
	}

	//set atribut pointers
	template<typename T>
	void setAttPointer(GLuint idx, GLint size, GLenum type, GLuint stride, GLuint positions, GLuint divisor = 0) {
		glVertexAttribPointer(idx, size, type, GL_FALSE, stride * sizeof(T), (void*)(positions * sizeof(T)));
		glEnableVertexAttribArray(idx);
		if (divisor > 0) {
			//reset idx attribute every divisor iteration (instancing)
			glVertexAttribDivisor(idx, divisor);

		}
	}

	//clear buffer objects(bind 0)
	void clear() {
		glBindBuffer(type, 0);

	}

	//cleanup
	void cleanUp() {
		glDeleteBuffers(1, &val);
	}

};

//class for aaray object VAO

class ArrayObject {
public:
	//value/location
	GLuint val;
	
	//map of names to buffers
	std::map<const char*, BufferObject> buffers;

	//get buffer ovveride operator []
	BufferObject& operator[] (const char* key) {
		return buffers[key];
	}

	//generate objects
	void generate() {
		glGenVertexArrays(1, &val);

	}

	//bind
	void bind() {
		glBindVertexArray(val);
	}

	//draw arrays
	void draw(GLenum mode, GLuint first, GLuint count) {
		glDrawArrays(mode, first, count);
	}

	//draw 
	void draw(GLenum mode, GLuint count, GLenum type, GLint indices, GLuint instancecount = 1) {
		glDrawElementsInstanced(mode, count, type, (void*)indices, instancecount);
	}


	//cleanup
	void cleanUp() {
		glDeleteVertexArrays(1, &val);
		for (auto& pair : buffers) {
			pair.second.cleanUp();
		}
	}

	//clear array objects (bind 0)
	static void clear() {
		glBindVertexArray(0);
	}

};

#endif // ! GLMEMORY_HPP
