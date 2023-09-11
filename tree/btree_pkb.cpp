#include "btree_pkb.h"

// Initialise the BPTreePkB NodePkB
BPTreePkB::BPTreePkB() {
    _root = NULL;
    max_level = 1;
}

// Destructor of BPTreePkB tree
BPTreePkB::~BPTreePkB() {
    delete _root;
}

// Function to get the root NodePkB
NodePkB *BPTreePkB::getRoot() {
    return _root;
}

// Function to find any element
// in B+ Tree
int BPTreePkB::search(const char *key) {
    int keylen = strlen(key);

    int offset = 0;
    bool equal = false;
    NodePkB *leaf = search_leaf_node(_root, key, keylen, offset);
    if (leaf == nullptr)
        return -1;
    findNodeResult result = find_node(leaf, key, keylen, offset, equal);
    if (result.low == result.high)
        return result.low;
    else
        return -1;
}

void BPTreePkB::insert(char *key) {
    int keylen = strlen(key);
    // auto it = strPtrMap.find(x);
    if (_root == nullptr) {
        _root = new NodePkB;
        // int rid = rand();
        // _root->keys.push_back(KeyPkB(0, x, it->second, rid));
        InsertKeyPkB(_root, 0, key, keylen, 0);
        _root->IS_LEAF = true;
        // _root->size = 1;
        return;
    }
    // vector<NodePkB *> parents;
    int offset = 0;
    int path_level = 0;
    NodePkB *search_path[max_level];
    NodePkB *leaf = search_leaf_node_for_insert(_root, key, keylen,
                                                search_path, path_level, offset);
    insert_leaf(leaf, search_path, path_level, key, keylen, offset);
}

void BPTreePkB::insert_nonleaf(NodePkB *node, NodePkB **path, int parentlevel, splitReturnPkB *childsplit) {
    // int offset;
    WTitem basekey;
    get_base_key_from_ancestor(path, parentlevel, node, basekey);
    int offset = get_common_prefix_len(basekey.addr, childsplit->promotekey.addr,
                                       basekey.size, childsplit->promotekey.size);

    if (check_split_condition(node, childsplit->promotekey.size)) {
        NodePkB *parent = nullptr;
        if (parentlevel >= 0) {
            parent = path[parentlevel];
        }
        splitReturnPkB currsplit = split_nonleaf(node, path, parentlevel, childsplit, offset);
        if (node == _root) {
            NodePkB *newRoot = new NodePkB;
            // int rid = rand();
            // newRoot->keys.push_back(KeyPkB(0, currsplit.promotekey, currsplit.keyptr, rid));
            // newRoot->ptrs.push_back(currsplit.left);
            // newRoot->ptrs.push_back(currsplit.right);
            InsertNode(newRoot, 0, currsplit.left);
            InsertKeyPkB(newRoot, 0,
                         currsplit.promotekey.addr, currsplit.promotekey.size, 0);
            InsertNode(newRoot, 1, currsplit.right);

            newRoot->IS_LEAF = false;
            _root = newRoot;
            max_level++;
        }
        else {
            if (parent == nullptr)
                return;
            insert_nonleaf(parent, path, parentlevel - 1, &currsplit);
        }
    }
    else {
        WTitem *promotekey = &(childsplit->promotekey);
        // WTitem basekey;
        // get_base_key_from_ancestor(path, parentlevel, node, basekey);
        // int offset = get_common_prefix_len(basekey.addr, promotekey->addr,
        //                                    basekey.size, promotekey->size);
        // int insertpos;
        // int rid = rand();
        bool equal = false;
        findNodeResult result = find_node(node, promotekey->addr, promotekey->size,
                                          offset, equal);
        int insertpos = result.high;

        WTitem prevkey;
        generate_pkb_key(node, promotekey->addr, promotekey->size, insertpos,
                         path, parentlevel, prevkey);
        int pfx_len = get_common_prefix_len(prevkey.addr, promotekey->addr, prevkey.size, promotekey->size);
        InsertKeyPkB(node, insertpos, promotekey->addr, promotekey->size, pfx_len);

        update_next_prefix(node, insertpos, promotekey->addr, promotekey->size);

        InsertNode(node, insertpos + 1, childsplit->right);
        // vector<KeyPkB> allkeys;
        // for (int i = 0; i < insertpos; i++) {
        //     allkeys.push_back(node->keys.at(i));
        // }
        // KeyPkB pkbKey = generate_pkb_key(node, promotekey, childsplit.keyptr, insertpos, parents, pos, rid);
        // allkeys.push_back(pkbKey);
        //
        // for (int i = insertpos; i < node->size; i++) {
        //     allkeys.push_back(node->keys.at(i));
        // }

        // vector<NodePkB *> allptrs;
        // for (int i = 0; i < insertpos + 1; i++) {
        //     allptrs.push_back(node->ptrs.at(i));
        // }
        // allptrs.push_back(childsplit.right);
        // for (int i = insertpos + 1; i < node->size + 1; i++) {
        //     allptrs.push_back(node->ptrs.at(i));
        // }
        // node->keys = allkeys;
        // node->ptrs = allptrs;
        // node->size = node->size + 1;
        //
        // build_page_prefixes(node, insertpos); // Populate new prefixes
    }
}

