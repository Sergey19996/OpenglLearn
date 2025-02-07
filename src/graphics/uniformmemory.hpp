#ifndef UNIFORMEMORY_H
#define UNIFORMEMORY_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include <vector>
#include <string>

#include "vertexmemory.hpp"

#define N 4 // word size

namespace UBO {
	enum class Type : unsigned char {
		BOOL = 0,
		SCALAR,
		VEC2,
		VEC3,
		VEC4,
		ARRAY,
		STRUCT,
		INVALID
  };

	
	typedef struct Element {
		Type type;
		unsigned int length; // length of the array or num elements in structure
		std::vector<Element> list; //for struct (list of sub-elements), or array (1st slot is the type)


		std::string typeStr() {
			switch (type) {
			case Type::BOOL: return "bool";
			case Type::SCALAR: return "scalar";  // any 4 byte vaule
			case Type::VEC2: return "vec2";
			case Type::VEC3: return "vec3";
			case Type::VEC4: return "vec4";
			case Type::ARRAY: return "array<" + list[0].typeStr() + ">";
			case Type::STRUCT: return "struct";
			default: return "invalid";

			}
		}
		Element(Type type = Type::SCALAR) : type(type), length(0), list(0) {}

	} Element;

	Element newScalar() {
		return Element();
	}

	Element newVec(unsigned char dim) {
		switch (dim) // witch vector to assign to it
		{
		case 2: return Element(Type::VEC2);
		case 3: return Element(Type::VEC3);
		case 4:
		default:
			return Element(Type::VEC4);

		};
	}

	Element newArray(unsigned int length, Element arrElement) {
		Element ret(Type::ARRAY);
		ret.length = length;
		ret.list = { arrElement };
		ret.list.shrink_to_fit();

		return ret;
	}

	Element newColMat(unsigned char cols, unsigned char rows) {
		return newArray(cols, newVec(rows));
	}

	Element newColMatArray(unsigned int noMatrices, unsigned char cols, unsigned char rows) {
		return newArray(noMatrices * cols, newVec(rows));

	}

	Element newRowMat(unsigned char rows, unsigned char cols) {
		return newArray(rows, newVec(cols));
	}

	Element newRowMatArray(unsigned int noMatrices, unsigned char cols, unsigned char rows) {
		return newArray(noMatrices * rows, newVec(cols));
	}

	Element newStruct(std::vector<Element> subElements) {
		Element ret(Type::STRUCT);
		ret.list.insert(ret.list.end(), subElements.begin(), subElements.end());
		ret.length = ret.list.size();
		return ret;
	}
	class UBO : public BufferObject {
	public:
		Element block; // root element of the UBO (struct)
		UBO()
			:BufferObject(GL_UNIFORM_BUFFER), block(newStruct({})) {}

		UBO(std::vector<Element>elements)
			:BufferObject(GL_UNIFORM_BUFFER), block(newStruct(elements)) {}

		void addElement(Element element) {
			block.list.push_back(element);
			block.length++;

		}

		//iteraation variables
		std::vector<std::pair<unsigned int, Element*>> indexStack; // stack to keep track of the  nested indices
		int currentDepth; // current size of the stack -1

		//initialize iterator
		void startWrite() {
			currentDepth = 0;
			indexStack.clear();
			indexStack.push_back({ 0,&block });
		}
		//Nex element in iteration
		Element getNextElement() {
			if (currentDepth < 0) {
				return Type::INVALID;

			}
			//get current deepest arraay/struct (last element in the stack)
			Element* currentElement = indexStack[currentDepth].second;


			//get element at the specified index within that iterable
			if (currentElement->type == Type::STRUCT) {
				currentElement = &currentElement->list[indexStack[currentDepth].first];
			}
			else { //array
				currentElement = &currentElement->list[0];
			}

			//traverse down to deepest array /struct
				while (currentElement->type == Type::STRUCT || currentElement->type == Type::ARRAY) {
					currentDepth++;
					indexStack.push_back({ 0,currentElement });
					currentElement = &currentElement->list[0];

				} 
				//now have current element (not an iterable)
				//pop from stack if necessary
				for (int i = currentDepth; i >= 0; i--) {
					int advanceIdx = ++indexStack[i].first; // move cursor forward int the iterrable
					if (advanceIdx >= indexStack[i].second->length) {
						//iterate through entire array or struct
						//pop iterate from the stack
						indexStack.erase(indexStack.begin() + i);
						currentDepth--;
					}
					else
					{
						break;
					}
				}

				return *currentElement;

			
		}
	};
}

#endif // !UNIFORMEMORY_H
