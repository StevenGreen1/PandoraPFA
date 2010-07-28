/**
 *  @file   PandoraPFANew/include/Algorithms/V0PfoCreationAlgorithm.h
 * 
 *  @brief  Header file for the template algorithm class.
 * 
 *  $Log: $
 */
#ifndef V0_PFO_CREATION_ALGORITHM_H
#define V0_PFO_CREATION_ALGORITHM_H 1
#include "Algorithms/Algorithm.h"
#include "Objects/CartesianVector.h"

/**
 *  @brief  V0PfoCreationAlgorithm class
 */
class V0PfoCreationAlgorithm : public pandora::Algorithm
{
public:
    /**
     *  @brief  Factory class for instantiating algorithm
     */
     

    class Factory : public pandora::AlgorithmFactory
    {
    public:
        Algorithm *CreateAlgorithm() const;
    };


    /**
     *  @brief  Factory class for instantiating algorithm
     */
  V0PfoCreationAlgorithm();
    /**
     *  @brief  Factory class for instantiating algorithm
     */
  ~V0PfoCreationAlgorithm();

private:
    StatusCode Run();

    StatusCode ReadSettings(const TiXmlHandle xmlHandle);
    
/**
 *  @brief  V0 mass calculation
 * 
 *  @param  momentum1 is cartesian vector of track1 at start
 *  @param  momentum2 is cartesian vector of track2 at start
 *  @param  mass1 is assumed mass of track1
 *  @param  mass2 is assumed mass of track2
 *  @param  V0mass is the resulting mass of the V0
 */
    StatusCode V0Mass(const pandora::CartesianVector &momentum1, const pandora::CartesianVector &momentum2, float mass1, float mass2, float &V0mass);

    bool m_mcMonitoring;

};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *V0PfoCreationAlgorithm::Factory::CreateAlgorithm() const
{
    return new V0PfoCreationAlgorithm();
}

#endif // #ifndef V0_PFO_ALGORITHM_H