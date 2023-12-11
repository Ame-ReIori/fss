#include "ggm_tree.h"

void ExpandNode(GGMNode *node,
                DPFKey key,
                uint8_t layer,
                uint8_t depth) {
  if (layer == depth) {
    node->l = nullptr;
    node->r = nullptr;
    return;
  } 

  GGMNode *lnode = (GGMNode *)malloc(sizeof(GGMNode));
  GGMNode *rnode = (GGMNode *)malloc(sizeof(GGMNode));

  osuCrypto::block word = node->word;
  osuCrypto::block seed = word >> 1;
  uint8_t t = *((uint8_t *)&word) & 1;
  osuCrypto::block s[2];
  osuCrypto::block lcw, rcw;

  prg(seed, s);
  
  uint8_t idx = layer;
  lcw = (key.scw[idx] << 1) | osuCrypto::block(key.tcw_l[idx]);
  rcw = (key.scw[idx] << 1) | osuCrypto::block(key.tcw_r[idx]);

  if (t) {
    lnode->word = s[0] ^ lcw;
    rnode->word = s[1] ^ rcw;
  } else {
    lnode->word = s[0];
    rnode->word = s[1];
  }

  node->l = lnode;
  node->r = rnode;

  ExpandNode(lnode, key, layer + 1, depth);
  ExpandNode(rnode, key, layer + 1, depth);
}

void GGMTree::FromDPFKey(DPFKey key, uint8_t b) {
  this->root = (GGMNode *)malloc(sizeof(GGMNode));
  this->root->word = (key.s << 1) | osuCrypto::block(b);
  ExpandNode(this->root, key, 0, this->depth);
}

void RevealCurDepth(GGMNode *node, int depth) {
  if (node == nullptr) {
    return;
  }

  if (depth == 0) {
    std::cout << node->word << " ";
  }

  RevealCurDepth(node->l, depth - 1);
  RevealCurDepth(node->r, depth - 1);
}

void BFS(GGMTree t) {
  for (int i = 0; i < t.depth + 1; i++) {
    RevealCurDepth(t.root, i);
    printf("\n");
  }
}

void ReconstructCurDepth(GGMNode *node0, GGMNode *node1, int depth) {
  if (node0 == nullptr && node1 == nullptr) {
    return;
  }

  if (depth == 0) {
    std::cout << (node0->word ^ node1->word) << " ";
  }

  ReconstructCurDepth(node0->l, node1->l, depth - 1);
  ReconstructCurDepth(node0->r, node1->r, depth - 1);
}

void ReconstrcutTwoGGMTree(GGMTree t0, GGMTree t1) {
  for (int i = 0; i < t0.depth + 1; i++) {
    ReconstructCurDepth(t0.root, t1.root, i);
    std::cout << std::endl;
  }
}

void test_ggm() {
  uint64_t alpha = 5;
  uint64_t beta = 1;

  DPF dpf(3);
  DPFKey key[2];

  dpf.InitKey(key);
  dpf.Gen(alpha, beta, key);

  // std::cout << key[0].s << std::endl;

  GGMTree tree0(dpf.bit_length);
  GGMTree tree1(dpf.bit_length);
  tree0.FromDPFKey(key[0], 0);
  tree1.FromDPFKey(key[1], 1);

  BFS(tree0);
  std::cout << std::endl;

  BFS(tree1);
  std::cout << std::endl;

  ReconstrcutTwoGGMTree(tree0, tree1);
}