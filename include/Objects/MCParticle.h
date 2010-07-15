/**
 *  @file   PandoraPFANew/include/Objects/MCParticle.h
 * 
 *  @brief  Header file for the mc particle class.
 * 
 *  $Log: $
 */
#ifndef MC_PARTICLE_H
#define MC_PARTICLE_H 1

#include "Api/PandoraApi.h"

#include "Pandora/PandoraInternal.h"

namespace pandora
{

/**
 *  @brief  MCParticle class
 */
class MCParticle 
{
public:
    /**
     *  @brief  Whether the mc particle properties have been initialized
     * 
     *  @return boolean
     */
    bool IsInitialized() const;

    /**
     *  @brief  Whether the mc particle is a root particle
     * 
     *  @return boolean
     */
    bool IsRootParticle() const;

    /**
     *  @brief  Whether the mc particle is a pfo target
     * 
     *  @return boolean
     */
    bool IsPfoTarget() const;

    /**
     *  @brief  Get energy of mc particle
     *
     *  @return the mc particle energy
     */
    float GetEnergy() const;

    /**
     *  @brief  Get momentum of mc particle
     *
     *  @return the mc particle momentum
     */
    const CartesianVector &GetMomentum() const;

    /**
     *  @brief  Get inner radius of mc particle
     * 
     *  @return the mc particle inner radius
     */
    float GetInnerRadius() const;

    /**
     *  @brief  Get outer radius of mc particle
     *
     *  @return the mc particle outer radius
     */
    float GetOuterRadius() const;

    /**
     *  @brief  Get the PDG code of the mc particle
     *
     *  @return the PDG code of the mc particle
     */
    int GetParticleId() const;

    /**
     *  @brief  Whether the pfo target been set
     *
     *  @return boolean
     */
    bool IsPfoTargetSet() const;

    /**
     *  @brief  Get pfo target particle
     * 
     *  @param  pMCParticle to receive the address of the pfo target
     */
    StatusCode GetPfoTarget(const MCParticle *&pMCParticle) const;

    /**
     *  @brief  Get the mc particle unique identifier
     * 
     *  @return the mc particle unique identifier
     */
    Uid GetUid() const;

    /**
     *  @brief  Get list of parents of mc particle
     * 
     *  @return the mc parent particle list
     */
    const MCParticleList &GetParentList() const;

    /**
     *  @brief  Get list of daughters of mc particle
     * 
     *  @return the mc daughter particle list
     */
    const MCParticleList &GetDaughterList() const;

private:
    /**
     *  @brief  Constructor
     * 
     *  @param  mcParticleParameters the mc particle parameters
     */
    MCParticle(const PandoraApi::MCParticleParameters &mcParticleParameters);

    /**
     *  @brief  Constructor
     * 
     *  @param  uid the unique identifier of the mc particle
     */
    MCParticle(const Uid uid);

    /**
     *  @brief  Destructor
     */
    ~MCParticle();

    /**
     *  @brief  Set mc particle properties
     * 
     *  @param  mcParticleParameters the mc particle parametersle id
     */
    void SetProperties(const PandoraApi::MCParticleParameters &mcParticleParameters);

    /**
     *  @brief  Add daughter particle
     * 
     *  @param  mcParticle the daughter particle
     */
    StatusCode AddDaughter(MCParticle *pMCParticle);

    /**
     *  @brief  Add parent particle
     * 
     *  @param  mcParticle the parent particle
     */
    StatusCode AddParent(MCParticle *pMCParticle);

    /**
     *  @brief  Remove daughter particle
     * 
     *  @param  mcParticle the daughter particle
     */
    StatusCode RemoveDaughter(MCParticle *pMCParticle);

    /**
     *  @brief  Remove parent particle
     * 
     *  @param  mcParticle the parent particle
     */
    StatusCode RemoveParent(MCParticle *pMCParticle);

    /**
     *  @brief  Set pfo target particle
     * 
     *  @param  mcParticle the pfo target particle
     */
    StatusCode SetPfoTarget(MCParticle *pMCParticle);

    /**
     *  @brief  Set pfo target for a mc tree
     * 
     *  @param  mcParticle particle in the mc tree
     *  @param  onlyDaughters if "true" go through daughters only, if false
     *          go through parents as well
     */
    StatusCode SetPfoTargetInTree(MCParticle *pMCParticle, bool onlyDaughters = false);

    Uid                     m_uid;                      ///< Unique identifier for the particle

    float                   m_energy;                   ///< The energy of the particle
    CartesianVector         m_momentum;                 ///< The momentum of the particle
    float                   m_innerRadius;              ///< Inner radius of the particle's path
    float                   m_outerRadius;              ///< Outer radius of the particle's path
    int                     m_particleId;               ///< The PDG code of the mc particle

    MCParticle             *m_pPfoTarget;               ///< The address of the pfo target
    MCParticleList          m_daughterList;             ///< The list of mc daughter particles
    MCParticleList          m_parentList;               ///< The list of mc parent particles

    bool                    m_isInitialized;            ///< Whether the particle information has been initialized

    friend class MCManager;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool MCParticle::IsInitialized() const
{
    return m_isInitialized;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool MCParticle::IsRootParticle() const
{
    return m_parentList.empty();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool MCParticle::IsPfoTarget() const
{
    return (this == m_pPfoTarget);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool MCParticle::IsPfoTargetSet() const
{
    return (NULL != m_pPfoTarget);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode MCParticle::GetPfoTarget(const MCParticle *&pMCParticle) const
{
    if (NULL == m_pPfoTarget)
        return STATUS_CODE_NOT_INITIALIZED;

    pMCParticle = m_pPfoTarget;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline Uid MCParticle::GetUid() const
{
   return m_uid;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode MCParticle::AddDaughter(MCParticle *pMCParticle)
{
    if (!m_daughterList.insert(pMCParticle).second)
        return STATUS_CODE_ALREADY_PRESENT;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode MCParticle::AddParent(MCParticle *pMCParticle)
{
    if (!m_parentList.insert(pMCParticle).second)
        return STATUS_CODE_ALREADY_PRESENT;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode MCParticle::RemoveDaughter(MCParticle *pMCParticle)
{
    MCParticleList::iterator iter = m_daughterList.find(pMCParticle);

    if (m_daughterList.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    m_daughterList.erase(iter);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode MCParticle::RemoveParent(MCParticle *pMCParticle)
{
    MCParticleList::iterator iter = m_parentList.find(pMCParticle);

    if (m_parentList.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    m_parentList.erase(iter);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode MCParticle::SetPfoTarget(MCParticle *pMCParticle)
{
    if (NULL == pMCParticle)
        return STATUS_CODE_FAILURE;

    m_pPfoTarget = pMCParticle;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float MCParticle::GetEnergy() const
{
    return m_energy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &MCParticle::GetMomentum() const
{
    return m_momentum;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float MCParticle::GetInnerRadius() const
{
    return m_innerRadius;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float MCParticle::GetOuterRadius() const
{
    return m_outerRadius;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline int MCParticle::GetParticleId() const
{
    return m_particleId;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const MCParticleList &MCParticle::GetParentList() const
{
    return m_parentList;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const MCParticleList &MCParticle::GetDaughterList() const
{
    return m_daughterList;
}

} // namespace pandora

#endif // #ifndef MC_PARTICLE_H
