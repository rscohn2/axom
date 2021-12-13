// Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
// other Axom Project Developers. See the top-level LICENSE file for details.
//
// SPDX-License-Identifier: (BSD-3-Clause)

#include "gtest/gtest.h"

#include "axom/config.hpp"
#include "axom/slic.hpp"

#include "axom/primal/geometry/Point.hpp"
#include "axom/primal/geometry/Tetrahedron.hpp"
#include "axom/primal/geometry/Sphere.hpp"
#include "axom/primal/geometry/OrientationResult.hpp"

#include "axom/fmt.hpp"

#include <cmath>

namespace primal = axom::primal;

/// Test fixture for testing primal::Tetrahedron
class TetrahedronTest : public ::testing::Test
{
public:
  static const int DIM = 3;

  using CoordType = double;
  using QPoint = primal::Point<CoordType, DIM>;
  using QTet = primal::Tetrahedron<CoordType, DIM>;

protected:
  virtual void SetUp()
  {
    EPS = 1e-12;

    // Define coordinates for first tetrahedron
    qData0[0] = QPoint {0, 0, 0};
    qData0[1] = QPoint {1, 0, 0};
    qData0[2] = QPoint {1, 1, 0};
    qData0[3] = QPoint {1, 1, 1};

    // Define coordinates for second tetrahedron
    qData1[0] = QPoint {1, 0, 0};
    qData1[1] = QPoint {0, 1, 0};
    qData1[2] = QPoint {0, 0, 1};
    qData1[3] = QPoint {0, 0, 0};

    // Define coordinates for third tetrahedron
    const double sc2 = .1;
    qData2[0] =
      QPoint {sc2 * std::cos(0 * M_PI / 3.), sc2 * std::sin(0 * M_PI / 3.), 0};
    qData2[1] =
      QPoint {sc2 * std::cos(1 * M_PI / 3.), sc2 * std::sin(1 * M_PI / 3.), 0};
    qData2[2] =
      QPoint {sc2 * std::cos(2 * M_PI / 3.), sc2 * std::sin(2 * M_PI / 3.), 0};
    qData2[3] = QPoint {0, 0, 100.};

    // Define coordinates for fourth tetrahedron
    const double sc3 = 100.;
    qData3[0] =
      QPoint {sc3 * std::cos(0 * M_PI / 3.), sc3 * std::sin(0 * M_PI / 3.), 0};
    qData3[1] =
      QPoint {sc3 * std::cos(1 * M_PI / 3.), sc3 * std::sin(1 * M_PI / 3.), 0};
    qData3[2] =
      QPoint {sc3 * std::cos(2 * M_PI / 3.), sc3 * std::sin(2 * M_PI / 3.), 0};
    qData3[3] = QPoint {0, 0, .1};
  }

  QTet getTet(int idx)
  {
    EXPECT_TRUE(idx >= 0 && idx < 4);

    QTet tet;

    switch(idx)
    {
    case 0:
      tet = QTet(qData0[0], qData0[1], qData0[2], qData0[3]);
      break;
    case 1:
      tet = QTet(qData1[0], qData1[1], qData1[2], qData1[3]);
      break;
    case 2:
      tet = QTet(qData2[0], qData2[1], qData2[2], qData2[3]);
      break;
    case 3:
      tet = QTet(qData3[0], qData3[1], qData3[2], qData3[3]);
      break;
    }

    return tet;
  }

  QPoint qData0[4];
  QPoint qData1[4];
  QPoint qData2[4];
  QPoint qData3[4];
  double EPS;
};

//------------------------------------------------------------------------------
TEST_F(TetrahedronTest, defaultConstructor)
{
  using QPoint = TetrahedronTest::QPoint;
  using QTet = TetrahedronTest::QTet;

  const QTet tet;

  // Test ostream operator
  SLIC_INFO("Empty tetrahedron coordinates: " << tet);

  // Check indirection operator
  EXPECT_EQ(QPoint::zero(), tet[0]);
  EXPECT_EQ(QPoint::zero(), tet[1]);
  EXPECT_EQ(QPoint::zero(), tet[2]);
  EXPECT_EQ(QPoint::zero(), tet[3]);
}

