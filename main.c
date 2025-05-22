#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NAME_SIZE 50
#define FAMILY_SIZE 4
#define CATEGORY_SIZE 15
#define DAY 10
#define MONTH 12
#define NO_CATEGORY 5
#define ORDER 5
#define MAX 4
#define MIN 2

typedef enum{FAILURE,SUCCESS} status_code;
typedef struct Family_Tag family;
typedef struct Expense_Tag expense;

//Date Structure
typedef struct Date_Tag
{
    int day;
    int month;
}date;

//User Structure
typedef struct User_Tag
{
    int user_id;
    char name[NAME_SIZE];
    float income;
    family* family_id; //Family id where it belongs
    expense* expenses;
}user;

//Family Structure
typedef struct Family_Tag
{
    int family_id;
    char family_name[NAME_SIZE];
    user* members[FAMILY_SIZE]; //Mmbers of the family
    float family_income; //Income of the all family
    float family_expense[MONTH]; //Expense for all months in array
}family;

//Expense Structure
typedef struct Expense_Tag
{
    int expense_id;
    user* user1;
    char category[CATEGORY_SIZE];
    float amount;
    date expense_date;
    struct Expense_Tag* next;
}expense;

//Btree user
typedef struct BTREE_TAG_USER
{
    user* user[MAX+1];
    struct BTREE_TAG_USER* children[MAX+1];
    int no_user;
}btuser;

//Btree family
typedef struct BTREE_TAG_FAMILY
{
    family* family[MAX+1];
    struct BTREE_TAG_FAMILY* children[MAX+1];
    int no_family;
}btfamily;

//btree expense
typedef struct BTREE_TAG_EXPENSE
{
    expense* expense[MAX+1];
    struct BTREE_TAG_EXPENSE* children[MAX+1];
    int no_expense;
}btexpense;

user* CreateUserN(int user_id,char* name,float income)
{
    //Creating user node
    user* node = (user*)malloc(sizeof(user));
    node->user_id = user_id;
    node->family_id = NULL;
    strcpy(node->name,name);
    node->income = income;
    node->expenses = NULL;
    return node;
}

family* CreateFamilyN(int family_id,char* fname)
{
    //Creating Family Node
    family* node = (family*)malloc(sizeof(family));
    node->family_id = family_id;
    strcpy(node->family_name,fname);
    node->family_income = 0;
    for(int i=0;i<MONTH;i++)
    {
        node->family_expense[i] = 0;
    }
    for(int i=0;i<FAMILY_SIZE;i++)
    {
        node->members[i] = NULL;
    }
    return node;
}

expense* CreateExpenseN(int expense_id , char* cat , float amt , date expdate , user* user)
{
    //Creating Expense node
    expense* node = (expense*)malloc(sizeof(expense));
    node->expense_id = expense_id;
    node->amount = amt;
    node->user1 = user;
    node->expense_date.day  = expdate.day;
    node->expense_date.month = expdate.month;
    node->next = NULL;
    strcpy(node->category,cat);
    return node;
}

btuser* Insert_User_Node(user* user_node , btuser* root);
int PushDownUser(user* new_user , btuser* current , user** med_user , btuser** medright);
void PushInUser(user* meduser, btuser *medright, btuser *current, int pos);
void SplitUser(user* meduser, btuser *medright, btuser *current, int pos,user**newmedian, btuser **newright);

//Searches to the node of the given btuser node
int SearchNodeUser(int user_id, btuser *current, int *pos)
{
    int ret_val;
    //pos is to get reference
    *pos = 0;
    //Goes to 0 if it is less than the 1st id
    if(user_id < current->user[1]->user_id)
    {
        *pos = 0;
        ret_val = 0;
    }
    else
    {
        //goes till where the id will become greter or equal than given
        for(*pos = current->no_user ; user_id < current->user[*pos]->user_id && (*pos)>1 ; (*pos)--);
        ret_val = (user_id == current->user[*pos]->user_id);
    }
    return ret_val;
}

//Searches whole btree to find the root where does it belong else returns null
btuser* SearchTreeUser(int user_id , btuser* root , int* targetpos)
{
    btuser* ret_val;
    if(!root)
    {
        ret_val = NULL;
    }
    else if(SearchNodeUser(user_id,root,targetpos))
    {
        ret_val = root;
    }
    else
    {
        ret_val = SearchTreeUser(user_id,root->children[*targetpos],targetpos);
    }
    return ret_val;
}

//Wrapper function of insertion of user
btuser* Insert_User_Node(user* user_node , btuser* root)
{
    user* meduser;
    btuser* medright;
    btuser* new_root;
    btuser* ret_val = root;

    //if 1 is there of pushdown then create a new root
    if(PushDownUser(user_node,root,&meduser,&medright))
    {
        new_root = (btuser*)malloc(sizeof(btuser));
        new_root->no_user = 1;
        new_root->user[1] = meduser;
        new_root->children[0] = root;
        new_root->children[1] = medright;
        ret_val = new_root;
    }
    return ret_val;
}

int PushDownUser(user* new_user , btuser* current , user** med_user , btuser** medright)
{
    int pos;
    int ret_val = 0;
    //New root to create if null
    if(current == NULL)
    {
        *med_user = new_user;
        *medright = NULL;
        ret_val = 1;
    }
    else
    {
        //If the node is present then it would give an non null which won't go through this
        if(!SearchNodeUser(new_user->user_id,current,&pos))
        {
            //Insert int the position pos as we got it from the searchnodeuser
            if(PushDownUser(new_user,current->children[pos],med_user,medright))
            {
                if(current->no_user < MAX)
                {
                    //If the number of keys are less than max
                    PushInUser(*med_user,*medright,current,pos);
                    ret_val = 0;
                    //No growth of tree
                }
                else
                {
                    SplitUser(*med_user,*medright,current,pos,med_user,medright);
                    ret_val = 1;
                    //Growth of tree by 1
                }
            }
            else
            {
                ret_val = 0;
            }
        }
    }
    return ret_val;
}

void PushInUser(user* meduser, btuser *medright, btuser *current, int pos)
{
    int i;
    //Insert the given node in this tree
    for (i = current->no_user; i > pos; i--) 
    {
        current->user[i + 1] = current->user[i];
        current->children[i + 1] = current->children[i];
    }
    current->user[pos + 1] = meduser;
    current->children[pos + 1] = medright;
    current->no_user++;
}

//Splits the btusernodes according to the position of it
void SplitUser(user* meduser, btuser *medright, btuser *current, int pos,user**newmedian, btuser **newright)
{
    int i, median;

    if (pos <= MIN)
        median = MIN;
    else
        median = MIN + 1;

    *newright = (btuser *)malloc(sizeof(btuser));

    for (i = median + 1; i <= MAX; i++) 
    {
        (*newright)->user[i - median] = current->user[i];
        (*newright)->children[i - median] = current->children[i];
    }

    (*newright)->no_user = MAX - median;
    current->no_user = median;

    if (pos <= MIN)
        PushInUser(meduser, medright, current, pos);
    else
        PushInUser(meduser, medright, *newright, pos - median);

    *newmedian = current->user[current->no_user];
    (*newright)->children[0] = current->children[current->no_user];
    current->no_user--;
}

