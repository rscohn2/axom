// Copyright (c) 2017-2020, Lawrence Livermore National Security, LLC and
// other Axom Project Developers. See the top-level COPYRIGHT file for details.
//
// SPDX-License-Identifier: (BSD-3-Clause)

#include "gtest/gtest.h"

#include <array>
#include <string>
#include <vector>
#include <unordered_map>

#include <iostream>

#include "axom/sidre.hpp"

#include "axom/inlet/LuaReader.hpp"
#include "axom/inlet/Inlet.hpp"

using axom::inlet::FunctionType;
using axom::inlet::Inlet;
using axom::inlet::InletType;
using axom::inlet::LuaReader;
using axom::sidre::DataStore;

Inlet createBasicInlet(DataStore* ds,
                       const std::string& luaString,
                       bool enableDocs = true)
{
  auto lr = std::make_unique<LuaReader>();
  lr->parseString(luaString);

  return Inlet(std::move(lr), ds->getRoot(), enableDocs);
}

TEST(inlet_function, simple_vec3_to_double_raw)
{
  std::string testString = "function foo (x, y, z) return x + y + z end";
  DataStore ds;
  auto inlet = createBasicInlet(&ds, testString);

  auto func =
    inlet.reader().getFunction("foo", FunctionType::Double, {FunctionType::Vec3D});

  EXPECT_TRUE(func);
  auto result = func.call<double>(axom::primal::Vector3D {1, 2, 3});
  EXPECT_FLOAT_EQ(result, 6);
}

TEST(inlet_function, simple_vec3_to_vec3_raw)
{
  std::string testString = "function foo (x, y, z) return 2*x, 2*y, 2*z end";
  DataStore ds;
  auto inlet = createBasicInlet(&ds, testString);

  auto func =
    inlet.reader().getFunction("foo", FunctionType::Vec3D, {FunctionType::Vec3D});

  EXPECT_TRUE(func);
  auto result =
    func.call<axom::primal::Vector3D>(axom::primal::Vector3D {1, 2, 3});
  EXPECT_FLOAT_EQ(result[0], 2);
  EXPECT_FLOAT_EQ(result[1], 4);
  EXPECT_FLOAT_EQ(result[2], 6);
}

TEST(inlet_function, simple_vec3_to_double_through_table)
{
  std::string testString = "function foo (x, y, z) return x + y + z end";
  DataStore ds;
  auto inlet = createBasicInlet(&ds, testString);

  inlet.addFunction("foo",
                    FunctionType::Double,
                    {FunctionType::Vec3D},
                    "foo's description");

  auto callable =
    inlet["foo"].get<std::function<double(axom::primal::Vector3D)>>();
  auto result = callable({1, 2, 3});
  EXPECT_FLOAT_EQ(result, 6);
}

TEST(inlet_function, simple_vec3_to_vec3_through_table)
{
  std::string testString = "function foo (x, y, z) return 2*x, 2*y, 2*z end";
  DataStore ds;
  auto inlet = createBasicInlet(&ds, testString);

  inlet.addFunction("foo",
                    FunctionType::Vec3D,
                    {FunctionType::Vec3D},
                    "foo's description");

  auto callable =
    inlet["foo"]
      .get<std::function<axom::primal::Vector3D(axom::primal::Vector3D)>>();
  auto result = callable({1, 2, 3});
  EXPECT_FLOAT_EQ(result[0], 2);
  EXPECT_FLOAT_EQ(result[1], 4);
  EXPECT_FLOAT_EQ(result[2], 6);
}

TEST(inlet_function, simple_vec3_to_double_through_table_call)
{
  std::string testString = "function foo (x, y, z) return x + y + z end";
  DataStore ds;
  auto inlet = createBasicInlet(&ds, testString);

  inlet.addFunction("foo",
                    FunctionType::Double,
                    {FunctionType::Vec3D},
                    "foo's description");

  auto result = inlet["foo"].call<double>(axom::primal::Vector3D {1, 2, 3});
  EXPECT_FLOAT_EQ(result, 6);
}

TEST(inlet_function, simple_vec3_to_vec3_through_table_call)
{
  std::string testString = "function foo (x, y, z) return 2*x, 2*y, 2*z end";
  DataStore ds;
  auto inlet = createBasicInlet(&ds, testString);

  inlet.addFunction("foo",
                    FunctionType::Vec3D,
                    {FunctionType::Vec3D},
                    "foo's description");

  auto result =
    inlet["foo"].call<axom::primal::Vector3D>(axom::primal::Vector3D {1, 2, 3});
  EXPECT_FLOAT_EQ(result[0], 2);
  EXPECT_FLOAT_EQ(result[1], 4);
  EXPECT_FLOAT_EQ(result[2], 6);
}

