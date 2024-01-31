#include "wsJson.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

namespace SgLib
{
    WsJson::WsJson()
    {
        m_iPos = 0;
        m_pszJson = NULL;
        m_pRootNode = NULL;
#ifdef DEBUG
        m_memNums = 0;
#endif
    }

    WsJson::~WsJson()
    {
        // 释放资源
#ifdef DEBUG
        printf("memory nums start: %d\n", m_memNums);
#endif
        FreeNode(m_pRootNode);
#ifdef DEBUG
        printf("memory nums end: %d\n", m_memNums);
#endif
    }

    void WsJson::FreeNode(T_JSON_NODE *pNode)
    {
        T_JSON_NODE *pCurNode = pNode;
        T_JSON_NODE *pNextNode;
        while (pCurNode != NULL)
        {
            if (pCurNode->emType == JSON_ARRAY)
            {
                FreeNode(pCurNode->array_val);
            }
            else if (pCurNode->emType == JSON_OBJECT)
            {
                FreeNode(pCurNode->object_val);
            }
            else if (pCurNode->emType == JSON_STRING)
            {
                free(pCurNode->string_val);
#ifdef DEBUG
                --m_memNums;
#endif
            }

            pNextNode = pCurNode->next;
            free(pCurNode);
#ifdef DEBUG
            --m_memNums;
#endif
            pCurNode = pNextNode;
        }
    }

    T_JSON_NODE &WsJson::parseJson(const char *pszJson)
    {
        m_pszJson = pszJson;
        m_pRootNode = (T_JSON_NODE *)malloc(sizeof(T_JSON_NODE));
#ifdef DEBUG 
        ++m_memNums;
#endif
        memset(m_pRootNode, 0, sizeof(T_JSON_NODE));
        if (m_pszJson[m_iPos] == '{')
        {
            m_pRootNode->emType = JSON_OBJECT;
            m_pRootNode->object_val = parseObject();
        }
        else if (m_pszJson[m_iPos] == '[')
        {
            m_pRootNode->emType = JSON_ARRAY;
            m_pRootNode->array_val = parseArray();
        }
        else
        {
            fprintf(stderr, "Error: Invalid JSON string: must start with object or array\n");
        }
        return *m_pRootNode;
    }

    T_JSON_NODE *WsJson::parseObject()
    {
        T_JSON_NODE *head = NULL, *tail = NULL;
        m_iPos++; // skip '{'
        while (m_pszJson[m_iPos] != '}')
        {
            T_JSON_NODE *pNode = (T_JSON_NODE *)malloc(sizeof(T_JSON_NODE));
#ifdef DEBUG
            ++m_memNums;
#endif
            memset(pNode, 0, sizeof(T_JSON_NODE));
            parseKey(pNode); // get key
            if (m_pszJson[m_iPos] != ':')
            {
                fprintf(stderr, "Invalid JSON string: expected ':' after name");
            }
            m_iPos++; // skip ':'
            parseValue(pNode);
            if (head == NULL)
            {
                head = tail = pNode;
            }
            else
            {
                tail->next = pNode;
                tail = pNode;
            }
            if (m_pszJson[m_iPos] == ',')
            {
                m_iPos++; // skip ','
            }
            else if (m_pszJson[m_iPos] != '}')
            {
                fprintf(stderr, "Invalid JSON string: expected ',' or '}' after value");
            }
        }
        m_iPos++;    // skip '}'
        return head; // 返回object{}的头结点
    }

    T_JSON_NODE *WsJson::parseArray()
    {
        T_JSON_NODE *head = NULL, *tail = NULL;
        m_iPos++; // skip '['
        while (m_pszJson[m_iPos] != ']')
        {
            T_JSON_NODE *pNode = (T_JSON_NODE *)malloc(sizeof(T_JSON_NODE));
#ifdef DEBUG
            ++m_memNums;
#endif
            memset(pNode, 0, sizeof(T_JSON_NODE));

            // 数组对象没有key，所以直接解析值
            parseValue(pNode);
            if (head == NULL)
            {
                head = tail = pNode;
            }
            else
            {
                tail->next = pNode;
                tail = pNode;
            }
            if (m_pszJson[m_iPos] == ',')
            {
                m_iPos++; // skip ','
            }
            else if (m_pszJson[m_iPos] != ']')
            {
                fprintf(stderr, "Invalid JSON string: expected ',' or ']' after value");
            }
        }
        m_iPos++; // skip ']'
        return head;
    }

    void WsJson::parseValue(T_JSON_NODE *pNode)
    {
        switch (m_pszJson[m_iPos])
        {
        case '{':
            pNode->emType = JSON_OBJECT;
            pNode->object_val = parseObject();
            break;
        case '[':
            pNode->emType = JSON_ARRAY;
            pNode->array_val = parseArray();
            break;
        case '"':
            pNode->emType = JSON_STRING;
            parseString(pNode);
            break;
        case 't':
        case 'f':
            pNode->emType = JSON_BOOLEAN;
            parseBoolean(pNode);
            break;
        default:
            pNode->emType = JSON_NUMBER;
            parseNumber(pNode);
            break;
        }
    }

    void WsJson::parseKey(T_JSON_NODE *pNode)
    {
        int start_pos = ++m_iPos; // skip leading quote
        while (m_pszJson[m_iPos] != '"')
        {
            if (m_pszJson[m_iPos] == '\\')
            {
                m_iPos++; // skip escape character
            }
            m_iPos++;
        }
        int len = m_iPos - start_pos;
        if (len > KEY_NAME_LEN - 1)
            strncpy(pNode->m_szKeyName, m_pszJson + start_pos, KEY_NAME_LEN - 1);
        else
        {
            strncpy(pNode->m_szKeyName, m_pszJson + start_pos, len);
        }
        m_iPos++; // skip trailing quote
    }

    void WsJson::parseString(T_JSON_NODE *pNode)
    {
        int start_pos = ++m_iPos; // skip leading quote
        while (m_pszJson[m_iPos] != '"')
        {
            if (m_pszJson[m_iPos] == '\\')
            {
                m_iPos++; // skip escape character
            }
            m_iPos++;
        }
        int len = m_iPos - start_pos;
        char *str = (char *)malloc(len + 1);
#ifdef DEBUG
        ++m_memNums;
#endif
        strncpy(str, m_pszJson + start_pos, len);
        str[len] = '\0';
        pNode->string_val = str;
        m_iPos++; // skip trailing quote
    }

    void WsJson::parseNumber(T_JSON_NODE *pNode)
    {
        char *endptr;
        double val = strtod(m_pszJson + m_iPos, &endptr);
        if (endptr == m_pszJson + m_iPos)
        {
            fprintf(stderr, "Invalid JSON string: expected number");
        }
        pNode->emType = JSON_NUMBER;
        pNode->number_val = val;
        m_iPos = endptr - m_pszJson;
    }

    void WsJson::parseBoolean(T_JSON_NODE *pNode)
    {
        if (strncmp(m_pszJson + m_iPos, "true", 4) == 0)
        {
            pNode->emType = JSON_BOOLEAN;
            pNode->boolean_val = 1;
            m_iPos += 4;
        }
        else if (strncmp(m_pszJson + m_iPos, "false", 5) == 0)
        {
            pNode->emType = JSON_BOOLEAN;
            pNode->boolean_val = 0;
            m_iPos += 5;
        }
        else
        {
            fprintf(stderr, "Invalid JSON string: expected 'true' or 'false'");
        }
    }

}