void BPTreePkB::insert_leaf(NodePkB *leaf, NodePkB **path, int path_level,
                            char *key, int keylen, int offset) {
    if (check_split_condition(leaf, keylen)) {
        splitReturnPkB split = split_leaf(leaf, path, path_level, key, keylen, offset);
        if (leaf == _root) {
            NodePkB *newRoot = new NodePkB;
            // int rid = rand();
            // newRoot->keys.push_back(KeyPkB(0, split.promotekey, split.keyptr, rid));
            // newRoot->ptrs.push_back(split.left);
            // newRoot->ptrs.push_back(split.right);
            InsertNode(newRoot, 0, split.left);
            InsertKeyPkB(newRoot, 0,
                         split.promotekey.addr, split.promotekey.size, 0);
            InsertNode(newRoot, 1, split.right);

            newRoot->IS_LEAF = false;
            // newRoot->size = 1;
            _root = newRoot;
            max_level++;
        }
        else {
            NodePkB *parent = path[path_level - 1];
            insert_nonleaf(parent, path, path_level - 2, &split);
        }
    }
    else {
        // int insertpos;
        // int rid = rand();
        bool equal = false;
        findNodeResult result = find_node(leaf, key, keylen, offset, equal);
        int insertpos = result.high;

        WTitem prevkey;
        generate_pkb_key(leaf, key, keylen, insertpos,
                         path, path_level - 1, prevkey);
        int pfx_len = get_common_prefix_len(prevkey.addr, key, prevkey.size, keylen);
        InsertKeyPkB(leaf, insertpos, key, keylen, pfx_len);

        // vector<KeyPkB> allkeys;
        // if (equal) {
        //     allkeys = leaf->keys;
        //     allkeys.at(insertpos).addRecord(rid);
        // }
        // else {
        //     for (int i = 0; i < insertpos; i++) {
        //         allkeys.push_back(leaf->keys.at(i));
        //     }
        //     // KeyPkB pkbKey = generate_pkb_key(leaf, key, keyptr, insertpos, parents, parents.size() - 1, rid);
        //     allkeys.push_back(pkbKey);
        //     for (int i = insertpos; i < leaf->size; i++) {
        //         allkeys.push_back(leaf->keys.at(i));
        //     }
        // }
        // leaf->keys = allkeys;

        if (!equal) {
            // leaf->size = leaf->size + 1;
            // build_page_prefixes(leaf, insertpos); // Populate new prefixes
            update_next_prefix(leaf, insertpos, key, keylen);
        }
    }
}

int BPTreePkB::split_point(NodePkB *node) {
    // int size = allkeys.size();
    int bestsplit = node->size / 2;
    return bestsplit;
}

