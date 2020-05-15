
/**
 * Copyright 2019 The Gamma Authors.
 *
 * This source code is licensed under the Apache License, Version 2.0 license
 * found in the LICENSE file in the root directory of this source tree.
 */

#ifndef VECTOR_MANAGER_H_
#define VECTOR_MANAGER_H_

#include <map>
#include <string>
#include "log.h"

#include "gamma_common_data.h"
#include "gamma_index.h"
#include "raw_vector.h"

namespace tig_gamma {

class VectorManager {
 public:
  VectorManager(const RetrievalModel &model,
                const VectorStorageType &store_type, const char *docids_bitmap,
                int max_doc_size, const std::string &root_path, GammaCounters *counters);
  ~VectorManager();

  int CreateVectorTable(Table &table);

  int AddToStore(int docid, std::vector<struct Field> &fields);
  int Update(int docid, std::vector<struct Field> &fields);

  int Indexing();

  int AddRTVecsToIndex();

  // int Add(int docid, const std::vector<Field *> &field_vecs);
  int Search(GammaQuery &query, GammaResult *results);

  int GetVector(const std::vector<std::pair<std::string, int>> &fields_ids,
                std::vector<std::string> &vec, bool is_bytearray = false);

  void GetTotalMemBytes(long &index_total_mem_bytes, long &vector_total_mem_bytes) {
    for (auto iter = vector_indexes_.begin(); iter != vector_indexes_.end();
         iter++) {
      index_total_mem_bytes += iter->second->GetTotalMemBytes();
    }

    for (auto iter = raw_vectors_.begin(); iter != raw_vectors_.end(); ++iter) {
      vector_total_mem_bytes += iter->second->GetTotalMemBytes();
    }
  }

  int Dump(const std::string &path, int dump_docid, int max_docid);
  int Load(const std::vector<std::string> &path, int doc_num);

  GammaIndex *GetVectorIndex(std::string &name) const {
    const auto &it = vector_indexes_.find(name);
    if (it == vector_indexes_.end()) {
      return nullptr;
    }
    return it->second;
  }

  void VectorNames(std::vector<std::string> &names) {
    for (const auto &it : vector_indexes_) {
      names.push_back(it.first);
    }
  }

  int Delete(int docid);

 private:
  void Close();  // release all resource

 private:
  RetrievalModel default_model_;
  VectorStorageType default_store_type_;
  const char *docids_bitmap_;
  int max_doc_size_;
  bool table_created_;
  RetrievalParams *retrieval_param_;
  std::string root_path_;
  GammaCounters *gamma_counters_;

  std::map<std::string, RawVector<float> *> raw_vectors_;
  std::map<std::string, RawVector<uint8_t> *> raw_binary_vectors_;
  std::map<std::string, GammaIndex *> vector_indexes_;
};

}  // namespace tig_gamma

#endif
