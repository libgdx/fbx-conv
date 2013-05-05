#ifndef G3DJMESHPART_H
#define G3DJMESHPART_H

using namespace gameplay;

namespace fbxconv {
	class G3djMeshPart : public MeshPart {
	public:
		void setMaterialId(const char* mId){
			materialId = new char[strlen(mId)+1];
			strcpy(materialId, mId);
		}

		char* getMaterialId(){ return materialId; }
	private:
		char* materialId;
	};
};

#endif