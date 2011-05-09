/**
 *  @file   PandoraPFANew/FineGranularityContent/include/ParticleId/PhotonReconstructionAlgorithm.h
 * 
 *  @brief  Header file for the photon reconstruction algorithm class.
 * 
 *  $Log: $
 */
#ifndef PHOTON_RECONSTRUCTION_ALGORITHM_H
#define PHOTON_RECONSTRUCTION_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

/**
 *  @brief  PhotonReconstructionAlgorithm class
 */
class PhotonReconstructionAlgorithm : public pandora::Algorithm
{
public:
    /**
     *  @brief  Factory class for instantiating algorithm
     */
    class Factory : public pandora::AlgorithmFactory
    {
    public:
        pandora::Algorithm *CreateAlgorithm() const;
    };

    /**
     *  @param  Destructor
     */
    ~PhotonReconstructionAlgorithm();

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    /**
     *  @brief  
     * 
     *  @param  
     *  @param  
     * 
     *  @return 
     */
    float GetMinDistanceToTrack(const pandora::Cluster *const pPeakCluster, const pandora::TrackVector &trackVector) const;

    /**
     *  @brief  
     * 
     *  @param  
     *  @param  
     *  @param  
     *  @param  
     *  @param  
     * 
     *  @return 
     */
    float GetPid(const float peakRms, const float longProfileStart, const float longProfileDiscrepancy, const float peakEnergyFraction,
        const float minDistanceToTrack) const;

    /**
     *  @brief  
     * 
     *  @param  
     *  @param  
     *  @param  
     *  @param  
     *  @param  
     *  @param  
     */
    void FillPdfHistograms(const pandora::Cluster *const pPeakCluster, const float peakRms, const float longProfileStart,
        const float longProfileDiscrepancy, const float peakEnergyFraction, const float minDistanceToTrack) const;

    /**
     *  @brief  Tidy histograms, normalizing them writing them to file if the algorithm is in histogram creation mode
     */
    void TidyHistograms();

    std::string         m_photonClusteringAlgName;          ///< The name of the photon clustering algorithm to run
    std::string         m_clusterListName;                  ///< The name of the output cluster list 

    std::string         m_histogramFile;                    ///< The name of the file containing (or to contain) pdf histograms
    bool                m_shouldMakePdfHistograms;          ///< Whether to create pdf histograms, rather than perform photon reconstruction

    pandora::Histogram *m_pSigPeakRms;                      ///< PDF histogram, signal peak rms
    pandora::Histogram *m_pBkgPeakRms;                      ///< PDF histogram, background peak rms
    pandora::Histogram *m_pSigLongProfileStart;             ///< PDF histogram, signal peak longitudinal profile start
    pandora::Histogram *m_pBkgLongProfileStart;             ///< PDF histogram, background peak longitudinal profile start
    pandora::Histogram *m_pSigLongProfileDiscrepancy;       ///< PDF histogram, signal peak longitudinal profile discrepancy
    pandora::Histogram *m_pBkgLongProfileDiscrepancy;       ///< PDF histogram, background peak longitudinal profile discrepancy
    pandora::Histogram *m_pSigPeakEnergyFraction;           ///< PDF histogram, signal peak energy fraction
    pandora::Histogram *m_pBkgPeakEnergyFraction;           ///< PDF histogram, background peak energy fraction
    pandora::Histogram *m_pSigMinDistanceToTrack;           ///< PDF histogram, signal peak min distance to track
    pandora::Histogram *m_pBkgMinDistanceToTrack;           ///< PDF histogram, background peak min distance to track

    float               m_minClusterEnergy;                 ///< The minimum energy to consider a cluster
    unsigned int        m_transProfileMaxLayer;             ///< Maximum layer to consider in calculation of shower transverse profiles
    float               m_minPeakEnergy;                    ///< The minimum energy to consider a transverse profile peak
    float               m_maxPeakRms;                       ///< The maximum rms value to consider a transverse profile peak
    unsigned int        m_minPeakCaloHits;                  ///< The minimum number of calo hits associated with a transverse profile peak
    float               m_maxLongProfileStart;              ///< The maximum longitudinal shower profile start
    float               m_maxLongProfileDiscrepancy;        ///< The maximum longitudinal shower profile discrepancy
    float               m_pidCut;                           ///< The pid cut to apply for photon cluster identification

    float               m_oldClusterEnergyFraction0;        ///< The cluster energy fraction above which original cluster will be used
    float               m_oldClusterEnergyFraction1;        ///< Decision to use original cluster: energy fraction 1
    float               m_oldClusterEnergyDifference1;      ///< Decision to use original cluster: energy difference 1
    float               m_oldClusterEnergyFraction2;        ///< Decision to use original cluster: energy fraction 2
    float               m_oldClusterEnergyDifference2;      ///< Decision to use original cluster: energy difference 2
    float               m_oldClusterEnergyFraction3;        ///< Decision to use original cluster: energy fraction 3
    float               m_oldClusterEnergyDifference3;      ///< Decision to use original cluster: energy difference 3
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *PhotonReconstructionAlgorithm::Factory::CreateAlgorithm() const
{
    return new PhotonReconstructionAlgorithm();
}

#endif // #ifndef PHOTON_RECONSTRUCTION_ALGORITHM_H
