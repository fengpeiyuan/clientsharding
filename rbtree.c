#include <stdio.h>
#include <stdlib.h>
#include "rbtree.h"


RBTreeNode * searchTreeNode(RBTree *rbTree,code_t hashCode){

	RBTreeNode *currentNode=*rbTree;

	while(currentNode != NULL && currentNode->hashCode != hashCode){
		if(hashCode < currentNode->hashCode){
			currentNode = currentNode->left;
		}else{
			currentNode = currentNode->right;
		}
	}
	return currentNode;
}

RBTreeNode *minTreeNode(RBTree *rbTree){
	RBTreeNode *currentNode = *rbTree,*targetNode=NULL;

	while(NULL != currentNode){
		targetNode  = currentNode;
		currentNode = currentNode->left;
	}
	return targetNode;
}

RBTreeNode *maxTreeNode(RBTree *rbTree){
	RBTreeNode *currentNode=*rbTree,*targetNode=NULL;

	while(NULL != currentNode){
		targetNode  = currentNode;
		currentNode = currentNode->right;
	}
	return targetNode;
}

RBTreeNode * tailTreeNode(RBTree *rbTree,code_t hashCode){

	RBTreeNode *currentNode=*rbTree,*targetNode=NULL;

	while(currentNode != NULL){
		if(hashCode == currentNode->hashCode){ /*找到结点，返回*/
			targetNode = currentNode;
			break; 
		}else if(hashCode < currentNode->hashCode){/*左树查找*/
			targetNode = currentNode;
			
			if(currentNode->left == NULL){			
				break;
			}else{
				currentNode = currentNode->left;
				continue;
			}
		}else{/*右树查找*/
			if(currentNode->right == NULL ){
				/*左树，返回上级结点*/
				break;
			}else{
				currentNode = currentNode->right;
			}
		}
		
	}
	return targetNode;
}

RBTreeNode *nextTreeNode(RBTreeNode *node){
	RBTreeNode *targetNode;
	if(NULL ==node ) return NULL;
	if (node->right != NULL){
	 	targetNode = minTreeNode(&(node->right));
	}else{
		while(node->parent != NULL && node->parent->left != node){
			node  = node->parent;
		}
		targetNode = node->parent;
	} 
	return targetNode;
}

RBTreeNode *lastTreeNode(RBTreeNode *node){
	RBTreeNode *targetNode;
	if (node == NULL) return NULL;
	if (node->left != NULL) {
		targetNode = maxTreeNode(&(node->left));
	} else {
		while (node->parent != NULL && node->parent->right != node) {
			node = node->parent;
		}
		targetNode = node->parent;
	}
	return targetNode;
}

/*
*二叉查找树的左旋操作，该操作要求node的右子树不能为空
*/
static void leftRotateRBTree(RBTree *rbTree,RBTreeNode *node);
/*
*二叉查找树的右旋操作，该操作要求node的左子树不能为空
*/
static void rightRotateRBTree(RBTree *rbTree,RBTreeNode *node);
/*
* 当插入一个节点后，用此过程来保持红黑树的性质
*/
static void balanceRBTreeAfterInsert(RBTree *rbTree ,RBTreeNode *node);
/*
* 当删除一个结点时，通过此过程保持红黑树的性质
*/
static void balanceRBTreeAfterDelete(RBTree *rbTree,RBTreeNode *parent,RBTreeNode *childNode);

/**
*红黑树节点创建插入操作
*
**/
BOOL insertTreeNode(RBTree *rbTree,code_t hashCode,data_t data){

	RBTreeNode *node,*parent=NULL,*currentNode;

	node=(RBTreeNode *)malloc(sizeof(RBTreeNode));

	if(NULL == node) return FALSE;

	node->data = (data_t)malloc(strlen(data));

	node->data = strcpy(node->data,data);

	/*node->data 		= data;*/
	node->color 	= RED;
	node->hashCode 	= hashCode;
	node->left 		= NULL;
	node->right 	= NULL;

	currentNode 	= *rbTree;

	while(currentNode != NULL){
		parent 	= currentNode;
		if( hashCode < currentNode->hashCode){
			currentNode = currentNode->left;
		}else{
			currentNode = currentNode->right;
		}
	}
	if(NULL == parent){
		*rbTree = node;
	}else{
		if(hashCode < parent->hashCode){
			parent->left  = node;
		}else{
			parent->right = node;
		}
	}
	node->parent = parent;
	
	/*调整红黑树,以保持性质*/
	balanceRBTreeAfterInsert(rbTree,node);

	return TRUE;
}