// uncompressedKey BPTreePkB::get_uncompressed_key_before_insert(NodePkB *node, int ind, int insertpos, string newkey, char *newkeyptr, bool equal) {
//     uncompressedKey uncompressed;
//     if (equal) {
//         uncompressed.key = get_key(node, ind);
//         uncompressed.keyptr = node->keys.at(ind).original;
//         return uncompressed;
//     }
//     if (insertpos == ind) {
//         uncompressed.key = newkey;
//         uncompressed.keyptr = newkeyptr;
//     }
//     else {
//         int origind = insertpos < ind ? ind - 1 : ind;
//         uncompressed.key = get_key(node, origind);
//         uncompressed.keyptr = node->keys.at(origind).original;
//     }
//     return uncompressed;
// }

splitReturnPkB BPTreePkB::split_nonleaf(NodePkB *node, NodePkB **path, int parentlevel,
                                        splitReturnPkB *childsplit, int offset) {
    splitReturnPkB newsplit;
    NodePkB *right = new NodePkB;
    // string promotekey = childsplit.promotekey;
    // char *promotekeyptr = childsplit.keyptr;
    WTitem *promotekey = &(childsplit->promotekey);
    // int insertpos;
    // int rid = rand();
    bool equal = false;
    findNodeResult result = find_node(node, promotekey->addr, promotekey->size,
                                      offset, equal);
    int insertpos = result.high;

    // Insert the new key
    WTitem prevkey;
    generate_pkb_key(node, promotekey->addr, promotekey->size, insertpos,
                     path, parentlevel, prevkey);
    int pfx_len = get_common_prefix_len(prevkey.addr, promotekey->addr, prevkey.size, promotekey->size);
    InsertKeyPkB(node, insertpos, promotekey->addr, promotekey->size, pfx_len);
    if (!equal) {
        update_next_prefix(node, insertpos, promotekey->addr, promotekey->size);
    }

    InsertNode(node, insertpos + 1, childsplit->right);
    // vector<KeyPkB> allkeys;
    // vector<NodePkB *> allptrs;
    //
    // for (int i = 0; i < insertpos; i++) {
    //     allkeys.push_back(node->keys.at(i));
    // }
    // KeyPkB pkbKey = generate_pkb_key(node, promotekey, promotekeyptr, insertpos, parents, pos, rid);
    // allkeys.push_back(pkbKey);
    //
    // for (int i = insertpos; i < node->size; i++) {
    //     allkeys.push_back(node->keys.at(i));
    // }

    // for (int i = 0; i < insertpos + 1; i++) {
    //     allptrs.push_back(node->ptrs.at(i));
    // }
    // allptrs.push_back(childsplit.right);
    // for (int i = insertpos + 1; i < node->size + 1; i++) {
    //     allptrs.push_back(node->ptrs.at(i));
    // }

    int split = split_point(node);

    // 1. get the full separator
    PkBhead *head_split = GetHeaderPkB(node, split);
    newsplit.promotekey.size = head_split->key_len;
    newsplit.promotekey.addr = new char[newsplit.promotekey.size + 1];
    strcpy(newsplit.promotekey.addr, PageOffset(node, head_split->key_offset));
    newsplit.promotekey.newallocated = true;

    // 2. fetch the full firstright(pos + 1), and compute pfx_len
    // update the firstright key
    // Or no need to update, cause it's originally set
    // Difference with split_leaf: the separator is indeed the firstright, when here are different
    PkBhead *head_rf = GetHeaderPkB(node, split + 1);
    int pfx_len_rf = get_common_prefix_len(newsplit.promotekey.addr, PageOffset(node, head_rf->key_offset),
                                           newsplit.promotekey.size, head_rf->key_len);

    // 3. copy the two parts into new pages
    char *leftbase = NewPage();
    SetEmptyPage(leftbase);
    char *rightbase = right->base;
    int left_top = 0, right_top = 0;

    CopyToNewPagePkB(node, 0, split, leftbase, left_top);
    CopyToNewPagePkB(node, split + 1, node->size, rightbase, right_top);

    // 4. update pointers
    vector<NodePkB *> leftptrs;
    vector<NodePkB *> rightptrs;
    copy(node->ptrs.begin(), node->ptrs.begin() + split + 1,
         back_inserter(leftptrs));
    copy(node->ptrs.begin() + split + 1, node->ptrs.end(),
         back_inserter(rightptrs));

    // vector<KeyPkB> leftkeys;
    // vector<KeyPkB> rightkeys;
    // vector<NodePkB *> leftptrs;
    // vector<NodePkB *> rightptrs;
    // copy(allkeys.begin(), allkeys.begin() + split, back_inserter(leftkeys));
    // copy(allkeys.begin() + split + 1, allkeys.end(), back_inserter(rightkeys));
    // copy(allptrs.begin(), allptrs.begin() + split + 1, back_inserter(leftptrs));
    // copy(allptrs.begin() + split + 1, allptrs.end(), back_inserter(rightptrs));
    //
    // string splitkey;

    // uncompressedKey firstrightKeyAndPtr = get_uncompressed_key_before_insert(node, split + 1, insertpos, promotekey, promotekeyptr, equal);
    // uncompressedKey splitKeyAndPtr = get_uncompressed_key_before_insert(node, split, insertpos, promotekey, promotekeyptr, equal);
    // splitkey = splitKeyAndPtr.key;
    // newsplit.promotekey = splitkey;
    // newsplit.keyptr = splitKeyAndPtr.keyptr;
    // int firstrightoffset = compute_offset(splitkey, firstrightKeyAndPtr.key);
    // rightkeys.at(0) = KeyPkB(firstrightoffset, firstrightKeyAndPtr.key, firstrightKeyAndPtr.keyptr, rightkeys.at(0).ridList);

    // newsplit.promotekey = splitkey;
    right->size = node->size - (split + 1);
    right->space_top = right_top;
    right->IS_LEAF = false;
    right->ptrs = rightptrs;
    right->ptr_cnt = node->size - split;

    node->size = split;
    node->space_top = left_top;
    UpdateBase(node, leftbase);
    node->ptrs = leftptrs;
    node->ptr_cnt = split + 1;

    // node->size = leftkeys.size();
    // node->keys = leftkeys;
    // node->ptrs = leftptrs;

    // right->size = rightkeys.size();
    // right->IS_LEAF = false;
    // right->keys = rightkeys;
    // right->ptrs = rightptrs;

    // if (insertpos < split) {
    //     build_page_prefixes(node, insertpos); // Populate new prefixes
    // }
    // build_page_prefixes(right, 0); // Populate new prefixes

    // Set next pointers
    NodePkB *next = node->next;
    right->prev = node;
    right->next = next;
    if (next)
        next->prev = right;
    node->next = right;

    newsplit.left = node;
    newsplit.right = right;

    return newsplit;
}