void RecDeleteTreeUser(int user_id, btuser *current, int rootorno);
void RemoveUser(btuser *current, int pos);
void SuccessorUser(btuser *current, int pos);
void RestoreUser(btuser *current, int pos);
void MoveRightUser(btuser *current, int pos);
void MoveLeftUser(btuser *current, int pos);
void CombineUser(btuser *current, int pos);

//Wrapper function for delete node
btuser *DeleteTreeUser(int user_id, btuser *root) 
{
    btuser *oldroot;
    int pos;

    //Keep of node to delete at last after deletion
    btuser* todeleteNode = SearchTreeUser(user_id,root,&pos);
    user* todelete = todeleteNode->user[pos];
    RecDeleteTreeUser(user_id, root,0);
    if (root && root->no_user == 0) 
    {
        oldroot = root;
        root = root->children[0];
        free(oldroot);
    }
    free(todelete);
    return root;
}

void RecDeleteTreeUser(int user_id, btuser *current, int rootorno)
{
    int pos;
    //If not available then do't do anything
    if (!current) 
    {
    } 
    else
    {
        //checks if it is present in this node
        if (SearchNodeUser(user_id, current, &pos)) 
        {
            //checks position of the node
            if (current->children[pos - 1]) 
            {
                //access succesor and delete it in the leaf
                SuccessorUser(current, pos);
                RecDeleteTreeUser(current->user[pos]->user_id, current->children[pos],rootorno);
                //if deleted from root then no chance to restore and make the nodes property hold so this condition
                if(rootorno == 0)
                {
                    if(current->children[pos]->no_user < MIN)
                    {
                        RestoreUser(current,pos);
                    }
                }
            } 
            else 
            {
                //if at leaf then remove dirctly
                RemoveUser(current, pos);
            }
        } 
        else 
        {
            RecDeleteTreeUser(user_id, current->children[pos],1);
            //checks if after deletion is it following the rule of btree or not else restore
            if (current->children[pos] && current->children[pos]->no_user < MIN) 
            {
                RestoreUser(current, pos);
            }
        }
    }
}

void RemoveUser(btuser *current, int pos) 
{
    int i;
    //simply move all nodes to left from that pos
    for (i = pos + 1; i <= current->no_user; i++) 
    {
        current->user[i - 1] = current->user[i];
        current->children[i - 1] = current->children[i];
    }
    current->no_user--;
}

void SuccessorUser(btuser* current, int pos) 
{
    //takes the immediate right and the number greater than only this and assign it as successor
    btuser* leaf;
    for (leaf = current->children[pos]; leaf->children[0]; leaf = leaf->children[0]);
    current->user[pos] = leaf->user[1];
}

void RestoreUser(btuser *current, int pos) 
{
    //if we have do it at start
    if (pos == 0) 
    {
        //if near by have more than min usernodes then get it from right children
        if (current->children[1]->no_user > MIN)
            MoveLeftUser(current, 1);
        else
            //take it from root and combine both children
            CombineUser(current, 1);
    } 
    else if (pos == current->no_user) 
    {
        //same as above
        if (current->children[pos - 1]->no_user > MIN)
            MoveRightUser(current, pos);
        else
            CombineUser(current, pos);
    } 
    else if (current->children[pos - 1]->no_user > MIN) 
    {
        //if the left have more than min then take from there
        MoveRightUser(current, pos);
    } 
    else if (current->children[pos + 1]->no_user > MIN) 
    {
        //if right have then take it from this
        MoveLeftUser(current, pos + 1);
    } 
    else 
    {
        //else at the last combine
        CombineUser(current, pos);
    }
}

void MoveRightUser(btuser *current, int pos) 
{
    int c;
    btuser *t = current->children[pos];
    //shift to right for all node and child
    for (c = t->no_user; c > 0; c--) 
    {
        t->user[c + 1] = t->user[c];
        t->children[c + 1] = t->children[c];
    }
    //put it in parent
    t->children[1] = t->children[0];
    t->no_user++;
    t->user[1] = current->user[pos];

    //put parents into the node
    t = current->children[pos - 1];
    current->user[pos] = t->user[t->no_user];
    current->children[pos]->children[0] = t->children[t->no_user];
    t->no_user--;
}

void MoveLeftUser(btuser *current, int pos) 
{
    int c;
    btuser *t = current->children[pos - 1];
    //same like moverightuser
    t->no_user++;
    t->user[t->no_user] = current->user[pos];
    t->children[t->no_user] = current->children[pos]->children[0];

    t = current->children[pos];
    current->user[pos] = t->user[1];
    t->children[0] = t->children[1];
    t->no_user--;
    for (c = 1; c <= t->no_user; c++) {
        t->user[c] = t->user[c + 1];
        t->children[c] = t->children[c + 1];
    }
}

void CombineUser(btuser *current, int pos) 
{
    int c;
    btuser *right = current->children[pos];
    btuser *left = current->children[pos - 1];

    //put parents into the left node child of current
    left->no_user++;
    left->user[left->no_user] = current->user[pos];
    left->children[left->no_user] = right->children[0];

    //put all the right nodes into left user node
    for (c = 1; c <= right->no_user; c++) 
    {
        left->no_user++;
        left->user[left->no_user] = right->user[c];
        left->children[left->no_user] = right->children[c];
    }

    //shift all the current nodes by l to left to get it done
    for (c = pos; c < current->no_user; c++) 
    {
        current->user[c] = current->user[c + 1];
        current->children[c] = current->children[c + 1];
    }
    current->no_user--;
    free(right);
}

status_code CreateFamily(btfamily** rootptr, btuser** userptr, btexpense** expenseptr);
status_code Update_Or_Delete_Individual_Family_Details(btuser** ruserptr, btfamily** rfamilyptr, btexpense** rexpenseptr);

//Addition of user to the tree by the menu function
status_code AddUser(btuser** rootptr,btfamily** familyptr, int in_family, btexpense** expenseptr)
{
    status_code sc = SUCCESS;
    int pos;
    int id;
    char name[NAME_SIZE];
    float inc;
    
    printf("Enter the id of the user\n");
    scanf("%d",&id);
    //Checks for the user if present then reply present already
    if(SearchTreeUser(id,*rootptr,&pos))
    {
        printf("User already Present\n");
        sc = FAILURE;
    }
    else
    {
        printf("Enter the name of the user\n");
        scanf("%s",name);
        printf("Enter the income of the user\n");
        scanf("%f",&inc);
        user* new_user = CreateUserN(id,name,inc);
        //Inserts the root user into the tree
        (*rootptr) = Insert_User_Node(new_user,(*rootptr));
        if(!in_family)
        {
            printf("Do you want to add this to family(1) or create a family(0)\n");
            int ip;
            scanf("%d",&ip);
            if(!ip)
            {
                CreateFamily(familyptr,rootptr,expenseptr);
                //Send this to create a family
            }
            else
            {
                Update_Or_Delete_Individual_Family_Details(rootptr,familyptr,expenseptr);
                //Send this to update family
            }
        }
    }
    return sc;
}

