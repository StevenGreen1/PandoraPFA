/**
 *  @file   PandoraPFANew/src/Helpers/ClusterHelper.cc
 * 
 *  @brief  Implementation of the cluster helper class.
 * 
 *  $Log: $
 */

#include "Helpers/ClusterHelper.h"
#include "Helpers/GeometryHelper.h"

#include "Objects/CaloHit.h"
#include "Objects/Cluster.h"
#include "Objects/OrderedCaloHitList.h"
#include "Objects/Track.h"

#include <cmath>

namespace pandora
{

StatusCode ClusterHelper::FitStart(const Cluster *const pCluster, unsigned int nOccupiedLayers, ClusterFitResult &clusterFitResult)
{
    if (nOccupiedLayers < 2)
        return STATUS_CODE_INVALID_PARAMETER;

    const OrderedCaloHitList &orderedCaloHitList = pCluster->GetOrderedCaloHitList();
    const unsigned int listSize(orderedCaloHitList.size());

    if (0 == listSize)
        return STATUS_CODE_NOT_INITIALIZED;

    if (nOccupiedLayers > listSize)
        return STATUS_CODE_OUT_OF_RANGE;

    unsigned int occupiedLayerCount(0);

    ClusterFitPointList clusterFitPointList;
    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        if (++occupiedLayerCount > nOccupiedLayers)
            break;

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            clusterFitPointList.push_back(ClusterFitPoint(*hitIter));
        }
    }

    return FitPoints(clusterFitPointList, clusterFitResult);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterHelper::FitEnd(const Cluster *const pCluster, unsigned int nOccupiedLayers, ClusterFitResult &clusterFitResult)
{
    if (nOccupiedLayers < 2)
        return STATUS_CODE_INVALID_PARAMETER;

    const OrderedCaloHitList &orderedCaloHitList = pCluster->GetOrderedCaloHitList();
    const unsigned int listSize(orderedCaloHitList.size());

    if (0 == listSize)
        return STATUS_CODE_NOT_INITIALIZED;

    if (nOccupiedLayers > listSize)
        return STATUS_CODE_OUT_OF_RANGE;

    unsigned int occupiedLayerCount(0);

    ClusterFitPointList clusterFitPointList;
    for (OrderedCaloHitList::const_reverse_iterator iter = orderedCaloHitList.rbegin(), iterEnd = orderedCaloHitList.rend(); iter != iterEnd; ++iter)
    {
        if (++occupiedLayerCount > nOccupiedLayers)
            break;

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            clusterFitPointList.push_back(ClusterFitPoint(*hitIter));
        }
    }

    return FitPoints(clusterFitPointList, clusterFitResult);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterHelper::FitLayers(const Cluster *const pCluster, PseudoLayer startLayer, PseudoLayer endLayer, ClusterFitResult &clusterFitResult)
{
    if (startLayer >= endLayer)
        return STATUS_CODE_INVALID_PARAMETER;

    const OrderedCaloHitList &orderedCaloHitList = pCluster->GetOrderedCaloHitList();
    const unsigned int listSize(orderedCaloHitList.size());

    if (0 == listSize)
        return STATUS_CODE_NOT_INITIALIZED;

    if (listSize < 2)
        return STATUS_CODE_OUT_OF_RANGE;

    ClusterFitPointList clusterFitPointList;
    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        const PseudoLayer pseudoLayer(iter->first);

        if (startLayer > pseudoLayer)
            continue;

        if (endLayer < pseudoLayer)
            break;

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            clusterFitPointList.push_back(ClusterFitPoint(*hitIter));
        }
    }

    return FitPoints(clusterFitPointList, clusterFitResult);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterHelper::FitPoints(const Cluster *const pCluster, ClusterFitResult &clusterFitResult)
{
    const OrderedCaloHitList &orderedCaloHitList = pCluster->GetOrderedCaloHitList();
    const unsigned int listSize(orderedCaloHitList.size());

    if (0 == listSize)
        return STATUS_CODE_NOT_INITIALIZED;

    if (listSize < 2)
        return STATUS_CODE_OUT_OF_RANGE;

    ClusterFitPointList clusterFitPointList;
    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            clusterFitPointList.push_back(ClusterFitPoint(*hitIter));
        }
    }

    return FitPoints(clusterFitPointList, clusterFitResult);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterHelper::FitPoints(const ClusterFitPointList &clusterFitPointList, ClusterFitResult &clusterFitResult)
{
    if (clusterFitPointList.size() <= 1)
        return STATUS_CODE_INVALID_PARAMETER;

    float sumX(0.), sumY(0.), sumZ(0.);
    clusterFitResult.Reset();

    for (ClusterFitPointList::const_iterator iter = clusterFitPointList.begin(), iterEnd = clusterFitPointList.end(); iter != iterEnd; ++iter)
    {
        sumX += iter->GetPosition().GetX();
        sumY += iter->GetPosition().GetY();
        sumZ += iter->GetPosition().GetZ();
    }

    const float meanZ(sumZ / static_cast<float>(clusterFitPointList.size()));
    static const float eCalEndCapZCoordinate(GeometryHelper::GetInstance()->GetECalEndCapParameters().GetInnerZCoordinate());
    const bool isInBarrelRegion(std::fabs(meanZ) < eCalEndCapZCoordinate);

    if (isInBarrelRegion)
    {
        const float meanX(sumX / static_cast<float>(clusterFitPointList.size()));
        const float meanY(sumY / static_cast<float>(clusterFitPointList.size()));
        const float rXY(std::sqrt(meanX * meanX + meanY * meanY));

        if (0 == rXY)
            return STATUS_CODE_FAILURE;

        return FitBarrelPoints(clusterFitPointList, meanX / rXY, meanY / rXY, clusterFitResult);
    }
    else
    {
        const bool isPositiveZ(meanZ > 0);
        return FitEndCapPoints(clusterFitPointList, isPositiveZ, clusterFitResult);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterHelper::FitBarrelPoints(const ClusterFitPointList &clusterFitPointList, float cosTheta, float sinTheta,
    ClusterFitResult &clusterFitResult)
{
    // Extract the data
    double sumU(0.), sumV(0.), sumZ(0.);
    double sumVU(0.), sumZU(0.), sumUU(0.);

    for (ClusterFitPointList::const_iterator iter = clusterFitPointList.begin(), iterEnd = clusterFitPointList.end(); iter != iterEnd; ++iter)
    {
        const double u(cosTheta * iter->GetPosition().GetX() + sinTheta * iter->GetPosition().GetY());
        const double v(cosTheta * iter->GetPosition().GetY() - sinTheta * iter->GetPosition().GetX());
        const double z(iter->GetPosition().GetZ());

        sumU += u; sumV += v; sumZ += z;
        sumVU += v * u; sumZU += z * u; sumUU += u * u;
    }

    // Perform the fit
    const double nPoints(static_cast<double>(clusterFitPointList.size()));
    const double denominatorU(sumU * sumU - nPoints * sumUU);

    if (0 == denominatorU)
        return STATUS_CODE_FAILURE;

    const double aV((sumU * sumV - nPoints * sumVU) / denominatorU);
    const double bV((sumV - aV * sumU) / nPoints);
    const double aZ((sumU * sumZ - nPoints * sumZU) / denominatorU);
    const double bZ((sumZ - aZ * sumU) / nPoints);

    if (0 == aV)
        return STATUS_CODE_FAILURE;

    const double bU(-bV / aV);
    const double r(std::sqrt(1. + aV * aV + aZ * aZ));
    const double dirU(1. / r), dirV(aV / r), dirZ(aZ / r);
    CartesianVector direction(cosTheta * dirU - sinTheta * dirV, sinTheta * dirU + cosTheta * dirV, dirZ);
    CartesianVector intercept(cosTheta * bU, sinTheta * bU, aZ * bU + bZ);

    float dirCosR(direction.GetDotProduct(intercept) / intercept.GetMagnitude());

    if (0 > dirCosR)
    {
        dirCosR = -dirCosR;
        direction = CartesianVector(0, 0, 0) - direction;
    }

    // Now calculate something like a chi2
    double chi2_V(0.), chi2_Z(0.), rms(0.);
    double sumA(0.), sumL(0.), sumAL(0.), sumLL(0.);

    for (ClusterFitPointList::const_iterator iter = clusterFitPointList.begin(), iterEnd = clusterFitPointList.end(); iter != iterEnd; ++iter)
    {
        const double u(cosTheta * iter->GetPosition().GetX() + sinTheta * iter->GetPosition().GetY());
        const double v(cosTheta * iter->GetPosition().GetY() - sinTheta * iter->GetPosition().GetX());
        const double z(iter->GetPosition().GetZ());

        const double error(iter->GetCellSize() / 3.46);
        const double chiV((v - aV * u - bV) / error);
        const double chiZ((z - aZ * u - bZ) / error);

        chi2_V += chiV * chiV;
        chi2_Z += chiZ * chiZ;

        const CartesianVector difference(iter->GetPosition() - intercept);
        rms += (direction.GetCrossProduct(difference)).GetMagnitudeSquared();

        const float a(direction.GetDotProduct(difference));
        const float l(static_cast<float>(iter->GetPseudoLayer()));

        sumA += a; sumL += l; sumAL += a * l; sumLL += l * l;
    }

    const double denominatorL(sumL * sumL - nPoints * sumLL);
    if (0 != denominatorL)
    {
        if (((sumL * sumA - nPoints * sumAL) / denominatorL) < 0)
            direction = CartesianVector(0, 0, 0) - direction;
    }

    clusterFitResult.SetDirection(direction);
    clusterFitResult.SetIntercept(intercept);
    clusterFitResult.SetChi2((chi2_V + chi2_Z) / nPoints);
    clusterFitResult.SetRms(std::sqrt(rms / nPoints));
    clusterFitResult.SetRadialDirectionCosine(dirCosR);
    clusterFitResult.SetSuccessFlag(true);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterHelper::FitEndCapPoints(const ClusterFitPointList &clusterFitPointList, bool isPositiveZ, ClusterFitResult &clusterFitResult)
{
    // Extract the data
    double sumX(0.), sumY(0.), sumZ(0.);
    double sumXZ(0.), sumYZ(0.), sumZZ(0.);

    static const float eCalEndCapZCoordinate(GeometryHelper::GetInstance()->GetECalEndCapParameters().GetInnerZCoordinate());

    for (ClusterFitPointList::const_iterator iter = clusterFitPointList.begin(), iterEnd = clusterFitPointList.end(); iter != iterEnd; ++iter)
    {
        const float x(iter->GetPosition().GetX());
        const float y(iter->GetPosition().GetY());
        const float z((isPositiveZ) ? iter->GetPosition().GetZ() - eCalEndCapZCoordinate : iter->GetPosition().GetZ() + eCalEndCapZCoordinate);

        sumX += x; sumY += y; sumZ += z;
        sumXZ += x * z; sumYZ += y * z; sumZZ += z * z;
    }

    // Perform the fit
    const double nPoints(static_cast<double>(clusterFitPointList.size()));
    const double denominatorZ(sumZ * sumZ - nPoints * sumZZ);

    if (0 == denominatorZ)
        return STATUS_CODE_FAILURE;

    const double aX((sumZ * sumX - nPoints * sumXZ) / denominatorZ);
    const double bX((sumX - aX * sumZ) / nPoints);
    const double aY((sumZ * sumY - nPoints * sumYZ) / denominatorZ);
    const double bY((sumY - aY * sumZ) / nPoints);

    const double r(std::sqrt(1. + aX * aX + aY * aY));
    CartesianVector direction(aX / r, aY / r, 1. / r);
    CartesianVector intercept(bX, bY, (isPositiveZ) ? eCalEndCapZCoordinate : -eCalEndCapZCoordinate);

    float dirCosR(direction.GetDotProduct(intercept) / intercept.GetMagnitude());
    if (0 > dirCosR)
    {
        dirCosR = -dirCosR;
        direction = CartesianVector(0, 0, 0) - direction;
    }

    // Now calculate something like a chi2
    double chi2_X(0.), chi2_Y(0.), rms(0.);
    double sumA(0.), sumL(0.), sumAL(0.), sumLL(0.);

    for (ClusterFitPointList::const_iterator iter = clusterFitPointList.begin(), iterEnd = clusterFitPointList.end(); iter != iterEnd; ++iter)
    {
        const CartesianVector difference(iter->GetPosition() - intercept);
        const float error(iter->GetCellSize() / 3.46);
        const double chiX((difference.GetX() - aX * difference.GetZ()) / error);
        const double chiY((difference.GetY() - aY * difference.GetZ()) / error);

        chi2_X += chiX * chiX;
        chi2_Y += chiY * chiY;
        rms += (direction.GetCrossProduct(difference)).GetMagnitudeSquared();

        const float a(direction.GetDotProduct(difference));
        const float l(static_cast<float>(iter->GetPseudoLayer()));
        sumA += a; sumL += l; sumAL += a * l; sumLL += l * l;
    }

    const double denominatorL(sumL * sumL - nPoints * sumLL);
    if (0 != denominatorL)
    {
        if (((sumL * sumA - nPoints * sumAL) / denominatorL) < 0)
            direction = CartesianVector(0, 0, 0) - direction;
    }

    clusterFitResult.SetDirection(direction);
    clusterFitResult.SetIntercept(intercept);
    clusterFitResult.SetChi2((chi2_X + chi2_Y) / nPoints);
    clusterFitResult.SetRms(std::sqrt(rms / nPoints));
    clusterFitResult.SetRadialDirectionCosine(dirCosR);
    clusterFitResult.SetSuccessFlag(true);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float ClusterHelper::GetFitResultsClosestApproach(const ClusterHelper::ClusterFitResult &lhs, const ClusterHelper::ClusterFitResult &rhs)
{
    if (!lhs.IsFitSuccessful() || !rhs.IsFitSuccessful())
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    const CartesianVector directionNormal((lhs.GetDirection().GetCrossProduct(rhs.GetDirection())).GetUnitVector());
    const CartesianVector interceptDifference(lhs.GetIntercept() - rhs.GetIntercept());

    return std::fabs(directionNormal.GetDotProduct(interceptDifference));
}

//------------------------------------------------------------------------------------------------------------------------------------------

float ClusterHelper::GetDistanceToClosestHit(const ClusterFitResult &clusterFitResult, const Cluster *const pCluster,
    PseudoLayer startLayer, PseudoLayer endLayer)
{
    if (startLayer > endLayer)
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    float minDistance(std::numeric_limits<float>::max());
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (PseudoLayer iLayer = startLayer; iLayer <= endLayer; ++iLayer)
    {
        OrderedCaloHitList::const_iterator iter = orderedCaloHitList.find(iLayer);

        if (orderedCaloHitList.end() == iter)
            continue;

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            CaloHit *pCaloHit = *hitIter;

            const CartesianVector interceptDifference(pCaloHit->GetPositionVector() - clusterFitResult.GetIntercept());
            const float distance(interceptDifference.GetCrossProduct(clusterFitResult.GetDirection()).GetMagnitude());

            if (distance < minDistance)
            {
                minDistance = distance;
            }
        }
    }

    return minDistance;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float ClusterHelper::GetDistanceToClosestHit(const Cluster *const pClusterI, const Cluster *const pClusterJ)
{
    float minDistance(std::numeric_limits<float>::max());
    CaloHitVector caloHitVectorI, caloHitVectorJ;
    pClusterI->GetOrderedCaloHitList().GetCaloHitVector(caloHitVectorI);
    pClusterJ->GetOrderedCaloHitList().GetCaloHitVector(caloHitVectorJ);

    for (CaloHitVector::const_iterator iterI = caloHitVectorI.begin(), iterIEnd = caloHitVectorI.end(); iterI != iterIEnd; ++iterI)
    {
        const CartesianVector &positionVectorI((*iterI)->GetPositionVector());

        for (CaloHitVector::const_iterator iterJ = caloHitVectorJ.begin(), iterIEnd = caloHitVectorJ.end(); iterJ != iterIEnd; ++iterJ)
        {
            const float distance((positionVectorI - (*iterJ)->GetPositionVector()).GetMagnitude());

            if (distance < minDistance)
            {
                minDistance = distance;
            }
        }
    }

    return minDistance;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float ClusterHelper::GetDistanceToClosestCentroid(const ClusterFitResult &clusterFitResult, const Cluster *const pCluster,
    PseudoLayer startLayer, PseudoLayer endLayer)
{
    if (startLayer > endLayer)
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    float minDistance(std::numeric_limits<float>::max());
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (PseudoLayer iLayer = startLayer; iLayer <= endLayer; ++iLayer)
    {
        OrderedCaloHitList::const_iterator iter = orderedCaloHitList.find(iLayer);

        if (orderedCaloHitList.end() == iter)
            continue;

        const CartesianVector interceptDifference(pCluster->GetCentroid(iLayer) - clusterFitResult.GetIntercept());
        const float distance(interceptDifference.GetCrossProduct(clusterFitResult.GetDirection()).GetMagnitude());

        if (distance < minDistance)
        {
            minDistance = distance;
        }
    }

    return minDistance;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterHelper::GetClosestIntraLayerDistance(const Cluster *const pClusterI, const Cluster *const pClusterJ, float &intraLayerDistance)
{
    // Return if clusters do not overlap
    if ((pClusterI->GetOuterPseudoLayer() < pClusterJ->GetInnerPseudoLayer()) || (pClusterJ->GetOuterPseudoLayer() < pClusterI->GetInnerPseudoLayer()))
        return STATUS_CODE_NOT_FOUND;

    bool distanceFound(false);
    float minDistance(std::numeric_limits<float>::max());
    const OrderedCaloHitList &orderedCaloHitListI(pClusterI->GetOrderedCaloHitList());
    const OrderedCaloHitList &orderedCaloHitListJ(pClusterJ->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iterI = orderedCaloHitListI.begin(), iterIEnd = orderedCaloHitListI.end(); iterI != iterIEnd; ++iterI)
    {
        const PseudoLayer pseudoLayer(iterI->first);
        OrderedCaloHitList::const_iterator iterJ = orderedCaloHitListJ.find(pseudoLayer);

        if (orderedCaloHitListJ.end() == iterJ)
            continue;

        const CartesianVector centroidI(pClusterI->GetCentroid(pseudoLayer));
        const CartesianVector centroidJ(pClusterJ->GetCentroid(pseudoLayer));

        const float distance((centroidI - centroidJ).GetMagnitude());
        if (distance < minDistance)
        {
            minDistance = distance;
            distanceFound = true;
        }
    }

    if (!distanceFound)
        return STATUS_CODE_NOT_FOUND;

    intraLayerDistance = minDistance;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterHelper::GetTrackClusterDistance(const Track *const pTrack, const Cluster *const pCluster,
    const PseudoLayer maxSearchLayer, float parallelDistanceCut, float &trackClusterDistance)
{
    if ((0 == pCluster->GetNCaloHits()) || (pCluster->GetInnerPseudoLayer() > maxSearchLayer))
        return STATUS_CODE_NOT_FOUND;

    bool distanceFound(false);
    float minDistance(std::numeric_limits<float>::max());

    if (STATUS_CODE_SUCCESS == ClusterHelper::GetTrackClusterDistance(pTrack->GetTrackStateAtECal(), pCluster, maxSearchLayer, parallelDistanceCut, minDistance))
        distanceFound = true;

    const TrackStateList &trackStateList(pTrack->GetCalorimeterProjections());
    for (TrackStateList::const_iterator iter = trackStateList.begin(), iterEnd = trackStateList.end(); iter != iterEnd; ++iter)
    {
        const TrackState *const pTrackState = *iter;

        if (STATUS_CODE_SUCCESS == ClusterHelper::GetTrackClusterDistance(*pTrackState, pCluster, maxSearchLayer, parallelDistanceCut, minDistance))
            distanceFound = true;
    }

    if (!distanceFound)
        return STATUS_CODE_NOT_FOUND;

    trackClusterDistance = minDistance;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterHelper::GetTrackClusterDistance(const TrackState &trackState, const Cluster *const pCluster,
    const PseudoLayer maxSearchLayer, float parallelDistanceCut, float &trackClusterDistance)
{
    if ((0 == pCluster->GetNCaloHits()) || (pCluster->GetInnerPseudoLayer() > maxSearchLayer))
        return STATUS_CODE_NOT_FOUND;

    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());
    const CartesianVector &trackPosition(trackState.GetPosition());
    const CartesianVector trackDirection(trackState.GetMomentum().GetUnitVector());

    bool distanceFound(false);
    float minDistance(std::numeric_limits<float>::max());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        if (iter->first > maxSearchLayer)
            break;

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            const CartesianVector positionDifference((*hitIter)->GetPositionVector() - trackPosition);
            const float parallelDistance(trackDirection.GetDotProduct(positionDifference));

            if (std::fabs(parallelDistance) > parallelDistanceCut)
                continue;

            const float perpendicularDistance((trackDirection.GetCrossProduct(positionDifference)).GetMagnitude());

            if (perpendicularDistance < minDistance)
            {
                minDistance = perpendicularDistance;
                distanceFound = true;
            }
        }
    }

    if (!distanceFound)
        return STATUS_CODE_NOT_FOUND;

    trackClusterDistance = minDistance;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ClusterHelper::CanMergeCluster(Cluster *const pCluster, float minMipFraction, float maxAllHitsFitRms)
{
    return ( (pCluster->GetNCaloHits() > 0) &&
        (!pCluster->IsPhoton() ||
        (pCluster->GetMipFraction() > minMipFraction) ||
        (pCluster->GetFitToAllHitsResult().IsFitSuccessful() && (pCluster->GetFitToAllHitsResult().GetRms() < maxAllHitsFitRms))) );
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

ClusterHelper::ClusterFitPoint::ClusterFitPoint(const CaloHit *const pCaloHit) :
    m_position(pCaloHit->GetPositionVector()),
    m_cellSize(std::sqrt(pCaloHit->GetCellSizeU() * pCaloHit->GetCellSizeV())),
    m_pseudoLayer(pCaloHit->GetPseudoLayer())
{
    if (!m_position.IsInitialized() || (0 == m_cellSize))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
}

//------------------------------------------------------------------------------------------------------------------------------------------

ClusterHelper::ClusterFitPoint::ClusterFitPoint(const CartesianVector &position, float cellSize, PseudoLayer pseudoLayer) :
    m_position(position),
    m_cellSize(cellSize),
    m_pseudoLayer(pseudoLayer)
{
    if (!m_position.IsInitialized() || (0 == m_cellSize))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
}

} // namespace pandora