splitReturnPkB BPTreePkB::split_leaf(NodePkB *node, NodePkB **path, int path_level,
                                     char *newkey, int keylen, int offset) {
    splitReturnPkB newsplit;
    NodePkB *right = new NodePkB;
    // int insertpos;
    // int rid = rand();
    bool equal = false;
    findNodeResult result = find_node(node, newkey, keylen, offset, equal);
    int insertpos = result.high;

    // Insert the new key
    WTitem prevkey;
    generate_pkb_key(node, newkey, keylen, insertpos,
                     path, path_level - 1, prevkey);
    int pfx_len = get_common_prefix_len(prevkey.addr, newkey, prevkey.size, keylen);
    InsertKeyPkB(node, insertpos, newkey, keylen, pfx_len);
    // vector<bool> flag(node->size);
    // printTree(node, flag, true);
    if (!equal) {
        update_next_prefix(node, insertpos, newkey, keylen);
    }
    // vector<bool> flag(node->size);
    // printTree(node, flag, true);
    // vector<KeyPkB> allkeys;
    // if (equal) {
    //     allkeys = node->keys;
    //     allkeys.at(insertpos).addRecord(rid);
    // }
    // else {
    //     for (int i = 0; i < insertpos; i++) {
    //         allkeys.push_back(node->keys.at(i));
    //     }
    //     KeyPkB pkbKey = generate_pkb_key(node, newkey, newkeyptr, insertpos, parents, parents.size() - 1, rid);
    //     allkeys.push_back(pkbKey);

    //     for (int i = insertpos; i < node->size; i++) {
    //         allkeys.push_back(node->keys.at(i));
    //     }
    // }

    int split = split_point(node);

    // 1. get full promotekey, make a copy for that
    PkBhead *head_split = GetHeaderPkB(node, split);
    newsplit.promotekey.size = head_split->key_len;
    newsplit.promotekey.addr = new char[newsplit.promotekey.size + 1];
    strcpy(newsplit.promotekey.addr, PageOffset(node, head_split->key_offset));
    newsplit.promotekey.newallocated = true;

    // 2. Set the pfx_len of firstright to its length, the base key is the ancester
    head_split->pfx_len = head_split->key_len;
    head_split->pk_len = 0;
    memset(head_split->pk, 0, sizeof(head_split->pk));

    // vector<KeyPkB> leftkeys;
    // vector<KeyPkB> rightkeys;
    // copy(allkeys.begin(), allkeys.begin() + split, back_inserter(leftkeys));
    // copy(allkeys.begin() + split, allkeys.end(), back_inserter(rightkeys));

    // string firstright;
    // uncompressedKey firstrightKeyAndPtr = get_uncompressed_key_before_insert(node, split, insertpos, newkey, newkeyptr, equal);
    // firstright = firstrightKeyAndPtr.key;
    // rightkeys.at(0) = KeyPkB(firstright.length(), firstright, firstrightKeyAndPtr.keyptr, rightkeys.at(0).ridList);
    // newsplit.keyptr = firstrightKeyAndPtr.keyptr;

    // newsplit.promotekey = firstright;

    // 3. copy the two parts into new pages
    char *leftbase = NewPage();
    SetEmptyPage(leftbase);
    char *rightbase = right->base;
    int left_top = 0, right_top = 0;

    CopyToNewPagePkB(node, 0, split, leftbase, left_top);
    CopyToNewPagePkB(node, split, node->size, rightbase, right_top);

    right->size = node->size - split;
    right->space_top = right_top;
    right->IS_LEAF = true;

    node->size = split;
    node->space_top = left_top;
    UpdateBase(node, leftbase);

    // node->size = leftkeys.size();
    // node->keys = leftkeys;

    // right->size = rightkeys.size();
    // right->IS_LEAF = true;
    // right->keys = rightkeys;

    // if (!equal && insertpos < split) {
    //     build_page_prefixes(node, insertpos); // Populate new prefixes
    // }
    // build_page_prefixes(right, 0); // Populate new prefixes

    // Set next pointers
    NodePkB *next = node->next;
    right->prev = node;
    right->next = next;
    if (next)
        next->prev = right;
    node->next = right;

    newsplit.left = node;
    newsplit.right = right;

    return newsplit;
}

