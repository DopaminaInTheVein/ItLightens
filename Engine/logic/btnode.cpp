#include <stdio.h>
#include "btnode.h"
#include "bt.h"


btnode::btnode(string s)
{
name=s;
}


void btnode::create(string s)
{
name=s;
}


bool btnode::isRoot()
{
return (parent==NULL);
}


void btnode::setParent(btnode *p)
{
parent=p;
}


void btnode::setRight(btnode *p)
{
right=p;
}


void btnode::setType(int t)
{
type=t;
}


string btnode::getName()
{
return name;
}


void btnode::addChild(btnode *c)
{
if (!children.empty()) // if this node already had children, connect the last one to this
	children.back()->setRight(c);  // new one so the new one is to the RIGHT of the last one
children.push_back(c); // in any case, insert it
c->right=NULL; // as we're adding from the right make sure right points to NULL
}


void btnode::recalc(bt *tree)
{
// activate the next line to debug
printf("recalcing node %s\n",name.c_str());
switch (type)
	{
	case ACTION:	
		{
		// run the controller of this node
		int res=tree->execAction(name);
		// now, the next lines compute what's the NEXT node to use in the next frame...
		if (res==STAY) { tree->setCurrent(this); return; }// looping vs. on-shot actions
		// climb tree iteratively, look for the next unfinished sequence to complete
		btnode *cand=this;
		while (cand->parent!=NULL)
			{
			btnode *daddy=cand->parent;
			if (daddy->type==SEQUENCE)
				// oh we were doing a sequence. make sure we finished it!!!
				{
				if (cand->right!=NULL)
					{
					tree->setCurrent(cand->right);
					break;
					}
				// sequence was finished (there is nobody on right). Go up to daddy.
				else cand=daddy;
				}
			// i'm not on a sequence, so keep moving up to the root of the BT
			else cand=daddy;
			}
		// if we've reached the root, means we can reset the traversal for next frame.
		if (cand->parent==NULL) tree->setCurrent(NULL);
		break;
		}
	case RANDOM:
		{
		int r=rand()%children.size();
		children[r]->recalc(tree);
		break;
		}
	case PRIORITY:
		{
		for (int i=0;i<children.size();i++)
			{
			if (tree->testCondition(children[i]->getName())) 
				{
				children[i]->recalc(tree);
				break;
				}
			}
		break;
		}
	case SEQUENCE:
		{
		// begin the sequence...the inner node (action) will take care of the sequence
		// via the "setCurrent" mechanism
		children[0]->recalc(tree);	
		break;
		}
	}
}
