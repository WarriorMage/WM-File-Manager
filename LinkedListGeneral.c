#include "LinkedListGeneral.h"
#include <stdlib.h>
#include <string.h>

typedef struct node
{
    void *data;
    struct node *next;
} node;

typedef struct list
{
    node *head;
    size_t element_size;
    size_t list_length;
} list;

list *createlist(long element_size)
{
    list *newlist = malloc(sizeof(list));
    if (!newlist || element_size < 0)
        return NULL;
    newlist->head = NULL;
    newlist->element_size = (size_t) element_size;
    newlist->list_length = 0;
    return newlist;
}

size_t length(const list *sizelist)
{
    return sizelist->list_length;
}

void freelist(list *dellist)
{
    if (dellist == NULL)
        return;
    node *temp = NULL;
    while (dellist->head != NULL)
    {
        temp = dellist->head->next;
        free(dellist->head->data);
        free(dellist->head);
        dellist->head = temp;
        dellist->list_length--;
    }
    free(dellist);
}

// Loop this function till you get NULL, deference the return value after typecasting
void *getnode(const list *travlist, long pos)
{
    if (!travlist || pos < 0 || pos > (long)(travlist->list_length) - 1)
        return NULL;
    node *travnode = travlist->head;
    for (size_t i = 0; i < pos; i++)
    {
        travnode = travnode->next;
    }
    return travnode->data;
}

retcode setnode(list *inslist, long pos, const void *data)
{
    if (!inslist)
        return INVINP;
    if (pos < 0 || pos > (long)(inslist->list_length) - 1)
        return INVIND;
    if (!data)
        return INVINP;

    node *trav = inslist->head;
    for (long i = 0; i < pos; i++)
        trav = trav->next;
    memcpy(trav->data, data, inslist->element_size);
    return SUCCESS;
}

retcode insertnode(list *inslist, long pos, const void *data)
{
    // Node index starts at 0
    if (pos < -1 || pos > (long) inslist->list_length)
        return INVIND;
    if (!inslist)
        return INVINP;
    if (!data)
        return INVINP;

    node *trav = inslist->head;
    node *newnode = (node *)malloc(sizeof(node));
    if (!newnode)
        return NOALLOC;

    if (pos == 0 || trav == NULL)
    {
        newnode->data = malloc(inslist->element_size);
        if (!newnode->data)
        {
            free(newnode);
            return NOALLOC;
        }
        memcpy(newnode->data, data, inslist->element_size);
        newnode->next = trav;
        inslist->head = newnode;
    }
    else
    {
        if (pos == -1)
        {
            while (trav->next != NULL)
                trav = trav->next;
        }
        else
        {
            for (size_t i = 0; i < pos - 1; i++)
            {
                if (trav->next == NULL)
                    return INVIND;
                trav = trav->next;
            }
        }
        newnode->data = malloc(inslist->element_size);
        if (!newnode->data)
        {
            free(newnode);
            return NOALLOC;
        }
        memcpy(newnode->data, data, inslist->element_size);
        newnode->next = trav->next;
        trav->next = newnode;
    }
    inslist->list_length++;
    return SUCCESS;
}

retcode deleteNodeByIndex(list *dellist, long pos)
{
    node *temp = NULL;
    if (pos < 0)
        return INVIND;
    if (!dellist)
        return INVINP;

    node *trav = dellist->head;
    if (trav == NULL)
        return DELOBD;

    if (pos == 0)
    {
        dellist->head = trav->next;
        free(trav->data);
        free(trav);
    }
    else
    {
        for (long i = 0; i < pos - 1; i++)
        {
            if (trav->next == NULL)
                return DELOBD;

            trav = trav->next;
        }
        temp = trav->next;
        if (temp == NULL)
            return DELOBD;

        trav->next = temp->next;
        free(temp->data);
        free(temp);
    }
    dellist->list_length--;
    return SUCCESS;
}

void copylist(list **dest, const list *src)
{
    freelist(*dest);
    *dest = createlist(src->element_size);
    if (!(*dest))
        return;
    void *ins = NULL;
    int pos = 0;
    while ((ins = getnode(src, pos++)) != NULL)
    {
        insertnode(*dest, length(*dest), ins);
    }
}

long member(const list *looplist, const void *check, int (*compare)(const void *, const void *))
{
    for (size_t i = 0; i < length(looplist); i++)
    {
        if (compare(check, getnode(looplist, i)) == 0)
            return (long)i;
    }
    return -1;
}
