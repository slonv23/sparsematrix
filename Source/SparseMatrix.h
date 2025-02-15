/*
  ==============================================================================

    SparseVector.h
    Created: 29 Jun 2024 1:57:30pm
    Author:  Illia

  ==============================================================================
*/

#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <complex>
#include <exception>
#include <optional>
#include <iterator>

struct Entry {
  unsigned colIndex;
  std::complex<double> value;

  Entry(unsigned col, std::complex<double> val) : colIndex(col), value(val) {}
};

class SparseMatrixOfComplexNumbersBuilder;

// uses Compressed sparse row format
class SparseMatrixOfComplexNumbers {
  friend class SparseMatrixOfComplexNumbersBuilder;

public:
  unsigned size() { return entries.size(); }
  double getTolerance() { return tolerance; }
  bool shouldAcceptValue(std::complex<double> value) { return std::abs(value) > tolerance; }

  std::optional<std::reference_wrapper<std::complex<double>>> getEntry(unsigned row, unsigned col) {
    if (row >= rowsCount) {
      throw std::invalid_argument("Invalid row");
    }
    if (col >= colsCount) {
      throw std::invalid_argument("Invalid col");
    }

    auto rowStartEntryIndex = rowIndexes[row];
    auto rowEndEntryIndex = rowIndexes[row + 1];

    // todo we can use binary search here
    for (auto i = rowStartEntryIndex; i < rowEndEntryIndex; i++) {
      auto &entry = entries[i];
      if (entry.colIndex == col) {
        return entry.value;
      }
    }

    return std::nullopt;
  }

  std::vector<Entry>::iterator rowBegin(unsigned row) {
    if (row >= rowsCount) {
      throw std::invalid_argument("Invalid row");
    }
    auto rowStartEntryIndex = rowIndexes[row];

    return entries.begin() + rowStartEntryIndex;
  }

  std::vector<Entry>::iterator rowEnd(unsigned row) {
    if (row >= rowsCount) {
      throw std::invalid_argument("Invalid row");
    }
    auto rowEndEntryIndex = rowIndexes[row + 1];

    return entries.begin() + rowEndEntryIndex;
  }

  unsigned getRowsCount() { return rowsCount; }

  unsigned getColsCount() { return colsCount; }

private:
  unsigned rowsCount;
  unsigned colsCount;
  unsigned elementsCount;

  std::vector<Entry> entries;
  std::vector<unsigned> rowIndexes;

  double tolerance;

  // needed for the builder
  SparseMatrixOfComplexNumbers(unsigned _rowsCount, unsigned _colsCount, double _tolerance = 1e-9)
      : tolerance(_tolerance), rowsCount(_rowsCount), colsCount(_colsCount), elementsCount(_rowsCount * _colsCount) {
    rowIndexes.reserve(_rowsCount + 1);
  }
};

// an instance of the builder class can be used only once
class SparseMatrixOfComplexNumbersBuilder {
private:
  SparseMatrixOfComplexNumbers matrix;

  void validateColIndex(unsigned colIndex) {
    if (!matrix.entries.empty()) {
      auto currentRowIndex = matrix.rowIndexes.back();
      if (currentRowIndex == matrix.entries.size()) {
        // row just started
        return;
      }
      auto &prevEntry = matrix.entries.back();
      if (colIndex <= prevEntry.colIndex) {
        throw std::invalid_argument("Previous inserted column index is greater or equal to the passed colIndex");
      }
    }
  }

public:
  SparseMatrixOfComplexNumbersBuilder(unsigned rowsCount, unsigned colsCount, double tolerance = 1e-9)
      : matrix(rowsCount, colsCount, tolerance) {}

  void startNextRow() {
    if (matrix.rowIndexes.size() >= matrix.rowsCount) {
      throw std::logic_error("Cannot start a new row after the last row. All rows are inserted.");
    }

    // add an index of the next element in the list of all entries where the new row will start
    matrix.rowIndexes.push_back(matrix.entries.size());
  }

  void addOrSkipNextEntry(unsigned colIndex, std::complex<double> value) {
    validateColIndex(colIndex);

    if (matrix.shouldAcceptValue(value)) {
      matrix.entries.emplace_back(colIndex, value);
    }
  }

  SparseMatrixOfComplexNumbers getResult() {
    auto expectedRowIndexesSize = matrix.rowsCount + 1;
    while (matrix.rowIndexes.size() != expectedRowIndexesSize) {
      matrix.rowIndexes.push_back(matrix.entries.size());
    }

    return std::move(matrix);
  }
};