// TODO: copy from myisam, need to be more accurate
bool BPTreePkB::check_split_condition(NodePkB *node, int keylen) {
    int currspace = node->space_top + node->size * sizeof(PkBhead);
    // Update prefix need more space, the newkey, the following key, the decompressed split point
    int splitcost = keylen + sizeof(PkBhead) + 2 * max(keylen, APPROX_KEY_SIZE);

    if (currspace + splitcost >= MAX_SIZE_IN_BYTES - SPLIT_LIMIT)
        return true;
    else
        return false;
}

NodePkB *BPTreePkB::search_leaf_node(NodePkB *searchroot, const char *key, int keylen, int &offset) {
    // Tree is empty
    if (searchroot == NULL) {
        cout << "Tree is empty" << endl;
        return nullptr;
    }

    NodePkB *cursor = searchroot;
    bool equal = false;
    // Till we reach leaf node
    while (!cursor->IS_LEAF) {
        // string_view searchkey = key;

        findNodeResult result = find_node(cursor, key, keylen, offset, equal);
        // If equal key found, choose next position
        int pos;
        if (result.low == result.high)
            pos = result.high + 1;
        else
            pos = result.high;
        offset = result.offset;
        cursor = cursor->ptrs[pos];
    }
    return cursor;
}

NodePkB *BPTreePkB::search_leaf_node_for_insert(NodePkB *searchroot, const char *key, int keylen,
                                                NodePkB **path, int &path_level, int &offset) {
    // Tree is empty
    if (searchroot == NULL) {
        cout << "Tree is empty" << endl;
        return nullptr;
    }

    NodePkB *cursor = searchroot;
    bool equal = false;
    // Till we reach leaf node
    while (!cursor->IS_LEAF) {
        // string_view searchkey = key;
        // parents.push_back(cursor);
        path[path_level++] = cursor;

        findNodeResult result = find_node(cursor, key, keylen, offset, equal);
        // If equal key found, choose next position
        int pos;
        if (result.low == result.high)
            pos = result.high + 1;
        else
            pos = result.high;
        offset = result.offset;
        cursor = cursor->ptrs[pos];
    }
    return cursor;
}

