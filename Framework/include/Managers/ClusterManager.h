/**
 *  @file   PandoraPFANew/Framework/include/Managers/ClusterManager.h
 * 
 *  @brief  Header file for the cluster manager class.
 * 
 *  $Log: $
 */
#ifndef CLUSTER_MANAGER_H
#define CLUSTER_MANAGER_H 1

#include "Api/PandoraContentApi.h"

#include "Pandora/PandoraInternal.h"

namespace pandora
{

class Algorithm;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *    @brief ClusterManager class
 */
class ClusterManager
{
public:
    /**
     *  @brief  Default constructor
     */
     ClusterManager();

    /**
     *  @brief  Destructor
     */
     ~ClusterManager();

private:
    /**
     *  @brief  Create cluster
     * 
     *  @param  clusterParameters the cluster parameters
     *  @param  pCluster to receive the address of the cluster created
     */
    template <typename CLUSTER_PARAMETERS>
    StatusCode CreateCluster(CLUSTER_PARAMETERS *pClusterParameters, Cluster *&pCluster);

    /**
     *  @brief  Create the null cluster list
     */
    StatusCode CreateNullList();

    /**
     *  @brief  Delete the null cluster list
     */
    void DeleteNullList();

    /**
     *  @brief  Get the current cluster list name
     * 
     *  @param  clusterListName to receive the current cluster list name
     */
    StatusCode GetCurrentListName(std::string &clusterListName) const;

    /**
     *  @brief  Get the algorithm input cluster list name
     * 
     *  @param  pAlgorithm address of the algorithm
     *  @param  clusterListName to receive the algorithm input cluster list name
     */
    StatusCode GetAlgorithmInputListName(const Algorithm *const pAlgorithm, std::string &clusterListName) const;

    /**
     *  @brief  Get the current cluster list
     * 
     *  @param  pClusterList to receive the current cluster list
     *  @param  clusterListName to receive the name of the current cluster list
     */
    StatusCode GetCurrentList(const ClusterList *&pClusterList, std::string &clusterListName) const;

    /**
     *  @brief  Get the algorithm input cluster list
     * 
     *  @param  pAlgorithm address of the algorithm
     *  @param  pClusterList to receive the algorithm input cluster list
     *  @param  clusterListName to receive the name of the algorithm input cluster list
     */
    StatusCode GetAlgorithmInputList(const Algorithm *const pAlgorithm, const ClusterList *&pClusterList, std::string &clusterListName) const;

    /**
     *  @brief  Get a cluster list
     * 
     *  @param  listName the name of the list
     *  @param  pClusterList to receive the cluster list
     */
    StatusCode GetList(const std::string &listName, const ClusterList *&pClusterList) const;

    /**
     *  @brief  Replace the current and algorithm input lists with a pre-existing list
     *
     *  @param  pAlgorithm address of the algorithm changing the cluster lists
     *  @param  clusterListName the name of the new current (and algorithm input) cluster list
     */
    StatusCode ReplaceCurrentAndAlgorithmInputLists(const Algorithm *const pAlgorithm, const std::string &clusterListName);

    /**
     *  @brief  Temporarily replace the current cluster list with another list, which may only be a temporary list.
     *          This switch will persist only for the duration of the algorithm and its daughters; unless otherwise
     *          specified, the current list will revert to the algorithm input list upon algorithm completion.
     * 
     *  @param  clusterListName the name of the new current (and algorithm input) cluster list
     */
    StatusCode TemporarilyReplaceCurrentList(const std::string &clusterListName);

    /**
     *  @brief  Reset the current list to the algorithm input list
     *
     *  @param  pAlgorithm address of the algorithm changing the cluster lists
     */
    StatusCode ResetCurrentListToAlgorithmInputList(const Algorithm *const pAlgorithm);

    /**
     *  @brief  Drop the current list, returning the current list to its default empty/null state
     */
    StatusCode DropCurrentList();

    /**
     *  @brief  Make a temporary cluster list and set it to be the current cluster list
     * 
     *  @param  pAlgorithm address of the algorithm requesting a temporary list
     *  @param  temporaryListName to receive the name of the temporary list
     */
    StatusCode MakeTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, std::string &temporaryListName);

