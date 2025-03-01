/*
  ==============================================================================

    test.cpp
    Created: 12 Jan 2025 10:49:12pm
    Author:  Illia

  ==============================================================================
*/

// #include "test.h"

#define CATCH_CONFIG_MAIN
#include "catch.h"
#include "SparseMatrix.h";

TEST_CASE("Using builder", "[builder]") {
  SparseMatrixOfComplexNumbersBuilder builder{2, 2};

  auto el1 = std::complex<double>(1.0, 0.0);
  auto el2 = std::complex<double>(2.0, 0.0);
  auto el3 = std::complex<double>(3.0, 0.0);
  auto el4 = std::complex<double>(4.0, 0.0);

  builder.addRow();
  builder.addOrSkipEntry(0, el1);
  builder.addOrSkipEntry(1, el2);

  builder.addRow();
  builder.addOrSkipEntry(0, el3);
  builder.addOrSkipEntry(1, el4);

  SparseMatrixOfComplexNumbers matrix = builder.getResult();

  REQUIRE(matrix.size() == 4);

  REQUIRE(matrix.getEntry(0, 0).value().get() == el1);
  REQUIRE(matrix.getEntry(0, 1).value().get() == el2);
  REQUIRE(matrix.getEntry(1, 0).value().get() == el3);
  REQUIRE(matrix.getEntry(1, 1).value().get() == el4);

  auto it = matrix.rowBegin(0);
  auto endIt = matrix.rowEnd(0);

  REQUIRE(it != endIt);
  REQUIRE(it->value == el1);

  it++;
  REQUIRE(it != endIt);
  REQUIRE(it->value == el2);

  it++;
  REQUIRE(it == endIt);

  it = matrix.rowBegin(1);
  endIt = matrix.rowEnd(1);

  REQUIRE(it != endIt);
  REQUIRE(it->value == el3);

  it++;
  REQUIRE(it != endIt);
  REQUIRE(it->value == el4);

  it++;
  REQUIRE(it == endIt);
}

TEST_CASE("Flip columns", "[util]") {
  SparseMatrixOfComplexNumbersBuilder builder{2, 2};

  auto el1 = std::complex<double>(1.0, 0.0);
  auto el2 = std::complex<double>(2.0, 0.0);
  auto el3 = std::complex<double>(3.0, 0.0);
  auto el4 = std::complex<double>(4.0, 0.0);

  builder.addRow();
  builder.addOrSkipEntry(0, el1);
  builder.addOrSkipEntry(1, el2);

  builder.addRow();
  builder.addOrSkipEntry(0, el3);
  builder.addOrSkipEntry(1, el4);

  SparseMatrixOfComplexNumbers matrix = builder.getResult();
  matrix.flipColumns();

  REQUIRE(matrix.getEntry(0, 0).value().get() == el2);
  REQUIRE(matrix.getEntry(0, 1).value().get() == el1);
  REQUIRE(matrix.getEntry(1, 0).value().get() == el4);
  REQUIRE(matrix.getEntry(1, 1).value().get() == el3);
}