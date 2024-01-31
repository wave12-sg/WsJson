#pragma once

#include <string.h>
#include <stdio.h>

#ifndef KEY_NAME_LEN
#define KEY_NAME_LEN 50
#endif

namespace SgLib
{
    typedef enum
    {
        JSON_NULL,
        JSON_BOOLEAN,
        JSON_NUMBER,
        JSON_STRING,
        JSON_ARRAY,
        JSON_OBJECT
    } EM_JSON_TYPE;

    typedef struct tagJsonNode
    {
        friend class WsJson;
    private:
        union
        {
            int boolean_val;
            double number_val;
            char *string_val;
            tagJsonNode *array_val;
            tagJsonNode *object_val;
        };

    public:
        EM_JSON_TYPE emType;
        char m_szKeyName[KEY_NAME_LEN];
        tagJsonNode *next;

        tagJsonNode &get(const char *pszKey)
        {
            tagJsonNode *pNode = object_val;
            while (pNode != NULL)
            {
                if (strcmp(pNode->m_szKeyName, pszKey) == 0)
                {
                    return *pNode;
                }
                else
                {
                    pNode = pNode->next;
                }
            }
            pNode == NULL;
            return *pNode;
        }

        tagJsonNode &operator[](const char *pszKey)
        {
            tagJsonNode *pNode = object_val;
            while (pNode != NULL)
            {
                if (strcmp(pNode->m_szKeyName, pszKey) == 0)
                {
                    return *pNode;
                }
                else
                {
                    pNode = pNode->next;
                }
            }
            pNode == NULL;
            return *pNode;
        }

        tagJsonNode &operator[](int iPos)
        {
            int iCurPos = 0;
            tagJsonNode *pNode = array_val;
            while (pNode != NULL)
            {
                if (iCurPos == iPos)
                {
                    return *pNode;
                }
                else
                {
                    pNode = pNode->next;
                    ++iCurPos;
                }
            }
            pNode == NULL;
            return *pNode;
        }

        const char *toString() { return string_val; }
        double toNumber() { return number_val; }
        int toInt() { return (int)number_val; }
        int toBoolean() { return number_val; }

    } T_JSON_NODE;

    class WsJson
    {
    public:
        WsJson();
        ~WsJson();
        T_JSON_NODE &parseJson(const char *pszJson);

    private:
        T_JSON_NODE *parseObject();
        T_JSON_NODE *parseArray();
        void parseKey(T_JSON_NODE *pNode);
        void parseValue(T_JSON_NODE *pNode);
        void parseString(T_JSON_NODE *pNode);
        void parseNumber(T_JSON_NODE * pNode);
        void parseBoolean(T_JSON_NODE * pNode);
        void FreeNode(T_JSON_NODE * pNode);

        int m_iPos;
        const char *m_pszJson;
        T_JSON_NODE *m_pRootNode;
        #ifdef DEBUG
        int m_memNums;
        #endif
    };
}