BOOL deleteTreeNode(RBTree *rbTree, code_t hashCode){
	

	RBTreeNode *targetNode, *deleteNode, *childNode;
	
	targetNode = searchTreeNode(rbTree,hashCode);

	if(targetNode != NULL){
		/*找到待删除的真正结点位置*/
		if(targetNode->left == NULL || targetNode ->right == NULL){
			deleteNode = targetNode;
		}else{
			deleteNode = nextTreeNode(targetNode);
		}
		/*将待删除节点的子树链接到其父节点上，待删除的节点最多只有一个子树*/
		if(deleteNode->left != NULL){
			childNode = deleteNode->left;
		}else{
			childNode = deleteNode->right;
		}

		if(childNode != NULL){
			childNode->parent = deleteNode->parent;
		}

		if(deleteNode->parent == NULL){
			*rbTree = childNode;
		}else{
			if(deleteNode->parent->left == deleteNode){
				deleteNode->parent->left = childNode;
			}else{
				deleteNode->parent->right =childNode;
			}
		}
		if(targetNode != deleteNode){
			targetNode->hashCode = deleteNode->hashCode;
			targetNode->data     = deleteNode->data;
		}
		if(deleteNode->color ==BLACK){
			balanceRBTreeAfterDelete(rbTree,deleteNode->parent,childNode);
		}
		free(deleteNode->data);
		free(deleteNode);
		return TRUE;
	}else{
		return FALSE;
	}

}



/*
*插入一个结点时,可能破坏如下性质：
*  (2) 根结点是黑色的
*  (4) 如果一个结点是红色的,那么它的左右两个子结点的颜色是黑色的
*/
static void balanceRBTreeAfterInsert(RBTree *rbTree ,RBTreeNode *node){
	RBTreeNode *parent, *gparent, *uncle;

	/*正性质(2)根结点是黑色的*/
	while ( (parent = node->parent) != NULL && parent->color == RED){
		gparent = parent->parent;
		/*如果父结点是祖父结点的左孩子(因为父结点是红色结点，肯定存在祖父结点)*/
		if(parent == gparent->left){
			uncle = gparent->right;
			/*情况1:：如果存在叔父结点，并且叔父结点颜色为红色时，则可以通过改变祖父，父亲和叔父结点的颜色使当前存在破坏性质的结点沿树上升，由node变为其祖父*/
			if (uncle != NULL && uncle->color == RED){
				gparent->color = RED;
				parent->color  = BLACK;
				uncle->color   = BLACK;
				node =gparent;
			}else{/*叔父结点不存在或者存在但是颜色是黑色的，则必须通过旋转来配合改变颜色来保持性质2*/
				/*情况2:node为其父结点的右孩子，通过左旋转转化为情况3*/
				if (node == parent->right){	
					/*基于node的父结点做左旋，记录原node结点位node'*/
					node = parent;
					leftRotateRBTree(rbTree,node);
					/*旋转后，重置node，使其仍为node结点的父结点(也即node')*/
					parent = node->parent;
				}
				/*情况3:node为其父结点的左孩子，调整父结点和祖父结点的颜色，以纠正性质4，但是破坏了性质5*/
				parent->color  = BLACK;
				gparent->color = RED;
				/*基于祖父结点右旋以保持性质5*/
				rightRotateRBTree(rbTree,gparent);
				/*此时node->parent->color = BLACK ,循环结束*/
			}
		}else{/*父结点为祖父结点的右孩子*/
			uncle = gparent->left;
			/*情况1:如果存在叔父结点，并且叔父结点颜色为红色，则可以通过改变祖父，父亲和叔父结点的颜色使得当前存在破坏性质得结点沿树上升，使node变为其祖父*/
			if (uncle != NULL && uncle->color == RED){
				gparent->color 	= RED;
				parent->color 	= BLACK;
				uncle->color 	= BLACK;
				node = gparent;
			}else{
				/*情况2:node为其父结点得左孩子，通过旋转转换为情况3*/
				if(node == parent->left){
					node = parent;
					rightRotateRBTree(rbTree,node);
					parent = node->parent;
				}
				/*情况3:node为其父结点得右孩子，调整父结点和祖父结点得颜色，以纠正性质4，但是破坏了性质5*/
				parent->color 	= BLACK;
				gparent->color	= RED;
				/*基于祖父结点左旋以保持性质5*/
				leftRotateRBTree(rbTree,gparent);
				/*此时node->parent->color =BLACK ，循环结束*/
			}
		}
	}
	/*保持性质2*/
	(*rbTree)->color = BLACK;
}


/*
* 删除一个黑结点会导致如下三个问题： 
* （1）如果被删除结点y是根结点，而y的一个红色孩子成为了新的根，则违反了性质2
* （2）如何y的父结点和其孩子结点都是红色的，则违反了性质4
* （3）删除y将导致先前包含y的任何路径上的黑结点树少一个，破坏了性质5。
* 解决方案是：被删除的结点黑色属性下移到其孩子结点x上。此时性质5都得以保持，于是存在2种情况：
* （1）x原来为红色，此时孩子结点属性是红黑，此时破坏了性质（1），（4），如果x还是树根则，破坏了性质（2）
*		处理方式为：将x重新着色为黑色（此操作同时去除其多余的黑色属性），处理完毕，红黑树性质得以保持
* （2）x原来为黑色，此时x的属性为双重黑色，破坏了性质（1），若x为树根，则可以只是简单的消除x多余的黑色属性
*		否则需要做必要的旋转和颜色修改
*/

