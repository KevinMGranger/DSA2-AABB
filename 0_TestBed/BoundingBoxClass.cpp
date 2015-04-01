
#include "BoundingBoxClass.h"
//  BoundingBoxClass
void BoundingBoxClass::Init(void)
{
	m_bInitialized = false;
	m_fRadius = 0.0f;
	m_v3Centroid = vector3(0.0f);
	m_sName = "NULL";
}
void BoundingBoxClass::Swap(BoundingBoxClass& other)
{
	std::swap(m_bInitialized, other.m_bInitialized);
	std::swap(m_fRadius, other.m_fRadius);
	std::swap(m_v3Centroid, other.m_v3Centroid);
	std::swap(m_sName, other.m_sName);
}
void BoundingBoxClass::Release(void)
{
	//No pointers to release
}
//The big 3
BoundingBoxClass::BoundingBoxClass(){Init();}
BoundingBoxClass::BoundingBoxClass(BoundingBoxClass const& other)
{
	m_bInitialized = other.m_bInitialized;
	m_fRadius = other.m_fRadius;
	m_v3Centroid = other.m_v3Centroid;
	m_sName = other.m_sName;
}
BoundingBoxClass& BoundingBoxClass::operator=(BoundingBoxClass const& other)
{
	if(this != &other)
	{
		Release();
		Init();
		BoundingBoxClass temp(other);
		Swap(temp);
	}
	return *this;
}
BoundingBoxClass::~BoundingBoxClass(){Release();};
//Accessors
bool BoundingBoxClass::IsInitialized(void){ return m_bInitialized; }
float BoundingBoxClass::GetRadius(void){ return m_fRadius; }
vector3 BoundingBoxClass::GetCentroid(void){ return m_v3Centroid; }
String BoundingBoxClass::GetName(void){return m_sName;}
//Methods
void BoundingBoxClass::GenerateBoundingBox(String a_sInstanceName)
{
	//If this has already been initialized there is nothing to do here
	if(m_bInitialized)
		return;
	MeshManagerSingleton* pMeshMngr = MeshManagerSingleton::GetInstance();
	if(pMeshMngr->IsInstanceCreated(a_sInstanceName))
	{
		m_sName = a_sInstanceName;
		
		std::vector<vector3> lVertices = pMeshMngr->GetVertices(m_sName);
		unsigned int nVertices = lVertices.size();
		m_v3Centroid = lVertices[0];

		v3Max = lVertices[0];
		v3Min = lVertices[0];

		auto &right = v3Max.x;
		auto &left = v3Min.x;
		auto &top = v3Max.y;
		auto &bottom = v3Min.y;
		auto &front = v3Max.z;
		auto &back = v3Min.z;

		for(unsigned int nVertex = 1; nVertex < nVertices; nVertex++)
		{
			//m_v3Centroid += lVertices[nVertex];
			//x
			if(left > lVertices[nVertex].x)
				left = lVertices[nVertex].x;
			else if(right < lVertices[nVertex].x)
				right = lVertices[nVertex].x;
			//y
			if(top > lVertices[nVertex].y)
				top = lVertices[nVertex].y;
			else if(bottom < lVertices[nVertex].y)
				bottom = lVertices[nVertex].y;
			//z
			if(front > lVertices[nVertex].z)
				front = lVertices[nVertex].z;
			else if(back < lVertices[nVertex].z)
				back = lVertices[nVertex].z;
		}

		m_v3Centroid = (v3Max + v3Min) / 2.0f;

		/*
		float radii[] = {
			abs(v3Max.x - m_v3Centroid.x),
			abs(v3Min.x - m_v3Centroid.x),
			abs(v3Max.y - m_v3Centroid.y),
			abs(v3Min.y - m_v3Centroid.y),
			abs(v3Max.z - m_v3Centroid.z),
			abs(v3Min.z - m_v3Centroid.z)
		};

		width = radii[0] + radii[1];
		height = radii[2] + radii[3];
		depth = radii[4] + radii[5];

		maxdim = width;

		if (height > maxdim) maxdim = height;
		if (depth > maxdim) maxdim = depth;

		m_fRadius = radii[0];

		for (int i = 1; i < 6; i++)
		{
			if (radii[i] > m_fRadius) m_fRadius = radii[i];
		}
		*/

		m_fRadius = glm::distance(m_v3Centroid, lVertices[0]);
		for(unsigned int nVertex = 1; nVertex < nVertices; nVertex++)
		{
			float m_fRadiusNew = glm::distance(m_v3Centroid, lVertices[nVertex]);
			if(m_fRadius < m_fRadiusNew)
				m_fRadius = m_fRadiusNew;
		}

		m_bInitialized = true;
	}
}
void BoundingBoxClass::AddBoxToRenderList(matrix4 a_mModelToWorld, vector3 a_vColor, bool a_bRenderCentroid)
{
	if(!m_bInitialized)
		return;
	MeshManagerSingleton* pMeshMngr = MeshManagerSingleton::GetInstance();
	if(a_bRenderCentroid)
		pMeshMngr->AddAxisToQueue(a_mModelToWorld * glm::translate(m_v3Centroid));
	pMeshMngr->AddCubeToQueue(a_mModelToWorld * glm::translate(m_v3Centroid) * glm::scale(vector3(m_fRadius * 2.0f)), a_vColor, MERENDER::WIRE);
}