//Here after do the same insertion and deletion like the user 
int SearchNodeExpense(int expense_id, btexpense* current, int* pos) 
{
    int ret_val;
    *pos = 0;
    
    if (expense_id < current->expense[1]->expense_id) 
    {
        *pos = 0;
        ret_val = 0;
    }
    else 
    {
        for (*pos = current->no_expense; 
             expense_id < current->expense[*pos]->expense_id && (*pos) > 1; 
             (*pos)--);
        ret_val = (expense_id == current->expense[*pos]->expense_id);
    }
    return ret_val;
}

btexpense* SearchTreeExpense(int expense_id, btexpense* root, int* targetpos) 
{
    btexpense* ret_val;
    
    if (!root) 
    {
        ret_val = NULL;
    }
    else if (SearchNodeExpense(expense_id, root, targetpos)) 
    {
        ret_val = root;
    }
    else 
    {
        ret_val = SearchTreeExpense(expense_id, root->children[*targetpos], targetpos);
    }
    
    return ret_val;
}

void PushInExpense(expense* med_expense, btexpense* med_right, btexpense* current, int pos) 
{
    for (int i = current->no_expense; i > pos; i--) 
    {
        current->expense[i + 1] = current->expense[i];
        current->children[i + 1] = current->children[i];
    }
    current->expense[pos + 1] = med_expense;
    current->children[pos + 1] = med_right;
    current->no_expense++;
}

void SplitExpense(expense* med_expense, btexpense* med_right, btexpense* current, int pos, expense** new_median, btexpense** new_right) 
{
    int median = (pos <= MIN) ? MIN : MIN + 1;
    *new_right = (btexpense*)malloc(sizeof(btexpense));
    (*new_right)->no_expense = 0;

    for (int i = median + 1; i <= MAX; i++) 
    {
        (*new_right)->expense[i - median] = current->expense[i];
        (*new_right)->children[i - median] = current->children[i];
    }
    (*new_right)->no_expense = MAX - median;
    current->no_expense = median;

    if (pos <= MIN) 
    {
        PushInExpense(med_expense, med_right, current, pos);
    } 
    else 
    {
        PushInExpense(med_expense, med_right, *new_right, pos - median);
    }

    *new_median = current->expense[current->no_expense];
    (*new_right)->children[0] = current->children[current->no_expense];
    current->no_expense--;
}

int PushDownExpense(expense* new_expense, btexpense* current, expense** med_expense, btexpense** med_right) 
{
    int pos;
    int ret_val = 0;
    if (!current) 
    {
        *med_expense = new_expense;
        *med_right = NULL;
        return 1;
    }

    if (!SearchNodeExpense(new_expense->expense_id, current, &pos)) 
    {
        if (PushDownExpense(new_expense, current->children[pos], med_expense, med_right)) 
        {
            if (current->no_expense < MAX) 
            {
                PushInExpense(*med_expense, *med_right, current, pos);
                return 0;
            } 
            else 
            {
                SplitExpense(*med_expense, *med_right, current, pos, med_expense, med_right);
                return 1;
            }
        }
    }
    return 0;
}

btexpense* Insert_Expense_Node(expense* new_expense, btexpense* root) 
{
    expense* med_expense;
    btexpense* med_right;

    if (PushDownExpense(new_expense, root, &med_expense, &med_right)) 
    {
        btexpense* new_root = (btexpense*)malloc(sizeof(btexpense));
        new_root->no_expense = 1;
        new_root->expense[1] = med_expense;
        new_root->children[0] = root;
        new_root->children[1] = med_right;
        return new_root;
    }
    return root;
}

void RemoveExpense(btexpense* current, int pos) 
{
    for (int i = pos + 1; i <= current->no_expense; i++) 
    {
        current->expense[i - 1] = current->expense[i];
        current->children[i - 1] = current->children[i];
    }
    current->no_expense--;
}

void SuccessorExpense(btexpense* current, int pos) 
{
    btexpense* leaf = current->children[pos];
    while (leaf->children[0]) 
    {
        leaf = leaf->children[0];
    }
    current->expense[pos] = leaf->expense[1];
}

void MoveRightExpense(btexpense* current, int pos) 
{
    btexpense* right = current->children[pos];
    for (int c = right->no_expense; c > 0; c--) 
    {
        right->expense[c + 1] = right->expense[c];
        right->children[c + 1] = right->children[c];
    }
    right->children[1] = right->children[0];
    right->no_expense++;
    right->expense[1] = current->expense[pos];

    btexpense* left = current->children[pos - 1];
    current->expense[pos] = left->expense[left->no_expense];
    right->children[0] = left->children[left->no_expense];
    left->no_expense--;
}

void MoveLeftExpense(btexpense* current, int pos) 
{
    btexpense* left = current->children[pos - 1];
    btexpense* right = current->children[pos];
    left->no_expense++;
    left->expense[left->no_expense] = current->expense[pos];
    left->children[left->no_expense] = right->children[0];

    current->expense[pos] = right->expense[1];
    right->children[0] = right->children[1];
    right->no_expense--;
    for (int c = 1; c <= right->no_expense; c++) 
    {
        right->expense[c] = right->expense[c + 1];
        right->children[c] = right->children[c + 1];
    }
}

void CombineExpense(btexpense* current, int pos) 
{
    btexpense* right = current->children[pos];
    btexpense* left = current->children[pos - 1];
    left->no_expense++;
    left->expense[left->no_expense] = current->expense[pos];
    left->children[left->no_expense] = right->children[0];

    for (int c = 1; c <= right->no_expense; c++) 
    {
        left->no_expense++;
        left->expense[left->no_expense] = right->expense[c];
        left->children[left->no_expense] = right->children[c];
    }

    for (int c = pos; c < current->no_expense; c++) 
    {
        current->expense[c] = current->expense[c + 1];
        current->children[c] = current->children[c + 1];
    }
    current->no_expense--;
    free(right);
}

void RestoreExpense(btexpense* current, int pos) 
{
    if (pos == 0) 
    {
        if (current->children[1]->no_expense > MIN) 
        {
            MoveLeftExpense(current, 1);
        } 
        else 
        {
            CombineExpense(current, 1);
        }
    } 
    else if (pos == current->no_expense) 
    {
        if (current->children[pos - 1]->no_expense > MIN) 
        {
            MoveRightExpense(current, pos);
        } 
        else 
        {
            CombineExpense(current, pos);
        }
    } 
    else if (current->children[pos - 1]->no_expense > MIN) 
    {
        MoveRightExpense(current, pos);
    } 
    else if (current->children[pos + 1]->no_expense > MIN) 
    {
        MoveLeftExpense(current, pos + 1);
    } 
    else 
    {
        CombineExpense(current, pos);
    }
}

void RecDeleteTreeExpense(expense* expense_id, btexpense* current, int rootorno) 
{
    if (!current) 
    {
        return;
    }
    int pos;
    if (SearchNodeExpense(expense_id->expense_id, current, &pos)) 
    {
        pos = pos+1;
        if (current->children[pos - 1]) 
        {
            SuccessorExpense(current, pos);
            RecDeleteTreeExpense(current->expense[pos], current->children[pos],rootorno);
            if(rootorno == 0)
            {
                if(current->children[pos]->no_expense < MIN)
                {
                    RestoreExpense(current,pos);
                }
            }
        } 
        else 
        {
            RemoveExpense(current, pos);
        }
    } 
    else 
    {
        pos++;
        RecDeleteTreeExpense(expense_id, current->children[pos],1);
        if (current->children[pos] && current->children[pos]->no_expense < MIN) 
        {
            RestoreExpense(current, pos);
        }
    }
}

