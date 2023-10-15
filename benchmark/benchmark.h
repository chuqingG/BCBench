#pragma once

#include "../tree/btree_db2.cpp"
#include "../tree/btree_myisam.cpp"
#include "../tree/btree_pkb.cpp"
#include "../tree/btree_std.cpp"
#include "../tree/btree_wt.cpp"
#include "../utils/util.h"
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

struct TreeStatistics {
  double avgNodeSize = 0;
  int numNodes = 0;
  unsigned long totalKeySize = 0;
  int totalPrefixSize = 0;
  int numKeys = 0;
  int nonLeafNodes = 0;
  int totalBranching = 0;
  int height = 0;
};

// Changing key word to char...
class Benchmark {
public:
  virtual ~Benchmark() = default;
  virtual void InitializeStructure() = 0;
  virtual void DeleteStructure() = 0;
  virtual void Insert(const std::vector<char *> &numbers) = 0;
  virtual bool Search(const std::vector<char *> &numbers) = 0;
  virtual bool SearchRange(std::vector<char *> &sorted_values,
                           std::vector<int> &minIdxs) = 0;
  virtual TreeStatistics CalcStatistics() = 0;
};

class BPTreeStdBenchmark : public Benchmark {
public:
  ~BPTreeStdBenchmark() override { delete _tree; }

  virtual void InitializeStructure() override { _tree = new BPTree(); }

  void DeleteStructure() override {
    delete _tree;
    _tree = nullptr;
  }

  void Insert(const vector<char *> &values) override {
    for (uint32_t i = 0; i < values.size(); i++) {
      _tree->insert(values[i]);
    }
  }

  bool Search(const std::vector<char *> &values) override {
    int count = 0;
    for (uint32_t i = 0; i < values.size(); i++)
      if (_tree->search(values.at(i)) == -1) {
        return false;
      }
    return true;
  }

  bool SearchRange(std::vector<char *> &sorted_values,
                   std::vector<int> &minIdxs) override {
    int range_size = sorted_values.size() * RANGE_SCOPE;

    for (uint32_t i = 0; i < minIdxs.size(); ++i) {
      char *min = sorted_values.at(minIdxs[i]);
      char *max = sorted_values.at(minIdxs[i] + range_size);
      int entries = _tree->searchRange(min, max);
    }
    return true;
  }

  TreeStatistics CalcStatistics() override {
    TreeStatistics statistics;
    statistics.height = _tree->getHeight(_tree->getRoot());
    _tree->getSize(_tree->getRoot(), statistics.numNodes,
                   statistics.nonLeafNodes, statistics.numKeys,
                   statistics.totalBranching, statistics.totalKeySize,
                   statistics.totalPrefixSize);
    return statistics;
  }

protected:
  BPTree *_tree;
};

class BPTreeHeadCompBenchmark : public BPTreeStdBenchmark {
public:
  void InitializeStructure() override { _tree = new BPTree(true, false); }

  bool SearchRange(std::vector<char *> &sorted_values,
                   std::vector<int> &minIdxs) override {
    int range_size = sorted_values.size() * RANGE_SCOPE;

    for (uint32_t i = 0; i < minIdxs.size(); ++i) {
      char *min = sorted_values.at(minIdxs[i]);
      char *max = sorted_values.at(minIdxs[i] + range_size);
      int entries = _tree->searchRangeHead(min, max);
    }
    return true;
  }
};

class BPTreeTailCompBenchmark : public BPTreeStdBenchmark {
public:
  void InitializeStructure() override { _tree = new BPTree(false, true); }
};

class BPTreeHeadTailCompBenchmark : public BPTreeStdBenchmark {
public:
  void InitializeStructure() override { _tree = new BPTree(true, true); }

  bool SearchRange(std::vector<char *> &sorted_values,
                   std::vector<int> &minIdxs) override {
    int range_size = sorted_values.size() * RANGE_SCOPE;

    for (uint32_t i = 0; i < minIdxs.size(); ++i) {
      char *min = sorted_values.at(minIdxs[i]);
      char *max = sorted_values.at(minIdxs[i] + range_size);
      int entries = _tree->searchRangeHead(min, max);
    }
    return true;
  }
};

class BPTreeDB2Benchmark : public Benchmark {
public:
  ~BPTreeDB2Benchmark() override { delete _tree; }

  void InitializeStructure() override { _tree = new BPTreeDB2(); }

  void DeleteStructure() override {
    delete _tree;
    _tree = nullptr;
  }

  void Insert(const std::vector<char *> &values) override {
    for (uint32_t i = 0; i < values.size(); ++i) {
      _tree->insert(values.at(i));
    }
  }
  bool Search(const std::vector<char *> &values) override {
    int count = 0;
    for (uint32_t i = 0; i < values.size(); ++i)
      if (_tree->search(values.at(i)) == -1)
        return false;
    return true;
  }

  bool SearchRange(std::vector<char *> &sorted_values,
                   std::vector<int> &minIdxs) override {
    int range_size = sorted_values.size() * RANGE_SCOPE;

    for (uint32_t i = 0; i < minIdxs.size(); ++i) {
      char *min = sorted_values.at(minIdxs[i]);
      char *max = sorted_values.at(minIdxs[i] + range_size);
      int entries = _tree->searchRange(min, max);
    }
    return true;
  }