/*
================================================
=============statistic function & printer=======
================================================
*/

void BPTreePkB::getSize(NodePkB *cursor, int &numNodes, int &numNonLeaf, int &numKeys,
                        int &totalBranching, unsigned long &totalKeySize, int &totalPrefixSize) {
    if (cursor != NULL) {
        unsigned long currSize = 0;
        int prefixSize = 0;
        // // One time computation
        // if (cursor == _root) {
        //     for (auto it = strPtrMap.begin(); it != strPtrMap.end(); it++) {
        //         currSize += it->first.length();
        //     }
        // }
        for (int i = 0; i < cursor->size; i++) {
            PkBhead *head = GetHeaderPkB(cursor, i);
            currSize += head->key_len + sizeof(head->pfx_len) + head->pk_len;
            prefixSize += sizeof(head->pfx_len) + head->pk_len;
        }
        totalKeySize += currSize;
        numKeys += cursor->size;
        totalPrefixSize += prefixSize;
        numNodes += 1;
        if (cursor->IS_LEAF != true) {
            totalBranching += cursor->ptrs.size();
            numNonLeaf += 1;
            for (int i = 0; i < cursor->size + 1; i++) {
                getSize(cursor->ptrs[i], numNodes, numNonLeaf, numKeys, totalBranching, totalKeySize, totalPrefixSize);
            }
        }
    }
}

int BPTreePkB::getHeight(NodePkB *cursor) {
    if (cursor == NULL)
        return 0;
    if (cursor->IS_LEAF == true)
        return 1;
    int maxHeight = 0;
    for (int i = 0; i < cursor->size + 1; i++) {
        maxHeight = max(maxHeight, getHeight(cursor->ptrs.at(i)));
    }
    return maxHeight + 1;
}

void BPTreePkB::printTree(NodePkB *x, vector<bool> flag, bool compressed, int depth, bool isLast) {
    // Condition when node is None
    if (x == NULL)
        return;

    // Loop to print the depths of the
    // current node
    for (int i = 1; i < depth; ++i) {
        // Condition when the depth
        // is exploring
        if (flag[i] == true) {
            cout << "| "
                 << " "
                 << " "
                 << " ";
        }

        // Otherwise print
        // the blank spaces
        else {
            cout << " "
                 << " "
                 << " "
                 << " ";
        }
    }

    // Condition when the current
    // node is the root node
    if (depth == 0) {
        printKeys_pkb(x, compressed);
        cout << endl;
    }

    // Condition when the node is
    // the last node of
    // the exploring depth
    else if (isLast) {
        cout << "+--- ";
        printKeys_pkb(x, compressed);
        cout << endl;

        // No more childrens turn it
        // to the non-exploring depth
        flag[depth] = false;
    }
    else {
        cout << "+--- ";
        printKeys_pkb(x, compressed);
        cout << endl;
    }

    int it = 0;
    for (auto i = x->ptrs.begin();
         i != x->ptrs.end(); ++i, ++it)

        // Recursive call for the
        // children nodes
        printTree(*i, flag, compressed, depth + 1,
                  it == (x->ptrs.size()) - 1);
    flag[depth] = true;
}