btexpense* DeleteTreeExpense(expense* expense_id, btexpense* root) 
{
    btexpense* oldroot;
    RecDeleteTreeExpense(expense_id, root,0);
    if (root && root->no_expense == 0) 
    {
        oldroot = root;
        root = root->children[0];
        free(oldroot);
    }
    return root;
}

status_code AddExpense(btexpense** rootptr, btuser** userptr)
{
    status_code sc = SUCCESS;
    int expid,uid;
    printf("Enter the expense id\n");
    scanf("%d",&expid);
    printf("Enter the user id\n");
    scanf("%d",&uid);
    int pos;
    //Searches the user for the expense
    btuser* userbyid = SearchTreeUser(uid,*userptr,&pos);
    //If no user present
    if(!userbyid)
    {
        printf("No such User Present\n");
        sc =FAILURE;
    }
    else
    {
        user* usernode = userbyid->user[pos];
        btexpense* ExpenseFind = SearchTreeExpense(expid,*rootptr,&pos);
        //If the node is not null and it is present initially
        if(ExpenseFind)
        {
            printf("Expense Already Exists\n");
            sc = FAILURE;
        }
        else
        {
            char cat[CATEGORY_SIZE];
            float amt;
            int day;
            int month;
            printf("Enter Category of expense\n");
            scanf("%s",cat);
            //checks if the input cat is valid or not
            if(strcmp(cat,"Rent") == 0 || strcmp(cat,"Utility") == 0 || strcmp(cat,"Stationary") == 0 || strcmp(cat,"Grocery") == 0 || strcmp(cat,"Leisure") == 0)
            {
                printf("Enter the amount of expense\n");
                scanf("%f",&amt);
                printf("Enter the day and month of expense\n");
                scanf("%d%d",&day,&month);
                date new_date;
                new_date.day = day;
                new_date.month = month;
                expense* new_exp = CreateExpenseN(expid,cat,amt,new_date,usernode);
                //Insert this expense to the expense btree tree
                *rootptr = Insert_Expense_Node(new_exp,*rootptr);
                new_exp->next = usernode->expenses;
                usernode->expenses = new_exp;
                usernode->family_id->family_expense[month-1] += amt;
            }
            else
            {
                printf("Invalid Category\n");
                sc = FAILURE;
            }
        }
    }
    return sc;
}

//Do the same form family id as of like user in btree insertion and deletion
int SearchNodeFamily(int family_id, btfamily *current, int *pos) 
{
    int ret_val;
    if(family_id < current->family[1]->family_id) 
    {
        *pos = 0;
        ret_val = 0;
    }
    else 
    {
        for(*pos = current->no_family; family_id < current->family[*pos]->family_id && (*pos)>1; (*pos)--);
        ret_val = (family_id == current->family[*pos]->family_id);
    }
    return ret_val;
}

btfamily* SearchTreeFamily(int family_id, btfamily *root, int *targetpos) 
{
    btfamily* ret_val;
    if(!root) 
    {
        ret_val = NULL;
    }
    else if(SearchNodeFamily(family_id, root, targetpos)) 
    {
        ret_val = root;
    }
    else 
    {
        ret_val = SearchTreeFamily(family_id, root->children[*targetpos], targetpos);
    }
    return ret_val;
}

void PushInFamily(family* medfamily, btfamily *medright, btfamily *current, int pos) 
{
    int i;
    for (i = current->no_family; i > pos; i--) 
    {
        current->family[i + 1] = current->family[i];
        current->children[i + 1] = current->children[i];
    }
    current->family[pos + 1] = medfamily;
    current->children[pos + 1] = medright;
    current->no_family++;
}

void SplitFamily(family* medfamily, btfamily *medright, btfamily *current, int pos, family** newmedian, btfamily **newright) 
{
    int i, median;

    if (pos <= MIN)
        median = MIN;
    else
        median = MIN + 1;

    *newright = (btfamily *)malloc(sizeof(btfamily));

    for (i = median + 1; i <= MAX; i++) 
    {
        (*newright)->family[i - median] = current->family[i];
        (*newright)->children[i - median] = current->children[i];
    }

    (*newright)->no_family = MAX - median;
    current->no_family = median;

    if (pos <= MIN)
        PushInFamily(medfamily, medright, current, pos);
    else
        PushInFamily(medfamily, medright, *newright, pos - median);

    *newmedian = current->family[current->no_family];
    (*newright)->children[0] = current->children[current->no_family];
    current->no_family--;
}

int PushDownFamily(family* new_family, btfamily *current, family** med_family, btfamily** medright) 
{
    int pos;
    int ret_val;
    if(current == NULL) 
    {
        *med_family = new_family;
        *medright = NULL;
        ret_val = 1;
    }
    else 
    {
        if(!SearchNodeFamily(new_family->family_id, current, &pos)) 
        {
            if(PushDownFamily(new_family, current->children[pos], med_family, medright)) 
            {
                if(current->no_family < MAX) 
                {
                    PushInFamily(*med_family, *medright, current, pos);
                    ret_val = 0;
                }
                else 
                {
                    SplitFamily(*med_family, *medright, current, pos, med_family, medright);
                    ret_val = 1;
                }
            }
            else 
            {
                ret_val = 0;
            }
        }
    }
    return ret_val;
}

btfamily* Insert_Family_Node(family* family_node, btfamily* root) 
{
    family* medfamily;
    btfamily* medright;
    btfamily* new_root;
    btfamily* ret_val = root;

    if(PushDownFamily(family_node, root, &medfamily, &medright)) 
    {
        new_root = (btfamily*)malloc(sizeof(btfamily));
        new_root->no_family = 1;
        new_root->family[1] = medfamily;
        new_root->children[0] = root;
        new_root->children[1] = medright;
        ret_val = new_root;
    }
    return ret_val;
}

void RemoveFamily(btfamily *current, int pos) 
{
    int i;
    for (i = pos + 1; i <= current->no_family; i++) 
    {
        current->family[i - 1] = current->family[i];
        current->children[i - 1] = current->children[i];
    }
    current->no_family--;
}

void SuccessorFamily(btfamily* current, int pos) 
{
    btfamily* leaf;
    for (leaf = current->children[pos]; leaf->children[0]; leaf = leaf->children[0]);
    current->family[pos] = leaf->family[1];
}

void MoveRightFamily(btfamily *current, int pos) 
{
    int c;
    btfamily *t = current->children[pos];
    for (c = t->no_family; c > 0; c--) 
    {
        t->family[c + 1] = t->family[c];
        t->children[c + 1] = t->children[c];
    }
    t->children[1] = t->children[0];
    t->no_family++;
    t->family[1] = current->family[pos];

    t = current->children[pos - 1];
    current->family[pos] = t->family[t->no_family];
    current->children[pos]->children[0] = t->children[t->no_family];
    t->no_family--;
}

