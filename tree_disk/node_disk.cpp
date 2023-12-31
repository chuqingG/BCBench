#pragma once
#include "node_disk.h"
#include "node_disk_inline.h"

// Constructor of Node
Node::Node() {
    id = 0;
    size = 0;
    ptr_cnt = 0;
    space_top = 0;
    base = NewPage();
    SetEmptyPage(base);

    lowkey = new Item(true);
    highkey = new Item(); // this hk will be deallocated automatically
    highkey->addr = new char[9];
    strcpy(highkey->addr, MAXHIGHKEY);
    highkey->newallocated = true;
    highkey->size = 0;
    prefix = new Item(true);
    IS_LEAF = true;
    prev = nullptr;
    next = nullptr;
}

// Destructor of Node
Node::~Node() {
    delete lowkey;
    delete highkey;
    delete prefix;
    if (!IS_LEAF)
        delete[] base;
}

void Node::fetch_page(FILE *fp) {
    base = NewPage();
    SetEmptyPage(base);
    fseek(fp, id * sizeof(char) * MAX_SIZE_IN_BYTES, SEEK_SET);
    fread(base, sizeof(char) * MAX_SIZE_IN_BYTES, 1, fp);
}

void Node::write_page(FILE *fp) {
    fseek(fp, id * sizeof(char) * MAX_SIZE_IN_BYTES, SEEK_SET);
    fwrite(base, sizeof(char) * MAX_SIZE_IN_BYTES, 1, fp);
    delete[] base;
}

void Node::delete_from_mem() {
    delete[] base;
}

//===============Below for DB2===========

NodeDB2::NodeDB2() {
    size = 0;
    pfx_size = 0;
    prev = nullptr;
    next = nullptr;
    ptr_cnt = 0;
    base = NewPageDB2();
    SetEmptyPageDB2(base);
    IS_LEAF = true;
    space_top = 0;
    pfx_top = 0;

    ptr_cnt = 0;
    InsertPfxDB2(this, 0, "", 0, 0, 0);
}

// Destructor of NodeDB2
NodeDB2::~NodeDB2() {
    if (!IS_LEAF)
        delete[] base;
}

void NodeDB2::fetch_page(FILE *fp) {
    base = NewPageDB2();
    SetEmptyPageDB2(base);
    FSeekLongDB2(id, fp);
    // fseek(fp, id * (MAX_SIZE_IN_BYTES + DB2_PFX_MAX_SIZE), SEEK_SET);
    fread(base, (MAX_SIZE_IN_BYTES + DB2_PFX_MAX_SIZE), 1, fp);
}

void NodeDB2::write_page(FILE *fp) {
    FSeekLongDB2(id, fp);
    // fseek(fp, id * (MAX_SIZE_IN_BYTES + DB2_PFX_MAX_SIZE), SEEK_SET);
    fwrite(base, (MAX_SIZE_IN_BYTES + DB2_PFX_MAX_SIZE), 1, fp);
    delete[] base;
}

void NodeDB2::delete_from_mem() {
    delete[] base;
}
/*
==================For WiredTiger================
*/

NodeWT::NodeWT() {
    size = 0;
    prev = nullptr;
    next = nullptr;
    ptr_cnt = 0;

    prefixstart = 0;
    prefixstop = 0;

    base = NewPage();
    SetEmptyPage(base);
    space_top = 0;
    IS_LEAF = true;
}

// Destructor of NodeWT
NodeWT::~NodeWT() {
    if (!IS_LEAF)
        delete[] base;
}

void NodeWT::fetch_page(FILE *fp) {
    base = NewPage();
    SetEmptyPage(base);
    fseek(fp, id * sizeof(char) * MAX_SIZE_IN_BYTES, SEEK_SET);
    fread(base, sizeof(char) * MAX_SIZE_IN_BYTES, 1, fp);
}

void NodeWT::write_page(FILE *fp) {
    fseek(fp, id * sizeof(char) * MAX_SIZE_IN_BYTES, SEEK_SET);
    fwrite(base, sizeof(char) * MAX_SIZE_IN_BYTES, 1, fp);
    delete[] base;
}

void NodeWT::delete_from_mem() {
    delete[] base;
}

NodeMyISAM::NodeMyISAM() {
    size = 0;
    prev = nullptr;
    next = nullptr;
    ptr_cnt = 0;
    base = NewPage();
    SetEmptyPage(base);
    space_top = 0;
    IS_LEAF = true;
    id = 0;
}

// Destructor of NodeMyISAM
NodeMyISAM::~NodeMyISAM() {
    if (!IS_LEAF)
        delete[] base;
}

void NodeMyISAM::fetch_page(FILE *fp) {
    base = NewPage();
    SetEmptyPage(base);
    fseek(fp, id * sizeof(char) * MAX_SIZE_IN_BYTES, SEEK_SET);
    fread(base, sizeof(char) * MAX_SIZE_IN_BYTES, 1, fp);
}

