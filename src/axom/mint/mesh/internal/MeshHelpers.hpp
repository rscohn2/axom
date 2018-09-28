/*
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Copyright (c) 2017-2018, Lawrence Livermore National Security, LLC.
 *
 * Produced at the Lawrence Livermore National Laboratory
 *
 * LLNL-CODE-741217
 *
 * All rights reserved.
 *
 * This file is part of Axom.
 *
 * For details about use and distribution, please read axom/LICENSE.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#ifndef MINT_MESH_HELPERS_HPP_
#define MINT_MESH_HELPERS_HPP_


#include "axom/core/Macros.hpp" // for AXOM_NOT_USED
#include "axom/core/Types.hpp"  // for nullptr
#include "axom/mint/config.hpp" // for mint compile-time type

namespace axom
{
namespace mint
{

class Mesh;  // forward declaration

namespace internal
{

inline int dim( const double* AXOM_NOT_USED(x),
                const double* y,
                const double* z )
{
  return ( ( z != nullptr ) ? 3 : ( (y != nullptr ) ? 2 : 1 ) );
}

namespace m = ::axom::mint;

/*! \brief Record a Mesh's face-to-cell and cell-to-face relations.
 *
 * \param [in] m pointer to a Mesh.
 * \param [out] facecount the number of unique faces of m's cells.
 * \param [out] f2c the relation between face f and its two incident cells
 *              with cellIDs at f2c[2*f] and f2c[2*f+1].
 * \param [out] c2f the relation between cell c and its n faces with faceIDs
 *              stored contiguously starting at c2f[c2foffsets[c]].
 * \param [out] c2n the relation between cell c and its n neighbors with
 *              cellIDs stored contiguously starting at c2n[c2foffsets[c]].
 * \param [out] c2foffsets the offset in c2f of the first face of each cell.
 *
 * \returns success true if each face has one or two incident cells.
 *
 * \note The four output arrays f2c, c2f, c2n, and c2foffsets are allocated
 * in this routine if the routine is successful.  It is the caller's
 * responsibility to free this memory.  If the routine returned false,
 * the output arrays are set to nullptr and facecount is set to 0.
 *
 * This routine visits each of the cells of the mesh.  For each cell face, it
 * retrieves the face's nodes and joins the sorted node IDs to make a unique
 * hash key.  The incident cells are recorded in a list for each face's hash
 * key.  The final face-cell and cell-face relations are constructed from this
 * data structure.
 *
 * This routine is intended to be used in constructing an UnstructuredMesh's
 * face relations, though it will give correct results for any Mesh.
 */
bool initFaces(m::Mesh * m,
               IndexType & facecount,
               m::IndexType *& f2c,
               m::IndexType *& c2f,
               m::IndexType *& c2n,
               m::IndexType *& c2foffsets);

} /* namespace internal */
} /* namespace mint */
} /* namespace axom */

#endif /* MINT_MESH_HELPERS_HPP_ */
