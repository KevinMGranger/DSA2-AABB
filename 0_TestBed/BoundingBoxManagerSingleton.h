/*
Programmer: Alberto Bobadilla (labigm@gmail.com)
Date: 2015/03
*/
#ifndef __BOUNDINGBOXMANAGERSINGLETON_H_
#define __BOUNDINGBOXMANAGERSINGLETON_H_

#include "BoundingBoxClass.h"

//System Class
class BoundingBoxManagerSingleton
{
	int m_nBoxen; //number of elements in the list
	std::vector<BoundingBoxClass*> m_lBox; //list of spheres
	std::vector<matrix4> m_lMatrix; //list of Global spaces of each sphere in the list
	std::vector<vector3> m_lColor; //list of colors of each sphere in the list
	static BoundingBoxManagerSingleton* m_pInstance; // Singleton pointer
	
public:
	/* Gets/Constructs the singleton pointer */
	static BoundingBoxManagerSingleton* GetInstance();
	/* Destroys the singleton */
	static void ReleaseInstance(void);

	/* Asks the singleton for the number of elements in the list */
	int GetBoxTotal(void);

	/*
	GenerateBoundingBox
		Generates a bounding sphere from the specified instance name and adds it to the list
	*/
	void GenerateBoundingBox(String a_sInstanceName);

	/*
	SetBoundingBoxSpace
		Sets the Matrix that represents the global position of the sphere specified by instance name
	*/
	void SetBoundingBoxSpace(matrix4 a_mModelToWorld, String a_sInstanceName);

	/*
	IdentifyBox
		Returns the index in which the specified sphere is stored in the list
	*/
	int IdentifyBox(String a_sInstanceName);

	/*
	AddBoxToRenderList
		Add the specified instance to the render list, ALL will add all of them at once
	*/
	void AddBoxToRenderList(String a_sInstanceName = "ALL");

	/*
	CalculateCollision
		Calculates the collision among all the spheres in the list
	*/
	void CalculateCollision(void);

private:
	/* Constructor */
	BoundingBoxManagerSingleton(void);
	/* Copy Constructor */
	BoundingBoxManagerSingleton(BoundingBoxManagerSingleton const& other);
	/* Copy Assignment Operator */
	BoundingBoxManagerSingleton& operator=(BoundingBoxManagerSingleton const& other);
	/* Destructor */
	~BoundingBoxManagerSingleton(void);

	/* Releases the objects memory */
	void Release(void);
	/* Initializates the objects fields */
	void Init(void);
};

#endif __BOUNDINGBOXMANAGERSINGLETON_H_