/**
 *  @file   PandoraPFANew/include/PandoraType.h
 * 
 *  @brief  Header file for the pandora type class and functions such as StringToType and TypeToString
 * 
 *  $Log: $
 */
#ifndef PANDORA_TYPES_H
#define PANDORA_TYPES_H 1

#include "Objects/CartesianVector.h"
#include "Objects/TrackState.h"

#include "StatusCodes.h"

#include <iostream>
#include <sstream>

namespace pandora
{

template <typename T>
class PandoraType
{
public:
    /**
     *  @brief  Default constructor
     */
    PandoraType();

    /**
     *  @brief  Constructor
     *
     *  @param  t the initial value 
     */
    PandoraType(const T &t);

    /**
     *  @brief  Set the value held by the pandora type
     *
     *  @param  t the value
     */
    void Set(const T &t);
    
    /**
     *  @brief  Get the value held by the pandora type
     *
     *  @return the value
     */
    T Get() const;

    /**
     *  @brief  Reset the pandora type
     */   
    void Reset();

    /**
     *  @brief  Whether the pandora type is initialized
     *
     *  @return boolean
     */
    bool IsInitialized() const;

    /**
     *  @brief  Assignment operator for the pandora type
     * 
     *  @return whether assignment succeeded
     */   
    bool operator= (const T &rhs); 

private:
    T       m_value;            ///< The actual value being held by the pandora type
    bool    m_isInitialized;    ///< Whether the pandora type is initialized
};

typedef PandoraType<unsigned int> InputUInt;
typedef PandoraType<int> InputInt;
typedef PandoraType<float> InputFloat;
typedef PandoraType<void *> InputAddress;
typedef PandoraType<CartesianVector> InputCartesianVector;
typedef PandoraType<TrackState> InputTrackState;

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
PandoraType<T>::PandoraType() :
    m_isInitialized(false)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
PandoraType<T>::PandoraType(const T &t) :
    m_value(t),
    m_isInitialized(true)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
void PandoraType<T>::Set(const T &t)
{
    m_value = t;
    m_isInitialized = true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
T PandoraType<T>::Get() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_value;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
void PandoraType<T>::Reset()
{
    m_isInitialized = false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
bool PandoraType<T>::IsInitialized() const
{
    return m_isInitialized;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
bool PandoraType<T>::operator= (const T &rhs)
{
    this->Set(rhs);
    return m_isInitialized;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

template <class T>
inline bool StringToType(const std::string &s, T &t)
{
    std::istringstream iss(s);
    return !(iss >> t).fail();
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

template <class T>
inline std::string TypeToString(const T &t)
{
    std::ostringstream oss;

    if ((oss << t).fail())
        throw;

    return oss.str();
}

} // namespace pandora

#endif // #ifndef PANDORA_TYPES_H
