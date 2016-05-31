#ifndef LUA_RBTREE
#define LUA_RBTREE
#ifndef NULL
#define NULL 0
#endif
#define BOOL  int
#define TRUE 1
#define FALSE 0
/*
* 红黑树是满足如下性质的一棵二叉查找树：
*  (1) 每个结点只有红和黑两种颜色
*  (2) 根结点是黑色的
*  (3) 空节点是黑色的（红黑树中，根节点的parent以及所有叶节点lchild、rchild都不指向NULL，而是指向一个定义好的空节点）。
*  (4) 如果一个结点是红色的,那么它的左右两个子结点的颜色是黑色的
*  (5) 对于每个结点而言,从这个结点到叶子结点的任何路径上的黑色结点的数目相同
*/
/*
* 红黑树的特点(也是红黑树是一棵好的二叉查找树的原因):
* 一棵具有n个内结点（即真正的数据结点）的红黑树的黑高度bh至多为2lg(n+1)
* 证明: 先求证：一棵以x的根的红黑树中至少包含2(hb(x))(指数) - 1 个内结点
* (1) 如果x的高度为0，则 x至少包含 2的0次方 -> 1 = 0 个内结点，成立。
* (2) 若x有子树，则其子树的黑高度为 bh(x) （孩子节点为黑色时），或者bh(x) -1（孩子结点为红色或者黑色时）
* (3) 因为x的孩子的黑高度小于x的黑高度，利用归纳假设，可以得出每个孩子至少包含2的 bh(x) -1 次方 - 1
* (4) 于是以x为根的子树至少包含 2 (bh(x) -1 次方) - 1 + 2 (bh(x) -1 次方) - 1 + 1 = 2 (bh(x)次方) - 1 个内结点
* 又根据性质（4），树的黑高度至少为 h / 2 , 所以 n >= 2 (h / 2 次方) - 1 => h <= 2 lg (n - 1)
*/

/*定义颜色类型*/
typedef enum color_t {
	RED = 0,
	BLACK = 1
}color_t; 

typedef long long int64_t;

/*定义数据类型*/
typedef const char *data_t; 
typedef int64_t code_t;

/*定义红黑树的节点结构*/
typedef struct RBTreeNode {
	code_t hashCode;
	data_t data;
	color_t color;
	struct RBTreeNode *left;
	struct RBTreeNode *right;
	struct RBTreeNode *parent;
}RBTreeNode,*RBTree;


/*查找操作，不存在返回NULL*/
RBTreeNode * searchTreeNode(RBTree *rbTree,code_t hashCode);
/*返回最小结点*/
RBTreeNode * minTreeNode(RBTree *rbTree);
/*返回最大结点*/
RBTreeNode * maxTreeNode(RBTree *rbTree);
/*返回node的后继结点*/
RBTreeNode * tailTreeNode(RBTree *rbTree,code_t hashCode);
/*返回node的后继结点*/
RBTreeNode * nextTreeNode(RBTreeNode *node);
/*返回node的前驱结点*/
RBTreeNode * lastTreeNode(RBTreeNode *node);

/*插入结点*/
BOOL insertTreeNode(RBTree *rbTree,code_t hashCode,data_t data);
/*删除第一个值为data的结点*/
BOOL deleteTreeNode(RBTree *rbTree, code_t hashCode);

/*释放*/
void destory(RBTree *rbTree);

/*中序遍历*/
void rbInorderTraversal(RBTree *rbTree, void (*visitor)(RBTreeNode *node));

#endif