void MoveLeftFamily(btfamily *current, int pos) 
{
    int c;
    btfamily *t = current->children[pos - 1];
    t->no_family++;
    t->family[t->no_family] = current->family[pos];
    t->children[t->no_family] = current->children[pos]->children[0];

    t = current->children[pos];
    current->family[pos] = t->family[1];
    t->children[0] = t->children[1];
    t->no_family--;
    for (c = 1; c <= t->no_family; c++) 
    {
        t->family[c] = t->family[c + 1];
        t->children[c] = t->children[c + 1];
    }
}

void CombineFamily(btfamily *current, int pos) 
{
    int c;
    btfamily *right = current->children[pos];
    btfamily *left = current->children[pos - 1];

    left->no_family++;
    left->family[left->no_family] = current->family[pos];
    left->children[left->no_family] = right->children[0];

    for (c = 1; c <= right->no_family; c++) 
    {
        left->no_family++;
        left->family[left->no_family] = right->family[c];
        left->children[left->no_family] = right->children[c];
    }

    for (c = pos; c < current->no_family; c++) 
    {
        current->family[c] = current->family[c + 1];
        current->children[c] = current->children[c + 1];
    }
    current->no_family--;
    free(right);
}

void RestoreFamily(btfamily *current, int pos) 
{
    if (pos == 0) 
    {
        if (current->children[1]->no_family > MIN)
            MoveLeftFamily(current, 1);
        else
            CombineFamily(current, 1);
    } 
    else if (pos == current->no_family) 
    {
        if (current->children[pos - 1]->no_family > MIN)
            MoveRightFamily(current, pos);
        else
            CombineFamily(current, pos);
    } 
    else if (current->children[pos - 1]->no_family > MIN) 
    {
        MoveRightFamily(current, pos);
    } 
    else if (current->children[pos + 1]->no_family > MIN) 
    {
        MoveLeftFamily(current, pos + 1);
    } 
    else 
    {
        CombineFamily(current, pos);
    }
}

void RecDeleteTreeFamily(int family_id, btfamily *current, int rootornno) 
{
    int pos;
    if (!current) 
    {
    }
    else {
        if (SearchNodeFamily(family_id, current, &pos)) 
        {
            if (current->children[pos - 1]) 
            {
                SuccessorFamily(current, pos);
                RecDeleteTreeFamily(current->family[pos]->family_id, current->children[pos],rootornno);
                if(rootornno == 0)
                {
                    if(current->children[pos]->no_family < MIN)
                    {
                        RestoreFamily(current,pos);
                    }
                }
            } 
            else 
            {
                RemoveFamily(current, pos);
            }
        } 
        else 
        {
            RecDeleteTreeFamily(family_id, current->children[pos],1);
            if (current->children[pos] && current->children[pos]->no_family < MIN) 
            {
                RestoreFamily(current, pos);
            }
        }
    }
}

btfamily* DeleteTreeFamily(int family_id, btfamily *root) 
{
    btfamily *oldroot;
    int pos;
    btfamily* todeleteNode = SearchTreeFamily(family_id, root, &pos);
    family* todelete = todeleteNode->family[pos];
    RecDeleteTreeFamily(family_id, root,0);
    if (root && root->no_family == 0) 
    {
        oldroot = root;
        root = root->children[0];
        free(oldroot);
    }
    free(todelete);
    return root;
}

//Creation of family from the given info
status_code CreateFamily(btfamily** rootptr, btuser** userptr, btexpense** expenseptr)
{
    status_code sc = SUCCESS;
    int flag = 0;
    int i=0;
    //Takes the input from the user
    int family_id;
    char family_name[NAME_SIZE];
    user* members[FAMILY_SIZE];
    float income = 0;
    float expense = 0;
    printf("Enter the family id\n");
    scanf("%d",&family_id);
    printf("Enter the name of family\n");
    scanf("%s",family_name);
    //creates the family node bby this info
    family* familynode = CreateFamilyN(family_id,family_name);
    int pos = 0;
    //If family already exists then return FAILURE
    if(SearchTreeFamily(family_id,*rootptr,&pos))
    {
        printf("Family Already exists\n");
        sc = FAILURE;
    }
    else
    {
        printf("Enter the user ids of the family users\n");
        for(i=0; i<FAMILY_SIZE && (!flag); i++)
        {
            user* member;
            int uid;
            printf("Enter the user id if no more users then enter -1\n");
            scanf("%d",&uid);
            //Takes uid and checks if it exists or not if not then pass it to adduser
            if(uid == -1)
            {
                flag = 0;
            }
            else
            {
                if(!SearchTreeUser(uid,*userptr,&pos))
                {
                    printf("Add this user first\n");
                    AddUser(userptr,rootptr,1,expenseptr);
                }
                btuser* usernodebt = SearchTreeUser(uid,*userptr,&pos);
                user* usernode = usernodebt->user[pos];
                familynode->family_income += usernode->income;
                familynode->members[i] = usernode;
                usernode->family_id = familynode;
            }
        }
        //Insert this family node
        *rootptr = Insert_Family_Node(familynode,*rootptr);
    }
    return sc;
}

