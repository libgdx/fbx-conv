#ifndef G3DJNODE_H
#define G3DJNODE_H

#include "gameplay/Node.h"
#include "gameplay/Vector3.h"
#include "gameplay/Vector4.h"

using namespace gameplay;

namespace fbxconv {
	class G3djNode : public Node {
	public:
		void setTranslation(float x, float y, float z);
		void setRotation(float x, float y, float z, float w);
		void setScale(float x, float y, float z);

		Vector3 getTranslation();
		Quaternion getRotation();
		Vector3 getScale();

	private:
		Vector3 translation;
		Quaternion rotation;
		Vector3 scale;
	};
};

#endif