TEST(inlet_function, simple_vec3_double_to_double_through_table_call)
{
  std::string testString =
    "function foo (x, y, z, t) return t * (x + y + z) end";
  DataStore ds;
  auto inlet = createBasicInlet(&ds, testString);

  inlet.addFunction("foo",
                    FunctionType::Double,
                    {FunctionType::Vec3D, FunctionType::Double},
                    "foo's description");

  auto result = inlet["foo"].call<double>(axom::primal::Vector3D {1, 2, 3}, 2.0);
  EXPECT_FLOAT_EQ(result, 12);
}

TEST(inlet_function, simple_vec3_double_to_vec3_through_table_call)
{
  std::string testString = "function foo (x, y, z, t) return t*x, t*y, t*z end";
  DataStore ds;
  auto inlet = createBasicInlet(&ds, testString);

  inlet.addFunction("foo",
                    FunctionType::Vec3D,
                    {FunctionType::Vec3D, FunctionType::Double},
                    "foo's description");

  auto result =
    inlet["foo"].call<axom::primal::Vector3D>(axom::primal::Vector3D {1, 2, 3},
                                              2.0);
  EXPECT_FLOAT_EQ(result[0], 2);
  EXPECT_FLOAT_EQ(result[1], 4);
  EXPECT_FLOAT_EQ(result[2], 6);
}

TEST(inlet_function, simple_vec3_to_vec3_verify_lambda_pass)
{
  std::string testString = "function foo (x, y, z) return 2*x, 2*y, 2*z end";
  DataStore ds;
  auto inlet = createBasicInlet(&ds, testString);

  auto& func = inlet
                 .addFunction("foo",
                              FunctionType::Vec3D,
                              {FunctionType::Vec3D},
                              "foo's description")
                 .required();
  func.registerVerifier([](const axom::inlet::Function& func) {
    auto result =
      func.call<axom::primal::Vector3D>(axom::primal::Vector3D {1, 0, 0});
    return std::abs(result[0] - 2) < 1e-5;
  });

  EXPECT_TRUE(inlet.verify());
}

TEST(inlet_function, simple_vec3_to_vec3_verify_lambda_fail)
{
  std::string testString = "function foo (x, y, z) return 2*x, 2*y, 2*z end";
  DataStore ds;
  auto inlet = createBasicInlet(&ds, testString);

  auto& func = inlet
                 .addFunction("foo",
                              FunctionType::Vec3D,
                              {FunctionType::Vec3D},
                              "foo's description")
                 .required();
  func.registerVerifier([](const axom::inlet::Function& func) {
    auto result =
      func.call<axom::primal::Vector3D>(axom::primal::Vector3D {2, 0, 0});
    return std::abs(result[0] - 2) < 1e-5;
  });

  EXPECT_FALSE(inlet.verify());
}

struct Foo
{
  bool bar;
  std::function<axom::primal::Vector3D(axom::primal::Vector3D)> baz;
};

template <>
struct FromInlet<Foo>
{
  Foo operator()(const axom::inlet::Table& base)
  {
    Foo f {base["bar"], base["baz"]};
    return f;
  }
};

TEST(inlet_function, simple_vec3_to_vec3_struct)
{
  std::string testString =
    "foo = { bar = true; baz = function (x, y, z) return 2*x, 2*y, 2*z end }";
  DataStore ds;
  auto inlet = createBasicInlet(&ds, testString);

  // Define schema
  inlet.addBool("foo/bar", "bar's description");
  inlet
    .addFunction("foo/baz",
                 FunctionType::Vec3D,
                 {FunctionType::Vec3D},
                 "baz's description")
    .required();
  Foo foo = inlet["foo"].get<Foo>();
  EXPECT_TRUE(foo.bar);
  auto result = foo.baz({4, 5, 6});
  EXPECT_FLOAT_EQ(result[0], 8);
  EXPECT_FLOAT_EQ(result[1], 10);
  EXPECT_FLOAT_EQ(result[2], 12);
}