status_code Update_Or_Delete_Individual_Family_Details(btuser** ruserptr, btfamily** rfamilyptr, btexpense** rexpenseptr)
{
    status_code sc = SUCCESS;
    int iorf,uord,pos;
    printf("Do you want to do functions on user(1) or family(0)\n");
    scanf("%d",&iorf);
    if(iorf)
    {
        printf("Do you want to update(1) or delete(0)\n");
        scanf("%d",&uord);
        int uid;
        printf("Enter the User id\n");
        scanf("%d",&uid);
        btuser* btusernode = SearchTreeUser(uid,*ruserptr,&pos);
        if(!btusernode)
        {
            printf("This user do not Exist\n");
            sc = FAILURE;
        }
        else
        {
            if(uord)
            {
                user* usernode = btusernode->user[pos];
                printf("Enter the new Income and Name of the user\n");
                float inc;
                char name[NAME_SIZE];
                scanf("%f",&inc);
                scanf("%s",name);
                usernode->family_id->family_income -= usernode->income;
                usernode->family_id->family_income += inc;
                usernode->income = inc;
                strcpy(usernode->name,name);
            }
            else
            {
                user* usernode = btusernode->user[pos];
                family* userfamily = usernode->family_id;
                int count = 0;
                while(userfamily->members[count])
                {
                    count++;
                }
                if(count != 1)
                {
                    for(int i=0;i<count-1;i++)
                    {
                        userfamily->members[i] = userfamily->members[i+1];
                    }
                    userfamily->family_income -= usernode->income;
                    expense* temp = usernode->expenses;
                    while(temp!=NULL)
                    {
                        userfamily->family_expense[temp->expense_date.month - 1] -= temp->amount;
                        temp = temp->next;
                    }
                    temp = usernode->expenses;
                    while(temp!= NULL)
                    {
                        expense* todelete = temp;
                        temp = temp->next;
                        (*rexpenseptr) = DeleteTreeExpense(todelete,*rexpenseptr);
                    }
                    (*ruserptr) = DeleteTreeUser(usernode->user_id,(*ruserptr));
                }
                else
                {
                    expense* temp = usernode->expenses;
                    while(temp!= NULL)
                    {
                        expense* todelete = temp;
                        temp = temp->next;
                        (*rexpenseptr) = DeleteTreeExpense(todelete,*rexpenseptr);
                    }
                    (*rfamilyptr) = DeleteTreeFamily(userfamily->family_id,*rfamilyptr);
                    (*ruserptr) = DeleteTreeUser(usernode->user_id,(*ruserptr));
                }
            }
        }
        
    }
    else
    {
        printf("Do you want to update(1) or delete(0)\n");
        scanf("%d",&uord);
        int fid;
        printf("Enter the family id\n");
        scanf("%d",&fid);
        btfamily* btfamilynode = SearchTreeFamily(fid,(*rfamilyptr),&pos);
        if(!btfamilynode)
        {
            printf("No such family Exists\n");
            sc = FAILURE;
        }
        else
        {
            if(uord)
            {
                family* familynode = btfamilynode->family[pos];
                int count = 0;
                while(familynode->members[count])
                {
                    count++;
                }
                if(count == FAMILY_SIZE)
                {
                    printf("There is no extra space for new member\n");
                    sc = FAILURE;
                }
                else
                {
                    int uid;
                    printf("Enter the id of user\n");
                    scanf("%d",&uid);
                    btuser* btusernode = SearchTreeUser(uid,(*ruserptr),&pos);
                    if(!btusernode)
                    {
                        printf("Add this user first\n");
                        AddUser(ruserptr,rfamilyptr,1,rexpenseptr);
                    }
                    btusernode = SearchTreeUser(uid,(*ruserptr),&pos);
                    user* usernode = btusernode->user[pos];
                    familynode->family_income += usernode->income;
                    usernode->family_id = familynode;
                    familynode->members[count] = usernode;
                }
            }
            else
            {
                family* familynode = btfamilynode->family[pos];
                int count = 0;
                while(familynode->members[count])
                {
                    count++;
                }
                for(int i=0; i<count; i++)
                {
                    expense* temp = familynode->members[i]->expenses;
                    while(temp!=NULL)
                    {
                        expense* todeletee = temp;
                        temp = temp->next;
                        (*rexpenseptr) = DeleteTreeExpense(todeletee,(*rexpenseptr));
                    }
                    (*ruserptr) = DeleteTreeUser(familynode->members[i]->user_id,(*ruserptr));
                }
                (*rfamilyptr) = DeleteTreeFamily(familynode->family_id,(*rfamilyptr));
            }
        }
    }
    return sc;
}

status_code Updat_Delete_Expense(btexpense** rexpenseptr, btuser** ruserptr)
{
    status_code sc = SUCCESS;
    printf("Do you want to update(1) or delete(0)\n");
    int uord;
    scanf("%d",&uord);
    if(uord)
    {
        int expid,uid;
        printf("Enter the expense id\n");
        scanf("%d",&expid);
        printf("Enter the user id\n");
        scanf("%d",&uid);
        int pos;
        btuser* btusernode = SearchTreeUser(uid,(*ruserptr),&pos);
        user* usernode = btusernode->user[pos];
        btexpense* btexpensenode = SearchTreeExpense(expid,(*rexpenseptr),&pos);
        if(!btexpensenode)
        {
            printf("No such Expense exists\n");
            sc = FAILURE;
        }
        else
        {
            expense* expensenode = btexpensenode->expense[pos];
            expensenode->user1->family_id->family_expense[expensenode->expense_date.month - 1] -= expensenode->amount;
            printf("Enter the new expense amount\n");
            float amt;
            scanf("%f",&amt);
            expensenode->amount = amt;
            expensenode->user1->family_id->family_expense[expensenode->expense_date.month - 1] += expensenode->amount;
        }
        
    }
    else
    {
        int expid,uid;
        printf("Enter the expense id\n");
        scanf("%d",&expid);
        printf("Enter the user id\n");
        scanf("%d",&uid);
        int pos;
        btuser* btusernode = SearchTreeUser(uid,(*ruserptr),&pos);
        user* usernode = btusernode->user[pos];
        btexpense* btexpensenode = SearchTreeExpense(expid,(*rexpenseptr),&pos);
        if(!btexpensenode)
        {
            printf("No such Expense exists\n");
            sc = FAILURE;
        }
        else
        {
            expense* expensenode = btexpensenode->expense[pos];
            expense* prev = NULL,*curr;
            expensenode->user1->family_id->family_expense[expensenode->expense_date.month - 1] -= expensenode->amount;
            curr = usernode->expenses;
            while(curr->expense_id != expensenode->expense_id)
            {
                prev = curr;
                curr = curr->next;
            }
            if(prev == NULL)
            {
                usernode->expenses = curr->next;
            }
            else
            {
                prev->next = curr->next;
            }
            free(curr);
        }
    }
    return sc;
}

status_code Get_Total_Expense(btfamily** rfamilyptr)
{
    status_code sc = SUCCESS;
    int fid,pos;
    printf("Enter the family id\n");
    scanf("%d",&fid);
    btfamily* btfamilynode = SearchTreeFamily(fid,(*rfamilyptr),&pos);
    if(!btfamilynode)
    {
        printf("No such family Exists\n");
        sc = FAILURE;
    }
    else
    {
        family* familynode = btfamilynode->family[pos];
        int month;
        printf("Enter the month for the total expense\n");
        scanf("%d",&month);
        float expamt = familynode->family_expense[month-1];
        printf("The total family expense for a month %d is %f\n",month,expamt);
        if(expamt>familynode->family_income)
        {
            printf("Epenses surpass by %f",expamt-familynode->family_income);
        }
        else
        {
            printf("Expenses are in limit with saving of %f",familynode->family_income-expamt);
        }
    }
    return sc;
}

status_code Get_Categorical_Expense(btfamily** rfamilyptr)
{
    status_code sc = SUCCESS;
    int fid;
    printf("Enter the family id\n");
    scanf("%d",&fid);
    int pos;
    btfamily* btfamilynode = SearchTreeFamily(fid,(*rfamilyptr),&pos);
    if(!btfamilynode)
    {
        printf("No such family Exists\n");
        sc = FAILURE;
    }
    else
    {
        family* familynode = btfamilynode->family[pos];
        printf("Enter the category\n");
        char cat[CATEGORY_SIZE];
        scanf("%s",cat);
        if(strcmp(cat,"Rent") == 0 || strcmp(cat,"Utility") == 0 || strcmp(cat,"Stationary") == 0 || strcmp(cat,"Grocery") == 0 || strcmp(cat,"Leisure") == 0)
        {
            typedef struct CatExp_Tag
            {
                int uid;
                float amt;
            }catexp;
            int count = 0;
            while(familynode->members[count] != NULL)
            {
                count++;
            }
            catexp exps[count];
            //printf("1\n");
            int i=0;
            float ans = 0;
            int flag = 0;
            while(familynode->members[i] != NULL && i<count && !flag)
            {
                expense* temp = familynode->members[i]->expenses;
                exps[i].uid = familynode->members[i]->user_id;
                exps[i].amt = 0;
                //printf("%d\n",exps[i].uid);
                while(temp != NULL)
                {
                    if(strcmp(temp->category,cat) == 0)
                    {
                        exps[i].amt += temp->amount;
                    }
                    temp = temp->next;
                }
                ans += exps[i].amt;
                i++;
                if(flag == FAMILY_SIZE)
                {
                    flag = 1;
                }
            }
            for (int j = 0; j < count - 1; j++)
            {
                for(int k=0 ; k<count - j - 1; k++)
                {
                    if(exps[k].amt < exps[k+1].amt)
                    {
                        float amt = exps[k].amt;
                        exps[k].amt = exps[k+1].amt;
                        exps[k+1].amt = amt;

                        int id = exps[k].uid;
                        exps[k].uid = exps[k+1].uid;
                        exps[k+1].uid = id;
                    }
                }
            }
            //printf("Hi\n");
            printf("Total expense for %s category is %f\n",cat,ans);
            for(int j=0; j<count; j++)
            {
                printf("User with id %d have expense as %f\n",exps[j].uid,exps[j].amt);
            }
        }
        else
        {
            printf("No such category exists check spelling again\n");
            sc = FAILURE;
        }
    }
    return sc;
}