TEST_F(TetrahedronTest, constructFromPoints)
{
  using QPoint = TetrahedronTest::QPoint;
  using QTet = TetrahedronTest::QTet;

  // Access the test data
  const QPoint* pt = this->qData0;

  QTet tet(pt[0], pt[1], pt[2], pt[3]);

  // Test ostream operator
  SLIC_INFO("Tetrahedron coordinates: " << tet);

  // Check indirection operator
  EXPECT_EQ(pt[0], tet[0]);
  EXPECT_EQ(pt[1], tet[1]);
  EXPECT_EQ(pt[2], tet[2]);
  EXPECT_EQ(pt[3], tet[3]);
}

TEST_F(TetrahedronTest, volume)
{
  using QPoint = TetrahedronTest::QPoint;
  using QTet = TetrahedronTest::QTet;

  // Access the test data
  const QPoint* pt = this->qData0;
  QTet tet(pt[0], pt[1], pt[2], pt[3]);

  double expVolume = 1. / 6.;
  EXPECT_EQ(expVolume, tet.signedVolume());
  EXPECT_EQ(tet.signedVolume(), tet.volume());
}

TEST_F(TetrahedronTest, degenerate)
{
  using QPoint = TetrahedronTest::QPoint;
  using QTet = TetrahedronTest::QTet;

  // Access the test data
  const QPoint* pt = this->qData0;
  QTet tet(pt[0], pt[1], pt[2], pt[3]);

  EXPECT_FALSE(tet.degenerate());

  // Make the tet degenerate by identifying two vertices
  tet[0] = tet[1];
  EXPECT_TRUE(tet.degenerate());
}

TEST_F(TetrahedronTest, barycentric)
{
  using CoordType = TetrahedronTest::CoordType;
  using QPoint = TetrahedronTest::QPoint;
  using QTet = TetrahedronTest::QTet;
  using RPoint = primal::Point<CoordType, 4>;
  using TestVec = std::vector<std::pair<QPoint, RPoint>>;

  const QPoint* pt = this->qData1;
  QTet tet(pt[0], pt[1], pt[2], pt[3]);

  TestVec testData;

  // Test the four vertices
  testData.push_back(std::make_pair(pt[0], RPoint {1., 0., 0., 0.}));
  testData.push_back(std::make_pair(pt[1], RPoint {0., 1., 0., 0.}));
  testData.push_back(std::make_pair(pt[2], RPoint {0., 0., 1., 0.}));
  testData.push_back(std::make_pair(pt[3], RPoint {0., 0., 0., 1.}));

  // Test some of the edge midpoints
  testData.push_back(
    std::make_pair(QPoint::midpoint(pt[0], pt[1]), RPoint {0.5, 0.5, 0., 0.}));
  testData.push_back(
    std::make_pair(QPoint::midpoint(pt[1], pt[2]), RPoint {0., 0.5, 0.5, 0.}));
  testData.push_back(
    std::make_pair(QPoint::midpoint(pt[2], pt[3]), RPoint {0., 0., 0.5, 0.5}));
  testData.push_back(
    std::make_pair(QPoint::midpoint(pt[0], pt[2]), RPoint {0.5, 0., 0.5, 0.}));
  testData.push_back(
    std::make_pair(QPoint::midpoint(pt[0], pt[3]), RPoint {0.5, 0., 0., 0.5}));
  testData.push_back(
    std::make_pair(QPoint::midpoint(pt[1], pt[3]), RPoint {0., 0.5, 0., 0.5}));

  // Test the centroid
  testData.push_back(std::make_pair(
    QPoint(.25 * (pt[0].array() + pt[1].array() + pt[2].array() + pt[3].array())),
    RPoint {.25, .25, .25, .25}));

  // Test a point outside the tetrahedron
  testData.push_back(std::make_pair(
    QPoint(-0.4 * pt[0].array() + 1.2 * pt[1].array() + 0.2 * pt[2].array()),
    RPoint {-0.4, 1.2, 0.2, 0.}));

  // Now run the actual tests
  for(const auto& data : testData)
  {
    const QPoint& query = data.first;
    const RPoint& expBary = data.second;
    RPoint bary = tet.physToBarycentric(query);

    SLIC_DEBUG(axom::fmt::format(
      "Computed barycentric coordinates for tetrahedron {} and point {} are {}",
      tet,
      query,
      bary));

    EXPECT_NEAR(bary[0], expBary[0], this->EPS);
    EXPECT_NEAR(bary[1], expBary[1], this->EPS);
    EXPECT_NEAR(bary[2], expBary[2], this->EPS);
    EXPECT_NEAR(bary[3], expBary[3], this->EPS);
  }
}