TEST(inlet_function, simple_vec3_to_vec3_array_of_struct)
{
  std::string testString =
    "foo = { [7] = { bar = true; baz = function (x, y, z) return 2*x, 2*y, 2*z "
    "end }, [12] = { bar = false; baz = function (x, y, z) return 3*x, 3*y, "
    "3*z end } }";
  DataStore ds;
  auto inlet = createBasicInlet(&ds, testString);

  auto& arr_table = inlet.addGenericArray("foo");

  // Define schema
  arr_table.addBool("bar", "bar's description");
  arr_table
    .addFunction("baz",
                 FunctionType::Vec3D,
                 {FunctionType::Vec3D},
                 "baz's description")
    .required();

  auto foos = inlet["foo"].get<std::unordered_map<int, Foo>>();
  EXPECT_TRUE(foos[7].bar);
  auto first_result = foos[7].baz({4, 5, 6});
  EXPECT_FLOAT_EQ(first_result[0], 8);
  EXPECT_FLOAT_EQ(first_result[1], 10);
  EXPECT_FLOAT_EQ(first_result[2], 12);

  EXPECT_FALSE(foos[12].bar);
  auto second_result = foos[12].baz({4, 5, 6});
  EXPECT_FLOAT_EQ(second_result[0], 12);
  EXPECT_FLOAT_EQ(second_result[1], 15);
  EXPECT_FLOAT_EQ(second_result[2], 18);
}

template <typename Ret, typename... Args>
Ret checkedCall(const sol::protected_function& func, Args&&... args)
{
  auto tentative_result = func(std::forward<Args>(args)...);
  EXPECT_TRUE(tentative_result.valid());
  return tentative_result;
}

TEST(inlet_function, lua_usertype_basic)
{
  std::string testString = "function func(vec) return 7 end";
  LuaReader lr;
  lr.parseString(testString);
  sol::protected_function func = lr.solState()["func"];
  axom::primal::Vector3D vec {1, 2, 3};
  int result = checkedCall<int>(func, vec);
  EXPECT_EQ(result, 7);
}

TEST(inlet_function, lua_usertype_basic_ret)
{
  std::string testString =
    "function func(x, y, z) return Vec3D.new(x, y, z) end";
  LuaReader lr;
  lr.parseString(testString);
  sol::protected_function func = lr.solState()["func"];
  axom::primal::Vector3D vec {1, 2, 3};
  auto result = checkedCall<axom::primal::Vector3D>(func, 1, 2, 3);
  EXPECT_EQ(vec, result);
}

TEST(inlet_function, lua_usertype_basic_ret_2d)
{
  std::string testString = "function func(x, y, z) return Vec3D.new(x, y) end";
  LuaReader lr;
  lr.parseString(testString);
  sol::protected_function func = lr.solState()["func"];
  axom::primal::Vector3D vec {1, 2, 0};
  auto result = checkedCall<axom::primal::Vector3D>(func, 1, 2, 3);
  EXPECT_EQ(vec, result);
}

TEST(inlet_function, lua_usertype_basic_ret_default)
{
  std::string testString = "function func(x, y, z) return Vec3D.new() end";
  LuaReader lr;
  lr.parseString(testString);
  sol::protected_function func = lr.solState()["func"];
  axom::primal::Vector3D vec {0, 0, 0};
  auto result = checkedCall<axom::primal::Vector3D>(func, 1, 2, 3);
  EXPECT_EQ(vec, result);
}

TEST(inlet_function, lua_usertype_basic_add)
{
  std::string testString = "function func(vec1, vec2) return vec1 + vec2 end";
  LuaReader lr;
  lr.parseString(testString);
  sol::protected_function func = lr.solState()["func"];
  axom::primal::Vector3D vec1 {1, 2, 3};
  axom::primal::Vector3D vec2 {4, 5, 6};
  auto result = checkedCall<axom::primal::Vector3D>(func, vec1, vec2);
  EXPECT_EQ(result, vec1 + vec2);
}

TEST(inlet_function, lua_usertype_basic_negate)
{
  std::string testString = "function func(vec) return -vec end";
  LuaReader lr;
  lr.parseString(testString);
  sol::protected_function func = lr.solState()["func"];
  axom::primal::Vector3D vec {1, 2, 3};
  auto result = checkedCall<axom::primal::Vector3D>(func, vec);
  EXPECT_EQ(result, -vec);
}

TEST(inlet_function, lua_usertype_basic_scalar_mult)
{
  std::string testString =
    "function func1(vec, x) return vec * x end; function func2(vec, x) return "
    "x * vec end";
  LuaReader lr;
  lr.parseString(testString);
  sol::protected_function func1 = lr.solState()["func1"];
  sol::protected_function func2 = lr.solState()["func2"];
  axom::primal::Vector3D vec {1, 2, 3};
  auto result = checkedCall<axom::primal::Vector3D>(func1, vec, 2.0);
  EXPECT_EQ(result, 2.0 * vec);
  result = checkedCall<axom::primal::Vector3D>(func2, vec, 3.0);
  EXPECT_EQ(result, 3.0 * vec);
}