status_code Get_Highest_Expense_Day(btfamily** rfamilyptr)
{
    status_code sc = SUCCESS;
    int fid;
    printf("Enter the family id\n");
    scanf("%d",&fid);
    int pos;
    btfamily* btfamilynode = SearchTreeFamily(fid,(*rfamilyptr),&pos);
    if(!btfamilynode)
    {
        printf("No such family Exists\n");
        sc = FAILURE;
    }
    else
    {
        family* familynode = btfamilynode->family[pos];
        int maxday = -1,maxmonth = -1;
        float maxamount = -1;
        int count = 0;
        while(familynode->members[count])
        {
           count++;
        }
        for(int i=0 ; i<DAY ; i++)
        {
            for(int j=0 ; j<MONTH ; j++)
            {
                float amount = 0;
                for(int o=0 ; o<count ; o++)
                {
                    user* usernode = familynode->members[o];
                    expense* temp = usernode->expenses;
                    while(temp != NULL)
                    {
                        if(temp->expense_date.day == i+1 && temp->expense_date.month == j+1)
                        {
                            amount += temp->amount;
                        }
                        temp = temp->next;
                    }
                }
                if(amount > maxamount)
                {
                    maxamount = amount;
                    maxday = i+1;
                    maxmonth = j+1;
                }
            }
        }
        printf("The maximum Expense was done on %d day and %d month and was %f rupees\n",maxday,maxmonth,maxamount);
    }
    return sc;
}

status_code Get_Individual_Expense(btuser** ruserptr)
{
    status_code sc = SUCCESS;
    int uid,pos;
    printf("Enter the used id\n");
    scanf("%d",&uid);
    btuser* btusernode = SearchTreeUser(uid,(*ruserptr),&pos);
    if(!btusernode)
    {
        printf("No such user present\n");
        sc = FAILURE;
    }
    else
    {
        user* usernode = btusernode->user[pos];
        int month;
        printf("Enter the month for the expense\n");
        scanf("%d",&month);
        typedef struct exps_tag
        {
            float amt;
            char cat[NAME_SIZE];
        }exps;
        exps uexps[CATEGORY_SIZE];
        strcpy(uexps[0].cat,"Rent");
        strcpy(uexps[1].cat,"Utility");
        strcpy(uexps[2].cat,"Grocery");
        strcpy(uexps[3].cat,"Stationary");
        strcpy(uexps[4].cat,"Leisure");

        for(int i=0; i<NO_CATEGORY ; i++)
        {
            uexps[i].amt = 0;
        }
        expense* temp = usernode->expenses;
        while(temp != NULL)
        {
            if(temp->expense_date.month == month)
            {
                for(int i=0 ; i<CATEGORY_SIZE ; i++)
                {
                    if(strcmp(uexps[i].cat,temp->category) == 0)
                    {
                        uexps[i].amt += temp->amount;
                    }
                }
            }
            temp = temp->next;
        }

        for(int j=0 ; j < NO_CATEGORY-1; j++)
        {
            for(int k=0 ; k<NO_CATEGORY-j-1 ; k++)
            {
                if(uexps[k].amt > uexps[k+1].amt)
                {
                    float temp = uexps[k].amt;
                    uexps[k].amt = uexps[k+1].amt;
                    uexps[k+1].amt = temp;

                    char name[CATEGORY_SIZE];
                    strcpy(name,uexps[k].cat);
                    strcpy(uexps[k].cat,uexps[k+1].cat);
                    strcpy(uexps[k+1].cat,name);
                }
            }
        }
        float total_expense = 0;
        for(int i=NO_CATEGORY-1 ; i>=0 ; i--)
        {
            total_expense += uexps[i].amt;
            printf("Category %s and expense amount is %f\n",uexps[i].cat,uexps[i].amt);
        }
        printf("Total expense for the month %d is %f\n",month,total_expense);
    }
    return sc;
}

void print_Expenses(btexpense* current , date date1 , date date2)
{
    if(current)
    {
        print_Expenses(current->children[0],date1,date2);
        for(int i=1 ; i<=current->no_expense ; i++)
        {
            expense* node = current->expense[i];
            if((node->expense_date.month < date2.month || (node->expense_date.month == date2.month && node->expense_date.day < date2.day)) &&
            (node->expense_date.month > date1.month || (node->expense_date.month == date1.month && node->expense_date.day > date1.day)) )
            {
                printf("Expense id is %d and amount is %f\n",node->expense_id,node->amount);
            }
            print_Expenses(current->children[i],date1,date2);
        }
    }
}

status_code Get_Expense_In_Period(btexpense** rexpenseptr)
{
    status_code sc = SUCCESS;
    int day1,month1,day2,month2;
    printf("Enter the date and month of both dates\n");
    scanf("%d%d%d%d",&day1,&month1,&day2,&month2);
    date date1,date2;
    date1.day = day1;
    date1.month = month1;
    date2.day = day2;
    date2.month = month2;
    print_Expenses((*rexpenseptr),date1,date2);
    return sc;
}

status_code Get_Expense_In_Range(btexpense** rexpenseptr, btuser** ruserptr)
{
    status_code sc = SUCCESS;
    int pos,uid;
    printf("Enter the user id\n");
    scanf("%d",&uid);
    btuser* btusernode = SearchTreeUser(uid,(*ruserptr),&pos);
    if(!btusernode)
    {
        printf("User not present with this id\n");
        sc = FAILURE;
    }
    else
    {
        user* usernode = btusernode->user[pos];
        printf("Enter the expense ids for finding the range\n");
        int exp1,exp2;
        scanf("%d%d",&exp1,&exp2);
        expense* temp = usernode->expenses;
        temp = usernode->expenses;
        while(temp != NULL)
        {
            if(temp->expense_id < exp2 && temp->expense_id > exp1)
            {
                printf("%d id of expense with amount as %f\n",temp->expense_id,temp->amount);
            }
            temp = temp->next;
        }
    }
    return sc;
}

