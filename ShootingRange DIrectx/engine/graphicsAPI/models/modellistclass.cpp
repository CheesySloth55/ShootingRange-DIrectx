#include "modellistclass.h"


ModelListClass::ModelListClass()
{
    m_ModelInfoList = 0;
}


ModelListClass::ModelListClass(const ModelListClass& other)
{
}


ModelListClass::~ModelListClass()
{
}

void ModelListClass::Initialize(int numModels)
{
    float spawnAreaSize = 10.0f;

    m_modelCount = numModels;

    m_ModelInfoList = new ModelInfoType[m_modelCount];

    srand((unsigned int)time(NULL));
    
    for (int i{}; i < m_modelCount; i++)
    {
        m_ModelInfoList[i].posX = (((float)rand() - (float)rand()) / RAND_MAX) * spawnAreaSize;
        m_ModelInfoList[i].posY = (((float)rand() - (float)rand()) / RAND_MAX) * spawnAreaSize;
        m_ModelInfoList[i].posZ = ((((float)rand() - (float)rand()) / RAND_MAX) * spawnAreaSize) + spawnAreaSize / 2;
    }

    return;
}

void ModelListClass::Shutdown()
{
    // Release the model information list.
    if (m_ModelInfoList)
    {
        delete[] m_ModelInfoList;
        m_ModelInfoList = 0;
    }

    return;
}

int ModelListClass::GetModelCount()
{
    return m_modelCount;
}

void ModelListClass::GetData(int index, float& posX, float& posY, float& posZ)
{
    posX = m_ModelInfoList[index].posX;
    posY = m_ModelInfoList[index].posY;
    posZ = m_ModelInfoList[index].posZ;

    return;
}