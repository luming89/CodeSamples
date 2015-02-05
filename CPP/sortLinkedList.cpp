/* Author: Luming Zhang
 * This is a merge sort for singly-linked list.
 * Time complexity is O(nlogn), space complexity is O(1).
 */

#include <iostream>
#include <vector>

using namespace std;

struct ListNode 
{
    int val;
    ListNode *next;
    ListNode(int x) : val(x), next(NULL) {}
};

class Solution 
{
public:
    ListNode *sortList(ListNode *head) 
    {
        if(head==NULL) return NULL;
        int n = 0;
        int block = 1;
        ListNode* it = head;
        while (it)
        {
            it = it->next;
            ++n;
        }
	cout << "Size is "<<n<<"\n";
        ListNode dummy(0);
        dummy.next = head;
        ListNode* p1 = NULL;
        ListNode* p2 = NULL;
        ListNode* prev = &dummy;
        while(block<n)
        {
            it = dummy.next;
            prev = &dummy;
            cout<<"Block size is "<<block<<"\n";
            printList(&dummy);
            while(it)
            {
                p1 = it;
                cout<<"P1 points to "<<p1->val<<"\n";
                p2 = it;
                int count = block;
                while(count&&p2) // position p2
                {
                    p2 = p2->next;
                    count--;
                }
                if(p2) cout<<"P2 points to "<<p2->val<<"\n";
                it = p2;
                count = block;
                while(count&&it) // position it
                {
                    it = it->next;
                    count--;
                }
                int b1 = block;
                int b2 = block;
                while(b1||b2)
                {
                    if(p1==NULL)
                        b1 = 0;
                    if(p2==NULL)
                        b2 = 0;
                    if(b1&&b2)
                    {
                        if(p1->val<=p2->val) // p1->val is smaller or equal
                        {
                            prev->next = p1;
                            prev = p1;
                            cout<<"P1 points to "<<p1->val<<"\n";
                            p1 = p1->next;

                            b1--;
                        }
                        else // p2->val is smaller
                        {
                            prev->next = p2;
                            prev = p2;
                            cout<<"P2 points to "<<p2->val<<"\n";
                            p2 = p2->next;
                            b2--;
                        }
                    }
                    else if(b1)
                    {
                        while(b1&&p1)
                        {
                            prev->next = p1;
                            prev = p1;
                            p1 = p1->next;
                            b1--;
                        }
                    }
                    else // b2
                    {
                        while(b2&&p2)
                        {
                            prev->next = p2;
                            prev = p2;
                            p2 = p2->next;
                            b2--;
                        }
                    }
                }
            }
            block *=2;
            prev->next = NULL;
        }
        return dummy.next;
    }
    void printList(ListNode* it)
    {
    	while(it)
        {
            cout<<it->val<<"->";
            it=it->next;
        }
        cout<<"NULL\n";

    }
};



int main()
{
    vector<int> vec{4,19,14,5,-3,1,8,5,11,15};
    //vector<int> vec{3,2,4};
    ListNode* head=NULL;
    ListNode* it = NULL;
    for(int i=0;i<vec.size();i++)
    {
	ListNode* p = new ListNode(vec[i]);
	if(head==NULL)
	{
	    head = p;
	    it = p;
	}
	else
	{
	    it->next = p;
	    it = it->next;
	}
    }
    Solution tmp;
    it = tmp.sortList(head);
    //it = head;
    tmp.printList(it);
}