void Input_Family(btfamily** rfamilyptr)
{
    FILE* file2 = fopen("families.csv","r");
    char line2[256];
    if(file2 == NULL)
    {
        printf("Unable to open family file\n");
    }
    else
    {
        while(fgets(line2, sizeof(line2), file2))
        {
            int family_id;
            char fname[NAME_SIZE];
            //Takes the input
            if(sscanf(line2, "%d,%49[^,]",&family_id,fname) == 2)
            {
                family* node = CreateFamilyN(family_id,fname);
                (*rfamilyptr) = Insert_Family_Node(node,(*rfamilyptr));
            }
        }
        fclose(file2);
    }
    
}

void Input_User(btuser** ruserptr, btfamily** rfamilyptr)
{
    FILE* file1 = fopen("users.csv","r");
    if (file1 == NULL) {
        printf("Unable to open users file\n");
    }
    else
    {
        char line1[256];
        while (fgets(line1, sizeof(line1), file1))
        {
            int uid, fid;
            char name[NAME_SIZE];
            float income;

            if (sscanf(line1, "%d,%49[^,],%f,%d", &uid, name, &income , &fid) == 4)
            {
                user* usernode = CreateUserN(uid,name,income);
                int pos;
                btfamily* btfamilynode = SearchTreeFamily(fid,(*rfamilyptr),&pos);
                if(btfamilynode)
                {
                    family* familynode = btfamilynode->family[pos];
                    usernode->family_id = familynode;
                    int count = 0;
                    while(familynode->members[count] != NULL)
                    {
                        count++;
                    }
                    familynode->members[count] = usernode;
                    familynode->family_income += usernode->income;
                    (*ruserptr) = Insert_User_Node(usernode,(*ruserptr));
                }
            }
        }
        fclose(file1);
    }
    
}

void Input_Expense(btuser** ruserptr , btexpense** rexpenseptr)
{
    FILE* file3 = fopen("expenses.csv", "r");
    if (file3 == NULL) 
    {
        printf("Unable to open expenses file\n");
    }
    else
    {
        char line3[256];
        //Go until the file ends
        while (fgets(line3, sizeof(line3), file3))
        {
            int exp_id, uid, day, month;
            char cat[CATEGORY_SIZE];
            float amt;
            //printf("1\n");
            if (sscanf(line3, "%d,%d,%14[^,\n],%f,%d,%d", &exp_id, &uid, cat, &amt, &month, &day) == 6)
            {
                //Takes input and assigns it
                date edate;
                edate.day = day;
                edate.month = month;
                //printf("2\n");
                int pos;
                btuser* btusernode = SearchTreeUser(uid,(*ruserptr),&pos);
                if(btusernode && pos >= 1 && pos <= btusernode->no_user)
                {
                    //printf("3\n");
                    user* usernode = btusernode->user[pos];
                    expense* node = CreateExpenseN(exp_id,cat,amt,edate,usernode);
                    node->next = usernode->expenses;
                    usernode->expenses = node;
                    family* familynode = usernode->family_id;
                    familynode->family_expense[month-1] += amt;
                    (*rexpenseptr) = Insert_Expense_Node(node,(*rexpenseptr));
                    //printf("4\n");
                }
            }
        }
        fclose(file3);
    }
    
}

int main()
{
    btuser* User_root = NULL;
    btexpense* Expense_root = NULL;
    btfamily* Family_root = NULL;
    
    Input_Family(&Family_root);
    
    Input_User(&User_root,&Family_root);
   
    Input_Expense(&User_root,&Expense_root);
    

    int menu_code,exitloop = 1;
    while(exitloop)
    {
        printf("\tExpense Tracking System\n");
        printf("Menu\n1.Add User\n2.Add Expense\n3.Create Family\n4.Update or delete individual family details\n5.Update or delete expense\n6.Get Total expense\n7.Get categorical expense\n8.Get highest expense day\n9.Get individual expense\n10.Get Expense in Period\n11.Get Expense in Range\n12.Exit\n");
        printf("Enter number between 1 to 12\n");
        scanf("%d",&menu_code);
        if(menu_code < 1 && menu_code > 12)
        {
            printf("Invalid input");
        }
        else
        {
            switch (menu_code)
            {
            case 1:
            {
                status_code sc;
                sc = AddUser(&User_root,&Family_root,0,&Expense_root);
                if(sc == FAILURE)
                {
                    printf("Operation unsuccessful\n");
                }
                else
                {
                    printf("Operation successful\n");
                }
                break;
            }
            
            case 2:
            {
                status_code sc;
                sc = AddExpense(&Expense_root,&User_root);
                if(sc == FAILURE)
                {
                    printf("Operation unsuccessful\n");
                }
                else
                {
                    printf("Operation successful\n");
                }
                break;
            }

            case 3:
            {
                status_code sc;
                sc = CreateFamily(&Family_root,&User_root,&Expense_root);
                if(sc == FAILURE)
                {
                    printf("Operation unsuccessful\n");
                }
                else
                {
                    printf("Operation successful\n");
                }
                break;
            }

            case 4:
            {
                status_code sc;
                sc = Update_Or_Delete_Individual_Family_Details(&User_root,&Family_root,&Expense_root);
                if(sc == FAILURE)
                {
                    printf("Operation unsuccessful\n");
                }
                else
                {
                    printf("Operation successful\n");
                }
                break;
            }

            case 5:
            {
                status_code sc;
                sc = Updat_Delete_Expense(&Expense_root,&User_root);
                if(sc == FAILURE)
                {
                    printf("Operation unsuccessful\n");
                }
                else
                {
                    printf("Operation successful\n");
                }
                break;
            }

            case 6:
            {
                status_code sc;
                sc = Get_Total_Expense(&Family_root);
                if(sc == FAILURE)
                {
                    printf("Operation unsuccessful\n");
                }
                else
                {
                    printf("Operation successful\n");
                }
                break;
            }

            case 7:
            {
                status_code sc;
                sc = Get_Categorical_Expense(&Family_root);
                if(sc == FAILURE)
                {
                    printf("Operation unsuccessful\n");
                }
                else
                {
                    printf("Operation successful\n");
                }
                break;
            }

            case 8:
            {
                status_code sc;
                sc = Get_Highest_Expense_Day(&Family_root);
                if(sc == FAILURE)
                {
                    printf("Operation unsuccessful\n");
                }
                else
                {
                    printf("Operation successful\n");
                }
                break;
            }

            case 9:
            {
                status_code sc;
                sc = Get_Individual_Expense(&User_root);
                if(sc == FAILURE)
                {
                    printf("Operation unsuccessful\n");
                }
                else
                {
                    printf("Operation successful\n");
                }
                break;
            }

            case 10:
            {
                status_code sc;
                sc = Get_Expense_In_Period(&Expense_root);
                if(sc == FAILURE)
                {
                    printf("Operation unsuccessful\n");
                }
                else
                {
                    printf("Operation successful\n");
                }
                break;
            }

            case 11:
            {
                status_code sc;
                sc = Get_Expense_In_Range(&Expense_root,&User_root);
                if(sc == FAILURE)
                {
                    printf("Operation unsuccessful\n");
                }
                else
                {
                    printf("Operation successful\n");
                }
                break;
            }
            
            case 12:
                exitloop = 0;
                break;
            default:
                break;
            }
        }
    }
}

