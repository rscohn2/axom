/*
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Copyright (c) 2017, Lawrence Livermore National Security, LLC.
 *
 * Produced at the Lawrence Livermore National Laboratory
 *
 * LLNL-CODE-xxxxxxx
 *
 * All rights reserved.
 *
 * This file is part of Axom.
 *
 * For details about use and distribution, please read axom/LICENSE.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#ifndef MINT_LAGRANGE_PYRA_5_HPP_
#define MINT_LAGRANGE_PYRA_5_HPP_

// Mint includes
#include "mint/CellType.hpp"
#include "mint/FEBasisTypes.hpp"
#include "mint/ShapeFunction.hpp"

// Slic includes
#include "slic/slic.hpp"

namespace axom {
namespace mint {

/*!
 * \brief Lagrange Finite Element definition for the Linear Pyramnid
 *
 * \verbatim
 *
 * pyra_5:
 *
 *              4
 *            / |\
 *           /  | \
 *          /   |  \
 *        0/_ __|_ 3\
 *         \    |    \
 *          \ _ |_  _ \
 *            1       2
 *
 * \endverbatim
 *
 * \warning The jacobian for the pyramid elements may become singular near the
 *  apex in which case the isoparametric mapping will fail.
 *
 * \see ShapeFunction
 */
template < >
class Lagrange< MINT_PYRAMID > :
  public ShapeFunction< Lagrange< MINT_PYRAMID > >
{
public:

  static int getCellType() { return MINT_PYRAMID; }

  static int getType() { return MINT_LAGRANGE_BASIS; }

  static int getNumDofs() { return 5; }

  static int getMaxNewtonIters() { return 16; }

  static int getDimension() { return 3; }

  static double getMin() { return 0; }

  static double getMax() { return 1; }

  static void getCenter( double* center )
  {
    SLIC_ASSERT( center != AXOM_NULLPTR );
    center[ 0 ] = center[ 1 ] = 0.4;
    center[ 2 ] = 0.2;
  }

  static void getCoords( double* coords )
  {
    SLIC_ASSERT( coords != AXOM_NULLPTR );

    // node 0
    coords[ 0 ] = 0.0;
    coords[ 1 ] = 0.0;
    coords[ 2 ] = 0.0;

    // node 1
    coords[ 3 ] = 1.0;
    coords[ 4 ] = 0.0;
    coords[ 5 ] = 0.0;

    // node 2
    coords[ 6 ] = 1.0;
    coords[ 7 ] = 1.0;
    coords[ 8 ] = 0.0;

    // node 3
    coords[ 9 ]  = 0.0;
    coords[ 10 ] = 1.0;
    coords[ 11 ] = 0.0;

    // node 4
    coords[ 12 ] = 0.0;
    coords[ 13 ] = 0.0;
    coords[ 14 ] = 1.0;
  }

  static void computeShape( const double* xr, double* phi )
  {
    SLIC_ASSERT(  xr != AXOM_NULLPTR );
    SLIC_ASSERT(  phi != AXOM_NULLPTR );

    const double r  = xr[0];
    const double s  = xr[1];
    const double t  = xr[2];
    const double rm = 1.-r;
    const double sm = 1.-s;
    const double tm = 1.-t;

    phi[ 0 ] = rm * sm * tm;
    phi[ 1 ] = r  * sm * tm;
    phi[ 2 ] = r  * s  * tm;
    phi[ 3 ] = rm * s  * tm;
    phi[ 4 ] = t;
  }

  static void computeDerivatives( const double* xr, double* phidot )
  {
    SLIC_ASSERT(  xr != AXOM_NULLPTR );
    SLIC_ASSERT(  phidot != AXOM_NULLPTR );

    const double r  = xr[0];
    const double s  = xr[1];
    const double t  = xr[2];
    const double rm = 1.-r;
    const double sm = 1.-s;
    const double tm = 1.-t;

    // r-derivatives
    phidot[ 0 ] = -sm * tm;
    phidot[ 1 ] =  sm * tm;
    phidot[ 2 ] =  s  * tm;
    phidot[ 3 ] = -s  * tm;
    phidot[ 4 ] = 0.0;

    // s-derivatives
    phidot[ 5 ] = -rm * tm;
    phidot[ 6 ] = -r  * tm;
    phidot[ 7 ] =  r  * tm;
    phidot[ 8 ] =  rm * tm;
    phidot[ 9 ] = 0.0;

    // t-derivatives
    phidot[ 10 ] = -rm * sm;
    phidot[ 11 ] = -r  * sm;
    phidot[ 12 ] = -r  * s;
    phidot[ 13 ] = -rm * s;
    phidot[ 14 ] = 1.0;
  }

};

} /* namespace mint */
} /* namespace axom */

#endif /* MINT_LAGRANGE_PYRA_5_HPP_ */
