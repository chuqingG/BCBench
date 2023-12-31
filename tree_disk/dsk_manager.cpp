#pragma once
#include <stdio.h>
#include <iostream>
#include "node_disk.h"

class DskManager {
public:
    FILE *fp;
    bool isopen;
    uint32_t page_count;
    char *path;
    uint32_t key_top;
    DskManager(const char *filename) {
        page_count = 0;
        key_top = 0;
        // create the file
        fp = fopen(filename, "w+");
        // fclose(fp);
        path = new char[strlen(filename) + 1];
        strcpy(path, filename);
    }
    ~DskManager() {
        cout << "total leaf: " << unsigned(page_count) << ", key space: " << unsigned(key_top) << endl;
        fclose(fp);
        delete[] path;
    }

    Node *get_new_leaf() {
        Node *n = new Node();
        n->id = page_count++;
        return n;
    }

    NodeDB2 *get_new_leaf_db2() {
        NodeDB2 *n = new NodeDB2();
        n->id = page_count++;
        return n;
    }

    NodeMyISAM *get_new_leaf_myisam() {
        NodeMyISAM *n = new NodeMyISAM();
        n->id = page_count++;
        return n;
    }

    NodeWT *get_new_leaf_wt() {
        NodeWT *n = new NodeWT();
        n->id = page_count++;
        return n;
    }

    // NodePkB *get_new_leaf_pkb() {
    //     NodePkB *n = new NodePkB();
    //     n->id = page_count++;
    //     return n;
    // }
};