void NodeMyISAM::write_page(FILE *fp) {
    fseek(fp, id * sizeof(char) * MAX_SIZE_IN_BYTES, SEEK_SET);
    fwrite(base, sizeof(char) * MAX_SIZE_IN_BYTES, 1, fp);
    delete[] base;
}

void NodeMyISAM::delete_from_mem() {
    delete[] base;
}

NodePkB::NodePkB() {
    size = 0;
    prev = nullptr;
    next = nullptr;
    ptr_cnt = 0;
    base = NewPage();
    SetEmptyPage(base);
    // space_top = 0;
    IS_LEAF = true;
}

// Destructor of NodePkB
NodePkB::~NodePkB() {
    delete[] base;
}

// void NodePkB::fetch_page(FILE *fp) {
//     base = NewPage();
//     SetEmptyPage(base);
//     fseek(fp, id * sizeof(char) * MAX_SIZE_IN_BYTES, SEEK_SET);
//     fread(base, sizeof(char) * MAX_SIZE_IN_BYTES, 1, fp);
// }

// void NodePkB::write_page(FILE *fp) {
//     fseek(fp, id * sizeof(char) * MAX_SIZE_IN_BYTES, SEEK_SET);
//     fwrite(base, sizeof(char) * MAX_SIZE_IN_BYTES, 1, fp);
//     delete[] base;
// }

// void NodePkB::delete_from_mem() {
//     delete[] base;
// }

void printKeys(Node *node, bool compressed) {
    if (compressed && node->prefix->addr)
        cout << node->prefix->addr << ": ";
    for (int i = 0; i < node->size; i++) {
        Stdhead *head = GetHeaderStd(node, i);
        if (compressed && node->prefix->addr) {
            cout << PageOffset(node, head->key_offset) << ",";
        }
        else {
            cout << node->prefix->addr << PageOffset(node, head->key_offset) << ",";
        }
    }
}

void printKeys_db2(NodeDB2 *node, bool compressed) {
    for (int i = 0; i < node->pfx_size; i++) {
        DB2pfxhead *pfx = GetHeaderDB2pfx(node, i);
        if (compressed) {
            cout << "Prefix " << PfxOffset(node, pfx->pfx_offset) << ": ";
        }
        for (int l = pfx->low; l <= pfx->high; l++) {
            // Loop through rid list to print duplicates
            DB2head *head = GetHeaderDB2(node, l);
            if (compressed) {
                cout << PageOffset(node, head->key_offset) << ",";
            }
            else {
                cout << PfxOffset(node, pfx->pfx_offset) << PageOffset(node, head->key_offset) << ",";
            }
        }
    }
}

void printKeys_myisam(NodeMyISAM *node, bool compressed) {
    char *prev_key;
    char *curr_key;
    for (int i = 0; i < node->size; i++) {
        MyISAMhead *head = GetHeaderMyISAM(node, i);
        if (compressed || head->pfx_len == 0 || i == 0) {
            curr_key = PageOffset(node, head->key_offset);
            cout << unsigned(head->pfx_len) << ":" << curr_key << ",";
        }
        else {
            curr_key = new char[head->pfx_len + head->key_len + 1];
            strncpy(curr_key, prev_key, head->pfx_len);
            strcpy(curr_key + head->pfx_len, PageOffset(node, head->key_offset));
            cout << curr_key << ",";
        }
        prev_key = curr_key;
    }
}

void printKeys_wt(NodeWT *node, bool compressed) {
    char *prev_key;
    char *curr_key;
    for (int i = 0; i < node->size; i++) {
        WThead *head = GetHeaderWT(node, i);
        if (compressed || head->pfx_len == 0 || i == 0) {
            curr_key = PageOffset(node, head->key_offset);
            cout << unsigned(head->pfx_len) << ":" << curr_key << ",";
        }
        else {
            curr_key = new char[head->pfx_len + head->key_len + 1];
            strncpy(curr_key, prev_key, head->pfx_len);
            strcpy(curr_key + head->pfx_len, PageOffset(node, head->key_offset));
            cout << curr_key << ",";
        }

        prev_key = curr_key;
    }
}

void printKeys_pkb(NodePkB *node, bool compressed, FILE *fp) {
    for (int i = 0; i < node->size; i++) {
        PkBhead *head = GetHeaderPkB(node, i);
        char *buf = new char[head->key_len + 1];
        // cout << "off:" << head->key_offset << " ";
        FSeekLongPos(head->key_offset, fp);
        fread(buf, head->key_len + 1, 1, fp);
        if (compressed) {
            cout << unsigned(head->pfx_len) << ":" << head->pk << "("
                 << buf << ")"
                 << ",";
        }
        else {
            cout << buf << ",";
        }
        delete[] buf;
    }
}