  TreeStatistics CalcStatistics() override {
    TreeStatistics statistics;
    statistics.height = _tree->getHeight(_tree->getRoot());
    _tree->getSize(_tree->getRoot(), statistics.numNodes,
                   statistics.nonLeafNodes, statistics.numKeys,
                   statistics.totalBranching, statistics.totalKeySize,
                   statistics.totalPrefixSize);
    return statistics;
  }

private:
  BPTreeDB2 *_tree;
};

class BPTreeMyISAMBenchmark : public Benchmark {
public:
  ~BPTreeMyISAMBenchmark() override { delete _tree; }

  void InitializeStructure() override { _tree = new BPTreeMyISAM(true); }

  void DeleteStructure() override {
    delete _tree;
    _tree = nullptr;
  }

  void Insert(const vector<char *> &values) override {
    for (uint32_t i = 0; i < values.size(); ++i) {
      _tree->insert(values.at(i));
    }
  }

  bool Search(const std::vector<char *> &values) override {
    for (uint32_t i = 0; i < values.size(); ++i)
      if (_tree->search(values[i]) == -1) {
        return false;
      }
    return true;
  }

  bool SearchRange(std::vector<char *> &sorted_values,
                   std::vector<int> &minIdxs) override {
    int range_size = sorted_values.size() * RANGE_SCOPE;

    for (uint32_t i = 0; i < minIdxs.size(); ++i) {
      char *min = sorted_values.at(minIdxs[i]);
      char *max = sorted_values.at(minIdxs[i] + range_size);
      int entries = _tree->searchRange(min, max);
    }
    return true;
  }

  TreeStatistics CalcStatistics() override {
    TreeStatistics statistics;
    statistics.height = _tree->getHeight(_tree->getRoot());
    _tree->getSize(_tree->getRoot(), statistics.numNodes,
                   statistics.nonLeafNodes, statistics.numKeys,
                   statistics.totalBranching, statistics.totalKeySize,
                   statistics.totalPrefixSize);
    return statistics;
  }

private:
  BPTreeMyISAM *_tree;
};

class BPTreeWTBenchmark : public Benchmark {
public:
  ~BPTreeWTBenchmark() override { delete _tree; }

  void InitializeStructure() override { _tree = new BPTreeWT(true, true); }

  void DeleteStructure() override {
    delete _tree;
    _tree = nullptr;
  }

  void Insert(const vector<char *> &values) override {
    for (uint32_t i = 0; i < values.size(); ++i) {
      _tree->insert(values[i]);
    }
  }

  bool Search(const std::vector<char *> &values) override {
    for (uint32_t i = 0; i < values.size(); ++i)
      if (_tree->search(values[i]) == -1)
        return false;
    return true;
  }

  bool SearchRange(std::vector<char *> &sorted_values,
                   std::vector<int> &minIdxs) override {
    int range_size = sorted_values.size() * RANGE_SCOPE;

    for (uint32_t i = 0; i < minIdxs.size(); ++i) {
      char *min = sorted_values.at(minIdxs[i]);
      char *max = sorted_values.at(minIdxs[i] + range_size);
      int entries = _tree->searchRange(min, max);
    }
    return true;
  }

  TreeStatistics CalcStatistics() override {
    TreeStatistics statistics;
    statistics.height = _tree->getHeight(_tree->getRoot());
    _tree->getSize(_tree->getRoot(), statistics.numNodes,
                   statistics.nonLeafNodes, statistics.numKeys,
                   statistics.totalBranching, statistics.totalKeySize,
                   statistics.totalPrefixSize);
    return statistics;
  }

private:
  BPTreeWT *_tree;
};

class BPTreePkBBenchmark : public Benchmark {
public:
  ~BPTreePkBBenchmark() override { delete _tree; }

  void InitializeStructure() override { _tree = new BPTreePkB(); }

  void DeleteStructure() override {
    delete _tree;
    _tree = nullptr;
  }

  void Insert(const vector<char *> &values) override {
    for (uint32_t i = 0; i < values.size(); ++i) {
      _tree->insert(values.at(i));
    }
  }

  bool Search(const std::vector<char *> &values) override {
    int count = 0;
    for (uint32_t i = 0; i < values.size(); ++i)
      if (_tree->search(values.at(i)) == -1)
        count++;
    cout << "count:" << count << endl;
    return true;
  }

  bool SearchRange(std::vector<char *> &sorted_values,
                   std::vector<int> &minIdxs) override {
    int range_size = sorted_values.size() * RANGE_SCOPE;

    for (uint32_t i = 0; i < minIdxs.size(); ++i) {
      char *min = sorted_values.at(minIdxs[i]);
      char *max = sorted_values.at(minIdxs[i] + range_size);
      int entries = _tree->searchRange(min, max);
      // }
    }
    return true;
  }

  TreeStatistics CalcStatistics() override {
    TreeStatistics statistics;
    statistics.height = _tree->getHeight(_tree->getRoot());
    _tree->getSize(_tree->getRoot(), statistics.numNodes,
                   statistics.nonLeafNodes, statistics.numKeys,
                   statistics.totalBranching, statistics.totalKeySize,
                   statistics.totalPrefixSize);
    return statistics;
  }

private:
  BPTreePkB *_tree;
};