//------------------------------------------------------------------------------
TEST_F(TetrahedronTest, tet_3D_circumsphere)
{
  using CoordType = TetrahedronTest::CoordType;
  using QPoint = TetrahedronTest::QPoint;
  using QTet = TetrahedronTest::QTet;
  using QSphere = primal::Sphere<CoordType, 3>;
  using RPoint = primal::Point<CoordType, 4>;
  const double EPS = 1e-9;

  using primal::ON_BOUNDARY;
  using primal::ON_NEGATIVE_SIDE;
  using primal::ON_POSITIVE_SIDE;

  // Test tets
  std::vector<QTet> tets = {this->getTet(0),
                            this->getTet(1),
                            this->getTet(2),
                            this->getTet(3)};

  // Compute circumsphere of test triangles and test some points
  for(const auto& tet : tets)
  {
    QSphere circumsphere = tet.circumsphere();

    SLIC_INFO("Circumsphere for tetrahedron: " << tet << " is " << circumsphere);

    // test vertices
    for(int i = 0; i < 4; ++i)
    {
      QPoint qpt = tet[i];
      EXPECT_EQ(ON_BOUNDARY, circumsphere.getOrientation(qpt.data(), EPS));
    }

    // test edge centers
    {
      QPoint qpt[6] = {QPoint::midpoint(tet[0], tet[1]),
                       QPoint::midpoint(tet[0], tet[2]),
                       QPoint::midpoint(tet[0], tet[3]),
                       QPoint::midpoint(tet[1], tet[2]),
                       QPoint::midpoint(tet[1], tet[3]),
                       QPoint::midpoint(tet[2], tet[3])};
      for(int j = 0; j < 6; ++j)
      {
        EXPECT_EQ(ON_NEGATIVE_SIDE,
                  circumsphere.getOrientation(qpt[j].data(), EPS));
      }
    }

    // test face centers
    {
      const CoordType third = 1. / 3.;
      const CoordType zero {0};
      QPoint qpt[4] = {tet.baryToPhysical(RPoint {third, third, third, zero}),
                       tet.baryToPhysical(RPoint {third, third, zero, third}),
                       tet.baryToPhysical(RPoint {third, zero, third, third}),
                       tet.baryToPhysical(RPoint {zero, third, third, third})};
      for(int j = 0; j < 4; ++j)
      {
        EXPECT_EQ(ON_NEGATIVE_SIDE,
                  circumsphere.getOrientation(qpt[j].data(), EPS));
      }
    }

    // test tet center
    {
      QPoint qpt = tet.baryToPhysical(RPoint {.25, .25, .25, .25});
      EXPECT_EQ(ON_NEGATIVE_SIDE, circumsphere.getOrientation(qpt.data(), EPS));
    }

    // test points that should be far outside tet
    {
      QPoint qpt[4] = {tet.baryToPhysical(RPoint {-1, 3, -1, 0}),
                       tet.baryToPhysical(RPoint {0, -1, 3, -1}),
                       tet.baryToPhysical(RPoint {-1, -1, 0, 3}),
                       tet.baryToPhysical(RPoint {3, -1, -1, 0})};
      for(int j = 0; j < 4; ++j)
      {
        EXPECT_EQ(ON_POSITIVE_SIDE,
                  circumsphere.getOrientation(qpt[j].data(), EPS));
      }
    }
  }
}
//----------------------------------------------------------------------
//----------------------------------------------------------------------
int main(int argc, char* argv[])
{
  ::testing::InitGoogleTest(&argc, argv);

  axom::slic::SimpleLogger logger;  // create & initialize test logger,
  axom::slic::setLoggingMsgLevel(axom::slic::message::Info);

  int result = RUN_ALL_TESTS();
  return result;
}