static void balanceRBTreeAfterDelete(RBTree *rbTree,RBTreeNode *parent,RBTreeNode *childNode){
	RBTreeNode *brother;

	while((childNode ==NULL || childNode->color == BLACK) && childNode != *rbTree){
		
		if(childNode == parent->left){
			brother=parent->right;
			/*
			 *因为被删除结点为黑色，其必然有兄弟结点,也即是现在x的兄弟结点（由性质5保证）
			 *情况1：如果兄弟结点为红色,则parent颜色比为黑色，此时调整颜色，并左旋，使得brother和parent位置调换，此操作不破坏别的性质，并将情况1变化为情况2，3，4
			 */
			if(brother->color ==RED){
				brother->color = BLACK;
				parent->color  = RED;
				/*左旋调整brother和parent的位置*/
				leftRotateRBTree(rbTree,parent);
				/*重置brother,转换到情况2,3,4*/
				brother = parent->right;
			}
			/*
			 *情况2：brother 有两个黑色结点（NULL也为黑色结点）:将x和brother抹除一重黑色具体操作为，brother的颜色变为红，x结点上移到其父结点
			 */
			if((brother->left ==NULL || brother->left->color ==BLACK)&&
				(brother->right ==NULL ||brother->right->color == BLACK)){
				brother->color = RED;
				childNode = parent;
				parent = parent->parent;
			}else{
				/*情况3:brother 左孩子为红色结点，右孩子为黑色结点*/
				if(brother->right == NULL || brother->color ==BLACK){
					brother->left->color = BLACK;
					brother->color = RED;
					/*右旋使情况变化为情况4*/
					rightRotateRBTree(rbTree,brother);
					brother = parent->right;
				}
				/*情况4：brother的右孩子为红色结点:交换brother和parent的颜色和位置，使得x的2重黑色属性中的一重转移到其parent上,
				 *此时到brother的右孩子的黑结点数少一，于是将右结点的颜色置黑，红黑树性质得以保持
				 */
				brother->color = parent->color;
				parent->color  = BLACK;
				brother->right->color = BLACK;
				leftRotateRBTree(rbTree,parent);

				childNode=*rbTree;
			}
		}else{
			brother = parent->left;
			/*情况1*/
			if(brother->color ==RED){
				brother->color = BLACK;
				parent->color  = RED;
				rightRotateRBTree(rbTree,parent);
				brother = parent->left;
			}
			/*情况2*/
			if((brother->left == NULL || brother->left->color == BLACK)&&
				(brother->right ==NULL || brother->right->color == BLACK)){
				brother->color =RED;
				childNode=parent;
				parent = parent->parent;
			}else{
				if(brother->left == NULL || brother->left->color ==BLACK){
					brother->right->color = BLACK;
					brother->color = RED;
					leftRotateRBTree(rbTree,brother);
					brother = parent->left;
				}
				/*情况4:brother 的左孩子为红色结点*/
				brother->color =  parent->color;
				parent->color  =  BLACK;
				brother->left->color = BLACK;
				rightRotateRBTree(rbTree,parent);

				childNode = *rbTree;
			}
		}
	}
	if(childNode != NULL){
		childNode->color = BLACK;
	}
}

/*
*		node 					   right
*		/  \					   /  \
*	   a    right 		==> 	node   c
*            /  \				/  \
*			b    c 			   a    b
*/
static void leftRotateRBTree(RBTree *rbTree,RBTreeNode *node){
	
	RBTreeNode *right=node->right;

	if((node->right = right->left)){
		right->left->parent = node;
	}

	right->left = node;
	if((right->parent = node->parent)){
		if(node ==node->parent->left){
			node->parent->left = right;
		}else{
			node->parent->right = right;
		}
	}else{
		*rbTree = right;
	}
	node->parent = right;
}

/*
*		node 					   left
*		/  \					   /  \
*	 left   c  		==> 	  	  a   node
*  	 /  \							  /  \
*	a    b 			                 b    c
*/
static void rightRotateRBTree(RBTree *rbTree,RBTreeNode *node){

	RBTreeNode *left = node->left;

	if((node->left = left->right)){
		left->right->parent = node;
	}
	left->right =node;

	if((left->parent = node->parent)){
		if(node == node->parent->right){
			node->parent->right = left;
		}else{
			node->parent->left = left;
		}
	}else{
		*rbTree =left;
	}
	node->parent =left;
}

void rbInorderTraversal(RBTree *rbTree, void (*visitor)(RBTreeNode *node)) {
	RBTreeNode *currentNode = *rbTree;

	if (currentNode != NULL) {

		rbInorderTraversal(&(currentNode->left), visitor);
		visitor(currentNode);
		rbInorderTraversal(&(currentNode->right), visitor);
	}
}

void destory(RBTree *rbTree){
	
	RBTreeNode *currentNode=*rbTree;

	if(currentNode != NULL){	
		destory(&(currentNode->left));
		destory(&(currentNode->right));
		free(currentNode->data);
		free(currentNode);
	}
}

