TEST(inlet_function, lua_usertype_basic_index_get)
{
  std::string testString = "function func(vec, idx) return vec[idx] end";
  LuaReader lr;
  lr.parseString(testString);
  sol::protected_function func = lr.solState()["func"];
  axom::primal::Vector3D vec {1, 2, 3};
  // Use 1-based indexing in these tests as lua is 1-indexed
  auto result = checkedCall<double>(func, vec, 1);
  EXPECT_FLOAT_EQ(1, result);
  result = checkedCall<double>(func, vec, 2);
  EXPECT_FLOAT_EQ(2, result);
  result = checkedCall<double>(func, vec, 3);
  EXPECT_FLOAT_EQ(3, result);
}

TEST(inlet_function, lua_usertype_basic_index_set)
{
  std::string testString =
    "function func(idx) vec = Vec3D.new(1,1,1); vec[idx] = -1; return vec end";
  LuaReader lr;
  lr.parseString(testString);
  sol::protected_function func = lr.solState()["func"];
  auto result = checkedCall<axom::primal::Vector3D>(func, 1);
  EXPECT_FLOAT_EQ(-1, result[0]);
  result = checkedCall<axom::primal::Vector3D>(func, 2);
  EXPECT_FLOAT_EQ(-1, result[1]);
  result = checkedCall<axom::primal::Vector3D>(func, 3);
  EXPECT_FLOAT_EQ(-1, result[2]);
}

TEST(inlet_function, lua_usertype_basic_norm)
{
  std::string testString = "function func(vec) return vec:norm() end";
  LuaReader lr;
  lr.parseString(testString);
  sol::protected_function func = lr.solState()["func"];
  axom::primal::Vector3D vec {1, 2, 3};
  auto result = checkedCall<double>(func, vec);
  EXPECT_FLOAT_EQ(vec.norm(), result);
}

TEST(inlet_function, lua_usertype_basic_squared_norm)
{
  std::string testString = "function func(vec) return vec:squared_norm() end";
  LuaReader lr;
  lr.parseString(testString);
  sol::protected_function func = lr.solState()["func"];
  axom::primal::Vector3D vec {1, 2, 3};
  auto result = checkedCall<double>(func, vec);
  EXPECT_FLOAT_EQ(vec.squared_norm(), result);
}

TEST(inlet_function, lua_usertype_basic_unit_vec)
{
  std::string testString = "function func(vec) return vec:unitVector() end";
  LuaReader lr;
  lr.parseString(testString);
  sol::protected_function func = lr.solState()["func"];
  axom::primal::Vector3D vec {1, 2, 3};
  auto result = checkedCall<axom::primal::Vector3D>(func, vec);
  EXPECT_EQ(vec.unitVector(), result);
}

TEST(inlet_function, lua_usertype_basic_dot)
{
  std::string testString =
    "function func(vec1, vec2) return vec1:dot(vec2) end";
  LuaReader lr;
  lr.parseString(testString);
  sol::protected_function func = lr.solState()["func"];
  axom::primal::Vector3D vec1 {1, 2, 3};
  axom::primal::Vector3D vec2 {4, 5, 6};
  auto result = checkedCall<double>(func, vec1, vec2);
  EXPECT_EQ(vec1.dot(vec2), result);
}

TEST(inlet_function, lua_usertype_basic_cross)
{
  std::string testString =
    "function func(vec1, vec2) return vec1:cross(vec2) end";
  LuaReader lr;
  lr.parseString(testString);
  sol::protected_function func = lr.solState()["func"];
  axom::primal::Vector3D vec1 {1, 2, 3};
  axom::primal::Vector3D vec2 {4, 5, 6};
  auto result = checkedCall<axom::primal::Vector3D>(func, vec1, vec2);
  EXPECT_EQ(axom::primal::Vector3D::cross_product(vec1, vec2), result);
}

//------------------------------------------------------------------------------
#include "axom/slic/core/UnitTestLogger.hpp"
using axom::slic::UnitTestLogger;

int main(int argc, char* argv[])
{
  int result = 0;

  ::testing::InitGoogleTest(&argc, argv);

  UnitTestLogger logger;  // create & initialize test logger,

  // finalized when exiting main scope

  result = RUN_ALL_TESTS();

  return result;
}