    /**
     *  @brief  Move clusters to a new temporary cluster list and set it to be the current cluster list
     * 
     *  @param  pAlgorithm address of the algorithm requesting a temporary list
     *  @param  originalListName the list in which the clusters currently exist
     *  @param  temporaryListName to receive the name of the temporary list
     *  @param  clustersToMove only clusters in both this and the current list will be moved
     *          - other clusters in the current list will remain in original list
     *          - an empty cluster vector will be rejected
     */
    StatusCode MoveClustersToTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, const std::string &originalListName,
        std::string &temporaryListName, const ClusterList &clustersToMove);

    /**
     *  @brief  Save a list of clusters
     * 
     *  @param  pAlgorithm the algorithm associated with the temporary clusters
     *  @param  targetListName the name of the target cluster list, which will be created if it doesn't currently exist
     *  @param  sourceListName the name of the (typically temporary) cluster list to save
     */
    StatusCode SaveClusters(const Algorithm *const pAlgorithm, const std::string &targetListName, const std::string &sourceListName);

    /**
     *  @brief  Save a list of clusters
     * 
     *  @param  pAlgorithm the algorithm associated with the temporary clusters
     *  @param  targetListName the name of the target cluster list, which will be created if it doesn't currently exist
     *  @param  sourceListName the name of the (typically temporary) cluster list containing clusters to save
     *  @param  clustersToSave only clusters in both this and the temporary list will be stored
     *          - other clusters will remain in the temporary list and will be deleted when the parent algorithm exits
     *          - an empty cluster vector will be rejected
     */
    StatusCode SaveClusters(const Algorithm *const pAlgorithm, const std::string &targetListName, const std::string &sourceListName,
        const ClusterList &clustersToSave);

    /**
     *  @brief  Add a calo hit to a cluster
     *
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the hit to add
     */
    StatusCode AddCaloHitToCluster(Cluster *pCluster, CaloHit *pCaloHit);

    /**
     *  @brief  Remove a calo hit from a cluster
     *
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the hit to remove
     */
    StatusCode RemoveCaloHitFromCluster(Cluster *pCluster, CaloHit *pCaloHit);

    /**
     *  @brief  Add an isolated calo hit to a cluster. This is not counted as a regular calo hit: it contributes only
     *          towards the cluster energy and does not affect any other cluster properties.
     *
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the hit to add
     */
    StatusCode AddIsolatedCaloHitToCluster(Cluster *pCluster, CaloHit *pCaloHit);

    /**
     *  @brief  Remove an isolated calo hit from a cluster
     *
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the hit to remove
     */
    StatusCode RemoveIsolatedCaloHitFromCluster(Cluster *pCluster, CaloHit *pCaloHit);

    /**
     *  @brief  Delete a cluster from the current list
     * 
     *  @param  pCluster address of the cluster to delete
     */
    StatusCode DeleteCluster(Cluster *pCluster);

    /**
     *  @brief  Delete a cluster from a specified list
     * 
     *  @param  pCluster address of the cluster to delete
     *  @param  listName the name of the list containing the cluster
     */
    StatusCode DeleteCluster(Cluster *pCluster, const std::string &listName);

    /**
     *  @brief  Delete a list of clusters from the current list
     * 
     *  @param  clusterList the list of addresses of clusters to delete
     */
    StatusCode DeleteClusters(const ClusterList &clusterList);

    /**
     *  @brief  Delete a list of clusters from a specified list
     * 
     *  @param  clusterList the list of addresses of clusters to delete
     *  @param  listName the name of the list containing the clusters
     */
    StatusCode DeleteClusters(const ClusterList &clusterList, const std::string &listName);

    /**
     *  @brief  Delete the contents of a temporary cluster list and remove the list from all relevant containers
     * 
     *  @param  pAlgorithm address of the algorithm calling this function
     *  @param  clusterListName the name of the temporary cluster list
     */
    StatusCode DeleteTemporaryClusterList(const Algorithm *const pAlgorithm, const std::string &clusterListName);

    /**
     *  @brief  Merge two clusters in the current list, enlarging one cluster and deleting the second
     * 
     *  @param  pClusterToEnlarge address of the cluster to enlarge
     *  @param  pClusterToDelete address of the cluster to delete
     */
    StatusCode MergeAndDeleteClusters(Cluster *pClusterToEnlarge, Cluster *pClusterToDelete);

    /**
     *  @brief  Merge two clusters from two specified lists, enlarging one cluster and deleting the second
     * 
     *  @param  pClusterToEnlarge address of the cluster to enlarge
     *  @param  pClusterToDelete address of the cluster to delete
     *  @param  enlargeListName name of the list containing the cluster to enlarge
     *  @param  deleteListName name of the list containing the cluster to delete
     */
    StatusCode MergeAndDeleteClusters(Cluster *pClusterToEnlarge, Cluster *pClusterToDelete, const std::string &enlargeListName,
        const std::string &deleteListName);

    /**
     *  @brief  Register an algorithm with the cluster manager
     * 
     *  @param  pAlgorithm address of the algorithm
     */
    StatusCode RegisterAlgorithm(const Algorithm *const pAlgorithm);

    /**
     *  @brief  Get the list of clusters that will be deleted when the algorithm info is reset
     * 
     *  @param  pAlgorithm address of the algorithm
     *  @param  clusterList to receive the list of clusters that will be deleted when the algorithm info is reset
     */
    StatusCode GetClustersToBeDeleted(const Algorithm *const pAlgorithm, ClusterList &clusterList) const;

    /**
     *  @brief  Remove temporary lists and reset the current cluster list to that when algorithm was initialized
     * 
     *  @param  pAlgorithm the algorithm associated with the temporary clusters
     *  @param  isAlgorithmFinished whether the algorithm has completely finished and the algorithm info should be entirely removed
     */
    StatusCode ResetAlgorithmInfo(const Algorithm *const pAlgorithm, bool isAlgorithmFinished);

    /**
     *  @brief  Reset the cluster manager
     */
    StatusCode ResetForNextEvent();

    /**
     *  @brief  Remove an empty cluster list
     * 
     *  @param  pAlgorithm the algorithm manipulating the cluster lists
     *  @param  clusterListName the name of the empty cluster list to be removed
     */
    StatusCode RemoveEmptyClusterList(const Algorithm *const pAlgorithm, const std::string &clusterListName);

    /**
     *  @brief  Remove all cluster to track associations
     */
    StatusCode RemoveAllTrackAssociations() const;

    /**
     *  @brief  Remove cluster to track associations from all clusters in the current list
     * 
     *  @param  danglingTracks to receive the list of "dangling" associations
     */
    StatusCode RemoveCurrentTrackAssociations(TrackList &danglingTracks) const;

    /**
     *  @brief  Remove a specified list of cluster to track associations
     * 
     *  @param  trackToClusterList the specified track to cluster list
     */
    StatusCode RemoveTrackAssociations(const TrackToClusterMap &trackToClusterList) const;

    /**
     *  @brief  AlgorithmInfo class
     */
    class AlgorithmInfo
    {
    public:
        std::string                 m_parentListName;                   ///< The current cluster list when algorithm was initialized
        StringSet                   m_temporaryListNames;               ///< The temporary cluster list names
        unsigned int                m_numberOfListsCreated;             ///< The number of cluster lists created by the algorithm
    };

    typedef std::map<std::string, ClusterList *> NameToClusterListMap;
    typedef std::map<const Algorithm *, AlgorithmInfo> AlgorithmInfoMap;

    NameToClusterListMap            m_nameToClusterListMap;             ///< The name to cluster list map
    AlgorithmInfoMap                m_algorithmInfoMap;                 ///< The algorithm info map

    bool                            m_canMakeNewClusters;               ///< Whether the manager is allowed to make new clusters
    std::string                     m_currentListName;                  ///< The name of the current cluster list
    StringSet                       m_savedLists;                       ///< The set of saved cluster lists
    static const std::string        NULL_LIST_NAME;                     ///< The name of the default empty (NULL) cluster list

    friend class PandoraApiImpl;
    friend class PandoraContentApiImpl;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode ClusterManager::GetCurrentListName(std::string &clusterListName) const
{
    if (m_currentListName.empty())
        return STATUS_CODE_NOT_INITIALIZED;

    clusterListName = m_currentListName;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode ClusterManager::GetAlgorithmInputListName(const Algorithm *const pAlgorithm, std::string &clusterListName) const
{
    AlgorithmInfoMap::const_iterator iter = m_algorithmInfoMap.find(pAlgorithm);

    if (m_algorithmInfoMap.end() == iter)
        return this->GetCurrentListName(clusterListName);

    clusterListName = iter->second.m_parentListName;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode ClusterManager::GetCurrentList(const ClusterList *&pClusterList, std::string &clusterListName) const
{
    clusterListName = m_currentListName;

    return this->GetList(clusterListName, pClusterList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode ClusterManager::GetAlgorithmInputList(const Algorithm *const pAlgorithm, const ClusterList *&pClusterList,
    std::string &clusterListName) const
{
    AlgorithmInfoMap::const_iterator iter = m_algorithmInfoMap.find(pAlgorithm);

    if (m_algorithmInfoMap.end() != iter)
    {
        clusterListName = iter->second.m_parentListName;
    }
    else
    {
        clusterListName = m_currentListName;
    }

    return this->GetList(clusterListName, pClusterList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode ClusterManager::ResetCurrentListToAlgorithmInputList(const Algorithm *const pAlgorithm)
{
    return this->GetAlgorithmInputListName(pAlgorithm, m_currentListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode ClusterManager::DropCurrentList()
{
    m_canMakeNewClusters = false;
    m_currentListName = NULL_LIST_NAME;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode ClusterManager::DeleteCluster(Cluster *pCluster)
{
    return this->DeleteCluster(pCluster, m_currentListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode ClusterManager::DeleteClusters(const ClusterList &clusterList)
{
    return this->DeleteClusters(clusterList, m_currentListName);
}

} // namespace pandora

#endif // #ifndef CLUSTER_MANAGER_H