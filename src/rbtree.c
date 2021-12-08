#include "rbtree.h"

#include <stdlib.h>
void left_rotate(rbtree *t, node_t* x);
void right_rotate(rbtree *t, node_t* x);
void del_node(rbtree *t,node_t *n);
void transplant(rbtree *t, node_t *u, node_t *v);
void rb_insert_fixup(rbtree* t, node_t* z);
void rb_delete_fixup(rbtree *t, node_t *x);

rbtree *new_rbtree(void) {
  //rbtree 사이즈 1개의 메모리를 할당하고 모두 0으로 초기화한다.
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  //sentinel을 사용하기 때문에 nil 설정해줌
  node_t* nilnode = (node_t *)calloc(1, sizeof(node_t));
  nilnode->color = RBTREE_BLACK;
  p->nil = nilnode;
  p->root = nilnode;  
  return p;
}
void del_node(rbtree *t,node_t *n){
  if(n != t->nil){
    del_node(t, n->left);
    del_node(t, n->right);
    free(n);
  }
  
}
void delete_rbtree(rbtree *t) {
  del_node(t,t->root); 
  free(t->nil);
  free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  //노드를 새로 만들어줘야 한다
  //node_t라는 구조체가 이미 있으므로 이 size로 메모리 할당
  node_t* z = (node_t*)malloc(sizeof(node_t));
  z->key = key;
  
  node_t* y = t->nil; // y를 x의 부모를 따라가는 pointer로 설정
  node_t* x = t->root; // x를 노드를 따라가는 pointer로 설정, 처음에는 root
  while(x != t->nil){
    y = x; //현재 값을 y에 넣어두고 바꾸면 parent가 된다
    if(z->key < x->key){
      x = x->left;
    }else{
      x = x->right;
    }
  }
  z->parent = y;
  if (y == t->nil){
    t->root = z;
  }else if(z->key < y->key){
    y->left = z;
  }else{
    y->right = z;
  }
  z->left = z->right = t->nil;
  z->color = RBTREE_RED;
  rb_insert_fixup(t,z);
  t->root->color = RBTREE_BLACK;
  return z;
}
void rb_insert_fixup(rbtree* t, node_t* z){
  while(z->parent->color == RBTREE_RED){
    if(z->parent == z->parent->parent->left){
      node_t* y = z->parent->parent->right;
      //경우 1
      if(y->color == RBTREE_RED){
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }else{
        //경우 2
        if(z==z->parent->right){
          z = z->parent;
          left_rotate(t,z);
        }
        //경우 3
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t,z->parent->parent);
      }
    }else{
      node_t* y = z->parent->parent->left;
      if(y->color == RBTREE_RED){
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }else{
        if(z==z->parent->left){
          z = z->parent;
          right_rotate(t,z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotate(t,z->parent->parent);
      }
    }
  }
}
void left_rotate(rbtree *t, node_t* x){
  node_t* y = x->right;
  x->right = y->left;
  if (y->left != t->nil){
    y->left->parent = x;
  }
  y->parent = x->parent;
  if (x->parent == t->nil){
    t->root = y;                      
  }else if(x == x->parent->left){
    x->parent->left = y;
  }else{
    x->parent->right = y;
  }
  y->left = x;
  x->parent = y;
} 
void right_rotate(rbtree *t, node_t* y){
  node_t*x = y->left;
  y->left = x->right;
  if(x->right != t->nil){
    x->right->parent = y;
  }
  x->parent = y->parent;
  if (y->parent == t->nil){
    t->root = x;       
  } else if (y == y->parent->right){
    y->parent->right = x;
  }else{
    y->parent->left = x;
  }
  x->right = y;
  y->parent = x;
}
node_t *rbtree_find(const rbtree *t, const key_t key) {
  if(t->root == t->nil){
    return NULL;
  }
  node_t* x = t->root; // node_t*형을 변환하는 x를 선언해서 node들을 가리키는 놈으로 사용
  while (x != t->nil){
    if (key < x->key){
      x = x->left;
    }else if (key > x->key){
      x = x->right;
    }else{
      return x;
    }
  }
  return NULL; // while문을 탈출했다는건 x==t.nil이라는 것이다.
}

node_t *rbtree_min(const rbtree *t) {
  if(t->root == t->nil){
    return NULL;
  }
  node_t* cur = t->root;
  while(cur->left != t->nil){
    cur = cur->left;
  }
  return cur;
}

node_t *rbtree_max(const rbtree *t) {
  if(t->root == t->nil){
    return NULL;
  }
  node_t*cur = t->root;
  while(cur->right != t->nil){
    cur = cur->right;
  }
  return cur;
}
void transplant(rbtree *t, node_t *u, node_t *v){
  if(u->parent == t->nil){
    t->root = v;
  }else if(u == u->parent->left){
    u->parent->left = v;
  }else{
    u->parent->right = v;
  }
  v->parent = u->parent;
}
void rb_delete_fixup(rbtree *t, node_t *x){
  while(x != t->root && x->color == RBTREE_BLACK){
    if(x == x->parent->left){
      node_t *w = x->parent->right;
      if(w->color == RBTREE_RED){
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t,x->parent);
        w = x->parent->right;
      }
      if(w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK){
        w->color = RBTREE_RED;
        x = x->parent;
      }else{
        if(w->right->color == RBTREE_BLACK){
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          right_rotate(t,w);
          w = x->parent->right;
        }
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t,x->parent);
        x = t->root;
      }
    }else{
      node_t* w = x->parent->left;
      if(w->color == RBTREE_RED){
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t,x->parent);
        w = x->parent->left;
      }
      if(w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK){
        w->color = RBTREE_RED;
        x = x->parent;
      }else{
        if(w->left->color == RBTREE_BLACK){
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotate(t,w);
          w = x->parent->left;
        }
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t,x->parent);
        x = t->root;
      }
    }
  }
  x->color = RBTREE_BLACK;
}
int rbtree_erase(rbtree *t, node_t *p) {
  if (p == NULL){
    return 0;
  }
  node_t *y = p;
  node_t *x;
  color_t y_original = y->color;
  if(p->left == t->nil){
    x = p->right;
    transplant(t,p,p->right);
  }else if(p->right == t->nil){
    x = p->left;
    transplant(t,p,p->left);    
  }else{
    y = p->right;
    while(y->left != t->nil){
      y = y->left;
    }
    y_original = y->color;
    x = y->right;
    if(y->parent == p){
      x->parent = y;
    }else{
      transplant(t,y,y->right);
      y->right = p->right;
      y->right->parent = y;
    }
    transplant(t,p,y);
    y->left = p->left;
    y->left->parent = y;
    y->color = p->color;
  }
  if(y_original == RBTREE_BLACK){
    rb_delete_fixup(t,x);
  }
  free(p);
  return 0;
}
void _rbtree_to_array(const rbtree *t, node_t *cur, key_t *arr, size_t *pcnt, const size_t n){
  if(cur == t->nil){
    return;
  }
  _rbtree_to_array(t,cur->left,arr,pcnt,n);
  if(*pcnt<n){
    arr[(*pcnt)++] = cur->key;
  }else{
    return;
  }
  _rbtree_to_array(t,cur->right,arr,pcnt,n);
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  if(t->root == t->nil){
    return 0;
  }else{
    size_t cnt = 0;
    _rbtree_to_array(t,t->root,arr,&cnt,n);
  }
  return 0;
  
}
