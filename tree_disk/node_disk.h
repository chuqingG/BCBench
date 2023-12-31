#pragma once
#include <iostream>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <string.h>
#include <memory>
#include <cstring>
#include "../utils/config.h"
#include "../utils/item.hpp"

using namespace std;

int MAX_NODE_SIZE = 4;

// Key represented as <key, {rid list}>
// str representation of rids for easy comparison and prefix compression
// if other approaches are used

// BP-std node

const char MAXHIGHKEY[] = "infinity";

struct Stdhead {
    uint16_t key_offset;
    uint16_t key_len;
} __attribute__((packed));

class Node {
public:
    bool IS_LEAF;
    int size;
    uint32_t id;
    char *base;
    uint16_t space_top;
    vector<Node *> ptrs;
    uint16_t ptr_cnt;

    Item *lowkey;
    Item *highkey;
    Item *prefix;
    Node *prev; // Prev node pointer
    Node *next; // Next node pointer
    Node();
    ~Node();
    void fetch_page(FILE *fp);
    void write_page(FILE *fp);
    void delete_from_mem();
};

struct DB2head {
    uint16_t key_offset;
    uint16_t key_len;
} __attribute__((packed));

struct DB2pfxhead {
    uint16_t pfx_offset;
    uint16_t pfx_len;
    uint16_t low;
    uint16_t high;
} __attribute__((packed));

class NodeDB2 {
public:
    bool IS_LEAF;
    int size;
    uint32_t id;
    int pfx_size;
    char *base;
    // char *pfxbase;
    uint16_t space_top;
    uint16_t pfx_top;
    vector<NodeDB2 *> ptrs;
    uint16_t ptr_cnt;
    NodeDB2 *prev; // Prev node pointer
    NodeDB2 *next; // Next node pointer
    NodeDB2();
    ~NodeDB2();
    void fetch_page(FILE *fp);
    void write_page(FILE *fp);
    void delete_from_mem();
};

// Key with prefix and suffix encoding
// Duplicates represented as <key, {rid list}>

struct MyISAMhead {
    uint16_t key_offset;
    uint16_t key_len;
    uint16_t pfx_len;
    /* We assume all the prefix are less than 128 Byte, so remove is1Byte in MyISAM
     *  May need to consider the case key/pfx_len in [128, 256)
     */
    // bool is1B;
} __attribute__((packed));

class NodeMyISAM {
public:
    bool IS_LEAF;
    int size;
    char *base;
    uint16_t space_top;
    uint32_t id;

    vector<NodeMyISAM *> ptrs;
    NodeMyISAM *prev; // Prev node pointer
    NodeMyISAM *next; // Next node pointer
    uint16_t ptr_cnt;
    NodeMyISAM();
    ~NodeMyISAM();
    void fetch_page(FILE *fp);
    void write_page(FILE *fp);
    void delete_from_mem();
};

// with no duplicate key
#ifdef WTCACHE
struct WThead {
    uint16_t key_offset;
    uint16_t initval_offset;
    uint16_t key_len;
    uint16_t init_len = 0;
    uint16_t pfx_len;
    bool initialized = false;
} __attribute__((packed));
#else
struct WThead {
    uint16_t key_offset;
    uint16_t key_len;
    uint16_t pfx_len;
    bool isinitialized = false;
} __attribute__((packed));
#endif

class NodeWT {
public:
    bool IS_LEAF;
    int size; // Total key number
    char *base;
    uint16_t space_top;
    uint32_t id;

    uint8_t prefixstart; /* Best page prefix starting slot */
    uint8_t prefixstop;  /* Maximum slot to which the best page prefix applies */

    vector<NodeWT *> ptrs;
    NodeWT *prev; // Prev node pointer
    NodeWT *next; // Next node pointer
    uint16_t ptr_cnt;
    NodeWT();
    ~NodeWT();
    void fetch_page(FILE *fp);
    void write_page(FILE *fp);
    void delete_from_mem();
};

#ifdef DUPKEY
// Duplicates represented as <key, {rid list}>
class KeyPkB {
public:
    int16_t offset;
    char partialKey[PKB_LEN + 1];
    char *original;
    int pkLength;
    vector<string> ridList;
    KeyPkB(int offset, string value, char *ptr, int rid);
    KeyPkB(int offset, string value, char *ptr, vector<string> ridList);
    void addRecord(int rid);
    int getSize();
    void updateOffset(int offset);
};

class NodePkB {
public:
    bool IS_LEAF;
    vector<KeyPkB> keys;
    int size;
    vector<NodePkB *> ptrs;
    NodePkB *prev; // Prev node pointer
    NodePkB *next; // Next node pointer
    NodePkB();
    ~NodePkB();
};

#else
struct PkBhead {
    uint16_t pfx_len;
    uint16_t key_len;
    uint8_t pk_len;
    char pk[PKB_LEN + 1];
    uint32_t key_offset;
} __attribute__((packed));

class NodePkB {
public:
    bool IS_LEAF;
    int size;
    char *base;
    // uint16_t space_top;
    // uint32_t id;

    vector<NodePkB *> ptrs;
    uint16_t ptr_cnt;
    NodePkB *prev; // Prev node pointer
    NodePkB *next; // Next node pointer
    NodePkB();
    ~NodePkB();
    // void fetch_page(FILE *fp);
    // void write_page(FILE *fp);
    // void delete_from_mem();
};
#endif

struct uncompressedKey { // for pkb
    string key;
    char *keyptr;
};

struct splitReturn {
    string promotekey;
    Node *left;
    Node *right;
};

struct splitReturn_new {
    Item promotekey;
    Node *left;
    Node *right;
};

struct splitReturnDB2 {
    Item promotekey;
    NodeDB2 *left;
    NodeDB2 *right;
};

struct splitReturnMyISAM {
    Item promotekey;
    NodeMyISAM *left;
    NodeMyISAM *right;
};

struct splitReturnWT {
    Item promotekey;
    NodeWT *left;
    NodeWT *right;
};

struct splitReturnPkB {
    Item promotekey;
    NodePkB *left;
    NodePkB *right;
};

struct nodeBounds {
    string lowerbound;
    string upperbound;
};

void printKeys(Node *node, bool compressed);
void printKeys_db2(NodeDB2 *node, bool compressed);
void printKeys_myisam(NodeMyISAM *node, bool compressed);
void printKeys_wt(NodeWT *node, bool compressed);
void printKeys_pkb(NodePkB *node, bool compressed, FILE *fp);
