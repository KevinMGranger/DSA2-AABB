#include "BoundingBoxManagerSingleton.h"
#include <cstdio>

//  BoundingBoxManagerSingleton
BoundingBoxManagerSingleton* BoundingBoxManagerSingleton::m_pInstance = nullptr;
void BoundingBoxManagerSingleton::Init(void)
{
	m_nBoxen = 0;
}
void BoundingBoxManagerSingleton::Release(void)
{
	//Clean the list of spheres
	for(int n = 0; n < m_nBoxen; n++)
	{
		//Make sure to release the memory of the pointers
		if(m_lBox[n] != nullptr)
		{
			delete m_lBox[n];
			m_lBox[n] = nullptr;
		}
	}
	m_lBox.clear();
	m_lMatrix.clear();
	m_lColor.clear();
	m_nBoxen = 0;
}
BoundingBoxManagerSingleton* BoundingBoxManagerSingleton::GetInstance()
{
	if(m_pInstance == nullptr)
	{
		m_pInstance = new BoundingBoxManagerSingleton();
	}
	return m_pInstance;
}
void BoundingBoxManagerSingleton::ReleaseInstance()
{
	if(m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
//The big 3
BoundingBoxManagerSingleton::BoundingBoxManagerSingleton(){Init();}
BoundingBoxManagerSingleton::BoundingBoxManagerSingleton(BoundingBoxManagerSingleton const& other){ }
BoundingBoxManagerSingleton& BoundingBoxManagerSingleton::operator=(BoundingBoxManagerSingleton const& other) { return *this; }
BoundingBoxManagerSingleton::~BoundingBoxManagerSingleton(){Release();};
//Accessors
//Get and set # of boxes
int BoundingBoxManagerSingleton::GetBoxTotal(void){ return m_nBoxen; }

//--- Non Standard Singleton Methods
void BoundingBoxManagerSingleton::GenerateBoundingBox(String a_sInstanceName)
{
	MeshManagerSingleton* pMeshMngr = MeshManagerSingleton::GetInstance();
	//Verify the instance is loaded
	if(pMeshMngr->IsInstanceCreated(a_sInstanceName))
	{//if it is check if the sphere has already been created
		if(IdentifyBox(a_sInstanceName) == -1)
		{
			//Create a new bounding sphere
			BoundingBoxClass* pBS = new BoundingBoxClass();
			//construct its information out of the instance name
			pBS->GenerateBoundingBox(a_sInstanceName);
			//Push the sphere back into the list
			m_lBox.push_back(pBS);
			//Push a new matrix into the list
			m_lMatrix.push_back(matrix4(IDENTITY));
			//Specify the color the sphere is going to have
			m_lColor.push_back(vector3(1.0f));
			//Increase the number of spheres
			m_nBoxen++;
		}
	}
}

void BoundingBoxManagerSingleton::SetBoundingBoxSpace(matrix4 a_mModelToWorld, String a_sInstanceName)
{
	int nBox = IdentifyBox(a_sInstanceName);
	//If the sphere was found
	if(nBox != -1)
	{
		//Set up the new matrix in the appropriate index
		m_lMatrix[nBox] = a_mModelToWorld;
	}
}

int BoundingBoxManagerSingleton::IdentifyBox(String a_sInstanceName)
{
	//Go one by one for all the spheres in the list
	for(int nBox = 0; nBox < m_nBoxen; nBox++)
	{
		//If the current sphere is the one we are looking for we return the index
		if(a_sInstanceName == m_lBox[nBox]->GetName())
			return nBox;
	}
	return -1;//couldnt find it return with no index
}

void BoundingBoxManagerSingleton::AddBoxToRenderList(String a_sInstanceName)
{
	//If I need to render all
	if(a_sInstanceName == "ALL")
	{
		for(int nBox = 0; nBox < m_nBoxen; nBox++)
		{
			m_lBox[nBox]->AddBoxToRenderList(m_lMatrix[nBox], m_lColor[nBox], true);
		}
	}
	else
	{
		int nBox = IdentifyBox(a_sInstanceName);
		if(nBox != -1)
		{
			m_lBox[nBox]->AddBoxToRenderList(m_lMatrix[nBox], m_lColor[nBox], true);
		}
	}
}

void BoundingBoxManagerSingleton::CalculateCollision(void)
{
	/* A collision occurs when the lines representing each dimension
	 * of each box overlap within their respective dimensions.
     *
	 * i.e. when the lines representing width overlap, as well as height and depth.
	 */

	//make all white at start
	for (auto &color : m_lColor) {
		color = vector3(1.0f);
	}

	// Given the centroid and maximum radius, find if the lines in each dimension from each cube
	// overlap.
	for(int i = 0; i < m_nBoxen - 1; i++)
	for(int j = i + 1; j < m_nBoxen; j++)
	{
		auto &a = m_lBox[i];
		auto &b = m_lBox[j];

		auto &atrans = m_lMatrix[i];
		auto &btrans = m_lMatrix[j];

		auto arad = a->GetRadius();
		auto brad = b->GetRadius();

		// translate into world space
		auto acent = atrans * vector4(a->GetCentroid(), 1.0f);
		auto bcent = btrans * vector4(b->GetCentroid(), 1.0f);

		auto aleft = acent.x - arad;
		auto aright = acent.x + arad;
		auto abottom = acent.y - arad;
		auto atop = acent.y + arad;
		auto aback = acent.z - arad;
		auto afront = acent.z + arad;

		auto bleft = bcent.x - brad;
		auto bright = bcent.x + brad;
		auto bbottom = bcent.y - brad;
		auto btop = bcent.y + brad;
		auto bback = bcent.z - brad;
		auto bfront = bcent.z + brad;

		// if we're colliding, mark it red
		if (aleft < bright && aright > bleft &&
			abottom < btop && atop > bbottom &&
			aback < bfront && afront > bback)
			m_lColor[i] = m_lColor[j] = MERED;
	}
}